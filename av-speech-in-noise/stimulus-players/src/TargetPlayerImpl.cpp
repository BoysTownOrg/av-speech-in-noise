#include "TargetPlayerImpl.hpp"
#include <gsl/gsl>
#include <cmath>
#include <algorithm>

namespace stimulus_players {
TargetPlayerImpl::TargetPlayerImpl(VideoPlayer *player, AudioReader *reader)
    : player{player}, reader{reader} {
    player->subscribe(this);
}

void TargetPlayerImpl::subscribe(TargetPlayer::EventListener *e) {
    listener_ = e;
}

void TargetPlayerImpl::play() { player->play(); }

void TargetPlayerImpl::playAt(const SystemTimeWithDelay &t) {
    player->playAt(t);
}

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
    return stimulus_players::rms(firstChannel);
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

constexpr auto begin(const gsl::span<float> &channel)
    -> gsl::span<float>::iterator {
    return channel.begin();
}

constexpr auto end(const gsl::span<float> &channel)
    -> gsl::span<float>::iterator {
    return channel.end();
}

void TargetPlayerImpl::fillAudioBuffer(
    const std::vector<gsl::span<float>> &audio) {
    auto scale = audioScale.load();
    auto usingFirstChannelOnly = useFirstChannelOnly_.load();
    auto afterFirstChannel{false};
    for (auto channel : audio) {
        if (usingFirstChannelOnly && afterFirstChannel)
            std::fill(begin(channel), end(channel), float{0});
        else
            std::transform(begin(channel), end(channel), begin(channel),
                [&](auto &x) { return gsl::narrow_cast<float>(x * scale); });
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
    std::vector<std::string> descriptions(
        gsl::narrow<std::size_t>(player->deviceCount()));
    std::generate(descriptions.begin(), descriptions.end(),
        [&, n = 0]() mutable { return player->deviceDescription(n++); });
    return descriptions;
}

static void store(std::atomic<bool> &where, bool what) { where.store(what); }

void TargetPlayerImpl::useFirstChannelOnly() {
    store(useFirstChannelOnly_, true);
}

void TargetPlayerImpl::useAllChannels() { store(useFirstChannelOnly_, false); }

auto TargetPlayerImpl::playing() -> bool { return player->playing(); }

void TargetPlayerImpl::subscribeToPlaybackCompletion() {
    player->subscribeToPlaybackCompletion();
}

auto TargetPlayerImpl::durationSeconds() -> double {
    return player->durationSeconds();
}
}
