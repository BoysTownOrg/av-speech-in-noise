#include "MaskerPlayerImpl.hpp"
#include <cmath>

namespace masker_player {
    MaskerPlayerImpl::MaskerPlayerImpl(AudioPlayer *player)
        : player{player}
    {
        player->subscribe(this);
    }

    void MaskerPlayerImpl::subscribe(MaskerPlayer::EventListener *e) {
        listener = e;
    }

    void MaskerPlayerImpl::fadeIn() {
        pleaseFadeIn.store(true);
        player->play();
        player->scheduleCallbackAfterSeconds(0.1);
    }

    void MaskerPlayerImpl::fadeOut() {
        pleaseFadeOut.store(true);
        player->scheduleCallbackAfterSeconds(0.1);
    }

    void MaskerPlayerImpl::loadFile(std::string filePath) {
        player->loadFile(std::move(filePath));
    }

    bool MaskerPlayerImpl::playing() {
        return player->playing();
    }

    void MaskerPlayerImpl::setLevel_dB(double x) {
        levelScalar.store(std::pow(10, x/20));
    }
    
    void MaskerPlayerImpl::setFadeInOutSeconds(double x) {
        fadeInOutSeconds.store(x);
    }
    
    void MaskerPlayerImpl::setAudioDevice(std::string device) {
        player->setDevice(findDeviceIndex(device));
    }
    
    int MaskerPlayerImpl::findDeviceIndex(const std::string &device) {
        auto devices_ = audioDeviceDescriptions_();
        auto found = std::find(
            devices_.begin(),
            devices_.end(),
            device
        );
        if (found == devices_.end())
            throw recognition_test::InvalidAudioDevice{};
        return gsl::narrow<int>(found - devices_.begin());
    }
    
    double MaskerPlayerImpl::rms() {
        auto audio = player->readAudio({});
        if (audio.size() == 0)
            return 0;
        auto channel = audio.front();
        return std::sqrt(
            std::accumulate(
                channel.begin(),
                channel.end(),
                0.0,
                [](float a, float b) { return a += b * b; }
            ) / channel.size()
        );
    }
    
    std::vector<std::string> MaskerPlayerImpl::audioDeviceDescriptions_() {
        std::vector<std::string> descriptions{};
        for (int i = 0; i < player->deviceCount(); ++i)
            descriptions.push_back(player->deviceDescription(i));
        return descriptions;
    }
    
    std::vector<std::string> MaskerPlayerImpl::outputAudioDeviceDescriptions() {
        std::vector<std::string> descriptions{};
        for (int i = 0; i < player->deviceCount(); ++i)
            if (player->outputDevice(i))
                descriptions.push_back(player->deviceDescription(i));
        return descriptions;
    }
    
    void MaskerPlayerImpl::timerCallback() {
        auto expectedFadeInComplete = true;
        if (fadeInComplete.compare_exchange_strong(
            expectedFadeInComplete,
            false
        )) {
            listener->fadeInComplete();
            return;
        }
        
        auto expectedFadeOutComplete = true;
        if (fadeOutComplete.compare_exchange_strong(
            expectedFadeOutComplete,
            false
        )) {
            listener->fadeOutComplete();
            player->stop();
            return;
        }
        
        player->scheduleCallbackAfterSeconds(0.1);
    }
    
    // high priority thread
    void MaskerPlayerImpl::fillAudioBuffer(
        const std::vector<gsl::span<float>> &audio
    ) {
        checkForFadeIn();
        checkForFadeOut();
        scaleAudio(audio);
    }
    
    // high priority thread
    void MaskerPlayerImpl::checkForFadeIn() {
        auto expected = true;
        if (pleaseFadeIn.compare_exchange_strong(expected, false))
            prepareToFadeIn();
    }
    
    // high priority thread
    void MaskerPlayerImpl::prepareToFadeIn() {
        updateWindowLength();
        hannCounter = 0;
        fadingIn = true;
    }
    
    // high priority thread
    void MaskerPlayerImpl::updateWindowLength() {
        halfWindowLength = levelTransitionSamples();
    }
    
    // high priority thread
    void MaskerPlayerImpl::checkForFadeOut() {
        auto expected = true;
        if (pleaseFadeOut.compare_exchange_strong(expected, false))
            prepareToFadeOut();
    }
    
    // high priority thread
    void MaskerPlayerImpl::prepareToFadeOut() {
        updateWindowLength();
        hannCounter = halfWindowLength;
        fadingOut = true;
    }

    // high priority thread
    int MaskerPlayerImpl::levelTransitionSamples() {
        return fadeInOutSeconds * player->sampleRateHz();
    }
    
    // high priority thread
    void MaskerPlayerImpl::scaleAudio(
        const std::vector<gsl::span<float>> &audio
    ) {
        auto levelScalar_ = levelScalar.load();
        for (int i = 0; i < audio.front().size(); ++i) {
            auto fadeScalar_ = fadeScalar();
            updateFadeState();
            for (size_t j = 0; j < audio.size(); ++j)
                audio.at(j).at(i) *= fadeScalar_ * levelScalar_;
        }
    }
    
    static const auto pi = std::acos(-1);
    
    // high priority thread
    double MaskerPlayerImpl::fadeScalar() {
        const auto squareRoot = halfWindowLength
            ? std::sin((pi*hannCounter) / (2*halfWindowLength))
            : 1;
        return squareRoot * squareRoot;
    }
    
    // high priority thread
    void MaskerPlayerImpl::updateFadeState() {
        checkForFadeInComplete();
        checkForFadeOutComplete();
        advanceCounterIfStillFading();
    }
    
    // high priority thread
    void MaskerPlayerImpl::checkForFadeInComplete() {
        if (doneFadingIn()) {
            fadeInComplete.store(true);
            fadingIn = false;
        }
    }
    
    // high priority thread
    bool MaskerPlayerImpl::doneFadingIn() {
        return fadingIn && hannCounter == halfWindowLength;
    }
    
    // high priority thread
    void MaskerPlayerImpl::checkForFadeOutComplete() {
        if (doneFadingOut()) {
            fadeOutComplete.store(true);
            fadingOut = false;
        }
    }
    
    // high priority thread
    bool MaskerPlayerImpl::doneFadingOut() {
        return fadingOut && hannCounter == 2*halfWindowLength;
    }
    
    // high priority thread
    void MaskerPlayerImpl::advanceCounterIfStillFading() {
        if (fadingIn || fadingOut)
            ++hannCounter;
    }
}
