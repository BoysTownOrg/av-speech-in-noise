#include "assert-utility.h"
#include "AudioReaderStub.h"
#include "stimulus-players/AudioReader.hpp"
#include <stimulus-players/MaskerPlayerImpl.hpp>
#include <gtest/gtest.h>
#include <cmath>
#include <utility>

namespace {
class AudioPlayerStub : public stimulus_players::AudioPlayer {
  public:
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

    [[nodiscard]] auto deviceIndex() const { return deviceIndex_; }

    [[nodiscard]] auto played() const { return played_; }

    void fillAudioBuffer(const std::vector<gsl::span<float>> &audio) {
        listener_->fillAudioBuffer(audio);
    }

  private:
    stimulus_players::audio_type audioRead_;
    std::vector<std::string> audioDeviceDescriptions_{10};
    std::string filePath_;
    std::string deviceDescription_;
    std::string audioFilePath_;
    std::map<int, bool> outputDevices;
    double sampleRateHz_{};
    double durationSeconds_{};
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

template <typename T>
auto elementWiseProduct(std::vector<T> v, const std::vector<T> &y)
    -> std::vector<T> {
    std::transform(
        v.begin(), v.end(), y.begin(), v.begin(), std::multiplies<T>());
    return v;
}

template <typename T>
auto subvector(const std::vector<T> &v, int offset, int size)
    -> std::vector<T> {
    const auto begin = v.begin() + offset;
    return {begin, begin + size};
}

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
    std::vector<float> window(length);
    std::generate(window.begin(), window.end(), [=, n = 0]() mutable {
        return (1 - std::cos((2 * pi * n++) / (N - 1))) / 2;
    });
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

    void fillAudioBufferMono(int n) {
        resizeLeftChannel(n);
        fillAudioBuffer({leftChannel});
    }

    void fillAudioBufferStereo(int n) {
        resizeChannels(n);
        fillAudioBuffer({leftChannel, rightChannel});
    }

    void resizeLeftChannel(int n) { leftChannel.resize(n); }

    void resizeRightChannel(int n) { rightChannel.resize(n); }

    void resizeChannels(int n) {
        resizeLeftChannel(n);
        resizeRightChannel(n);
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
        loadMonoAudio({0});
        fillAudioBufferMono(2 * 3 + 1);
    }

    void fadeIn() { player.fadeIn(); }

    void fadeOut() { player.fadeOut(); }

    void timerCallback() { timer.callback(); }

    void assertCallbackScheduled() { assertTrue(callbackScheduled()); }

    auto callbackScheduled() -> bool { return timer.callbackScheduled(); }

    void assertCallbackNotScheduled() { assertFalse(callbackScheduled()); }

    void loadAudio(stimulus_players::audio_type x) {
        audioReader.set(std::move(x));
        loadFile();
    }

    void loadMonoAudio(std::vector<float> x) { loadAudio({std::move(x)}); }

    void loadStereoAudio(std::vector<float> left, std::vector<float> right) {
        loadAudio({std::move(left), std::move(right)});
    }

    void assertLeftChannelEqualsProductAfterFilling_Buffered(
        const std::vector<float> &multiplicand,
        const std::vector<float> &multiplier, int buffers,
        int framesPerBuffer) {
        for (int i = 0; i < buffers; ++i) {
            const auto offset = i * framesPerBuffer;
            assertLeftChannelEqualsProductAfterFilling(
                subvector(multiplicand, offset, framesPerBuffer),
                subvector(multiplier, offset, framesPerBuffer));
        }
    }

    void assertLeftChannelEqualsProductAfterFilling(
        std::vector<float> multiplicand, const std::vector<float> &multiplier) {
        fillAudioBufferMono(multiplier.size());
        assertLeftChannelEquals(
            elementWiseProduct(std::move(multiplicand), multiplier));
    }

    void assertStereoChannelsEqualProductAfterFilling_Buffered(
        const std::vector<float> &multiplicand,
        const std::vector<float> &leftMultiplier,
        const std::vector<float> &rightMultiplier, int buffers,
        int framesPerBuffer) {
        for (int i = 0; i < buffers; ++i) {
            auto offset = i * framesPerBuffer;
            assertStereoChannelsEqualProductAfterFilling(
                subvector(multiplicand, offset, framesPerBuffer),
                subvector(leftMultiplier, offset, framesPerBuffer),
                subvector(rightMultiplier, offset, framesPerBuffer));
        }
    }

    void assertStereoChannelsEqualProductAfterFilling(
        std::vector<float> multiplicand,
        const std::vector<float> &leftMultiplier,
        const std::vector<float> &rightMultiplier) {
        fillAudioBufferStereo(leftMultiplier.size());
        assertLeftChannelEquals(
            elementWiseProduct(multiplicand, leftMultiplier));
        assertRightChannelEquals(
            elementWiseProduct(std::move(multiplicand), rightMultiplier));
    }

    void assertLeftChannelEquals(const std::vector<float> &x) {
        assertChannelEqual(leftChannel, x);
    }

    static void assertChannelEqual(
        const std::vector<float> &channel, const std::vector<float> &x) {
        assertEqual(x, channel, 1e-6F);
    }

    void assertRightChannelEquals(const std::vector<float> &x) {
        assertChannelEqual(rightChannel, x);
    }

    void assertFadeInNotCompletedAfterMonoFill(int n) {
        callbackAfterMonoFill(n);
        assertFalse(listener.fadeInCompleted());
    }

    void callbackAfterMonoFill(int n = 0) {
        fillAudioBufferMono(n);
        timerCallback();
    }

    void assertFadeInCompletedAfterMonoFill(int n) {
        callbackAfterMonoFill(n);
        assertTrue(listener.fadeInCompleted());
    }

    void assertFadeOutNotCompletedAfterMonoFill(int n) {
        callbackAfterMonoFill(n);
        assertFalse(fadeOutCompleted());
    }

    auto fadeOutCompleted() -> bool { return listener.fadeOutCompleted(); }

    void assertFadeOutCompletedAfterMonoFill(int n) {
        callbackAfterMonoFill(n);
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

    void fadeInFillAndCallback(int n) {
        fadeIn();
        callbackAfterMonoFill(n);
    }

    void fadeOutFillAndCallback(int n) {
        fadeOut();
        callbackAfterMonoFill(n);
    }

    void fadeOutAndFill(int n) {
        fadeOut();
        fillAudioBufferMono(n);
    }

    void setChannelDelaySeconds(int channel, double seconds) {
        player.setChannelDelaySeconds(channel, seconds);
    }

    void clearChannelDelays() { player.clearChannelDelays(); }

    void useFirstChannelOnly() { player.useFirstChannelOnly(); }

    void useAllChannels() { player.useAllChannels(); }

};

TEST_F(MaskerPlayerTests, playingWhenAudioPlayerPlaying) {
    audioPlayer.setPlaying();
    assertTrue(player.playing());
}

TEST_F(MaskerPlayerTests, durationReturnsDuration) {
    setSampleRateHz(3);
    loadMonoAudio({1, 2, 3, 4, 5, 6});
    assertEqual(6. / 3, player.durationSeconds());
}

TEST_F(MaskerPlayerTests, seekSeeksAudio) {
    setSampleRateHz(3);
    loadMonoAudio({1, 2, 3, 4, 5, 6, 7, 8, 9});
    player.seekSeconds(2);
    fillAudioBufferMono(4);
    assertLeftChannelEquals({7, 8, 9, 1});
}

TEST_F(MaskerPlayerTests, delayChannelMono) {
    setSampleRateHz(3);
    setChannelDelaySeconds(0, 1);
    loadMonoAudio({1, 2, 3});
    fillAudioBufferMono(6);
    assertLeftChannelEquals({0, 0, 0, 1, 2, 3});
}

TEST_F(MaskerPlayerTests, clearChannelDelaysMono) {
    setSampleRateHz(3);
    setChannelDelaySeconds(0, 1);
    loadMonoAudio({1, 2, 3});
    fillAudioBufferMono(6);
    clearChannelDelays();
    loadMonoAudio({1, 2, 3});
    fillAudioBufferMono(3);
    assertLeftChannelEquals({1, 2, 3});
}

TEST_F(MaskerPlayerTests, delayChannelStereo) {
    setSampleRateHz(3);
    setChannelDelaySeconds(1, 1);
    loadStereoAudio({1, 2, 3, 4, 5, 6}, {7, 8, 9, 10, 11, 12});
    fillAudioBufferStereo(6);
    assertLeftChannelEquals({1, 2, 3, 4, 5, 6});
    assertRightChannelEquals({0, 0, 0, 7, 8, 9});
}

TEST_F(MaskerPlayerTests, delayChannelStereo_Buffered) {
    setSampleRateHz(3);
    setChannelDelaySeconds(1, 1);
    loadStereoAudio({1, 2, 3, 4, 5, 6}, {7, 8, 9, 10, 11, 12});
    fillAudioBufferStereo(2);
    assertLeftChannelEquals({1, 2});
    assertRightChannelEquals({0, 0});
    fillAudioBufferStereo(2);
    assertLeftChannelEquals({3, 4});
    assertRightChannelEquals({0, 7});
    fillAudioBufferStereo(2);
    assertLeftChannelEquals({5, 6});
    assertRightChannelEquals({8, 9});
}

TEST_F(MaskerPlayerTests, delayChannelMonoWithSeek) {
    setSampleRateHz(3);
    setChannelDelaySeconds(0, 1);
    loadMonoAudio({1, 2, 3, 4, 5, 6, 7, 8, 9});
    fillAudioBufferMono(6);
    assertLeftChannelEquals({0, 0, 0, 1, 2, 3});
    player.seekSeconds(2);
    fillAudioBufferMono(6);
    assertLeftChannelEquals({0, 0, 0, 7, 8, 9});
}

TEST_F(MaskerPlayerTests, moreChannelsRequestedThanAvailableCopiesChannel) {
    loadMonoAudio({1, 2, 3});
    fillAudioBufferStereo(3);
    assertLeftChannelEquals({1, 2, 3});
    assertRightChannelEquals({1, 2, 3});
}

TEST_F(MaskerPlayerTests, moreChannelsAvailableThanRequestedTruncates) {
    loadAudio({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    fillAudioBufferStereo(3);
    assertLeftChannelEquals({1, 2, 3});
    assertRightChannelEquals({4, 5, 6});
}

TEST_F(MaskerPlayerTests, onlyPlayFirstChannel) {
    useFirstChannelOnly();
    loadStereoAudio({1, 2, 3}, {4, 5, 6});
    fillAudioBufferStereo(3);
    assertLeftChannelEquals({1, 2, 3});
    assertRightChannelEquals({0, 0, 0});
}

TEST_F(MaskerPlayerTests, switchBackToAllChannels) {
    useFirstChannelOnly();
    loadStereoAudio({1, 2, 3, 4, 5, 6}, {7, 8, 9, 10, 11, 12});
    fillAudioBufferStereo(3);
    useAllChannels();
    fillAudioBufferStereo(3);
    assertLeftChannelEquals({4, 5, 6});
    assertRightChannelEquals({10, 11, 12});
}

TEST_F(MaskerPlayerTests, noAudioLoadedFillsZeros) {
    leftChannel = {-1, -1, -1};
    fillAudioBufferMono(3);
    assertLeftChannelEquals({0, 0, 0});
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
    loadMonoAudio({1, 2, 3});
    fillAudioBufferMono(3);
    assertLeftChannelEquals({10, 20, 30});
}

TEST_F(MaskerPlayerTests, loadFileResetsSampleIndex) {
    player.setLevel_dB(20);
    loadMonoAudio({1});
    fillAudioBufferMono(1);
    loadMonoAudio({1, 2, 3});
    fillAudioBufferMono(3);
    assertLeftChannelEquals({10, 20, 30});
}

TEST_F(MaskerPlayerTests, fillAudioBufferWraps) {
    loadMonoAudio({1, 2, 3});
    fillAudioBufferMono(4);
    assertLeftChannelEquals({1, 2, 3, 1});
}

TEST_F(MaskerPlayerTests, fillAudioBufferTwoWraps) {
    loadMonoAudio({1, 2, 3});
    fillAudioBufferMono(4);
    assertLeftChannelEquals({1, 2, 3, 1});
    fillAudioBufferMono(4);
    assertLeftChannelEquals({2, 3, 1, 2});
}

TEST_F(MaskerPlayerTests, fillAudioBufferWrapsStereo) {
    loadStereoAudio({1, 2, 3}, {4, 5, 6});
    fillAudioBufferStereo(4);
    assertLeftChannelEquals({1, 2, 3, 1});
    assertRightChannelEquals({4, 5, 6, 4});
}

TEST_F(MaskerPlayerTests, fadesInAccordingToHannFunctionMultipleFills) {
    // For this test:
    // halfWindowLength is determined by fade time and sample rate...
    // but must be divisible by framesPerBuffer.
    setFadeInOutSeconds(3);
    setSampleRateHz(5);
    auto halfWindowLength = 3 * 5 + 1;
    auto framesPerBuffer = 4;

    loadMonoAudio(oneToN(halfWindowLength));
    fadeIn();
    const auto buffers = halfWindowLength / framesPerBuffer;
    assertLeftChannelEqualsProductAfterFilling_Buffered(
        halfHannWindow(halfWindowLength), oneToN(halfWindowLength), buffers,
        framesPerBuffer);
}

TEST_F(MaskerPlayerTests, fadesInAccordingToHannFunctionOneFill) {
    setFadeInOutSeconds(2);
    setSampleRateHz(3);
    auto halfWindowLength = 2 * 3 + 1;

    loadMonoAudio(oneToN(halfWindowLength));
    fadeIn();
    assertLeftChannelEqualsProductAfterFilling(
        halfHannWindow(halfWindowLength), oneToN(halfWindowLength));
}

TEST_F(MaskerPlayerTests, fadesInAccordingToHannFunctionStereoMultipleFills) {
    // For this test:
    // halfWindowLength is determined by fade time and sample rate...
    // but must be divisible by framesPerBuffer.
    setFadeInOutSeconds(3);
    setSampleRateHz(5);
    auto halfWindowLength = 3 * 5 + 1;
    auto framesPerBuffer = 4;

    loadStereoAudio(oneToN(halfWindowLength), NtoOne(halfWindowLength));
    fadeIn();
    auto buffers = halfWindowLength / framesPerBuffer;
    assertStereoChannelsEqualProductAfterFilling_Buffered(
        halfHannWindow(halfWindowLength), oneToN(halfWindowLength),
        NtoOne(halfWindowLength), buffers, framesPerBuffer);
}

TEST_F(MaskerPlayerTests, fadesInAccordingToHannFunctionStereoOneFill) {
    setFadeInOutSeconds(2);
    setSampleRateHz(3);
    auto halfWindowLength = 2 * 3 + 1;

    loadStereoAudio(oneToN(halfWindowLength), NtoOne(halfWindowLength));
    fadeIn();
    assertStereoChannelsEqualProductAfterFilling(
        halfHannWindow(halfWindowLength), oneToN(halfWindowLength),
        NtoOne(halfWindowLength));
}

TEST_F(MaskerPlayerTests, steadyLevelFollowingFadeIn) {
    fadeInToFullLevel();

    loadMonoAudio({1, 2, 3});
    fillAudioBufferMono(3);
    assertLeftChannelEquals({1, 2, 3});
}

TEST_F(MaskerPlayerTests, fadesOutAccordingToHannFunctionMultipleFills) {
    // For this test:
    // halfWindowLength is determined by fade time and sample rate...
    // but must be divisible by framesPerBuffer.
    setFadeInOutSeconds(3);
    setSampleRateHz(5);
    auto halfWindowLength = 3 * 5 + 1;
    auto framesPerBuffer = 4;
    loadMonoAudio(oneToN(halfWindowLength));
    fadeInFillAndCallback(halfWindowLength);

    fadeOut();
    assertLeftChannelEqualsProductAfterFilling_Buffered(
        backHalfHannWindow(halfWindowLength), oneToN(halfWindowLength),
        halfWindowLength / framesPerBuffer, framesPerBuffer);
}

TEST_F(MaskerPlayerTests, fadesOutAccordingToHannFunctionOneFill) {
    setFadeInOutSeconds(2);
    setSampleRateHz(3);
    auto halfWindowLength = 2 * 3 + 1;

    loadMonoAudio(oneToN(halfWindowLength));
    fadeInFillAndCallback(halfWindowLength);
    fadeOut();
    assertLeftChannelEqualsProductAfterFilling(
        backHalfHannWindow(halfWindowLength), oneToN(halfWindowLength));
}

TEST_F(MaskerPlayerTests, steadyLevelFollowingFadeOut) {
    setFadeInOutSeconds(2);
    setSampleRateHz(3);
    auto halfWindowLength = 2 * 3 + 1;

    loadMonoAudio({4, 5, 6});
    fadeInFillAndCallback(halfWindowLength);
    fadeOutAndFill(halfWindowLength);

    fillAudioBufferMono(3);
    assertEqual({0, 0, 0}, leftChannel, 1e-15F);
}

TEST_F(MaskerPlayerTests, fadeInCompleteOnlyAfterFadeTime) {
    setFadeInOutSeconds(3);
    setSampleRateHz(4);

    loadMonoAudio({0});
    fadeIn();
    for (int i = 0; i < 3 * 4; ++i)
        assertFadeInNotCompletedAfterMonoFill(1);
    assertFadeInCompletedAfterMonoFill(1);
}

TEST_F(MaskerPlayerTests, observerNotifiedOnceForFadeIn) {
    fadeInCompletely();
    assertEqual(1, listener.fadeInCompletions());
    callbackAfterMonoFill();
    assertEqual(1, listener.fadeInCompletions());
}

TEST_F(MaskerPlayerTests, fadeOutCompleteOnlyAfterFadeTime) {
    setFadeInOutSeconds(3);
    setSampleRateHz(4);
    loadMonoAudio({0});
    fadeInFillAndCallback(3 * 4 + 1);

    fadeOut();
    for (int i = 0; i < 3 * 4; ++i)
        assertFadeOutNotCompletedAfterMonoFill(1);
    assertFadeOutCompletedAfterMonoFill(1);
}

TEST_F(MaskerPlayerTests, observerNotifiedOnceForFadeOut) {
    setFadeInOutSeconds(2);
    setSampleRateHz(3);
    auto halfWindowLength = 2 * 3 + 1;
    loadMonoAudio({0});
    fadeInFillAndCallback(halfWindowLength);
    fadeOutFillAndCallback(halfWindowLength);

    assertEqual(1, listener.fadeOutCompletions());
    callbackAfterMonoFill();
    assertEqual(1, listener.fadeOutCompletions());
}

TEST_F(MaskerPlayerTests, audioPlayerStoppedOnlyAtEndOfFadeOutTime) {
    setFadeInOutSeconds(3);
    setSampleRateHz(4);
    auto halfWindowLength = 3 * 4 + 1;
    loadMonoAudio({0});
    fadeInFillAndCallback(halfWindowLength);

    fadeOut();
    for (int i = 0; i < 3 * 4; ++i) {
        callbackAfterMonoFill(1);
        assertFalse(playerStopped());
    }
    callbackAfterMonoFill(1);
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
    loadAudio({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
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
