#include "assert-utility.hpp"
#include "AudioReaderStub.hpp"
#include <stimulus-players/AudioReader.hpp>
#include <stimulus-players/MaskerPlayerImpl.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>
#include <cmath>
#include <utility>
#include <future>

namespace av_speech_in_noise {
constexpr auto operator==(const PlayerTime &a, const PlayerTime &b) -> bool {
    return a.system == b.system;
}
}

namespace av_speech_in_noise {
namespace {
auto at(const std::vector<std::string> &v, gsl::index n) -> std::string {
    return v.at(n);
}

class AudioPlayerStub : public AudioPlayer {
  public:
    ~AudioPlayerStub() override {
        if (audioThread.joinable())
            audioThread.join();
    }

    void setNanoseconds(std::uintmax_t t) { nanoseconds_ = t; }

    void setCurrentSystemTime(player_system_time_type t) {
        currentSystemTime_ = t;
    }

    auto currentSystemTime() -> PlayerTime override {
        return {currentSystemTime_};
    }

    auto outputDevice(int index) -> bool override {
        return outputDevices[index];
    }

    void setAudioDeviceDescriptions(std::vector<std::string> v) {
        audioDeviceDescriptions_ = std::move(v);
    }

    void setAsOutputDevice(int index) { outputDevices[index] = true; }

    void stop() override {
        stopped_ = true;
        if (realisticExecution_)
            pleaseStopAudioThread.store(true);
    }

    [[nodiscard]] auto stopped() const { return stopped_; }

    auto sampleRateHz() -> double override { return sampleRateHz_; }

    void setSampleRateHz(double x) { sampleRateHz_ = x; }

    void setPlaying() { playing_ = true; }

    auto playing() -> bool override { return playing_; }

    void loadFile(std::string s) override { filePath_ = std::move(s); }

    void setDevice(int index) override { deviceIndex_ = index; }

    auto deviceCount() -> int override {
        return gsl::narrow<int>(audioDeviceDescriptions_.size());
    }

    auto deviceDescription(int index) -> std::string override {
        deviceDescriptionDeviceIndex_ = index;
        return at(audioDeviceDescriptions_, index);
    }

    void play() override {
        played_ = true;
        if (onPlayTask.valid()) {
            std::thread t{std::move(onPlayTask), observer};
            t.detach();
        }
        if (realisticExecution_) {
            audioThread = std::thread{[&]() {
                std::vector<float> left(9999999);
                std::vector<float> right(9999999);
                auto expected{true};
                while (!pleaseStopAudioThread.compare_exchange_weak(
                    expected, false)) {
                    std::this_thread::sleep_for(std::chrono::milliseconds{10});
                    observer->fillAudioBuffer({left, right}, {});
                    expected = true;
                }
            }};
        }
    }

    void attach(Observer *a) override { observer = a; }

    [[nodiscard]] auto filePath() const -> std::string { return filePath_; }

    [[nodiscard]] auto deviceIndex() const -> int { return deviceIndex_; }

    [[nodiscard]] auto played() const -> bool { return played_; }

    void fillAudioBuffer(const std::vector<gsl::span<float>> &audio,
        player_system_time_type t = {}) {
        observer->fillAudioBuffer(audio, t);
    }

    auto nanoseconds(PlayerTime t) -> std::uintmax_t override {
        systemTimeForNanoseconds_ = t.system;
        return nanoseconds_;
    }

    [[nodiscard]] auto systemTimeForNanoseconds() const
        -> player_system_time_type {
        return systemTimeForNanoseconds_;
    }

    void setOnPlayTask(
        std::packaged_task<std::vector<std::vector<float>>(Observer *)> task) {
        onPlayTask = std::move(task);
    }

    void setRealisticExecution() { realisticExecution_ = true; }

    void clearRealisticExecution() { realisticExecution_ = false; }

    void joinAudioThread() {
        if (audioThread.joinable())
            audioThread.join();
    }

  private:
    std::packaged_task<std::vector<std::vector<float>>(Observer *)> onPlayTask;
    std::thread audioThread;
    audio_type audioRead_;
    std::vector<std::string> audioDeviceDescriptions_{10};
    std::string filePath_;
    std::string deviceDescription_;
    std::string audioFilePath_;
    std::map<int, bool> outputDevices;
    std::uintmax_t nanoseconds_{};
    player_system_time_type systemTimeForNanoseconds_{};
    player_system_time_type currentSystemTime_{};
    Observer *observer{};
    double sampleRateHz_{};
    int deviceIndex_{};
    int deviceDescriptionDeviceIndex_{};
    std::atomic<bool> pleaseStopAudioThread{};
    bool playing_{};
    bool played_{};
    bool stopped_{};
    bool realisticExecution_{};
};

class MaskerPlayerListenerStub : public MaskerPlayer::Observer {
  public:
    void fadeInComplete(const AudioSampleTimeWithOffset &t) override {
        fadeInCompleteSystemTime_ = t.playerTime.system;
        fadeInCompleteSystemTimeSampleOffset_ = t.sampleOffset;
        fadeInCompleted_ = true;
        ++fadeInCompletions_;
    }

    void fadeOutComplete() override {
        ++fadeOutCompletions_;
        fadeOutCompleted_ = true;
    }

    [[nodiscard]] auto fadeInCompleteSystemTime() const
        -> player_system_time_type {
        return fadeInCompleteSystemTime_;
    }

    [[nodiscard]] auto fadeInCompleteSystemTimeSampleOffset() const
        -> gsl::index {
        return fadeInCompleteSystemTimeSampleOffset_;
    }

    [[nodiscard]] auto fadeInCompleted() const -> bool {
        return fadeInCompleted_;
    }

    [[nodiscard]] auto fadeOutCompleted() const -> bool {
        return fadeOutCompleted_;
    }

    [[nodiscard]] auto fadeInCompletions() const -> int {
        return fadeInCompletions_;
    }

    [[nodiscard]] auto fadeOutCompletions() const -> int {
        return fadeOutCompletions_;
    }

  private:
    player_system_time_type fadeInCompleteSystemTime_{};
    gsl::index fadeInCompleteSystemTimeSampleOffset_{};
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

class TimerStub : public Timer {
  public:
    void scheduleCallbackAfterSeconds(double) override {
        callbackScheduled_ = true;
    }

    [[nodiscard]] auto callbackScheduled() const { return callbackScheduled_; }

    void clearCallbackCount() { callbackScheduled_ = false; }

    void callback() { observer->callback(); }

    void attach(Observer *a) override { observer = a; }

  private:
    Observer *observer{};
    bool callbackScheduled_{};
};

template <typename T> auto vector(gsl::index size) -> std::vector<T> {
    return std::vector<T>(size);
}

auto halfHannWindow(gsl::index length) -> std::vector<float> {
    auto N{2 * length - 1};
    const auto pi{std::acos(-1)};
    auto window{vector<float>(length)};
    std::generate(window.begin(), window.end(), [=, n = 0]() mutable {
        return gsl::narrow_cast<float>(
            (1 - std::cos((2 * pi * n++) / (N - 1))) / 2);
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
    auto result{vector<float>(N - M + 1)};
    std::iota(result.begin(), result.end(), gsl::narrow<float>(M));
    return result;
}

auto oneToN(int N) -> std::vector<float> { return mToN(1, N); }

auto NtoOne(int N) -> std::vector<float> { return reverse(oneToN(N)); }

auto size(const std::vector<float> &v) -> gsl::index { return v.size(); }

void resize(std::vector<float> &v, gsl::index n) { v.resize(n); }

void setNanoseconds(AudioPlayerStub &player, std::uintmax_t t) {
    player.setNanoseconds(t);
}

void setCurrentSystemTime(AudioPlayerStub &player, std::uintmax_t t) {
    player.setCurrentSystemTime(t);
}

auto nanoseconds(MaskerPlayerImpl &player, player_system_time_type t = {})
    -> std::uintmax_t {
    return player.nanoseconds({t});
}

auto currentSystemTime(MaskerPlayerImpl &player) -> PlayerTime {
    return player.currentSystemTime();
}

auto systemTimeForNanoseconds(AudioPlayerStub &player)
    -> player_system_time_type {
    return player.systemTimeForNanoseconds();
}

auto fillAudioBuffer(AudioPlayer::Observer *observer, gsl::index channels,
    gsl::index frames, player_system_time_type t = {})
    -> std::vector<std::vector<float>> {
    std::vector<std::vector<float>> audio(channels);
    std::vector<gsl::span<float>> adapted;
    for (auto &channel : audio) {
        channel.resize(frames, -1.F);
        adapted.emplace_back(channel);
    }
    observer->fillAudioBuffer(adapted, t);
    return audio;
}

auto fillAudioBufferAsync(AudioPlayerStub &audioPlayer, gsl::index channels,
    gsl::index frames) -> std::future<std::vector<std::vector<float>>> {
    std::packaged_task<std::vector<std::vector<float>>(AudioPlayer::Observer *)>
        task{[=](AudioPlayer::Observer *observer) {
            return fillAudioBuffer(observer, channels, frames);
        }};
    auto result{task.get_future()};
    audioPlayer.setOnPlayTask(std::move(task));
    audioPlayer.play();
    return result;
}

auto fillAudioBufferAsync(MaskerPlayerImpl &player,
    AudioPlayerStub &audioPlayer, gsl::index channels, gsl::index frames)
    -> std::future<std::vector<std::vector<float>>> {
    std::packaged_task<std::vector<std::vector<float>>(AudioPlayer::Observer *)>
        task{[=](AudioPlayer::Observer *observer) {
            return fillAudioBuffer(observer, channels, frames);
        }};
    auto result{task.get_future()};
    audioPlayer.setOnPlayTask(std::move(task));
    player.play();
    return result;
}

auto fillAudioBufferMonoAsync(AudioPlayerStub &audioPlayer, gsl::index frames)
    -> std::future<std::vector<std::vector<float>>> {
    return fillAudioBufferAsync(audioPlayer, 1, frames);
}

auto fillAudioBufferMonoAsync(
    MaskerPlayerImpl &player, AudioPlayerStub &audioPlayer, gsl::index frames)
    -> std::future<std::vector<std::vector<float>>> {
    return fillAudioBufferAsync(player, audioPlayer, 1, frames);
}

auto fillAudioBufferStereoAsync(AudioPlayerStub &audioPlayer, gsl::index frames)
    -> std::future<std::vector<std::vector<float>>> {
    return fillAudioBufferAsync(audioPlayer, 2, frames);
}

auto fillAudioBufferStereoAsync(
    MaskerPlayerImpl &player, AudioPlayerStub &audioPlayer, gsl::index frames)
    -> std::future<std::vector<std::vector<float>>> {
    return fillAudioBufferAsync(player, audioPlayer, 2, frames);
}

void assertChannelEqual(
    const std::vector<float> &channel, const std::vector<float> &x) {
    assertEqual(x, channel, 1e-29F);
}

void assertStereoChannelsEqual(const std::vector<std::vector<float>> &expected,
    const std::vector<float> &left, const std::vector<float> &right) {
    assertChannelEqual(left, expected.at(0));
    assertChannelEqual(right, expected.at(1));
}

void assertAsyncLoadedMonoChannelEquals(MaskerPlayerImpl &player,
    AudioPlayerStub &audioPlayer, const std::vector<float> &expected) {
    assertChannelEqual(
        fillAudioBufferMonoAsync(player, audioPlayer, expected.size())
            .get()
            .front(),
        expected);
}

void assertAsyncLoadedStereoChannelsEquals(MaskerPlayerImpl &player,
    AudioPlayerStub &audioPlayer, const std::vector<float> &left,
    const std::vector<float> &right) {
    assertStereoChannelsEqual(
        fillAudioBufferStereoAsync(player, audioPlayer, left.size()).get(),
        left, right);
}

void callInRealisticExecutionContext(MaskerPlayerImpl &player,
    AudioPlayerStub &audioPlayer, const std::function<void()> &f) {
    audioPlayer.setRealisticExecution();
    player.play();
    player.stop();
    f();
    audioPlayer.clearRealisticExecution();
    audioPlayer.joinAudioThread();
}

using channel_index_type = gsl::index;

class MaskerPlayerTests : public ::testing::Test {
  protected:
    AudioPlayerStub audioPlayer;
    MaskerPlayerListenerStub listener;
    AudioReaderStub audioReader;
    TimerStub timer;
    MaskerPlayerImpl player{&audioPlayer, &audioReader, &timer};
    std::vector<float> leftChannel;
    std::vector<float> rightChannel;

    MaskerPlayerTests() { player.attach(&listener); }

    void fillAudioBuffer(const std::vector<gsl::span<float>> &audio,
        player_system_time_type t = {}) {
        audioPlayer.fillAudioBuffer(audio, t);
    }

    void fillAudioBufferMono(
        channel_index_type n, player_system_time_type t = {}) {
        resizeLeftChannel(n);
        fillAudioBuffer({leftChannel}, t);
    }

    void fillAudioBufferStereo(channel_index_type n) {
        resizeChannels(n);
        fillAudioBuffer({leftChannel, rightChannel});
    }

    void resizeLeftChannel(channel_index_type n) { resize(leftChannel, n); }

    void resizeRightChannel(channel_index_type n) { resize(rightChannel, n); }

    void resizeChannels(channel_index_type n) {
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

    void assertCallbackScheduled() {
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(callbackScheduled());
    }

    auto callbackScheduled() -> bool { return timer.callbackScheduled(); }

    void assertCallbackNotScheduled() {
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(callbackScheduled());
    }

    void loadAudio(audio_type x) {
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
        fillAudioBufferMono(size(multiplier));
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
        fillAudioBufferStereo(size(leftMultiplier));
        assertLeftChannelEquals(
            elementWiseProduct(multiplicand, leftMultiplier));
        assertRightChannelEquals(
            elementWiseProduct(std::move(multiplicand), rightMultiplier));
    }

    void assertLeftChannelEquals(const std::vector<float> &x) {
        assertChannelEqual(leftChannel, x);
    }

    void assertRightChannelEquals(const std::vector<float> &x) {
        assertChannelEqual(rightChannel, x);
    }

    void assertFadeInNotCompletedAfterMonoFill(channel_index_type n) {
        callbackAfterMonoFill(n);
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(listener.fadeInCompleted());
    }

    void callbackAfterMonoFill(channel_index_type n = 0) {
        fillAudioBufferMono(n);
        timerCallback();
    }

    void assertFadeInCompletedAfterMonoFill(channel_index_type n) {
        callbackAfterMonoFill(n);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(listener.fadeInCompleted());
    }

    void assertFadeOutNotCompletedAfterMonoFill(channel_index_type n) {
        callbackAfterMonoFill(n);
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(fadeOutCompleted());
    }

    auto fadeOutCompleted() -> bool { return listener.fadeOutCompleted(); }

    void assertFadeOutCompletedAfterMonoFill(channel_index_type n) {
        callbackAfterMonoFill(n);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(fadeOutCompleted());
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

    void loadFile(const std::string &s = {}) { player.loadFile({s}); }

    void fadeInFillAndCallback(channel_index_type n) {
        fadeIn();
        callbackAfterMonoFill(n);
    }

    void fadeOutFillAndCallback(channel_index_type n) {
        fadeOut();
        callbackAfterMonoFill(n);
    }

    void fadeOutAndFill(channel_index_type n) {
        fadeOut();
        fillAudioBufferMono(n);
    }

    void setChannelDelaySeconds(channel_index_type channel, double seconds) {
        player.setChannelDelaySeconds(channel, seconds);
    }

    void clearChannelDelays() { player.clearChannelDelays(); }

    void useFirstChannelOnly() { player.useFirstChannelOnly(); }

    void useAllChannels() { player.useAllChannels(); }

    void seekSeconds(double x) { player.seekSeconds(x); }

    void assertFadeInCompletions(int n) {
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(n, listener.fadeInCompletions());
    }

    void assertFadeOutCompletions(int n) {
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(n, listener.fadeOutCompletions());
    }

    auto digitalLevel() -> DigitalLevel { return player.digitalLevel(); }
};

#define MASKER_PLAYER_TEST(a) TEST_F(MaskerPlayerTests, a)

MASKER_PLAYER_TEST(stopThenLoad) {
    loadMonoAudio(std::vector<float>(9999999));
    std::vector<float> next(999);
    next.at(0) = 4;
    next.at(1) = 5;
    next.at(2) = 6;
    audioReader.set({next});
    audioPlayer.setRealisticExecution();
    player.play();
    std::this_thread::sleep_for(std::chrono::milliseconds{20});
    player.stop();
    player.loadFile({});
    audioPlayer.clearRealisticExecution();
    audioPlayer.joinAudioThread();
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {4, 5, 6});
}

MASKER_PLAYER_TEST(playingWhenAudioPlayerPlaying) {
    audioPlayer.setPlaying();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(player.playing());
}

MASKER_PLAYER_TEST(durationReturnsDuration) {
    setSampleRateHz(3);
    loadMonoAudio({1, 2, 3, 4, 5, 6});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(6. / 3, player.duration().seconds);
}

MASKER_PLAYER_TEST(seekSeeksAudio) {
    setSampleRateHz(3);
    loadMonoAudio({1, 2, 3, 4, 5, 6, 7, 8, 9});
    seekSeconds(2);
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {7, 8, 9, 1});
}

MASKER_PLAYER_TEST(seekNegativeTime) {
    setSampleRateHz(3);
    loadMonoAudio({1, 2, 3, 4, 5, 6, 7, 8, 9});
    seekSeconds(-2);
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {4, 5, 6, 7});
}

MASKER_PLAYER_TEST(setChannelDelayMono) {
    setSampleRateHz(3);
    setChannelDelaySeconds(0, 1);
    loadMonoAudio({1, 2, 3});
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {0, 0, 0, 1, 2, 3});
}

MASKER_PLAYER_TEST(setChannelDelayAfterLoadMono) {
    setSampleRateHz(3);
    loadMonoAudio({1, 2, 3});
    setChannelDelaySeconds(0, 1);
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {0, 0, 0, 1, 2, 3});
}

MASKER_PLAYER_TEST(setChannelDelayMono_Buffered) {
    setSampleRateHz(3);
    setChannelDelaySeconds(0, 1);
    loadMonoAudio({4, 5, 6});
    std::packaged_task<std::vector<std::vector<float>>(AudioPlayer::Observer *)>
        task{[=](AudioPlayer::Observer *observer) {
            std::vector<float> first(2);
            std::vector<float> second(4);
            observer->fillAudioBuffer({first}, {});
            observer->fillAudioBuffer({second}, {});
            return std::vector<std::vector<float>>{first, second};
        }};
    auto result{task.get_future()};
    audioPlayer.setOnPlayTask(std::move(task));
    player.play();
    auto twoMonoBuffers{result.get()};
    assertChannelEqual(twoMonoBuffers.at(0), {0, 0});
    assertChannelEqual(twoMonoBuffers.at(1), {0, 4, 5, 6});
}

MASKER_PLAYER_TEST(setChannelDelayMonoLoadNewAudio) {
    setSampleRateHz(3);
    setChannelDelaySeconds(0, 1);
    loadMonoAudio({4, 5, 6});
    callInRealisticExecutionContext(player, audioPlayer, [&]() {
        loadMonoAudio({1, 2, 3});
    });
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {0, 0, 0});
}

MASKER_PLAYER_TEST(setChannelDelayMonoWithSeek) {
    setSampleRateHz(3);
    setChannelDelaySeconds(0, 1);
    loadMonoAudio({1, 2, 3, 4, 5, 6, 7, 8, 9});
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {0, 0, 0, 1, 2, 3});
}

MASKER_PLAYER_TEST(setChannelDelayMonoWithSeek2) {
    setSampleRateHz(3);
    setChannelDelaySeconds(0, 1);
    loadMonoAudio({1, 2, 3, 4, 5, 6, 7, 8, 9});
    callInRealisticExecutionContext(
        player, audioPlayer, [&]() { seekSeconds(2); });
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {0, 0, 0, 7, 8, 9});
}

MASKER_PLAYER_TEST(clearChannelDelaysMono) {
    setSampleRateHz(3);
    setChannelDelaySeconds(0, 1);
    loadMonoAudio({4, 5, 6});
    callInRealisticExecutionContext(player, audioPlayer, [&]() {
        clearChannelDelays();
        loadMonoAudio({1, 2, 3});
    });
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {1, 2, 3});
}

MASKER_PLAYER_TEST(clearChannelDelaysAfterLoadMono) {
    setSampleRateHz(3);
    setChannelDelaySeconds(0, 1);
    loadMonoAudio({4, 5, 6});
    callInRealisticExecutionContext(player, audioPlayer, [&]() {
        loadMonoAudio({1, 2, 3});
        clearChannelDelays();
    });
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {1, 2, 3});
}

MASKER_PLAYER_TEST(setChannelDelayStereo) {
    setSampleRateHz(3);
    setChannelDelaySeconds(1, 1);
    loadStereoAudio({1, 2, 3, 4, 5, 6}, {7, 8, 9, 10, 11, 12});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {1, 2, 3, 4, 5, 6}, {0, 0, 0, 7, 8, 9});
}

MASKER_PLAYER_TEST(setChannelDelayStereo_Buffered) {
    setSampleRateHz(3);
    setChannelDelaySeconds(1, 1);
    loadStereoAudio({1, 2, 3, 4, 5, 6}, {7, 8, 9, 10, 11, 12});
    std::packaged_task<std::vector<std::vector<float>>(AudioPlayer::Observer *)>
        task{[=](AudioPlayer::Observer *observer) {
            std::vector<float> firstLeft(2);
            std::vector<float> firstRight(2);
            std::vector<float> secondLeft(2);
            std::vector<float> secondRight(2);
            std::vector<float> thirdLeft(2);
            std::vector<float> thirdRight(2);
            observer->fillAudioBuffer({firstLeft, firstRight}, {});
            observer->fillAudioBuffer({secondLeft, secondRight}, {});
            observer->fillAudioBuffer({thirdLeft, thirdRight}, {});
            return std::vector<std::vector<float>>{firstLeft, firstRight,
                secondLeft, secondRight, thirdLeft, thirdRight};
        }};
    auto result{task.get_future()};
    audioPlayer.setOnPlayTask(std::move(task));
    player.play();
    auto sixMonoBuffers{result.get()};
    assertChannelEqual(sixMonoBuffers.at(0), {1, 2});
    assertChannelEqual(sixMonoBuffers.at(1), {0, 0});
    assertChannelEqual(sixMonoBuffers.at(2), {3, 4});
    assertChannelEqual(sixMonoBuffers.at(3), {0, 7});
    assertChannelEqual(sixMonoBuffers.at(4), {5, 6});
    assertChannelEqual(sixMonoBuffers.at(5), {8, 9});
}

MASKER_PLAYER_TEST(moreChannelsRequestedThanAvailableCopiesChannel) {
    loadMonoAudio({1, 2, 3});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {1, 2, 3}, {1, 2, 3});
}

MASKER_PLAYER_TEST(moreChannelsAvailableThanRequestedTruncates) {
    loadAudio({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {1, 2, 3}, {4, 5, 6});
}

MASKER_PLAYER_TEST(useFirstChannelOnlyMutesOtherChannels) {
    useFirstChannelOnly();
    loadStereoAudio({1, 2, 3}, {4, 5, 6});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {1, 2, 3}, {0, 0, 0});
}

void useSecondChannelOnly(MaskerPlayerImpl &player) {
    player.useSecondChannelOnly();
}

MASKER_PLAYER_TEST(
    useFirstChannelOnlyAfterUsingSecondChannelOnlyMutesOtherChannels) {
    useSecondChannelOnly(player);
    useFirstChannelOnly();
    loadStereoAudio({1, 2, 3}, {4, 5, 6});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {1, 2, 3}, {0, 0, 0});
}

MASKER_PLAYER_TEST(useSecondChannelOnlyMutesOtherChannels) {
    useSecondChannelOnly(player);
    loadStereoAudio({1, 2, 3}, {4, 5, 6});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {0, 0, 0}, {4, 5, 6});
}

MASKER_PLAYER_TEST(
    useSecondChannelOnlyAfterUsingFirstChannelOnlyMutesOtherChannels) {
    useFirstChannelOnly();
    useSecondChannelOnly(player);
    loadStereoAudio({1, 2, 3}, {4, 5, 6});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {0, 0, 0}, {4, 5, 6});
}

MASKER_PLAYER_TEST(useAllChannelsAfterUsingFirstChannelOnly) {
    useFirstChannelOnly();
    useAllChannels();
    loadStereoAudio({1, 2, 3}, {4, 5, 6});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {1, 2, 3}, {4, 5, 6});
}

MASKER_PLAYER_TEST(useAllChannelsAfterUsingSecondChannelOnly) {
    useSecondChannelOnly(player);
    useAllChannels();
    loadStereoAudio({1, 2, 3}, {4, 5, 6});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {1, 2, 3}, {4, 5, 6});
}

MASKER_PLAYER_TEST(noAudioLoadedMutesChannel) {
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {0, 0, 0});
}

MASKER_PLAYER_TEST(fadeTimeReturnsFadeTime) {
    player.setFadeInOutSeconds(1);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1., player.fadeTime().seconds);
}

MASKER_PLAYER_TEST(loadFileLoadsAudioFile) {
    loadFile("a");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, audioPlayer.filePath());
}

MASKER_PLAYER_TEST(fadeInPlaysAudioPlayer) {
    fadeIn();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(audioPlayer.played());
}

MASKER_PLAYER_TEST(playPlaysAudioPlayer) {
    player.play();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(audioPlayer.played());
}

MASKER_PLAYER_TEST(stopStopsAudioPlayer) {
    player.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(audioPlayer.stopped());
}

MASKER_PLAYER_TEST(twentydBMultipliesSignalByTen) {
    player.apply(LevelAmplification{20});
    loadMonoAudio({1, 2, 3});
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {10, 20, 30});
}

MASKER_PLAYER_TEST(loadFileResetsSampleIndex) {
    loadMonoAudio({1, 2, 3});
    callInRealisticExecutionContext(player, audioPlayer, [&]() {
        loadMonoAudio({4, 5, 6});
    });
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {4, 5, 6});
}

MASKER_PLAYER_TEST(fillAudioBufferWrapsMonoChannel) {
    loadMonoAudio({1, 2, 3});
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {1, 2, 3, 1});
}

MASKER_PLAYER_TEST(DISABLED_fillAudioBufferWrapsMonoChannel_Buffered) {
    loadMonoAudio({1, 2, 3});
    fillAudioBufferMono(4);
    assertLeftChannelEquals({1, 2, 3, 1});
    fillAudioBufferMono(4);
    assertLeftChannelEquals({2, 3, 1, 2});
}

MASKER_PLAYER_TEST(fillAudioBufferWrapsStereoChannel) {
    loadStereoAudio({1, 2, 3}, {4, 5, 6});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {1, 2, 3, 1}, {4, 5, 6, 4});
}

MASKER_PLAYER_TEST(DISABLED_fillAudioBufferWrapsStereoChannel_Buffered) {
    loadStereoAudio({1, 2, 3}, {4, 5, 6});
    fillAudioBufferStereo(2);
    assertLeftChannelEquals({1, 2});
    assertRightChannelEquals({4, 5});
    fillAudioBufferStereo(4);
    assertLeftChannelEquals({3, 1, 2, 3});
    assertRightChannelEquals({6, 4, 5, 6});
}

MASKER_PLAYER_TEST(DISABLED_fadesInAccordingToHannFunctionMultipleFills) {
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

MASKER_PLAYER_TEST(DISABLED_fadesInAccordingToHannFunctionOneFill) {
    setFadeInOutSeconds(2);
    setSampleRateHz(3);
    auto halfWindowLength = 2 * 3 + 1;

    loadMonoAudio(oneToN(halfWindowLength));
    fadeIn();
    assertLeftChannelEqualsProductAfterFilling(
        halfHannWindow(halfWindowLength), oneToN(halfWindowLength));
}

MASKER_PLAYER_TEST(DISABLED_fadesInAccordingToHannFunctionStereoMultipleFills) {
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

MASKER_PLAYER_TEST(DISABLED_fadesInAccordingToHannFunctionStereoOneFill) {
    setFadeInOutSeconds(2);
    setSampleRateHz(3);
    auto halfWindowLength = 2 * 3 + 1;

    loadStereoAudio(oneToN(halfWindowLength), NtoOne(halfWindowLength));
    fadeIn();
    assertStereoChannelsEqualProductAfterFilling(
        halfHannWindow(halfWindowLength), oneToN(halfWindowLength),
        NtoOne(halfWindowLength));
}

MASKER_PLAYER_TEST(DISABLED_steadyLevelFollowingFadeIn) {
    setFadeInOutSeconds(2);
    setSampleRateHz(3);
    loadMonoAudio({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    fadeIn();
    auto halfWindowLength = 2 * 3 + 1;
    fillAudioBufferMono(halfWindowLength);

    fillAudioBufferMono(3);
    assertLeftChannelEquals({8, 9, 10});
}

MASKER_PLAYER_TEST(DISABLED_fadesOutAccordingToHannFunctionMultipleFills) {
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

MASKER_PLAYER_TEST(DISABLED_fadesOutAccordingToHannFunctionOneFill) {
    setFadeInOutSeconds(2);
    setSampleRateHz(3);
    auto halfWindowLength = 2 * 3 + 1;

    loadMonoAudio(oneToN(halfWindowLength));
    fadeInFillAndCallback(halfWindowLength);
    fadeOut();
    assertLeftChannelEqualsProductAfterFilling(
        backHalfHannWindow(halfWindowLength), oneToN(halfWindowLength));
}

MASKER_PLAYER_TEST(DISABLED_steadyLevelFollowingFadeOut) {
    setFadeInOutSeconds(2);
    setSampleRateHz(3);
    auto halfWindowLength = 2 * 3 + 1;

    loadMonoAudio({4, 5, 6});
    fadeInFillAndCallback(halfWindowLength);
    fadeOutAndFill(halfWindowLength);

    fillAudioBufferMono(3);
    assertLeftChannelEquals({0, 0, 0});
}

MASKER_PLAYER_TEST(DISABLED_fadeInCompleteOnlyAfterFadeTime) {
    setFadeInOutSeconds(3);
    setSampleRateHz(4);

    loadMonoAudio({0});
    fadeIn();
    for (int i = 0; i < 3 * 4; ++i)
        assertFadeInNotCompletedAfterMonoFill(1);
    assertFadeInCompletedAfterMonoFill(1);
}

MASKER_PLAYER_TEST(DISABLED_fadeInCompletePassesSystemTimeAndSampleOffset) {
    setFadeInOutSeconds(3);
    setSampleRateHz(4);

    loadMonoAudio({0});
    fadeIn();
    fillAudioBufferMono(5);
    fillAudioBufferMono(3 * 4 - 5 + 1, 6);
    timerCallback();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        player_system_time_type{6}, listener.fadeInCompleteSystemTime());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(gsl::index{3 * 4 - 5 + 1},
        listener.fadeInCompleteSystemTimeSampleOffset());
}

MASKER_PLAYER_TEST(DISABLED_observerNotifiedOnlyOnceForFadeIn) {
    assertFadeInCompletions(0);
    fadeInCompletely();
    assertFadeInCompletions(1);
    callbackAfterMonoFill();
    assertFadeInCompletions(1);
}

MASKER_PLAYER_TEST(DISABLED_fadeOutCompleteOnlyAfterFadeTime) {
    setFadeInOutSeconds(3);
    setSampleRateHz(4);
    loadMonoAudio({0});
    fadeInFillAndCallback(3 * 4 + 1);

    fadeOut();
    for (int i = 0; i < 3 * 4; ++i)
        assertFadeOutNotCompletedAfterMonoFill(1);
    assertFadeOutCompletedAfterMonoFill(1);
}

MASKER_PLAYER_TEST(DISABLED_observerNotifiedOnceForFadeOut) {
    setFadeInOutSeconds(2);
    setSampleRateHz(3);
    auto halfWindowLength = 2 * 3 + 1;
    loadMonoAudio({0});
    fadeInFillAndCallback(halfWindowLength);
    fadeOutFillAndCallback(halfWindowLength);

    assertFadeOutCompletions(1);
    callbackAfterMonoFill();
    assertFadeOutCompletions(1);
}

MASKER_PLAYER_TEST(DISABLED_audioPlayerStoppedOnlyAtEndOfFadeOutTime) {
    setFadeInOutSeconds(3);
    setSampleRateHz(4);
    auto halfWindowLength = 3 * 4 + 1;
    loadMonoAudio({0});
    fadeInFillAndCallback(halfWindowLength);

    fadeOut();
    for (int i = 0; i < 3 * 4; ++i) {
        callbackAfterMonoFill(1);
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(playerStopped());
    }
    callbackAfterMonoFill(1);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(playerStopped());
}

MASKER_PLAYER_TEST(fadeInSchedulesCallback) { assertFadeInSchedulesCallback(); }

MASKER_PLAYER_TEST(DISABLED_fadeInTwiceDoesNotScheduleAdditionalCallback) {
    fadeIn();
    assertFadeInDoesNotScheduleAdditionalCallback();
}

MASKER_PLAYER_TEST(DISABLED_fadeOutSchedulesCallback) {
    fadeOut();
    assertCallbackScheduled();
}

MASKER_PLAYER_TEST(DISABLED_fadeOutTwiceDoesNotScheduleAdditionalCallback) {
    fadeOut();
    assertFadeOutDoesNotScheduleAdditionalCallback();
}

MASKER_PLAYER_TEST(
    DISABLED_fadeOutWhileFadingInDoesNotScheduleAdditionalCallback) {
    fadeIn();
    assertFadeOutDoesNotScheduleAdditionalCallback();
}

MASKER_PLAYER_TEST(
    DISABLED_fadeInWhileFadingOutDoesNotScheduleAdditionalCallback) {
    fadeOut();
    assertFadeInDoesNotScheduleAdditionalCallback();
}

MASKER_PLAYER_TEST(DISABLED_fadeInAfterFadingOutSchedulesCallback) {
    fadeOutCompletely();
    assertFadeInSchedulesCallback();
}

MASKER_PLAYER_TEST(callbackSchedulesAdditionalCallback) {
    timerCallback();
    assertCallbackScheduled();
}

MASKER_PLAYER_TEST(
    DISABLED_callbackDoesNotScheduleAdditionalCallbackWhenFadeInComplete) {
    fadeInToFullLevel();
    assertTimerCallbackDoesNotScheduleAdditionalCallback();
}

MASKER_PLAYER_TEST(
    DISABLED_callbackDoesNotScheduleAdditionalCallbackWhenFadeOutComplete) {
    fadeInCompletely();
    fadeOutToSilence();
    assertTimerCallbackDoesNotScheduleAdditionalCallback();
}

MASKER_PLAYER_TEST(setAudioDeviceFindsIndex) {
    setAudioDeviceDescriptions({"zeroth", "first", "second", "third"});
    setAudioDevice("second");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(2, audioPlayer.deviceIndex());
}

MASKER_PLAYER_TEST(setAudioDeviceThrowsInvalidAudioDeviceIfDoesntExist) {
    setAudioDeviceDescriptions({"zeroth", "first", "second"});
    try {
        setAudioDevice("third");
        FAIL() << "Expected recognition_test::InvalidAudioDevice";
    } catch (const InvalidAudioDevice &) {
    }
}

MASKER_PLAYER_TEST(outputAudioDevicesReturnsDescriptions) {
    setAudioDeviceDescriptions({"a", "b", "c"});
    setAsOutputDevice(0);
    setAsOutputDevice(2);
    assertEqual({"a", "c"}, player.outputAudioDeviceDescriptions());
}

MASKER_PLAYER_TEST(digitalLevelComputedFromFirstChannel) {
    loadAudio({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        20 * std::log10(std::sqrt((1 * 1 + 2 * 2 + 3 * 3) / 3.F)),
        digitalLevel().dBov);
}

MASKER_PLAYER_TEST(digitalLevelPassesLoadedFileToVideoPlayer) {
    loadFile("a");
    digitalLevel();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, audioReader.filePath());
}

MASKER_PLAYER_TEST(returnsNanosecondConversion) {
    setNanoseconds(audioPlayer, 1);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::uintmax_t{1}, nanoseconds(player));
}

MASKER_PLAYER_TEST(returnsCurrentSystemTime) {
    setCurrentSystemTime(audioPlayer, 1);
    PlayerTime expected{};
    expected.system = 1;
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected, currentSystemTime(player));
}

MASKER_PLAYER_TEST(passesSystemTimeToAudioPlayerForNanoseconds) {
    nanoseconds(player, 1);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        player_system_time_type{1}, systemTimeForNanoseconds(audioPlayer));
}

MASKER_PLAYER_TEST(loadFileThrowsInvalidAudioFileWhenAudioReaderThrows) {
    audioReader.throwOnRead();
    try {
        loadFile();
        FAIL() << "Expected av_coordinate_response_measure::InvalidAudioFile";
    } catch (const InvalidAudioFile &) {
    }
}
}
}
