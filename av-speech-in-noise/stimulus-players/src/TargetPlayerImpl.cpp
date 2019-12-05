#include "TargetPlayerImpl.hpp"
#include <cmath>

namespace stimulus_players {
TargetPlayerImpl::TargetPlayerImpl(VideoPlayer *player, AudioReader *reader)
    : player{player}, reader{reader} {
    player->subscribe(this);
}

void TargetPlayerImpl::subscribe(TargetPlayer::EventListener *e) {
    listener_ = e;
}

void TargetPlayerImpl::play() { player->play(); }

void TargetPlayerImpl::loadFile(std::string filePath) {
    player->loadFile(filePath_ = std::move(filePath));
}

void TargetPlayerImpl::hideVideo() { player->hide(); }

void TargetPlayerImpl::showVideo() { player->show(); }

template <typename T> auto rms(const std::vector<T> &x) -> T {
    return std::sqrt(std::accumulate(x.begin(), x.end(), T{0}, [](T a, T b) {
        return a += b * b;
    }) / x.size());
}

auto TargetPlayerImpl::rms() -> double {
    auto audio = readAudio_();
    if (audio.empty())
        return 0;

    auto firstChannel = audio.front();
    return ::stimulus_players::rms(firstChannel);
}

auto TargetPlayerImpl::readAudio_() -> audio_type {
    try {
        return reader->read(filePath_);
    } catch (const AudioReader::InvalidFile &) {
        throw av_speech_in_noise::InvalidAudioFile{};
    }
}

void TargetPlayerImpl::setLevel_dB(double x) {
    audioScale.store(std::pow(10, x / 20));
}

void TargetPlayerImpl::playbackComplete() { listener_->playbackComplete(); }

void TargetPlayerImpl::fillAudioBuffer(
    const std::vector<gsl::span<float>> &audio) {
    auto scale = audioScale.load();
    auto useFirstChannelOnly__ = useFirstChannelOnly_.load();
    auto afterFirstChannel{false};
    for (auto channel : audio) {
        if (useFirstChannelOnly__ && afterFirstChannel)
            std::fill(channel.begin(), channel.end(), 0);
        else
            for (auto &x : channel)
                x *= scale;
        afterFirstChannel = true;
    }
}

void TargetPlayerImpl::setAudioDevice(std::string device) {
    auto devices_ = audioDevices();
    auto found = std::find(devices_.begin(), devices_.end(), device);
    if (found == devices_.end())
        throw av_speech_in_noise::InvalidAudioDevice{};
    auto deviceIndex = gsl::narrow<int>(found - devices_.begin());
    player->setDevice(deviceIndex);
}

auto TargetPlayerImpl::audioDevices() -> std::vector<std::string> {
    std::vector<std::string> descriptions{};
    for (int i = 0; i < player->deviceCount(); ++i)
        descriptions.push_back(player->deviceDescription(i));
    return descriptions;
}

void TargetPlayerImpl::useFirstChannelOnly() {
    useFirstChannelOnly_.store(true);
}

void TargetPlayerImpl::useAllChannels() {
    useFirstChannelOnly_.store(false);
}

auto TargetPlayerImpl::playing() -> bool { return player->playing(); }

void TargetPlayerImpl::subscribeToPlaybackCompletion() {
    player->subscribeToPlaybackCompletion();
}

auto TargetPlayerImpl::durationSeconds() -> double {
    return player->durationSeconds();
}
}
