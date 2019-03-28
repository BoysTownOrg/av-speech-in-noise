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
        audioScale.store(std::pow(10, x/20));
    }
    
    void RandomizedMaskerPlayer::setFadeInOutSeconds(double x) {
        fadeInOutSeconds.store(x);
    }
    
    void RandomizedMaskerPlayer::setAudioDevice(std::string device) {
        auto devices_ = audioDeviceDescriptions();
        auto deviceIndex = gsl::narrow<int>(
            std::find(
                devices_.begin(),
                devices_.end(),
                device
            ) - devices_.begin()
        );
        player->setDevice(deviceIndex);
    }
    
    std::vector<std::string> RandomizedMaskerPlayer::audioDeviceDescriptions() {
        std::vector<std::string> descriptions{};
        for (int i = 0; i < player->deviceCount(); ++i)
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
        if (pleaseFadeIn.compare_exchange_strong(expected, false)) {
            hannCounter = 0;
            halfWindowLength = levelTransitionSamples();
            fadingIn = true;
        }
    }
    
    // high priority thread
    void RandomizedMaskerPlayer::checkForFadeOut() {
        auto expected = true;
        if (pleaseFadeOut.compare_exchange_strong(expected, false)) {
            hannCounter = halfWindowLength = levelTransitionSamples();
            fadingOut = true;
        }
    }

    // high priority thread
    int RandomizedMaskerPlayer::levelTransitionSamples() {
        return fadeInOutSeconds * player->sampleRateHz();
    }
    
    // high priority thread
    void RandomizedMaskerPlayer::scaleAudio(
        const std::vector<gsl::span<float>> &audio
    ) {
        auto scale = audioScale.load();
        for (auto channel : audio)
            for (auto &x : channel)
                x *= transitionScale() * scale;
    }
    
    // high priority thread
    double RandomizedMaskerPlayer::transitionScale() {
        auto scale = nextScale();
        
        if (hannCounter == halfWindowLength && fadingIn) {
            fadeInComplete.store(true);
            fadingIn = false;
        }
        if (hannCounter == 2*halfWindowLength && fadingOut) {
            fadeOutComplete.store(true);
            fadingOut = false;
        }
        if (fadingIn || fadingOut)
            ++hannCounter;
        return scale;
    }
    
    // high priority thread
    double RandomizedMaskerPlayer::nextScale() {
        const auto pi = std::acos(-1);
        const auto squareRoot = halfWindowLength
            ? std::sin((pi*hannCounter) / (2*halfWindowLength))
            : 1;
        return squareRoot * squareRoot;
    }
}
