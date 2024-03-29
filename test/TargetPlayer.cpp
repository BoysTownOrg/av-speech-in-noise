#include "AudioReaderStub.hpp"
#include "assert-utility.hpp"
#include <av-speech-in-noise/player/TargetPlayerImpl.hpp>
#include <gtest/gtest.h>
#include <cmath>

namespace av_speech_in_noise {
namespace {
class VideoPlayerStub : public VideoPlayer {
  public:
    void preRollComplete() { listener_->notifyThatPreRollHasCompleted(); }

    [[nodiscard]] auto preRolled() const -> bool { return preRolled_; }

    void preRoll() override { preRolled_ = true; }

    auto durationSeconds() -> double override { return durationSeconds_; }

    void subscribeToPlaybackCompletion() override {
        playbackCompletionSubscribedTo_ = true;
    }

    auto playing() -> bool override { return playing_; }

    [[nodiscard]] auto playbackCompletionSubscribedTo() const {
        return playbackCompletionSubscribedTo_;
    }

    auto deviceCount() -> int override {
        return gsl::narrow<int>(audioDeviceDescriptions_.size());
    }

    auto deviceDescription(int index) -> std::string override {
        return audioDeviceDescriptions_.at(index);
    }

    void setAudioDeviceDescriptions(std::vector<std::string> v) {
        audioDeviceDescriptions_ = std::move(v);
    }

    void playbackComplete() { listener_->playbackComplete(); }

    void setDevice(int index) override { deviceIndex_ = index; }

    [[nodiscard]] auto deviceIndex() const { return deviceIndex_; }

    void play() override { played_ = true; }

    [[nodiscard]] auto played() const { return played_; }

    void loadFile(std::string f, RationalNumber videoScale) override {
        filePath_ = std::move(f);
        videoScale_ = videoScale;
    }

    [[nodiscard]] auto filePath() const { return filePath_; }

    auto videoScale() { return videoScale_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const { return hidden_; }

    [[nodiscard]] auto shown() const { return shown_; }

    void setPlaying() { playing_ = true; }

    void setDurationSeconds(double x) { durationSeconds_ = x; }

    void show() override { shown_ = true; }

    void attach(Observer *e) override { listener_ = e; }

    void fillAudioBuffer(const std::vector<gsl::span<float>> &audio) {
        listener_->fillAudioBuffer(audio);
    }

    [[nodiscard]] auto baseSystemTimePlayedAt() const {
        return baseSystemTimePlayedAt_;
    }

    [[nodiscard]] auto secondsDelayedPlayedAt() const {
        return secondsDelayedPlayedAt_;
    }

    void playAt(const PlayerTimeWithDelay &t) override {
        baseSystemTimePlayedAt_ = t.playerTime.system;
        secondsDelayedPlayedAt_ = t.delay.seconds;
    }

  private:
    std::vector<std::string> audioDeviceDescriptions_{};
    std::vector<std::vector<float>> audioRead_{};
    std::string filePath_{};
    std::string audioFilePath_{};
    double durationSeconds_{};
    double secondsDelayedPlayedAt_{};
    player_system_time_type baseSystemTimePlayedAt_{};
    int deviceIndex_{};
    RationalNumber videoScale_{};
    Observer *listener_{};
    bool shown_{};
    bool hidden_{};
    bool played_{};
    bool playing_{};
    bool playbackCompletionSubscribedTo_{};
    bool preRolled_{};
};

class TargetPlayerListenerStub : public TargetPlayer::Observer {
  public:
    void playbackComplete() override {
        notifiedThatPlaybackHasCompleted_ = true;
    }

    void notifyThatPreRollHasCompleted() override {
        notifiedThatPreRollHasCompleted_ = true;
    }

    [[nodiscard]] auto notifiedThatPlaybackHasCompleted() const -> bool {
        return notifiedThatPlaybackHasCompleted_;
    }

    [[nodiscard]] auto notifiedThatPreRollHasCompleted() const -> bool {
        return notifiedThatPreRollHasCompleted_;
    }

  private:
    bool notifiedThatPlaybackHasCompleted_{};
    bool notifiedThatPreRollHasCompleted_{};
};

class TargetPlayerTests : public ::testing::Test {
  protected:
    std::vector<float> leftChannel{};
    std::vector<float> rightChannel{};
    VideoPlayerStub videoPlayer;
    TargetPlayerListenerStub listener;
    AudioReaderStub audioReader{};
    TargetPlayerImpl player{&videoPlayer, &audioReader};
    PlayerTimeWithDelay systemTimeWithDelay{};

    TargetPlayerTests() { player.attach(&listener); }

    void fillAudioBuffer(const std::vector<gsl::span<float>> &x) {
        videoPlayer.fillAudioBuffer(x);
    }

    void fillAudioBufferMono() { fillAudioBuffer({leftChannel}); }

    void fillAudioBufferStereo() {
        fillAudioBuffer({leftChannel, rightChannel});
    }

    void setAudioDeviceDescriptions(std::vector<std::string> v) {
        videoPlayer.setAudioDeviceDescriptions(std::move(v));
    }

    void setLeftChannel(std::vector<float> x) { leftChannel = std::move(x); }

    void setRightChannel(std::vector<float> x) { rightChannel = std::move(x); }

    void assertLeftChannelEquals(const std::vector<float> &x) {
        assertEqual(x, leftChannel);
    }

    void assertRightChannelEquals(const std::vector<float> &x) {
        assertEqual(x, rightChannel);
    }

    void setLevel_dB(double x) { player.apply(LevelAmplification{x}); }

    void setFirstChannelOnly() { player.useFirstChannelOnly(); }

    void useAllChannels() { player.useAllChannels(); }

    void setBaseSystemTimeToPlayAt(player_system_time_type t) {
        systemTimeWithDelay.playerTime.system = t;
    }

    void setSecondsDelayedToPlayAt(double x) {
        systemTimeWithDelay.delay.seconds = x;
    }

    void playAt() { player.playAt(systemTimeWithDelay); }
};

#define TARGET_PLAYER_TEST(a) TEST_F(TargetPlayerTests, a)

TARGET_PLAYER_TEST(playingWhenVideoPlayerPlaying) {
    videoPlayer.setPlaying();
    EXPECT_TRUE(player.playing());
}

TARGET_PLAYER_TEST(playPlaysVideo) {
    player.play();
    EXPECT_TRUE(videoPlayer.played());
}

TARGET_PLAYER_TEST(playAtPlaysVideoAt) {
    setBaseSystemTimeToPlayAt(1);
    setSecondsDelayedToPlayAt(2);
    playAt();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        player_system_time_type{1}, videoPlayer.baseSystemTimePlayedAt());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(2., videoPlayer.secondsDelayedPlayedAt());
}

TARGET_PLAYER_TEST(durationReturnsDuration) {
    videoPlayer.setDurationSeconds(1);
    EXPECT_EQ(1, player.duration().seconds);
}

TARGET_PLAYER_TEST(showVideoShowsVideo) {
    player.showVideo();
    EXPECT_TRUE(videoPlayer.shown());
}

TARGET_PLAYER_TEST(preRollPreRollsVideo) {
    player.preRoll();
    EXPECT_TRUE(videoPlayer.preRolled());
}

TARGET_PLAYER_TEST(hideVideoHidesVideo) {
    player.hideVideo();
    EXPECT_TRUE(videoPlayer.hidden());
}

TARGET_PLAYER_TEST(loadFileLoadsFile) {
    player.loadFile({"a"}, RationalNumber{2, 3});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, videoPlayer.filePath());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(2, videoPlayer.videoScale().numerator);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(3, videoPlayer.videoScale().denominator);
}

TARGET_PLAYER_TEST(videoPlaybackCompleteNotifiesSubscriber) {
    videoPlayer.playbackComplete();
    EXPECT_TRUE(listener.notifiedThatPlaybackHasCompleted());
}

TARGET_PLAYER_TEST(videoPreRollCompleteNotifiesSubscriber) {
    videoPlayer.preRollComplete();
    EXPECT_TRUE(listener.notifiedThatPreRollHasCompleted());
}

TARGET_PLAYER_TEST(twentydBMultipliesSignalByTen) {
    setLevel_dB(20);
    setLeftChannel({1, 2, 3});
    fillAudioBufferMono();
    assertLeftChannelEquals({10, 20, 30});
}

TARGET_PLAYER_TEST(twentydBMultipliesSignalByTen_Stereo) {
    setLevel_dB(20);
    setLeftChannel({1, 2, 3});
    setRightChannel({4, 5, 6});
    fillAudioBufferStereo();
    assertLeftChannelEquals({10, 20, 30});
    assertRightChannelEquals({40, 50, 60});
}

TARGET_PLAYER_TEST(onlyPlayFirstChannel) {
    setFirstChannelOnly();
    setLeftChannel({1, 2, 3});
    setRightChannel({4, 5, 6});
    fillAudioBufferStereo();
    assertLeftChannelEquals({1, 2, 3});
    assertRightChannelEquals({0, 0, 0});
}

TARGET_PLAYER_TEST(switchBackToAllChannels) {
    setFirstChannelOnly();
    setLeftChannel({1, 2, 3});
    setRightChannel({7, 8, 9});
    fillAudioBufferStereo();
    useAllChannels();
    setLeftChannel({4, 5, 6});
    setRightChannel({10, 11, 12});
    fillAudioBufferStereo();
    assertLeftChannelEquals({4, 5, 6});
    assertRightChannelEquals({10, 11, 12});
}

TARGET_PLAYER_TEST(setAudioDeviceFindsIndex) {
    setAudioDeviceDescriptions({"zeroth", "first", "second", "third"});
    player.setAudioDevice("second");
    EXPECT_EQ(2, videoPlayer.deviceIndex());
}

TARGET_PLAYER_TEST(setAudioDeviceThrowsInvalidAudioDeviceIfDoesntExist) {
    setAudioDeviceDescriptions({"zeroth", "first", "second"});
    try {
        player.setAudioDevice("third");
        FAIL() << "Expected av_coordinate_response_measure::InvalidAudioDevice";
    } catch (const InvalidAudioDevice &) {
    }
}

TARGET_PLAYER_TEST(audioDevicesReturnsDescriptions) {
    setAudioDeviceDescriptions({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, player.audioDevices());
}

TARGET_PLAYER_TEST(digitalLevelComputesFirstChannel) {
    audioReader.set({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        20 * std::log10(std::sqrt((1 * 1 + 2 * 2 + 3 * 3) / 3.F)),
        player.digitalLevel().dBov);
}

TARGET_PLAYER_TEST(digitalLevelPassesLoadedFileToVideoPlayer) {
    player.loadFile({"a"}, {});
    player.digitalLevel();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, audioReader.filePath());
}

TARGET_PLAYER_TEST(subscribesToTargetPlaybackCompletionNotification) {
    player.subscribeToPlaybackCompletion();
    EXPECT_TRUE(videoPlayer.playbackCompletionSubscribedTo());
}

TARGET_PLAYER_TEST(digitalLevelThrowsInvalidAudioFileWhenAudioReaderThrows) {
    audioReader.throwOnRead();
    try {
        player.digitalLevel();
        FAIL() << "Expected av_coordinate_response_measure::InvalidAudioFile";
    } catch (const InvalidAudioFile &) {
    }
}
}
}
