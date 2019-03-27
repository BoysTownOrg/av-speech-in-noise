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
        auto expected = false;
        while (fadingIn.compare_exchange_weak(expected, true))
            ;
        hannCounter.store(0);
        player->play();
        //player->scheduleCallbackAfterSeconds(0.1);
    }

    void RandomizedMaskerPlayer::fadeOut() {
        auto expected = false;
        while (fadingOut.compare_exchange_weak(expected, true))
            ;
        hannCounter.store(levelTransitionSamples());
        //player->scheduleCallbackAfterSeconds(0.1);
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
        audioScale.store(std::pow(10, x/20));
    }
    
    void RandomizedMaskerPlayer::fillAudioBuffer(
        const std::vector<gsl::span<float>> &audio
    ) {
        auto scale = audioScale.load();
        for (auto channel : audio)
            for (auto &x : channel)
                x *= transitionScale() * scale;
    }
    
    /*
    // low priority thread
    void RandomizedMaskerPlayer::timerCallback() {
        if (fadeInComplete.compare_exchange_strong(true, false))
            listener->fadeInComplete();
        else if (fadeOutComplete.compare_exchange_strong(true, false)) {
            listener->fadeOutComplete();
            player->stop();
        }
        else
            player->scheduleCallbackAfterSeconds(0.1);
    }
    */
    
    void RandomizedMaskerPlayer::setFadeInOutSeconds(double x) {
        fadeInOutSeconds = x;
    }
    
    double RandomizedMaskerPlayer::transitionScale() {
        const auto pi = std::acos(-1);
        auto counter = hannCounter.load();
        auto halfWindowLength = levelTransitionSamples();
        const auto squareRoot = halfWindowLength
            ? std::sin((pi*counter) / (2*halfWindowLength))
            : 1;
        
        if (counter == halfWindowLength)
        //  fadeInComplete.store(true);
            fadingIn.store(false);
        if (counter == 2*halfWindowLength)
        //  fadeOutComplete.store(true);
            fadingOut.store(false);
        if (fadingIn || fadingOut)
            hannCounter.store(counter+1);
        return squareRoot * squareRoot;
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
}
