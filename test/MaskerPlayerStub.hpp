#ifndef TESTS_MASKERPLAYERSTUB_HPP_
#define TESTS_MASKERPLAYERSTUB_HPP_

#include "LogString.hpp"
#include <recognition-test/RecognitionTestModel.hpp>
#include <utility>

namespace av_speech_in_noise {
class MaskerPlayerStub : public MaskerPlayer {
    std::vector<std::string> outputAudioDeviceDescriptions_;
    std::string filePath_;
    std::string device_;
    DigitalLevel digitalLevel_{};
    double level_dB_{};
    double fadeTimeSeconds_{};
    double durationSeconds_{};
    double secondsSeeked_{};
    double channelDelaySeconds_{};
    double sampleRateHz_{};
    gsl::index channelDelayed_{};
    Observer *listener_{};
    PlayerTime currentSystemTime_{};
    std::vector<player_system_time_type> toNanosecondsSystemTime_{};
    std::uintmax_t nanoseconds_{};
    bool fadeInCalled_{};
    bool fadeOutCalled_{};
    bool playing_{};
    bool setDeviceCalled_{};
    bool throwInvalidAudioDeviceWhenDeviceSet_{};
    bool throwInvalidAudioFileOnLoad_{};
    bool usingAllChannels_{};
    bool usingFirstChannelOnly_{};
    bool usingSecondChannelOnly_{};
    bool channelDelaysCleared_{};
    bool played_{};

  public:
    [[nodiscard]] auto played() const -> bool { return played_; }

    void play() override { played_ = true; }

    auto currentSystemTime() -> PlayerTime override {
        return currentSystemTime_;
    }

    [[nodiscard]] auto toNanosecondsSystemTime() const
        -> std::vector<player_system_time_type> {
        return toNanosecondsSystemTime_;
    }

    auto nanoseconds(PlayerTime t) -> std::uintmax_t override {
        toNanosecondsSystemTime_.push_back(t.system);
        return nanoseconds_;
    }

    void setSampleRateHz(double x) { sampleRateHz_ = x; }

    auto sampleRateHz() -> double override { return sampleRateHz_; }

    void setChannelDelaySeconds(gsl::index channel, double seconds) override {
        channelDelaySeconds_ = seconds;
        channelDelayed_ = channel;
    }

    [[nodiscard]] auto channelDelaySeconds() const {
        return channelDelaySeconds_;
    }

    [[nodiscard]] auto channelDelayed() const { return channelDelayed_; }

    void clearChannelDelays() override { channelDelaysCleared_ = true; }

    void useFirstChannelOnly() override { usingFirstChannelOnly_ = true; }

    void useSecondChannelOnly() override { usingSecondChannelOnly_ = true; }

    [[nodiscard]] auto usingFirstChannelOnly() const {
        return usingFirstChannelOnly_;
    }

    [[nodiscard]] auto usingSecondChannelOnly() const {
        return usingSecondChannelOnly_;
    }

    [[nodiscard]] auto channelDelaysCleared() const {
        return channelDelaysCleared_;
    }

    void useAllChannels() override { usingAllChannels_ = true; }

    [[nodiscard]] auto usingAllChannels() const { return usingAllChannels_; }

    void throwInvalidAudioFileOnLoad() { throwInvalidAudioFileOnLoad_ = true; }

    void fadeOutComplete() { listener_->fadeOutComplete(); }

    void throwInvalidAudioDeviceWhenDeviceSet() {
        throwInvalidAudioDeviceWhenDeviceSet_ = true;
    }

    void setAudioDevice(std::string s) override {
        device_ = std::move(s);
        if (throwInvalidAudioDeviceWhenDeviceSet_)
            throw av_speech_in_noise::InvalidAudioDevice{};
    }

    [[nodiscard]] auto device() const -> std::string { return device_; }

    [[nodiscard]] auto setDeviceCalled() const -> bool {
        return setDeviceCalled_;
    }

    auto playing() -> bool override { return playing_; }

    void setPlaying() { playing_ = true; }

    void setOutputAudioDeviceDescriptions(std::vector<std::string> v) {
        outputAudioDeviceDescriptions_ = std::move(v);
    }

    [[nodiscard]] auto fadeInCalled() const -> bool { return fadeInCalled_; }

    void fadeIn() override { fadeInCalled_ = true; }

    [[nodiscard]] auto fadeOutCalled() const -> bool { return fadeOutCalled_; }

    void attach(Observer *e) override { listener_ = e; }

    void fadeOut() override { fadeOutCalled_ = true; }

    void loadFile(const LocalUrl &filePath) override {
        filePath_ = filePath.path;
        if (throwInvalidAudioFileOnLoad_)
            throw InvalidAudioFile{};
    }

    auto outputAudioDeviceDescriptions() -> std::vector<std::string> override {
        return outputAudioDeviceDescriptions_;
    }

    [[nodiscard]] auto listener() const -> const Observer * {
        return listener_;
    }

    void fadeInComplete(const AudioSampleTimeWithOffset &t = {}) {
        listener_->fadeInComplete(t);
    }

    [[nodiscard]] auto filePath() const -> std::string { return filePath_; }

    void setDigitalLevel(DigitalLevel x) { digitalLevel_ = x; }

    [[nodiscard]] auto level_dB() const -> double { return level_dB_; }

    auto digitalLevel() -> DigitalLevel override { return digitalLevel_; }

    void apply(LevelAmplification x) override { level_dB_ = x.dB; }

    auto duration() -> Duration override { return Duration{durationSeconds_}; }

    void seekSeconds(double x) override { secondsSeeked_ = x; }

    [[nodiscard]] auto secondsSeeked() const { return secondsSeeked_; }

    auto fadeTime() -> Duration override { return Duration{fadeTimeSeconds_}; }

    void setFadeTimeSeconds(double x) { fadeTimeSeconds_ = x; }

    void setDurationSeconds(double x) { durationSeconds_ = x; }

    void setNanosecondsFromPlayerTime(std::uintmax_t t) { nanoseconds_ = t; }

    void setCurrentSystemTime(PlayerTime t) { currentSystemTime_ = t; }
};
}

#endif
