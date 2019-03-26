#include "StimulusPlayerImpl.hpp"
#include <cmath>

namespace stimulus_player {
    StimulusPlayerImpl::StimulusPlayerImpl(VideoPlayer *player) :
        player{player}
    {
        player->subscribe(this);
    }

    void StimulusPlayerImpl::subscribe(StimulusPlayer::EventListener *e) {
        listener_ = e;
    }

    void StimulusPlayerImpl::play() {
        player->play();
    }

    void StimulusPlayerImpl::loadFile(std::string filePath) {
        player->loadFile(filePath);
    }

    void StimulusPlayerImpl::hideVideo() {
        player->hide();
    }

    void StimulusPlayerImpl::showVideo() {
        player->show();
    }

    double StimulusPlayerImpl::rms() {
        return 1;
    }

    void StimulusPlayerImpl::setLevel_dB(double x) {
        audioScale = std::pow(10, x/20);
    }
    
    void StimulusPlayerImpl::playbackComplete() { 
        listener_->playbackComplete();
    }
    
    void StimulusPlayerImpl::fillAudioBuffer(
        const std::vector<gsl::span<float>> &audio
    ) {
        for (auto channel : audio)
            for (auto &x : channel)
                x *= audioScale;
    }
    
    void StimulusPlayerImpl::setAudioDevice(std::string device) {
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
    
    std::vector<std::string> StimulusPlayerImpl::audioDevices() {
        std::vector<std::string> descriptions{};
        for (int i = 0; i < player->deviceCount(); ++i)
            descriptions.push_back(player->deviceDescription(i));
        return descriptions;
    }
}
