#ifndef TESTS_TARGETPLAYERSTUB_HPP_
#define TESTS_TARGETPLAYERSTUB_HPP_

#include "LogString.hpp"
#include <recognition-test/RecognitionTestModel.hpp>
#include <utility>
#include <string>

namespace av_speech_in_noise {
class TargetPlayerStub : public TargetPlayer {
  public:
    void useFirstChannelOnly() override { usingFirstChannelOnly_ = true; }

    void useAllChannels() override { usingAllChannels_ = true; }

    auto usingAllChannels() const { return usingAllChannels_; }

    auto usingFirstChannelOnly() const { return usingFirstChannelOnly_; }

    auto baseSystemTimePlayedAt() const { return baseSystemTimePlayedAt_; }

    void subscribeToPlaybackCompletion() override {
        playbackCompletionSubscribedTo_ = true;
    }

    auto playing() -> bool override { return playing_; }

    auto log() const -> auto & { return log_; }

    auto setDeviceCalled() const { return setDeviceCalled_; }

    void throwInvalidAudioDeviceWhenDeviceSet() {
        throwInvalidAudioDeviceWhenDeviceSet_ = true;
    }

    void setAudioDevice(std::string s) override {
        setDeviceCalled_ = true;
        device_ = std::move(s);
        if (throwInvalidAudioDeviceWhenDeviceSet_)
            throw InvalidAudioDevice{};
    }

    auto device() const { return device_; }

    auto level_dB() const { return level_dB_; }

    void setRms(double x) { rms_ = x; }

    void showVideo() override { videoShown_ = true; }

    auto videoShown() const { return videoShown_; }

    void hideVideo() override { videoHidden_ = true; }

    auto videoHidden() const { return videoHidden_; }

    auto played() const { return played_; }

    void play() override { played_ = true; }

    void playAt(const PlayerTimeWithDelay &t) override {
        baseSystemTimePlayedAt_ = t.playerTime.system;
        secondsDelayedPlayedAt_ = t.delay.seconds;
    }

    auto secondsDelayedPlayedAt() const { return secondsDelayedPlayedAt_; }

    void subscribe(EventListener *listener) override { listener_ = listener; }

    void loadFile(std::string filePath) override {
        addToLog("loadFile ");
        filePath_ = std::move(filePath);
    }

    void addToLog(const std::string &s) { log_.insert(s); }

    auto rms() -> double override {
        addToLog("rms ");
        if (throwInvalidAudioFileOnRms_)
            throw InvalidAudioFile{};
        return rms_;
    }

    void setLevel_dB(double x) override { level_dB_ = x; }

    auto durationSeconds() -> double override { return durationSeconds_; }

    void playbackComplete() { listener_->playbackComplete(); }

    auto listener() const -> const EventListener * { return listener_; }

    auto filePath() const { return filePath_; }

    auto playbackCompletionSubscribedTo() const {
        return playbackCompletionSubscribedTo_;
    }

    void setDurationSeconds(double x) { durationSeconds_ = x; }

    void throwInvalidAudioFileOnRms() { throwInvalidAudioFileOnRms_ = true; }

  private:
    LogString log_;
    std::string filePath_;
    std::string device_;
    double rms_{};
    double level_dB_{};
    double durationSeconds_{};
    double secondsDelayedPlayedAt_{};
    system_time baseSystemTimePlayedAt_{};
    EventListener *listener_{};
    bool played_{};
    bool videoHidden_{};
    bool videoShown_{};
    bool throwInvalidAudioDeviceWhenDeviceSet_{};
    bool throwInvalidAudioFileOnRms_{};
    bool setDeviceCalled_{};
    bool playing_{};
    bool playbackCompletionSubscribedTo_{};
    bool usingAllChannels_{};
    bool usingFirstChannelOnly_{};
};
}

#endif
