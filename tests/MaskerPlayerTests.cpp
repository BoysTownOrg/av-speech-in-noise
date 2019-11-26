#include "assert-utility.h"
#include "AudioReaderStub.h"
#include <stimulus-players/MaskerPlayerImpl.hpp>
#include <gtest/gtest.h>
#include <cmath>

namespace {
class AudioPlayerStub : public stimulus_players::AudioPlayer {
  public:
    void seekSeconds(double x) override { secondsSeeked_ = x; }

    auto durationSeconds() -> double override { return durationSeconds_; }

    auto outputDevice(int index) -> bool override {
        return outputDevices[index];
    }

    void setAudioDeviceDescriptions(std::vector<std::string> v) {
        audioDeviceDescriptions_ = std::move(v);
    }

    void setAsOutputDevice(int index) { outputDevices[index] = true; }

    void stop() override { stopped_ = true; }

    [[nodiscard]] auto stopped() const { return stopped_; }

    auto sampleRateHz() -> double override { return sampleRateHz_; }

    void setSampleRateHz(double x) { sampleRateHz_ = x; }

    void setPlaying() { playing_ = true; }

    void setDurationSeconds(double x) { durationSeconds_ = x; }

    auto playing() -> bool override { return playing_; }

    void loadFile(std::string s) override { filePath_ = std::move(s); }

    void setDevice(int index) override { deviceIndex_ = index; }

    auto deviceCount() -> int override {
        return gsl::narrow<int>(audioDeviceDescriptions_.size());
    }

    auto deviceDescription(int index) -> std::string override {
        deviceDescriptionDeviceIndex_ = index;
        return audioDeviceDescriptions_.at(index);
    }

    void play() override { played_ = true; }

    void subscribe(EventListener *listener) override { listener_ = listener; }

    [[nodiscard]] auto filePath() const { return filePath_; }

    [[nodiscard]] auto secondsSeeked() const { return secondsSeeked_; }

    [[nodiscard]] auto deviceIndex() const { return deviceIndex_; }

    [[nodiscard]] auto played() const { return played_; }

    void fillAudioBuffer(const std::vector<gsl::span<float>> &audio) {
        listener_->fillAudioBuffer(audio);
    }

  private:
    std::vector<std::vector<float>> audioRead_;
    std::vector<std::string> audioDeviceDescriptions_{10};
    std::string filePath_;
    std::string deviceDescription_;
    std::string audioFilePath_;
    std::map<int, bool> outputDevices;
    double sampleRateHz_{};
    double durationSeconds_{};
    double secondsSeeked_{};
    int deviceIndex_{};
    int deviceDescriptionDeviceIndex_{};
    EventListener *listener_{};
    bool playing_{};
    bool played_{};
    bool stopped_{};
};

class MaskerPlayerListenerStub
    : public av_speech_in_noise::MaskerPlayer::EventListener {
  public:
    void fadeInComplete() override {
        fadeInCompleted_ = true;
        ++fadeInCompletions_;
    }

    void fadeOutComplete() override {
        ++fadeOutCompletions_;
        fadeOutCompleted_ = true;
    }

    [[nodiscard]] auto fadeInCompleted() const { return fadeInCompleted_; }

    [[nodiscard]] auto fadeOutCompleted() const { return fadeOutCompleted_; }

    [[nodiscard]] auto fadeInCompletions() const { return fadeInCompletions_; }

    [[nodiscard]] auto fadeOutCompletions() const {
        return fadeOutCompletions_;
    }

  private:
    int fadeInCompletions_{};
    int fadeOutCompletions_{};
    bool fadeInCompleted_{};
    bool fadeOutCompleted_{};
};

template <typename T> class VectorFacade {
  public:
    explicit VectorFacade(std::vector<T> v) : v{std::move(v)} {}

    auto elementWiseProduct(std::vector<T> y) -> std::vector<T> {
        std::vector<T> product;
        std::transform(v.begin(), v.end(), y.begin(),
            std::back_inserter(product), std::multiplies<T>());
        return product;
    }

    auto subvector(int b, int e) -> VectorFacade<T> {
        return VectorFacade<T>{{v.begin() + b, v.begin() + e}};
    }

  private:
    std::vector<T> v;
};

class TimerStub : public stimulus_players::Timer {
  public:
    void scheduleCallbackAfterSeconds(double) override {
        callbackScheduled_ = true;
    }

    [[nodiscard]] auto callbackScheduled() const { return callbackScheduled_; }

    void clearCallbackCount() { callbackScheduled_ = false; }

    void callback() { listener_->callback(); }

    void subscribe(EventListener *listener) override { listener_ = listener; }

  private:
    EventListener *listener_{};
    bool callbackScheduled_{};
};

auto halfHannWindow(int length) -> std::vector<float> {
    auto N = 2 * length - 1;
    const auto pi = std::acos(-1);
    std::vector<float> window;
    for (int n = 0; n < length; ++n)
        window.push_back((1 - std::cos((2 * pi * n) / (N - 1))) / 2);
    return window;
}

auto reverse(std::vector<float> x) {
    std::reverse(x.begin(), x.end());
    return x;
}

auto backHalfHannWindow(int length) -> std::vector<float> {
    return reverse(halfHannWindow(length));
}

auto mToN(int M, int N) -> std::vector<float> {
    if (M > N)
        return reverse(mToN(N, M));
    std::vector<float> result(N - M + 1);
    std::iota(result.begin(), result.end(), M);
    return result;
}

auto oneToN(int N) -> std::vector<float> { return mToN(1, N); }

auto NtoOne(int N) -> std::vector<float> { return reverse(oneToN(N)); }

class MaskerPlayerTests : public ::testing::Test {
  protected:
    AudioPlayerStub audioPlayer;
    MaskerPlayerListenerStub listener;
    stimulus_players::tests::AudioReaderStub audioReader;
    TimerStub timer;
    stimulus_players::MaskerPlayerImpl player{
        &audioPlayer, &audioReader, &timer};
    std::vector<float> leftChannel;
    std::vector<float> rightChannel;

    MaskerPlayerTests() { player.subscribe(&listener); }

    void fillAudioBuffer(const std::vector<gsl::span<float>> &audio) {
        audioPlayer.fillAudioBuffer(audio);
    }

    void fillAudioBufferMono() { fillAudioBuffer({leftChannel}); }

    void fillAudioBufferStereo() {
        fillAudioBuffer({leftChannel, rightChannel});
    }

    void setAudioDeviceDescriptions(std::vector<std::string> v) {
        audioPlayer.setAudioDeviceDescriptions(std::move(v));
    }

    void setAsOutputDevice(int i) { audioPlayer.setAsOutputDevice(i); }

    void fadeInToFullLevel() {
        completeOneFadeCycle(&MaskerPlayerTests::fadeIn);
    }

    void fadeOutToSilence() {
        completeOneFadeCycle(&MaskerPlayerTests::fadeOut);
    }

    void completeOneFadeCycle(void (MaskerPlayerTests::*fade)()) {
        setFadeInOutSeconds(2);
        setSampleRateHz(3);
        (this->*fade)();
        resizeChannels(2 * 3 + 1);
        fillAudioBufferMono();
    }

    void fadeIn() { player.fadeIn(); }

    void fadeOut() { player.fadeOut(); }

    void resizeChannels(int n) {
        leftChannel.resize(n);
        rightChannel.resize(n);
    }

    void timerCallback() { timer.callback(); }

    void assertCallbackScheduled() { assertTrue(callbackScheduled()); }

    auto callbackScheduled() -> bool { return timer.callbackScheduled(); }

    void assertCallbackNotScheduled() { assertFalse(callbackScheduled()); }

    void setAudio(std::vector<std::vector<float>> x) {
        audioReader.set(std::move(x));
    }

    void assertFillingLeftChannelMultipliesBy_Buffered(
        VectorFacade<float> multiplicand, int buffers, int framesPerBuffer) {
        setAudio({oneToN(buffers * framesPerBuffer)});
        loadFile();
        for (int i = 0; i < buffers; ++i) {
            auto offset = i * framesPerBuffer;
            assertFillingLeftChannelMultipliesBy(
                multiplicand.subvector(offset, offset + framesPerBuffer),
                mToN(offset + 1, offset + framesPerBuffer));
        }
    }

    void assertFillingStereoChannelsMultipliesBy_Buffered(
        VectorFacade<float> multiplicand, int buffers, int framesPerBuffer) {
        setAudio({oneToN(buffers * framesPerBuffer),
            NtoOne(buffers * framesPerBuffer)});
        loadFile();
        for (int i = 0; i < buffers; ++i) {
            auto offset = i * framesPerBuffer;
            assertFillingStereoChannelsMultipliesBy(
                multiplicand.subvector(offset, offset + framesPerBuffer),
                mToN(offset + 1, offset + framesPerBuffer),
                mToN(buffers * framesPerBuffer - offset,
                    (buffers - 1) * framesPerBuffer - offset + 1));
        }
    }

    void matchRightChannelSize(const std::vector<float> &other) {
        rightChannel.resize(other.size());
    }

    void matchLeftChannelSize(const std::vector<float> &other) {
        leftChannel.resize(other.size());
    }

    void assertFillingLeftChannelMultipliesBy(VectorFacade<float> multiplicand,
        const std::vector<float> &multiplier) {
        matchLeftChannelSize(multiplier);
        fillAudioBufferMono();
        assertLeftChannelEquals(multiplicand.elementWiseProduct(multiplier));
    }

    void assertLeftChannelEquals(const std::vector<float> &x) {
        assertChannelEqual(leftChannel, x);
    }

    static void assertChannelEqual(
        const std::vector<float> &channel, const std::vector<float> &x) {
        assertEqual(x, channel, 1e-6F);
    }

    void assertFillingStereoChannelsMultipliesBy(
        VectorFacade<float> multiplicand,
        const std::vector<float> &leftMultiplier,
        const std::vector<float> &rightMultiplier) {
        matchLeftChannelSize(leftMultiplier);
        matchRightChannelSize(rightMultiplier);
        fillAudioBufferStereo();
        assertLeftChannelEquals(
            multiplicand.elementWiseProduct(leftMultiplier));
        assertRightChannelEquals(
            multiplicand.elementWiseProduct(rightMultiplier));
    }

    void assertRightChannelEquals(const std::vector<float> &x) {
        assertChannelEqual(rightChannel, x);
    }

    void assertFadeInNotCompletedAfterMonoFill() {
        callbackAfterMonoFill();
        assertFalse(listener.fadeInCompleted());
    }

    void callbackAfterMonoFill() {
        fillAudioBufferMono();
        timerCallback();
    }

    void assertFadeInCompletedAfterMonoFill() {
        callbackAfterMonoFill();
        assertTrue(listener.fadeInCompleted());
    }

    void assertFadeOutNotCompletedAfterMonoFill() {
        callbackAfterMonoFill();
        assertFalse(fadeOutCompleted());
    }

    auto fadeOutCompleted() -> bool { return listener.fadeOutCompleted(); }

    void assertFadeOutCompletedAfterMonoFill() {
        callbackAfterMonoFill();
        assertTrue(fadeOutCompleted());
    }

    void fadeInCompletely() {
        fadeInToFullLevel();
        timerCallback();
    }

    void clearCallbackCount() { timer.clearCallbackCount(); }

    void setAudioDevice(std::string s) { player.setAudioDevice(std::move(s)); }

    void assertTimerCallbackDoesNotScheduleAdditionalCallback() {
        assertCallDoesNotScheduleAdditionalCallback(
            &MaskerPlayerTests::timerCallback);
    }

    void assertCallDoesNotScheduleAdditionalCallback(
        void (MaskerPlayerTests::*f)()) {
        clearCallbackCount();
        (this->*f)();
        assertCallbackNotScheduled();
    }

    void assertFadeInDoesNotScheduleAdditionalCallback() {
        assertCallDoesNotScheduleAdditionalCallback(&MaskerPlayerTests::fadeIn);
    }

    void assertFadeOutDoesNotScheduleAdditionalCallback() {
        assertCallDoesNotScheduleAdditionalCallback(
            &MaskerPlayerTests::fadeOut);
    }

    void assertFadeInSchedulesCallback() {
        clearCallbackCount();
        fadeIn();
        assertCallbackScheduled();
    }

    auto playerStopped() -> bool { return audioPlayer.stopped(); }

    void fadeOutCompletely() {
        fadeOutToSilence();
        timerCallback();
    }

    void setFadeInOutSeconds(double x) { player.setFadeInOutSeconds(x); }

    void setSampleRateHz(double x) { audioPlayer.setSampleRateHz(x); }

    void loadFile(std::string s = {}) { player.loadFile(std::move(s)); }
};

TEST_F(MaskerPlayerTests, playingWhenVideoPlayerPlaying) {
    audioPlayer.setPlaying();
    assertTrue(player.playing());
}

TEST_F(MaskerPlayerTests, durationReturnsDuration) {
    setAudio({{1, 2, 3, 4, 5, 6}});
    loadFile();
    setSampleRateHz(3);
    assertEqual(2., player.durationSeconds());
}

TEST_F(MaskerPlayerTests, seekSeeksAudioPlayer) {
    player.seekSeconds(1);
    assertEqual(1., audioPlayer.secondsSeeked());
    FAIL();
}

TEST_F(MaskerPlayerTests, fadeTimeReturnsFadeTime) {
    player.setFadeInOutSeconds(1);
    assertEqual(1., player.fadeTimeSeconds());
}

TEST_F(MaskerPlayerTests, loadFileLoadsAudioFile) {
    loadFile("a");
    assertEqual("a", audioPlayer.filePath());
}

TEST_F(MaskerPlayerTests, fadeInPlaysVideoPlayer) {
    fadeIn();
    assertTrue(audioPlayer.played());
}

TEST_F(MaskerPlayerTests, twentydBMultipliesSignalByTen) {
    player.setLevel_dB(20);
    leftChannel = {1, 2, 3};
    fillAudioBufferMono();
    assertEqual({10, 20, 30}, leftChannel);
}

TEST_F(MaskerPlayerTests, loadFileResetsSampleIndex) {
    player.setLevel_dB(20);
    audioReader.set({oneToN(1)});
    loadFile();
    leftChannel.resize(1);
    fillAudioBufferMono();
    audioReader.set({oneToN(3)});
    loadFile();
    leftChannel.resize(3);
    fillAudioBufferMono();
    assertEqual({10, 20, 30}, leftChannel);
}

TEST_F(MaskerPlayerTests, fillAudioBufferWraps) {
    player.setLevel_dB(20);
    audioReader.set({oneToN(3)});
    loadFile();
    leftChannel.resize(4);
    fillAudioBufferMono();
    assertEqual({10, 20, 30, 10}, leftChannel);
}

TEST_F(MaskerPlayerTests, fadesInAccordingToHannFunctionMultipleFills) {
    // For this test:
    // halfWindowLength is determined by fade time and sample rate...
    // but must be divisible by framesPerBuffer.
    setFadeInOutSeconds(3);
    setSampleRateHz(5);
    auto halfWindowLength = 3 * 5 + 1;
    auto framesPerBuffer = 4;

    fadeIn();
    assertFillingLeftChannelMultipliesBy_Buffered(
        VectorFacade<float>{halfHannWindow(halfWindowLength)},
        halfWindowLength / framesPerBuffer, framesPerBuffer);
}

TEST_F(MaskerPlayerTests, fadesInAccordingToHannFunctionOneFill) {
    setFadeInOutSeconds(2);
    setSampleRateHz(3);
    auto halfWindowLength = 2 * 3 + 1;

    audioReader.set({oneToN(halfWindowLength)});
    loadFile();
    fadeIn();
    assertFillingLeftChannelMultipliesBy(
        VectorFacade<float>{halfHannWindow(halfWindowLength)},
        oneToN(halfWindowLength));
}

TEST_F(MaskerPlayerTests, fadesInAccordingToHannFunctionStereoMultipleFills) {
    // For this test:
    // halfWindowLength is determined by fade time and sample rate...
    // but must be divisible by framesPerBuffer.
    setFadeInOutSeconds(3);
    setSampleRateHz(5);
    auto halfWindowLength = 3 * 5 + 1;
    auto framesPerBuffer = 4;

    fadeIn();
    assertFillingStereoChannelsMultipliesBy_Buffered(
        VectorFacade<float>{halfHannWindow(halfWindowLength)},
        halfWindowLength / framesPerBuffer, framesPerBuffer);
}

TEST_F(MaskerPlayerTests, fadesInAccordingToHannFunctionStereoOneFill) {
    setFadeInOutSeconds(2);
    setSampleRateHz(3);
    auto halfWindowLength = 2 * 3 + 1;

    audioReader.set({oneToN(halfWindowLength), NtoOne(halfWindowLength)});
    loadFile();
    fadeIn();
    assertFillingStereoChannelsMultipliesBy(
        VectorFacade<float>{halfHannWindow(halfWindowLength)},
        oneToN(halfWindowLength), NtoOne(halfWindowLength));
}

TEST_F(MaskerPlayerTests, steadyLevelFollowingFadeIn) {
    fadeInToFullLevel();

    leftChannel = {1, 2, 3};
    fillAudioBufferMono();
    assertEqual({1, 2, 3}, leftChannel);
}

TEST_F(MaskerPlayerTests, fadesOutAccordingToHannFunctionMultipleFills) {
    // For this test:
    // halfWindowLength is determined by fade time and sample rate...
    // but must be divisible by framesPerBuffer.
    fadeInCompletely();
    setFadeInOutSeconds(3);
    setSampleRateHz(5);
    auto halfWindowLength = 3 * 5 + 1;
    auto framesPerBuffer = 4;

    fadeOut();
    assertFillingLeftChannelMultipliesBy_Buffered(
        VectorFacade<float>{backHalfHannWindow(halfWindowLength)},
        halfWindowLength / framesPerBuffer, framesPerBuffer);
}

TEST_F(MaskerPlayerTests, fadesOutAccordingToHannFunctionOneFill) {
    fadeInCompletely();
    setFadeInOutSeconds(2);
    setSampleRateHz(3);
    auto halfWindowLength = 2 * 3 + 1;

    audioReader.set({oneToN(halfWindowLength)});
    loadFile();
    fadeOut();
    assertFillingLeftChannelMultipliesBy(
        VectorFacade<float>{backHalfHannWindow(halfWindowLength)},
        oneToN(halfWindowLength));
}

TEST_F(MaskerPlayerTests, steadyLevelFollowingFadeOut) {
    fadeInCompletely();
    fadeOutToSilence();

    leftChannel = {1, 2, 3};
    fillAudioBufferMono();
    assertEqual({0, 0, 0}, leftChannel, 1e-15F);
}

TEST_F(MaskerPlayerTests, fadeInCompleteOnlyAfterFadeTime) {
    setFadeInOutSeconds(3);
    setSampleRateHz(4);

    fadeIn();
    resizeChannels(1);
    for (int i = 0; i < 3 * 4; ++i)
        assertFadeInNotCompletedAfterMonoFill();
    assertFadeInCompletedAfterMonoFill();
}

TEST_F(MaskerPlayerTests, observerNotifiedOnceForFadeIn) {
    fadeInCompletely();
    assertEqual(1, listener.fadeInCompletions());
    callbackAfterMonoFill();
    assertEqual(1, listener.fadeInCompletions());
}

TEST_F(MaskerPlayerTests, fadeOutCompleteOnlyAfterFadeTime) {
    fadeInCompletely();
    setFadeInOutSeconds(3);
    setSampleRateHz(4);

    fadeOut();
    resizeChannels(1);
    for (int i = 0; i < 3 * 4; ++i)
        assertFadeOutNotCompletedAfterMonoFill();
    assertFadeOutCompletedAfterMonoFill();
}

TEST_F(MaskerPlayerTests, observerNotifiedOnceForFadeOut) {
    fadeInCompletely();

    fadeOutCompletely();
    assertEqual(1, listener.fadeOutCompletions());
    callbackAfterMonoFill();
    assertEqual(1, listener.fadeOutCompletions());
}

TEST_F(MaskerPlayerTests, audioPlayerStoppedOnlyAtEndOfFadeOutTime) {
    fadeInCompletely();
    setFadeInOutSeconds(3);
    setSampleRateHz(4);

    fadeOut();
    resizeChannels(1);
    for (int i = 0; i < 3 * 4; ++i) {
        callbackAfterMonoFill();
        assertFalse(playerStopped());
    }
    callbackAfterMonoFill();
    assertTrue(playerStopped());
}

TEST_F(MaskerPlayerTests, fadeInSchedulesCallback) {
    assertFadeInSchedulesCallback();
}

TEST_F(MaskerPlayerTests, fadeInTwiceDoesNotScheduleAdditionalCallback) {
    fadeIn();
    assertFadeInDoesNotScheduleAdditionalCallback();
}

TEST_F(MaskerPlayerTests, fadeOutSchedulesCallback) {
    fadeOut();
    assertCallbackScheduled();
}

TEST_F(MaskerPlayerTests, fadeOutTwiceDoesNotScheduleAdditionalCallback) {
    fadeOut();
    assertFadeOutDoesNotScheduleAdditionalCallback();
}

TEST_F(
    MaskerPlayerTests, fadeOutWhileFadingInDoesNotScheduleAdditionalCallback) {
    fadeIn();
    assertFadeOutDoesNotScheduleAdditionalCallback();
}

TEST_F(
    MaskerPlayerTests, fadeInWhileFadingOutDoesNotScheduleAdditionalCallback) {
    fadeOut();
    assertFadeInDoesNotScheduleAdditionalCallback();
}

TEST_F(MaskerPlayerTests, fadeInAfterFadingOutSchedulesCallback) {
    fadeOutCompletely();
    assertFadeInSchedulesCallback();
}

TEST_F(MaskerPlayerTests, callbackSchedulesAdditionalCallback) {
    timerCallback();
    assertCallbackScheduled();
}

TEST_F(MaskerPlayerTests,
    callbackDoesNotScheduleAdditionalCallbackWhenFadeInComplete) {
    fadeInToFullLevel();
    assertTimerCallbackDoesNotScheduleAdditionalCallback();
}

TEST_F(MaskerPlayerTests,
    callbackDoesNotScheduleAdditionalCallbackWhenFadeOutComplete) {
    fadeInCompletely();
    fadeOutToSilence();
    assertTimerCallbackDoesNotScheduleAdditionalCallback();
}

TEST_F(MaskerPlayerTests, setAudioDeviceFindsIndex) {
    setAudioDeviceDescriptions({"zeroth", "first", "second", "third"});
    setAudioDevice("second");
    assertEqual(2, audioPlayer.deviceIndex());
}

TEST_F(MaskerPlayerTests, setAudioDeviceThrowsInvalidAudioDeviceIfDoesntExist) {
    setAudioDeviceDescriptions({"zeroth", "first", "second"});
    try {
        setAudioDevice("third");
        FAIL() << "Expected recognition_test::InvalidAudioDevice";
    } catch (const av_speech_in_noise::InvalidAudioDevice &) {
    }
}

TEST_F(MaskerPlayerTests, outputAudioDevicesReturnsDescriptions) {
    setAudioDeviceDescriptions({"a", "b", "c"});
    setAsOutputDevice(0);
    setAsOutputDevice(2);
    assertEqual({"a", "c"}, player.outputAudioDeviceDescriptions());
}

TEST_F(MaskerPlayerTests, rmsComputesFirstChannel) {
    audioReader.set({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    loadFile();
    assertEqual(std::sqrt((1 * 1 + 2 * 2 + 3 * 3) / 3.), player.rms(), 1e-6);
}

TEST_F(MaskerPlayerTests, rmsPassesLoadedFileToVideoPlayer) {
    loadFile("a");
    player.rms();
    assertEqual("a", audioReader.filePath());
}

TEST_F(MaskerPlayerTests, loadFileThrowsInvalidAudioFileWhenAudioReaderThrows) {
    audioReader.throwOnRead();
    try {
        loadFile();
        FAIL() << "Expected av_coordinate_response_measure::InvalidAudioFile";
    } catch (const av_speech_in_noise::InvalidAudioFile &) {
    }
}
}
