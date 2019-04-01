#include "StimulusPlayerImpl.hpp"
#include <cmath>

namespace stimulus_player {
    StimulusPlayerImpl::StimulusPlayerImpl(VideoPlayer *player) :
        player{player}
    {
        player->subscribe(this);
    }

    void StimulusPlayerImpl::subscribe(TargetPlayer::EventListener *e) {
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
        audioScale.store(std::pow(10, x/20));
    }
    
    void StimulusPlayerImpl::playbackComplete() { 
        listener_->playbackComplete();
    }
    
    void StimulusPlayerImpl::fillAudioBuffer(
        const std::vector<gsl::span<float>> &audio
    ) {
        auto scale = audioScale.load();
        for (auto channel : audio)
            for (auto &x : channel)
                x *= scale;
    }
    
    void StimulusPlayerImpl::setAudioDevice(std::string device) {
        auto devices_ = audioDevices();
        auto found = std::find(
            devices_.begin(),
            devices_.end(),
            device
        );
        if (found == devices_.end())
            throw recognition_test::InvalidAudioDevice{};
        auto deviceIndex = gsl::narrow<int>(found - devices_.begin());
        player->setDevice(deviceIndex);
    }
    
    std::vector<std::string> StimulusPlayerImpl::audioDevices() {
        std::vector<std::string> descriptions{};
        for (int i = 0; i < player->deviceCount(); ++i)
            descriptions.push_back(player->deviceDescription(i));
        return descriptions;
    }
}
