#include "TargetPlayerImpl.hpp"
#include <cmath>

namespace target_player {
    TargetPlayerImpl::TargetPlayerImpl(VideoPlayer *player) :
        player{player}
    {
        player->subscribe(this);
    }

    void TargetPlayerImpl::subscribe(TargetPlayer::EventListener *e) {
        listener_ = e;
    }

    void TargetPlayerImpl::play() {
        player->play();
    }

    void TargetPlayerImpl::loadFile(std::string filePath) {
        player->loadFile(std::move(filePath));
    }

    void TargetPlayerImpl::hideVideo() {
        player->hide();
    }

    void TargetPlayerImpl::showVideo() {
        player->show();
    }

    double TargetPlayerImpl::rms() {
        return 1;
    }

    void TargetPlayerImpl::setLevel_dB(double x) {
        audioScale.store(std::pow(10, x/20));
    }
    
    void TargetPlayerImpl::playbackComplete() { 
        listener_->playbackComplete();
    }
    
    void TargetPlayerImpl::fillAudioBuffer(
        const std::vector<gsl::span<float>> &audio
    ) {
        auto scale = audioScale.load();
        for (auto channel : audio)
            for (auto &x : channel)
                x *= scale;
    }
    
    void TargetPlayerImpl::setAudioDevice(std::string device) {
        auto devices_ = audioDevices();
        auto found = std::find(
            devices_.begin(),
            devices_.end(),
            std::move(device)
        );
        if (found == devices_.end())
            throw recognition_test::InvalidAudioDevice{};
        auto deviceIndex = gsl::narrow<int>(found - devices_.begin());
        player->setDevice(deviceIndex);
    }
    
    std::vector<std::string> TargetPlayerImpl::audioDevices() {
        std::vector<std::string> descriptions{};
        for (int i = 0; i < player->deviceCount(); ++i)
            descriptions.push_back(player->deviceDescription(i));
        return descriptions;
    }
}
