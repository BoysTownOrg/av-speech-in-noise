#ifndef TESTS_MASKERPLAYERSTUB_HPP_
#define TESTS_MASKERPLAYERSTUB_HPP_

#include "LogString.hpp"
#include <recognition-test/RecognitionTestModel.hpp>
#include <utility>

namespace av_speech_in_noise {
class MaskerPlayerStub : public MaskerPlayer {
    std::vector<std::string> outputAudioDeviceDescriptions_;
    LogString log_;
    std::string filePath_;
    std::string device_;
    double rms_{};
    double level_dB_{};
    double fadeTimeSeconds_{};
    double durationSeconds_{};
    double secondsSeeked_{};
    double channelDelaySeconds_{};
    double sampleRateHz_{};
    gsl::index channelDelayed_{};
    EventListener *listener_{};
    TargetPlayerSystemTime currentSystemTime_{};
    std::uintmax_t toNanosecondsSystemTime_{};
    std::uintmax_t nanoseconds_{};
    bool fadeInCalled_{};
    bool fadeOutCalled_{};
    bool playing_{};
    bool setDeviceCalled_{};
    bool throwInvalidAudioDeviceWhenDeviceSet_{};
    bool throwInvalidAudioFileOnLoad_{};
    bool usingAllChannels_{};
    bool usingFirstChannelOnly_{};
    bool channelDelaysCleared_{};

  public:
    auto currentSystemTime() -> TargetPlayerSystemTime override {
        return currentSystemTime_;
    }

    auto toNanosecondsSystemTime() const -> player_system_time_type {
        return toNanosecondsSystemTime_;
    }

    auto nanoseconds(PlayerTime t) -> std::uintmax_t override {
        toNanosecondsSystemTime_ = t.system;
        return nanoseconds_;
    }

    void setSampleRateHz(double x) { sampleRateHz_ = x; }

    auto sampleRateHz() -> double override { return sampleRateHz_; }

    void setChannelDelaySeconds(gsl::index channel, double seconds) override {
        channelDelaySeconds_ = seconds;
        channelDelayed_ = channel;
    }

    auto channelDelaySeconds() const { return channelDelaySeconds_; }

    auto channelDelayed() const { return channelDelayed_; }

    void clearChannelDelays() override { channelDelaysCleared_ = true; }

    void useFirstChannelOnly() override { usingFirstChannelOnly_ = true; }

    auto usingFirstChannelOnly() const { return usingFirstChannelOnly_; }

    auto channelDelaysCleared() const { return channelDelaysCleared_; }

    void useAllChannels() override { usingAllChannels_ = true; }

    auto usingAllChannels() const { return usingAllChannels_; }

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

    auto device() const -> std::string { return device_; }

    auto setDeviceCalled() const -> bool { return setDeviceCalled_; }

    auto playing() -> bool override { return playing_; }

    void setPlaying() { playing_ = true; }

    void setOutputAudioDeviceDescriptions(std::vector<std::string> v) {
        outputAudioDeviceDescriptions_ = std::move(v);
    }

    auto fadeInCalled() const -> bool { return fadeInCalled_; }

    void fadeIn() override { fadeInCalled_ = true; }

    auto fadeOutCalled() const -> bool { return fadeOutCalled_; }

    void subscribe(EventListener *e) override { listener_ = e; }

    void fadeOut() override { fadeOutCalled_ = true; }

    void loadFile(std::string filePath) override {
        addToLog("loadFile ");
        filePath_ = std::move(filePath);
        if (throwInvalidAudioFileOnLoad_)
            throw InvalidAudioFile{};
    }

    void addToLog(const std::string &s) { log_.insert(s); }

    auto outputAudioDeviceDescriptions() -> std::vector<std::string> override {
        return outputAudioDeviceDescriptions_;
    }

    auto listener() const -> const EventListener * { return listener_; }

    void fadeInComplete(const AudioSampleTimeWithOffset &t = {}) {
        listener_->fadeInComplete(t);
    }

    auto filePath() const -> std::string { return filePath_; }

    void setRms(double x) { rms_ = x; }

    auto level_dB() const -> double { return level_dB_; }

    auto rms() -> double override {
        addToLog("rms ");
        return rms_;
    }

    void setLevel_dB(double x) override { level_dB_ = x; }

    auto durationSeconds() -> double override { return durationSeconds_; }

    void seekSeconds(double x) override { secondsSeeked_ = x; }

    auto secondsSeeked() const { return secondsSeeked_; }

    auto fadeTimeSeconds() -> double override { return fadeTimeSeconds_; }

    void setFadeTimeSeconds(double x) { fadeTimeSeconds_ = x; }

    void setDurationSeconds(double x) { durationSeconds_ = x; }

    void setNanosecondsFromPlayerTime(std::uintmax_t t) { nanoseconds_ = t; }

    void setCurrentSystemTime(TargetPlayerSystemTime t) { currentSystemTime_ = t; }
};
}

#endif
