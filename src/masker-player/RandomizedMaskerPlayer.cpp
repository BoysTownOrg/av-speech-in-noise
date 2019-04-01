#include "RandomizedMaskerPlayer.hpp"
#include <cmath>

namespace masker_player {
    RandomizedMaskerPlayer::RandomizedMaskerPlayer(AudioPlayer *player)
        : player{player}
    {
        player->subscribe(this);
    }

    void RandomizedMaskerPlayer::subscribe(MaskerPlayer::EventListener *e) {
        listener = e;
    }

    void RandomizedMaskerPlayer::fadeIn() {
        pleaseFadeIn.store(true);
        player->play();
        player->scheduleCallbackAfterSeconds(0.1);
    }

    void RandomizedMaskerPlayer::fadeOut() {
        pleaseFadeOut.store(true);
        player->scheduleCallbackAfterSeconds(0.1);
    }

    void RandomizedMaskerPlayer::loadFile(std::string filePath) {
        player->loadFile(std::move(filePath));
    }

    bool RandomizedMaskerPlayer::playing() {
        return player->playing();
    }

    void RandomizedMaskerPlayer::setLevel_dB(double x) {
        levelScalar.store(std::pow(10, x/20));
    }
    
    void RandomizedMaskerPlayer::setFadeInOutSeconds(double x) {
        fadeInOutSeconds.store(x);
    }
    
    void RandomizedMaskerPlayer::setAudioDevice(std::string device) {
        player->setDevice(findDeviceIndex(device));
    }
    
    int RandomizedMaskerPlayer::findDeviceIndex(const std::string &device) {
        auto devices_ = audioDeviceDescriptions_();
        auto deviceIndex = gsl::narrow<int>(
            std::find(
                devices_.begin(),
                devices_.end(),
                device
            ) - devices_.begin()
        );
        return deviceIndex;
    }
    
    std::vector<std::string> RandomizedMaskerPlayer::audioDeviceDescriptions() {
        return audioDeviceDescriptions_();
    }
    
    std::vector<std::string> RandomizedMaskerPlayer::audioDeviceDescriptions_() {
        std::vector<std::string> descriptions{};
        for (int i = 0; i < player->deviceCount(); ++i)
            descriptions.push_back(player->deviceDescription(i));
        return descriptions;
    }
    
    std::vector<std::string> RandomizedMaskerPlayer::outputAudioDeviceDescriptions() {
        std::vector<std::string> descriptions{};
        for (int i = 0; i < player->deviceCount(); ++i)
            if (player->outputDevice(i))
                descriptions.push_back(player->deviceDescription(i));
        return descriptions;
    }
    
    void RandomizedMaskerPlayer::timerCallback() {
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
    void RandomizedMaskerPlayer::fillAudioBuffer(
        const std::vector<gsl::span<float>> &audio
    ) {
        checkForFadeIn();
        checkForFadeOut();
        scaleAudio(audio);
    }
    
    // high priority thread
    void RandomizedMaskerPlayer::checkForFadeIn() {
        auto expected = true;
        if (pleaseFadeIn.compare_exchange_strong(expected, false))
            prepareToFadeIn();
    }
    
    // high priority thread
    void RandomizedMaskerPlayer::prepareToFadeIn() {
        updateWindowLength();
        hannCounter = 0;
        fadingIn = true;
    }
    
    // high priority thread
    void RandomizedMaskerPlayer::updateWindowLength() {
        halfWindowLength = levelTransitionSamples();
    }
    
    // high priority thread
    void RandomizedMaskerPlayer::checkForFadeOut() {
        auto expected = true;
        if (pleaseFadeOut.compare_exchange_strong(expected, false))
            prepareToFadeOut();
    }
    
    // high priority thread
    void RandomizedMaskerPlayer::prepareToFadeOut() {
        updateWindowLength();
        hannCounter = halfWindowLength;
        fadingOut = true;
    }

    // high priority thread
    int RandomizedMaskerPlayer::levelTransitionSamples() {
        return fadeInOutSeconds * player->sampleRateHz();
    }
    
    // high priority thread
    void RandomizedMaskerPlayer::scaleAudio(
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
    double RandomizedMaskerPlayer::fadeScalar() {
        const auto squareRoot = halfWindowLength
            ? std::sin((pi*hannCounter) / (2*halfWindowLength))
            : 1;
        return squareRoot * squareRoot;
    }
    
    // high priority thread
    void RandomizedMaskerPlayer::updateFadeState() {
        checkForFadeInComplete();
        checkForFadeOutComplete();
        advanceCounterIfStillFading();
    }
    
    // high priority thread
    void RandomizedMaskerPlayer::checkForFadeInComplete() {
        if (doneFadingIn()) {
            fadeInComplete.store(true);
            fadingIn = false;
        }
    }
    
    // high priority thread
    bool RandomizedMaskerPlayer::doneFadingIn() {
        return fadingIn && hannCounter == halfWindowLength;
    }
    
    // high priority thread
    void RandomizedMaskerPlayer::checkForFadeOutComplete() {
        if (doneFadingOut()) {
            fadeOutComplete.store(true);
            fadingOut = false;
        }
    }
    
    // high priority thread
    bool RandomizedMaskerPlayer::doneFadingOut() {
        return fadingOut && hannCounter == 2*halfWindowLength;
    }
    
    // high priority thread
    void RandomizedMaskerPlayer::advanceCounterIfStillFading() {
        if (fadingIn || fadingOut)
            ++hannCounter;
    }
}
