#include "RandomizedMaskerPlayer.hpp"

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
        for (auto channel : audio)
            for (auto &x : channel)
                x *= transitionScale() * audioScale;
        notifyIfFadeInComplete();
    }

    void RandomizedMaskerPlayer::notifyIfFadeInComplete() {
        if (hannCounter > levelTransitionSamples() && fadingIn) {
            fadingIn = false;
            listener->fadeInComplete();
        }
    }
    
    void RandomizedMaskerPlayer::setFadeInOutSeconds(double x) {
        fadeInOutSeconds = x;
    }
    
    double RandomizedMaskerPlayer::transitionScale() {
        if (!fadingIn && !fadingOut)
            return 1;
        
        const auto pi = std::acos(-1);
        const auto squareRoot = std::sin((pi*hannCounter++) / (2*levelTransitionSamples()));
        return squareRoot * squareRoot;
    }
}
