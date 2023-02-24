#ifndef TESTS_TARGETPLAYERSTUB_HPP_
#define TESTS_TARGETPLAYERSTUB_HPP_

#include "LogString.hpp"

#include <av-speech-in-noise/core/ITargetPlayer.hpp>

#include <utility>
#include <string>

namespace av_speech_in_noise {
class TargetPlayerStub : public TargetPlayer {
  public:
    auto timesPreRolled() const -> int { return timesPreRolled_; }

    void preRollComplete() { listener_->notifyThatPreRollHasCompleted(); }

    auto preRolling() const -> bool { return preRolling_; }

    void preRoll() override {
        preRolling_ = true;
        ++timesPreRolled_;
    }

    auto timesSetDeviceCalled() const -> int { return timesSetDeviceCalled_; }

    void useFirstChannelOnly() override { usingFirstChannelOnly_ = true; }

    void useAllChannels() override { usingAllChannels_ = true; }

    auto usingAllChannels() const { return usingAllChannels_; }

    auto usingFirstChannelOnly() const { return usingFirstChannelOnly_; }

    auto playing() -> bool override { return playing_; }

    auto log() const -> const std::stringstream & { return log_; }

    auto setDeviceCalled() const { return setDeviceCalled_; }

    void throwInvalidAudioDeviceWhenDeviceSet() {
        throwInvalidAudioDeviceWhenDeviceSet_ = true;
    }

    void setAudioDevice(std::string s) override {
        setDeviceCalled_ = true;
        device_ = std::move(s);
        ++timesSetDeviceCalled_;
        if (throwInvalidAudioDeviceWhenDeviceSet_)
            throw InvalidAudioDevice{};
    }

    auto device() const { return device_; }

    auto level_dB() const { return level_dB_; }

    void setDigitalLevel(DigitalLevel x) { digitalLevel_ = x; }

    void showVideo() override { videoShown_ = true; }

    auto videoShown() const { return videoShown_; }

    void hideVideo() override { videoHidden_ = true; }

    void clearHidden() { videoHidden_ = false; }

    auto videoHidden() const { return videoHidden_; }

    auto played() const { return played_; }

    void play() override { played_ = true; }

    void playAt(const PlayerTimeWithDelay &t) override { timePlayedAt_ = t; }

    void attach(Observer *listener) override { listener_ = listener; }

    void loadFile(const LocalUrl &filePath) override {
        addToLog("loadFile ");
        filePath_ = filePath.path;
    }

    void addToLog(const std::string &s) { insert(log_, s); }

    auto digitalLevel() -> DigitalLevel override {
        addToLog("digitalLevel ");
        if (throwInvalidAudioFileOnDigitalLevel_)
            throw InvalidAudioFile{};
        return digitalLevel_;
    }

    auto levelAmplification() -> LevelAmplification {
        return levelAmplification_;
    }

    void apply(LevelAmplification x) override {
        level_dB_ = x.dB;
        levelAmplification_ = x;
    }

    auto duration() -> Duration override { return Duration{durationSeconds_}; }

    void playbackComplete() { listener_->playbackComplete(); }

    auto listener() const -> const Observer * { return listener_; }

    auto filePath() const { return filePath_; }

    void setDurationSeconds(double x) { durationSeconds_ = x; }

    void throwInvalidAudioFileOnDigitalLevel() {
        throwInvalidAudioFileOnDigitalLevel_ = true;
    }

    auto timePlayedAt() -> PlayerTimeWithDelay { return timePlayedAt_; }

  private:
    std::stringstream log_;
    std::string filePath_;
    std::string device_;
    PlayerTimeWithDelay timePlayedAt_{};
    DigitalLevel digitalLevel_{};
    LevelAmplification levelAmplification_{};
    double level_dB_{};
    double durationSeconds_{};
    Observer *listener_{};
    int timesSetDeviceCalled_{};
    int timesPreRolled_{};
    bool played_{};
    bool videoHidden_{};
    bool videoShown_{};
    bool throwInvalidAudioDeviceWhenDeviceSet_{};
    bool throwInvalidAudioFileOnDigitalLevel_{};
    bool setDeviceCalled_{};
    bool playing_{};
    bool usingAllChannels_{};
    bool usingFirstChannelOnly_{};
    bool preRolling_{};
};
}

#endif
