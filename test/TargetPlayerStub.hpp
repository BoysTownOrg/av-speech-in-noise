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

    void subscribeToPlaybackCompletion() override {
        playbackCompletionSubscribedTo_ = true;
    }

    auto playing() -> bool override { return playing_; }

    auto log() const -> const std::stringstream & { return log_; }

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

    void setDigitalLevel(DigitalLevel x) { digitalLevel_ = x; }

    void showVideo() override { videoShown_ = true; }

    auto videoShown() const { return videoShown_; }

    void hideVideo() override { videoHidden_ = true; }

    auto videoHidden() const { return videoHidden_; }

    auto played() const { return played_; }

    void play() override { played_ = true; }

    void playAt(const PlayerTimeWithDelay &t) override { timePlayedAt_ = t; }

    void subscribe(EventListener *listener) override { listener_ = listener; }

    void loadFile(const LocalUrl &filePath) override {
        addToLog("loadFile ");
        filePath_ = filePath.path;
    }

    void addToLog(const std::string &s) { insert(log_, s); }

    auto digitalLevel() -> DigitalLevel override {
        addToLog("digitalLevel ");
        if (throwInvalidAudioFileOnRms_)
            throw InvalidAudioFile{};
        return digitalLevel_;
    }

    void apply(LevelAmplification x) override { level_dB_ = x.dB; }

    auto duration() -> Duration override { return Duration{durationSeconds_}; }

    void playbackComplete() { listener_->playbackComplete(); }

    auto listener() const -> const EventListener * { return listener_; }

    auto filePath() const { return filePath_; }

    auto playbackCompletionSubscribedTo() const {
        return playbackCompletionSubscribedTo_;
    }

    void setDurationSeconds(double x) { durationSeconds_ = x; }

    void throwInvalidAudioFileOnRms() { throwInvalidAudioFileOnRms_ = true; }

    auto timePlayedAt() -> PlayerTimeWithDelay { return timePlayedAt_; }

  private:
    std::stringstream log_;
    std::string filePath_;
    std::string device_;
    PlayerTimeWithDelay timePlayedAt_{};
    DigitalLevel digitalLevel_{};
    double level_dB_{};
    double durationSeconds_{};
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
