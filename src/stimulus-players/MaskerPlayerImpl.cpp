#include "MaskerPlayerImpl.hpp"
#include <cmath>

namespace stimulus_players {
    MaskerPlayerImpl::MaskerPlayerImpl(
        AudioPlayer *player,
        AudioReader *reader
    ) :
        player{player},
        reader{reader}
    
    {
        player->subscribe(this);
    }

    void MaskerPlayerImpl::subscribe(MaskerPlayer::EventListener *e) {
        listener = e;
    }
    
    double MaskerPlayerImpl::durationSeconds() {
        return player->durationSeconds();
    }
    
    void MaskerPlayerImpl::seekSeconds(double x) {
        player->seekSeconds(x);
    }
    
    double MaskerPlayerImpl::fadeTimeSeconds() {
        return fadeInOutSeconds.load();
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
        player->loadFile(filePath_ = std::move(filePath));
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
            throw av_coordinate_response_measure::InvalidAudioDevice{};
        return gsl::narrow<int>(found - devices_.begin());
    }
    
    template<typename T>
    T rms(const std::vector<T> &x) {
        return std::sqrt(
            std::accumulate(
                x.begin(),
                x.end(),
                T{ 0 },
                [](T a, T b) { return a += b * b; }
            ) / x.size()
        );
    }

    double MaskerPlayerImpl::rms() {
        auto audio = readAudio_();
        if (audio.empty())
            return 0;
        
        auto firstChannel = audio.front();
        return ::stimulus_players::rms(firstChannel);
    }

    std::vector<std::vector<float>> MaskerPlayerImpl::readAudio_() {
        try {
            return reader->read(filePath_);
        } catch (const AudioReader::InvalidFile &) {
            throw av_coordinate_response_measure::InvalidAudioFile{};
        }
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
    
    // real-time audio thread
    void MaskerPlayerImpl::fillAudioBuffer(
        const std::vector<gsl::span<float>> &audio
    ) {
        checkForFadeIn();
        checkForFadeOut();
        scaleAudio(audio);
    }
    
    // real-time audio thread
    void MaskerPlayerImpl::checkForFadeIn() {
        auto expected = true;
        if (pleaseFadeIn.compare_exchange_strong(expected, false))
            prepareToFadeIn();
    }
    
    // real-time audio thread
    void MaskerPlayerImpl::prepareToFadeIn() {
        updateWindowLength();
        hannCounter = 0;
        fadingIn = true;
    }
    
    // real-time audio thread
    void MaskerPlayerImpl::updateWindowLength() {
        halfWindowLength = levelTransitionSamples();
    }
    
    // real-time audio thread
    void MaskerPlayerImpl::checkForFadeOut() {
        auto expected = true;
        if (pleaseFadeOut.compare_exchange_strong(expected, false))
            prepareToFadeOut();
    }
    
    // real-time audio thread
    void MaskerPlayerImpl::prepareToFadeOut() {
        updateWindowLength();
        hannCounter = halfWindowLength;
        fadingOut = true;
    }

    // real-time audio thread
    int MaskerPlayerImpl::levelTransitionSamples() {
        return fadeInOutSeconds * player->sampleRateHz();
    }
    
    // real-time audio thread
    void MaskerPlayerImpl::scaleAudio(
        const std::vector<gsl::span<float>> &audio
    ) {
        if (audio.size() == 0)
            return;
        
        auto firstChannel = audio.front();
        auto levelScalar_ = levelScalar.load();
        for (int i = 0; i < firstChannel.size(); ++i) {
            auto fadeScalar_ = fadeScalar();
            updateFadeState();
            for (size_t channel = 0; channel < audio.size(); ++channel)
                audio.at(channel).at(i) *= fadeScalar_ * levelScalar_;
        }
    }
    
    static const auto pi = std::acos(-1);
    
    // real-time audio thread
    double MaskerPlayerImpl::fadeScalar() {
        const auto squareRoot = halfWindowLength
            ? std::sin((pi*hannCounter) / (2*halfWindowLength))
            : 1;
        return squareRoot * squareRoot;
    }
    
    // real-time audio thread
    void MaskerPlayerImpl::updateFadeState() {
        checkForFadeInComplete();
        checkForFadeOutComplete();
        advanceCounterIfStillFading();
    }
    
    // real-time audio thread
    void MaskerPlayerImpl::checkForFadeInComplete() {
        if (doneFadingIn()) {
            fadeInComplete.store(true);
            fadingIn = false;
        }
    }
    
    // real-time audio thread
    bool MaskerPlayerImpl::doneFadingIn() {
        return fadingIn && hannCounter == halfWindowLength;
    }
    
    // real-time audio thread
    void MaskerPlayerImpl::checkForFadeOutComplete() {
        if (doneFadingOut()) {
            fadeOutComplete.store(true);
            fadingOut = false;
        }
    }
    
    // real-time audio thread
    bool MaskerPlayerImpl::doneFadingOut() {
        return fadingOut && hannCounter == 2*halfWindowLength;
    }
    
    // real-time audio thread
    void MaskerPlayerImpl::advanceCounterIfStillFading() {
        if (fadingIn || fadingOut)
            ++hannCounter;
    }
}
