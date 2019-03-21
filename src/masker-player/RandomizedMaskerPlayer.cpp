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

    int RandomizedMaskerPlayer::deviceCount() {
        return player->deviceCount();
    }

    std::string RandomizedMaskerPlayer::deviceDescription(int index) {
        return player->deviceDescription(index);
    }

    void RandomizedMaskerPlayer::setDevice(int index) {
        player->setDevice(index);
    }

    void RandomizedMaskerPlayer::fadeIn() {
        fadingIn = true;
        hannCounter = 0;
        player->play();
    }

    void RandomizedMaskerPlayer::fadeOut() {
        fadingOut = true;
        hannCounter = levelTransitionSamples();
    }

    int RandomizedMaskerPlayer::levelTransitionSamples() {
        return fadeInOutSeconds * player->sampleRateHz();
    }

    void RandomizedMaskerPlayer::loadFile(std::string filePath) {
        player->loadFile(std::move(filePath));
    }

    bool RandomizedMaskerPlayer::playing() {
        return player->playing();
    }

    void RandomizedMaskerPlayer::setLevel_dB(double x) {
        audioScale = std::pow(10, x/20);
    }
    
    void RandomizedMaskerPlayer::fillAudioBuffer(
        const std::vector<gsl::span<float>> &audio
    ) {
        bool wasFadingIn = fadingIn;
        bool wasFadingOut = fadingOut;
        for (auto channel : audio)
            for (auto &x : channel)
                x *= transitionScale() * audioScale;
        if (wasFadingIn && !fadingIn)
            listener->fadeInComplete();
        if (wasFadingOut && !fadingOut)
            player->stop();
    }
    
    void RandomizedMaskerPlayer::setFadeInOutSeconds(double x) {
        fadeInOutSeconds = x;
    }
    
    double RandomizedMaskerPlayer::transitionScale() {
        const auto pi = std::acos(-1);
        auto halfWindowLength = levelTransitionSamples();
        const auto squareRoot = halfWindowLength
            ? std::sin((pi*hannCounter) / (2*halfWindowLength))
            : 1;
        
        if (hannCounter == halfWindowLength)
            fadingIn = false;
        if (hannCounter == 2*halfWindowLength)
            fadingOut = false;
        if (fadingIn || fadingOut)
            ++hannCounter;
        return squareRoot * squareRoot;
    }
    
    void RandomizedMaskerPlayer::setAudioDevice(std::string device) {
        auto devices_ = audioDevices();
        auto deviceIndex = gsl::narrow<int>(
            std::find(
                devices_.begin(),
                devices_.end(),
                device
            ) - devices_.begin()
        );
        player->setDevice(deviceIndex);
    }
    
    std::vector<std::string> RandomizedMaskerPlayer::audioDevices() {
        std::vector<std::string> descriptions{};
        for (int i = 0; i < player->deviceCount(); ++i)
            descriptions.push_back(player->deviceDescription(i));
        return descriptions;
    }
}
