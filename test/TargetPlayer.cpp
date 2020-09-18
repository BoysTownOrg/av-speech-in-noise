#include "assert-utility.hpp"
#include "AudioReaderStub.hpp"
#include <stimulus-players/TargetPlayerImpl.hpp>
#include <gtest/gtest.h>
#include <cmath>
#include <functional>
#include <utility>

namespace av_speech_in_noise {
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
    Observer *listener_{};
    bool shown_{};
    bool hidden_{};
    bool played_{};
    bool playing_{};
    bool playbackCompletionSubscribedTo_{};
};

class TargetPlayerListenerStub : public TargetPlayer::Observer {
    bool notified_{};

  public:
    void playbackComplete() override { notified_ = true; }

    [[nodiscard]] auto notified() const { return notified_; }
};

class SignalProcessorStub : public SignalProcessor {
  public:
    void initialize(const audio_type &a) override { initializingAudio_ = a; }
    void process(const std::vector<gsl::span<float>> &s) override {
        signal_ = s;
    }
    auto signal() -> std::vector<gsl::span<float>> { return signal_; }
    auto initializingAudio() -> audio_type { return initializingAudio_; }

  private:
    std::vector<gsl::span<float>> signal_;
    audio_type initializingAudio_;
};

class TargetPlayerTests : public ::testing::Test {
  protected:
    std::vector<float> leftChannel{};
    std::vector<float> rightChannel{};
    VideoPlayerStub videoPlayer;
    TargetPlayerListenerStub listener;
    AudioReaderStub audioReader{};
    SignalProcessorStub signalProcessor;
    TargetPlayerImpl player{&videoPlayer, &audioReader, &signalProcessor};
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

    void assertThrowsInvalidAudioFileWhenReaderThrows(
        const std::function<void()> &f) {
        audioReader.throwOnRead();
        try {
            f();
            FAIL() << "Expected av_speech_in_noise::InvalidAudioFile";
        } catch (const InvalidAudioFile &) {
        }
    }
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
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        player_system_time_type{1}, videoPlayer.baseSystemTimePlayedAt());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(2., videoPlayer.secondsDelayedPlayedAt());
}

TEST_F(TargetPlayerTests, durationReturnsDuration) {
    videoPlayer.setDurationSeconds(1);
    EXPECT_EQ(1, player.duration().seconds);
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
    player.loadFile({"a"});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, videoPlayer.filePath());
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

template <typename T>
void assertEqual_(const std::vector<T> &expected, gsl::span<T> actual) {
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(expected.size(), actual.size());
    for (typename gsl::span<T>::size_type i{0}; i < expected.size(); ++i)
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.at(i), at(actual, i));
}

TEST_F(TargetPlayerTests, passesSignalToProcessor) {
    setLeftChannel({1, 2, 3});
    setRightChannel({4, 5, 6});
    fillAudioBufferStereo();
    assertEqual_({1, 2, 3}, signalProcessor.signal().at(0));
    assertEqual_({4, 5, 6}, signalProcessor.signal().at(1));
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
        FAIL() << "Expected av_speech_in_noise::InvalidAudioDevice";
    } catch (const InvalidAudioDevice &) {
    }
}

TEST_F(TargetPlayerTests, audioDevicesReturnsDescriptions) {
    setAudioDeviceDescriptions({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, player.audioDevices());
}

void set(AudioReaderStub &audioReader, audio_type x) {
    audioReader.set(std::move(x));
};

TEST_F(TargetPlayerTests, digitalLevelComputesFirstChannel) {
    set(audioReader, {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        20 * std::log10(std::sqrt((1 * 1 + 2 * 2 + 3 * 3) / 3.F)),
        player.digitalLevel().dBov);
}

void initializeProcessor(TargetPlayerImpl &player, const LocalUrl &url = {}) {
    player.initializeProcessor(url);
}

TEST_F(TargetPlayerTests, initializeProcessorPassesAudioToProcessor) {
    set(audioReader, {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    initializeProcessor(player);
    assertEqual(
        {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, signalProcessor.initializingAudio());
}

#define AV_SPEECH_IN_NOISE_EXPECT_AUDIO_READER_FILE_PATH(                      \
    audioReader, expected)                                                     \
    AV_SPEECH_IN_NOISE_EXPECT_STRING_EQUAL(expected, audioReader.filePath())

TEST_F(TargetPlayerTests, digitalLevelPassesLoadedFileToVideoPlayer) {
    player.loadFile({"a"});
    player.digitalLevel();
    AV_SPEECH_IN_NOISE_EXPECT_AUDIO_READER_FILE_PATH(audioReader, "a");
}

TEST_F(TargetPlayerTests, initializeProcessorPassFileToAudioReader) {
    initializeProcessor(player, {"a"});
    AV_SPEECH_IN_NOISE_EXPECT_AUDIO_READER_FILE_PATH(audioReader, "a");
}

TEST_F(TargetPlayerTests, subscribesToTargetPlaybackCompletionNotification) {
    player.subscribeToPlaybackCompletion();
    EXPECT_TRUE(videoPlayer.playbackCompletionSubscribedTo());
}

TEST_F(TargetPlayerTests,
    digitalLevelThrowsInvalidAudioFileWhenAudioReaderThrows) {
    assertThrowsInvalidAudioFileWhenReaderThrows(
        [&]() { player.digitalLevel(); });
}

TEST_F(TargetPlayerTests,
    initializeProcessorThrowsInvalidAudioFileWhenAudioReaderThrows) {
    assertThrowsInvalidAudioFileWhenReaderThrows(
        [&]() { initializeProcessor(player); });
}
}
}
