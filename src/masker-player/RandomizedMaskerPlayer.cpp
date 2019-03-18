#include "RandomizedMaskerPlayer.hpp"

namespace masker_player {
    RandomizedMaskerPlayer::RandomizedMaskerPlayer(VideoPlayer *player) : player{player}
    {
        player->subscribe(this);
    }

    void RandomizedMaskerPlayer::subscribe(MaskerPlayer::EventListener *) {
    
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
        player->play();
    }

    void RandomizedMaskerPlayer::fadeOut() {
    
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

    void RandomizedMaskerPlayer::fillAudioBuffer(const std::vector<gsl::span<float>> &audio) {
        for (auto channel : audio)
            for (auto &x : channel)
                x *= transitionScale() * audioScale;
    }
    
    void RandomizedMaskerPlayer::setFadeInSeconds(double x) {
        levelTransitionSamples = x * player->sampleRateHz();
    }

    double RandomizedMaskerPlayer::transitionScale() {
        if (hannCounter == levelTransitionSamples)
            return 1;
        
        const auto pi = std::acos(-1);
        const auto squareRoot = std::sin((pi*hannCounter++) / (2*levelTransitionSamples));
        return squareRoot * squareRoot;
    }
}
