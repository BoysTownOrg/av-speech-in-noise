#include "AudioReaderStub.hpp"
#include "assert-utility.hpp"
#include "recognition-test/RecognitionTestModel.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <stimulus-players/TargetPlayerImpl.hpp>

namespace stimulus_players {
namespace {
class VideoPlayerStub : public VideoPlayer {
  public:
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

    void loadFile(std::string f) override { filePath_ = std::move(f); }

    [[nodiscard]] auto filePath() const { return filePath_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const { return hidden_; }

    [[nodiscard]] auto shown() const { return shown_; }

    void setPlaying() { playing_ = true; }

    void setDurationSeconds(double x) { durationSeconds_ = x; }

    void show() override { shown_ = true; }

    void subscribe(EventListener *e) override { listener_ = e; }

    void fillAudioBuffer(const std::vector<gsl::span<float>> &audio) {
        listener_->fillAudioBuffer(audio);
    }

    [[nodiscard]] auto baseSystemTimePlayedAt() const {
        return baseSystemTimePlayedAt_;
    }

    [[nodiscard]] auto secondsDelayedPlayedAt() const {
        return secondsDelayedPlayedAt_;
    }

    void playAt(const av_speech_in_noise::TargetTimeWithDelay &t) override {
        baseSystemTimePlayedAt_ = t.system;
        secondsDelayedPlayedAt_ = t.secondsDelayed;
    }

  private:
    std::vector<std::string> audioDeviceDescriptions_{};
    std::vector<std::vector<float>> audioRead_{};
    std::string filePath_{};
    std::string audioFilePath_{};
    double durationSeconds_{};
    double secondsDelayedPlayedAt_{};
    av_speech_in_noise::system_time baseSystemTimePlayedAt_{};
    int deviceIndex_{};
    EventListener *listener_{};
    bool shown_{};
    bool hidden_{};
    bool played_{};
    bool playing_{};
    bool playbackCompletionSubscribedTo_{};
};

class TargetPlayerListenerStub
    : public av_speech_in_noise::TargetPlayer::EventListener {
    bool notified_{};

  public:
    void playbackComplete() override { notified_ = true; }

    [[nodiscard]] auto notified() const { return notified_; }
};

class TargetPlayerTests : public ::testing::Test {
  protected:
    std::vector<float> leftChannel{};
    std::vector<float> rightChannel{};
    VideoPlayerStub videoPlayer;
    TargetPlayerListenerStub listener;
    AudioReaderStub audioReader{};
    TargetPlayerImpl player{&videoPlayer, &audioReader};
    av_speech_in_noise::TargetTimeWithDelay systemTimeWithDelay{};

    TargetPlayerTests() { player.subscribe(&listener); }

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

    void setLevel_dB(double x) { player.setLevel_dB(x); }

    void setFirstChannelOnly() { player.useFirstChannelOnly(); }

    void useAllChannels() { player.useAllChannels(); }

    void setBaseSystemTimeToPlayAt(av_speech_in_noise::system_time t) {
        systemTimeWithDelay.system = t;
    }

    void setSecondsDelayedToPlayAt(double x) {
        systemTimeWithDelay.secondsDelayed = x;
    }

    void playAt() { player.playAt(systemTimeWithDelay); }
};

TEST_F(TargetPlayerTests, playingWhenVideoPlayerPlaying) {
    videoPlayer.setPlaying();
    EXPECT_TRUE(player.playing());
}

TEST_F(TargetPlayerTests, playPlaysVideo) {
    player.play();
    EXPECT_TRUE(videoPlayer.played());
}

TEST_F(TargetPlayerTests, playAtPlaysVideoAt) {
    setBaseSystemTimeToPlayAt(1);
    setSecondsDelayedToPlayAt(2);
    playAt();
    assertEqual(av_speech_in_noise::system_time{1},
        videoPlayer.baseSystemTimePlayedAt());
    assertEqual(2., videoPlayer.secondsDelayedPlayedAt());
}

TEST_F(TargetPlayerTests, durationReturnsDuration) {
    videoPlayer.setDurationSeconds(1);
    EXPECT_EQ(1, player.durationSeconds());
}

TEST_F(TargetPlayerTests, showVideoShowsVideo) {
    player.showVideo();
    EXPECT_TRUE(videoPlayer.shown());
}

TEST_F(TargetPlayerTests, hideVideoHidesVideo) {
    player.hideVideo();
    EXPECT_TRUE(videoPlayer.hidden());
}

TEST_F(TargetPlayerTests, loadFileLoadsFile) {
    player.loadFile("a");
    assertEqual("a", videoPlayer.filePath());
}

TEST_F(TargetPlayerTests, videoPlaybackCompleteNotifiesSubscriber) {
    videoPlayer.playbackComplete();
    EXPECT_TRUE(listener.notified());
}

TEST_F(TargetPlayerTests, twentydBMultipliesSignalByTen) {
    setLevel_dB(20);
    setLeftChannel({1, 2, 3});
    fillAudioBufferMono();
    assertLeftChannelEquals({10, 20, 30});
}

TEST_F(TargetPlayerTests, twentydBMultipliesSignalByTen_Stereo) {
    setLevel_dB(20);
    setLeftChannel({1, 2, 3});
    setRightChannel({4, 5, 6});
    fillAudioBufferStereo();
    assertLeftChannelEquals({10, 20, 30});
    assertRightChannelEquals({40, 50, 60});
}

TEST_F(TargetPlayerTests, onlyPlayFirstChannel) {
    setFirstChannelOnly();
    setLeftChannel({1, 2, 3});
    setRightChannel({4, 5, 6});
    fillAudioBufferStereo();
    assertLeftChannelEquals({1, 2, 3});
    assertRightChannelEquals({0, 0, 0});
}

TEST_F(TargetPlayerTests, switchBackToAllChannels) {
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

TEST_F(TargetPlayerTests, setAudioDeviceFindsIndex) {
    setAudioDeviceDescriptions({"zeroth", "first", "second", "third"});
    player.setAudioDevice("second");
    EXPECT_EQ(2, videoPlayer.deviceIndex());
}

TEST_F(TargetPlayerTests, setAudioDeviceThrowsInvalidAudioDeviceIfDoesntExist) {
    setAudioDeviceDescriptions({"zeroth", "first", "second"});
    try {
        player.setAudioDevice("third");
        FAIL() << "Expected av_coordinate_response_measure::InvalidAudioDevice";
    } catch (const av_speech_in_noise::InvalidAudioDevice &) {
    }
}

TEST_F(TargetPlayerTests, audioDevicesReturnsDescriptions) {
    setAudioDeviceDescriptions({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, player.audioDevices());
}

TEST_F(TargetPlayerTests, rmsComputesFirstChannel) {
    audioReader.set({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    EXPECT_EQ(std::sqrt((1 * 1 + 2 * 2 + 3 * 3) / 3.F), player.rms());
}

TEST_F(TargetPlayerTests, rmsPassesLoadedFileToVideoPlayer) {
    player.loadFile("a");
    player.rms();
    assertEqual("a", audioReader.filePath());
}

TEST_F(TargetPlayerTests, subscribesToTargetPlaybackCompletionNotification) {
    player.subscribeToPlaybackCompletion();
    EXPECT_TRUE(videoPlayer.playbackCompletionSubscribedTo());
}

TEST_F(TargetPlayerTests, rmsThrowsInvalidAudioFileWhenAudioReaderThrows) {
    audioReader.throwOnRead();
    try {
        player.rms();
        FAIL() << "Expected av_coordinate_response_measure::InvalidAudioFile";
    } catch (const av_speech_in_noise::InvalidAudioFile &) {
    }
}
}
}