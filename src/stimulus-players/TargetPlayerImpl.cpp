#include "TargetPlayerImpl.hpp"
#include <cmath>

namespace stimulus_players {
    TargetPlayerImpl::TargetPlayerImpl(
        VideoPlayer *player,
        AudioReader *reader
    ) :
        player{player},
        reader{reader}
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
        filePath_ = filePath;
        player->loadFile(std::move(filePath));
    }

    void TargetPlayerImpl::hideVideo() {
        player->hide();
    }

    void TargetPlayerImpl::showVideo() {
        player->show();
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
    
    double TargetPlayerImpl::rms() {
        auto audio = readAudio_();
        if (audio.size() == 0)
            return 0;
        
        auto firstChannel = audio.front();
        return ::stimulus_players::rms(firstChannel);
    }

    std::vector<std::vector<float>> TargetPlayerImpl::readAudio_() {
        try {
            return reader->read(filePath_);
        } catch (const AudioReader::InvalidFile &) {
            throw av_coordinate_response_measure::InvalidAudioFile{};
        }
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
            throw av_coordinate_response_measure::InvalidAudioDevice{};
        auto deviceIndex = gsl::narrow<int>(found - devices_.begin());
        player->setDevice(deviceIndex);
    }
    
    std::vector<std::string> TargetPlayerImpl::audioDevices() {
        std::vector<std::string> descriptions{};
        for (int i = 0; i < player->deviceCount(); ++i)
            descriptions.push_back(player->deviceDescription(i));
        return descriptions;
    }
    
    bool TargetPlayerImpl::playing() { 
        return player->playing();
    }
    
    void TargetPlayerImpl::subscribeToPlaybackCompletion() { 
        player->subscribeToPlaybackCompletion();
    }
}
