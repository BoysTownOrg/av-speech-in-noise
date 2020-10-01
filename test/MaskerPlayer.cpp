#include "assert-utility.hpp"
#include "AudioReaderStub.hpp"
#include <stimulus-players/AudioReader.hpp>
#include <stimulus-players/MaskerPlayerImpl.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>
#include <cmath>
#include <utility>
#include <future>
#include <mutex>
#include <atomic>
#include <condition_variable>

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

auto setOnPlayTask(AudioPlayerStub &audioPlayer,
    std::function<std::vector<std::vector<float>>(AudioPlayer::Observer *)> f)
    -> std::future<std::vector<std::vector<float>>> {
    std::packaged_task<std::vector<std::vector<float>>(AudioPlayer::Observer *)>
        task{std::move(f)};
    auto future{task.get_future()};
    audioPlayer.setOnPlayTask(std::move(task));
    return future;
}

auto fillAudioBufferAsync(MaskerPlayerImpl &player,
    AudioPlayerStub &audioPlayer, gsl::index channels, gsl::index frames)
    -> std::future<std::vector<std::vector<float>>> {
    auto future{
        setOnPlayTask(audioPlayer, [=](AudioPlayer::Observer *observer) {
            return fillAudioBuffer(observer, channels, frames);
        })};
    player.play();
    return future;
}

auto fillAudioBufferMonoAsync(
    MaskerPlayerImpl &player, AudioPlayerStub &audioPlayer, gsl::index frames)
    -> std::future<std::vector<std::vector<float>>> {
    return fillAudioBufferAsync(player, audioPlayer, 1, frames);
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

void setSampleRateHz(AudioPlayerStub &player, double x) {
    player.setSampleRateHz(x);
}

void loadFile(MaskerPlayerImpl &player, const std::string &s = {}) {
    player.loadFile({s});
}

void loadAudio(
    MaskerPlayerImpl &player, AudioReaderStub &audioReader, audio_type x) {
    audioReader.set(std::move(x));
    loadFile(player);
}

void loadMonoAudio(MaskerPlayerImpl &player, AudioReaderStub &audioReader,
    std::vector<float> x) {
    loadAudio(player, audioReader, {std::move(x)});
}

void setChannelDelaySeconds(
    MaskerPlayerImpl &player, channel_index_type channel, double seconds) {
    player.setChannelDelaySeconds(channel, seconds);
}

void clearChannelDelays(MaskerPlayerImpl &player) {
    player.clearChannelDelays();
}

void useFirstChannelOnly(MaskerPlayerImpl &player) {
    player.useFirstChannelOnly();
}

void useAllChannels(MaskerPlayerImpl &player) { player.useAllChannels(); }

void seekSeconds(MaskerPlayerImpl &player, double x) { player.seekSeconds(x); }

void loadStereoAudio(MaskerPlayerImpl &player, AudioReaderStub &audioReader,
    std::vector<float> left, std::vector<float> right) {
    loadAudio(player, audioReader, {std::move(left), std::move(right)});
}

void fadeIn(MaskerPlayerImpl &player) { player.fadeIn(); }

void fadeOut(MaskerPlayerImpl &player) { player.fadeOut(); }

void callback(TimerStub &timer) { timer.callback(); }

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

    void resizeLeftChannel(channel_index_type n) { resize(leftChannel, n); }

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
        setSampleRateHz(audioPlayer, 3);
        (this->*fade)();
        loadMonoAudio(player, audioReader, {0});
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

    void assertMonoAudioEqualsProductAfterFilling_BufferedAsyncFadeIn(
        const std::vector<float> &multiplicand,
        const std::vector<float> &multiplier, int buffers,
        int framesPerBuffer) {
        auto future{
            setOnPlayTask(audioPlayer, [=](AudioPlayer::Observer *observer) {
                std::vector<std::vector<float>> result;
                for (int i = 0; i < buffers; ++i) {
                    std::vector<float> mono(framesPerBuffer);
                    observer->fillAudioBuffer({mono}, {});
                    result.push_back(mono);
                }
                return result;
            })};
        player.fadeIn();
        auto audioBuffers{future.get()};
        for (int i = 0; i < buffers; ++i) {
            const auto offset = i * framesPerBuffer;
            assertChannelEqual(audioBuffers.at(i),
                elementWiseProduct(
                    subvector(multiplicand, offset, framesPerBuffer),
                    subvector(multiplier, offset, framesPerBuffer)));
        }
    }

    void assertStereoAudioEqualsProductAfterFilling_BufferedAsyncFadeIn(
        const std::vector<float> &multiplicand,
        const std::vector<float> &leftMultiplier,
        const std::vector<float> &rightMultiplier, int buffers,
        int framesPerBuffer) {
        auto future{
            setOnPlayTask(audioPlayer, [=](AudioPlayer::Observer *observer) {
                std::vector<std::vector<float>> result;
                for (int i = 0; i < buffers; ++i) {
                    std::vector<float> left(framesPerBuffer);
                    std::vector<float> right(framesPerBuffer);
                    observer->fillAudioBuffer({left, right}, {});
                    result.push_back(left);
                    result.push_back(right);
                }
                return result;
            })};
        player.fadeIn();
        auto audioBuffers{future.get()};
        for (int i = 0; i < buffers; ++i) {
            const auto offset = i * framesPerBuffer;
            assertChannelEqual(audioBuffers.at(2 * i),
                elementWiseProduct(
                    subvector(multiplicand, offset, framesPerBuffer),
                    subvector(leftMultiplier, offset, framesPerBuffer)));
            assertChannelEqual(audioBuffers.at(2 * i + 1),
                elementWiseProduct(
                    subvector(multiplicand, offset, framesPerBuffer),
                    subvector(rightMultiplier, offset, framesPerBuffer)));
        }
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

    void fadeInFillAndCallback(channel_index_type n) {
        fadeIn();
        fillAudioBufferMono(n);
        timerCallback();
    }

    void fadeOutFillAndCallback(channel_index_type n) {
        fadeOut();
        callbackAfterMonoFill(n);
    }

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
    loadMonoAudio(player, audioReader, std::vector<float>(9999999));
    std::vector<float> next(999);
    next.at(0) = 4;
    next.at(1) = 5;
    next.at(2) = 6;
    audioReader.set({next});
    audioPlayer.setRealisticExecution();
    player.play();
    std::this_thread::sleep_for(std::chrono::milliseconds{20});
    player.stop();
    loadFile(player);
    audioPlayer.clearRealisticExecution();
    audioPlayer.joinAudioThread();
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {4, 5, 6});
}

MASKER_PLAYER_TEST(playingWhenAudioPlayerPlaying) {
    audioPlayer.setPlaying();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(player.playing());
}

MASKER_PLAYER_TEST(durationReturnsDuration) {
    setSampleRateHz(audioPlayer, 3);
    loadMonoAudio(player, audioReader, {1, 2, 3, 4, 5, 6});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(6. / 3, player.duration().seconds);
}

MASKER_PLAYER_TEST(seekSeeksAudio) {
    setSampleRateHz(audioPlayer, 3);
    loadMonoAudio(player, audioReader, {1, 2, 3, 4, 5, 6, 7, 8, 9});
    seekSeconds(player, 2);
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {7, 8, 9, 1});
}

MASKER_PLAYER_TEST(seekNegativeTime) {
    setSampleRateHz(audioPlayer, 3);
    loadMonoAudio(player, audioReader, {1, 2, 3, 4, 5, 6, 7, 8, 9});
    seekSeconds(player, -2);
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {4, 5, 6, 7});
}

MASKER_PLAYER_TEST(setChannelDelayMono) {
    setSampleRateHz(audioPlayer, 3);
    setChannelDelaySeconds(player, 0, 1);
    loadMonoAudio(player, audioReader, {1, 2, 3});
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {0, 0, 0, 1, 2, 3});
}

MASKER_PLAYER_TEST(setChannelDelayAfterLoadMono) {
    setSampleRateHz(audioPlayer, 3);
    loadMonoAudio(player, audioReader, {1, 2, 3});
    setChannelDelaySeconds(player, 0, 1);
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {0, 0, 0, 1, 2, 3});
}

MASKER_PLAYER_TEST(setChannelDelayMono_Buffered) {
    setSampleRateHz(audioPlayer, 3);
    setChannelDelaySeconds(player, 0, 1);
    loadMonoAudio(player, audioReader, {4, 5, 6});
    auto future{
        setOnPlayTask(audioPlayer, [=](AudioPlayer::Observer *observer) {
            std::vector<float> first(2);
            std::vector<float> second(4);
            observer->fillAudioBuffer({first}, {});
            observer->fillAudioBuffer({second}, {});
            return std::vector<std::vector<float>>{first, second};
        })};
    player.play();
    auto twoMonoBuffers{future.get()};
    assertChannelEqual(twoMonoBuffers.at(0), {0, 0});
    assertChannelEqual(twoMonoBuffers.at(1), {0, 4, 5, 6});
}

MASKER_PLAYER_TEST(setChannelDelayMonoLoadNewAudio) {
    setSampleRateHz(audioPlayer, 3);
    setChannelDelaySeconds(player, 0, 1);
    loadMonoAudio(player, audioReader, {4, 5, 6});
    callInRealisticExecutionContext(player, audioPlayer, [&]() {
        loadMonoAudio(player, audioReader, {1, 2, 3});
    });
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {0, 0, 0});
}

MASKER_PLAYER_TEST(setChannelDelayMonoWithSeek) {
    setSampleRateHz(audioPlayer, 3);
    setChannelDelaySeconds(player, 0, 1);
    loadMonoAudio(player, audioReader, {1, 2, 3, 4, 5, 6, 7, 8, 9});
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {0, 0, 0, 1, 2, 3});
}

MASKER_PLAYER_TEST(setChannelDelayMonoWithSeek2) {
    setSampleRateHz(audioPlayer, 3);
    setChannelDelaySeconds(player, 0, 1);
    loadMonoAudio(player, audioReader, {1, 2, 3, 4, 5, 6, 7, 8, 9});
    callInRealisticExecutionContext(
        player, audioPlayer, [&]() { seekSeconds(player, 2); });
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {0, 0, 0, 7, 8, 9});
}

MASKER_PLAYER_TEST(clearChannelDelaysMono) {
    setSampleRateHz(audioPlayer, 3);
    setChannelDelaySeconds(player, 0, 1);
    loadMonoAudio(player, audioReader, {4, 5, 6});
    callInRealisticExecutionContext(player, audioPlayer, [&]() {
        clearChannelDelays(player);
        loadMonoAudio(player, audioReader, {1, 2, 3});
    });
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {1, 2, 3});
}

MASKER_PLAYER_TEST(clearChannelDelaysAfterLoadMono) {
    setSampleRateHz(audioPlayer, 3);
    setChannelDelaySeconds(player, 0, 1);
    loadMonoAudio(player, audioReader, {4, 5, 6});
    callInRealisticExecutionContext(player, audioPlayer, [&]() {
        loadMonoAudio(player, audioReader, {1, 2, 3});
        clearChannelDelays(player);
    });
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {1, 2, 3});
}

MASKER_PLAYER_TEST(setChannelDelayStereo) {
    setSampleRateHz(audioPlayer, 3);
    setChannelDelaySeconds(player, 1, 1);
    loadStereoAudio(
        player, audioReader, {1, 2, 3, 4, 5, 6}, {7, 8, 9, 10, 11, 12});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {1, 2, 3, 4, 5, 6}, {0, 0, 0, 7, 8, 9});
}

MASKER_PLAYER_TEST(setChannelDelayStereo_Buffered) {
    setSampleRateHz(audioPlayer, 3);
    setChannelDelaySeconds(player, 1, 1);
    loadStereoAudio(
        player, audioReader, {1, 2, 3, 4, 5, 6}, {7, 8, 9, 10, 11, 12});
    auto future{
        setOnPlayTask(audioPlayer, [=](AudioPlayer::Observer *observer) {
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
        })};
    player.play();
    auto sixMonoBuffers{future.get()};
    assertChannelEqual(sixMonoBuffers.at(0), {1, 2});
    assertChannelEqual(sixMonoBuffers.at(1), {0, 0});
    assertChannelEqual(sixMonoBuffers.at(2), {3, 4});
    assertChannelEqual(sixMonoBuffers.at(3), {0, 7});
    assertChannelEqual(sixMonoBuffers.at(4), {5, 6});
    assertChannelEqual(sixMonoBuffers.at(5), {8, 9});
}

MASKER_PLAYER_TEST(moreChannelsRequestedThanAvailableCopiesChannel) {
    loadMonoAudio(player, audioReader, {1, 2, 3});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {1, 2, 3}, {1, 2, 3});
}

MASKER_PLAYER_TEST(moreChannelsAvailableThanRequestedTruncates) {
    loadAudio(player, audioReader, {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {1, 2, 3}, {4, 5, 6});
}

MASKER_PLAYER_TEST(useFirstChannelOnlyMutesOtherChannels) {
    useFirstChannelOnly(player);
    loadStereoAudio(player, audioReader, {1, 2, 3}, {4, 5, 6});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {1, 2, 3}, {0, 0, 0});
}

void useSecondChannelOnly(MaskerPlayerImpl &player) {
    player.useSecondChannelOnly();
}

MASKER_PLAYER_TEST(
    useFirstChannelOnlyAfterUsingSecondChannelOnlyMutesOtherChannels) {
    useSecondChannelOnly(player);
    useFirstChannelOnly(player);
    loadStereoAudio(player, audioReader, {1, 2, 3}, {4, 5, 6});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {1, 2, 3}, {0, 0, 0});
}

MASKER_PLAYER_TEST(useSecondChannelOnlyMutesOtherChannels) {
    useSecondChannelOnly(player);
    loadStereoAudio(player, audioReader, {1, 2, 3}, {4, 5, 6});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {0, 0, 0}, {4, 5, 6});
}

MASKER_PLAYER_TEST(
    useSecondChannelOnlyAfterUsingFirstChannelOnlyMutesOtherChannels) {
    useFirstChannelOnly(player);
    useSecondChannelOnly(player);
    loadStereoAudio(player, audioReader, {1, 2, 3}, {4, 5, 6});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {0, 0, 0}, {4, 5, 6});
}

MASKER_PLAYER_TEST(useAllChannelsAfterUsingFirstChannelOnly) {
    useFirstChannelOnly(player);
    useAllChannels(player);
    loadStereoAudio(player, audioReader, {1, 2, 3}, {4, 5, 6});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {1, 2, 3}, {4, 5, 6});
}

MASKER_PLAYER_TEST(useAllChannelsAfterUsingSecondChannelOnly) {
    useSecondChannelOnly(player);
    useAllChannels(player);
    loadStereoAudio(player, audioReader, {1, 2, 3}, {4, 5, 6});
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
    loadFile(player, "a");
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
    loadMonoAudio(player, audioReader, {1, 2, 3});
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {10, 20, 30});
}

MASKER_PLAYER_TEST(loadFileResetsSampleIndex) {
    loadMonoAudio(player, audioReader, {1, 2, 3});
    callInRealisticExecutionContext(player, audioPlayer, [&]() {
        loadMonoAudio(player, audioReader, {4, 5, 6});
    });
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {4, 5, 6});
}

MASKER_PLAYER_TEST(fillAudioBufferWrapsMonoChannel) {
    loadMonoAudio(player, audioReader, {1, 2, 3});
    assertAsyncLoadedMonoChannelEquals(player, audioPlayer, {1, 2, 3, 1});
}

MASKER_PLAYER_TEST(fillAudioBufferWrapsMonoChannel_Buffered) {
    loadMonoAudio(player, audioReader, {1, 2, 3});
    auto future{
        setOnPlayTask(audioPlayer, [=](AudioPlayer::Observer *observer) {
            std::vector<float> first(4);
            std::vector<float> second(4);
            observer->fillAudioBuffer({first}, {});
            observer->fillAudioBuffer({second}, {});
            return std::vector<std::vector<float>>{first, second};
        })};
    player.play();
    auto twoMonoBuffers{future.get()};
    assertChannelEqual(twoMonoBuffers.at(0), {1, 2, 3, 1});
    assertChannelEqual(twoMonoBuffers.at(1), {2, 3, 1, 2});
}

MASKER_PLAYER_TEST(fillAudioBufferWrapsStereoChannel) {
    loadStereoAudio(player, audioReader, {1, 2, 3}, {4, 5, 6});
    assertAsyncLoadedStereoChannelsEquals(
        player, audioPlayer, {1, 2, 3, 1}, {4, 5, 6, 4});
}

MASKER_PLAYER_TEST(fillAudioBufferWrapsStereoChannel_Buffered) {
    loadStereoAudio(player, audioReader, {1, 2, 3}, {4, 5, 6});
    auto future{
        setOnPlayTask(audioPlayer, [=](AudioPlayer::Observer *observer) {
            std::vector<float> firstLeft(2);
            std::vector<float> firstRight(2);
            std::vector<float> secondLeft(4);
            std::vector<float> secondRight(4);
            observer->fillAudioBuffer({firstLeft, firstRight}, {});
            observer->fillAudioBuffer({secondLeft, secondRight}, {});
            return std::vector<std::vector<float>>{
                firstLeft, firstRight, secondLeft, secondRight};
        })};
    player.play();
    auto fourMonoBuffers{future.get()};
    assertChannelEqual(fourMonoBuffers.at(0), {1, 2});
    assertChannelEqual(fourMonoBuffers.at(1), {4, 5});
    assertChannelEqual(fourMonoBuffers.at(2), {3, 1, 2, 3});
    assertChannelEqual(fourMonoBuffers.at(3), {6, 4, 5, 6});
}

MASKER_PLAYER_TEST(fadesInAccordingToHannFunctionMultipleFills) {
    // For this test:
    // halfWindowLength is determined by fade time and sample rate...
    // but must be divisible by framesPerBuffer.
    setFadeInOutSeconds(3);
    setSampleRateHz(audioPlayer, 5);
    auto halfWindowLength = 3 * 5 + 1;
    auto framesPerBuffer = 4;

    loadMonoAudio(player, audioReader, oneToN(halfWindowLength));
    const auto buffers = halfWindowLength / framesPerBuffer;
    assertMonoAudioEqualsProductAfterFilling_BufferedAsyncFadeIn(
        halfHannWindow(halfWindowLength), oneToN(halfWindowLength), buffers,
        framesPerBuffer);
}

MASKER_PLAYER_TEST(fadesInAccordingToHannFunctionOneFill) {
    setFadeInOutSeconds(2);
    setSampleRateHz(audioPlayer, 3);
    auto halfWindowLength = 2 * 3 + 1;

    loadMonoAudio(player, audioReader, oneToN(halfWindowLength));
    auto future{
        setOnPlayTask(audioPlayer, [=](AudioPlayer::Observer *observer) {
            return av_speech_in_noise::fillAudioBuffer(
                observer, 1, halfWindowLength);
        })};
    fadeIn();
    assertChannelEqual(future.get().at(0),
        elementWiseProduct(
            halfHannWindow(halfWindowLength), oneToN(halfWindowLength)));
}

MASKER_PLAYER_TEST(fadesInAccordingToHannFunctionStereoMultipleFills) {
    // For this test:
    // halfWindowLength is determined by fade time and sample rate...
    // but must be divisible by framesPerBuffer.
    setFadeInOutSeconds(3);
    setSampleRateHz(audioPlayer, 5);
    auto halfWindowLength = 3 * 5 + 1;
    auto framesPerBuffer = 4;

    loadStereoAudio(player, audioReader, oneToN(halfWindowLength),
        NtoOne(halfWindowLength));
    auto buffers = halfWindowLength / framesPerBuffer;
    assertStereoAudioEqualsProductAfterFilling_BufferedAsyncFadeIn(
        halfHannWindow(halfWindowLength), oneToN(halfWindowLength),
        NtoOne(halfWindowLength), buffers, framesPerBuffer);
}

MASKER_PLAYER_TEST(fadesInAccordingToHannFunctionStereoOneFill) {
    setFadeInOutSeconds(2);
    setSampleRateHz(audioPlayer, 3);
    auto halfWindowLength = 2 * 3 + 1;

    loadStereoAudio(player, audioReader, oneToN(halfWindowLength),
        NtoOne(halfWindowLength));
    auto future{
        setOnPlayTask(audioPlayer, [=](AudioPlayer::Observer *observer) {
            return av_speech_in_noise::fillAudioBuffer(
                observer, 2, halfWindowLength);
        })};
    fadeIn();
    assertStereoChannelsEqual(future.get(),
        elementWiseProduct(
            halfHannWindow(halfWindowLength), oneToN(halfWindowLength)),
        elementWiseProduct(
            halfHannWindow(halfWindowLength), NtoOne(halfWindowLength)));
}

MASKER_PLAYER_TEST(steadyLevelFollowingFadeIn) {
    setFadeInOutSeconds(2);
    setSampleRateHz(audioPlayer, 3);
    loadMonoAudio(player, audioReader, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    auto halfWindowLength = 2 * 3 + 1;
    auto future{
        setOnPlayTask(audioPlayer, [=](AudioPlayer::Observer *observer) {
            std::vector<float> first(halfWindowLength);
            std::vector<float> second(3);
            observer->fillAudioBuffer({first}, {});
            observer->fillAudioBuffer({second}, {});
            return std::vector<std::vector<float>>{first, second};
        })};
    fadeIn();
    assertChannelEqual(future.get().at(1), {8, 9, 10});
}

void assertOnPlayTaskAfterFadeOut(MaskerPlayerImpl &player,
    AudioPlayerStub &audioPlayer, TimerStub &timer, gsl::index halfWindowLength,
    const std::function<std::vector<std::vector<float>>(
        AudioPlayer::Observer *)> &afterFadeOut,
    const std::function<void(std::future<std::vector<std::vector<float>>>)>
        &assertion) {
    bool fadeInComplete{};
    bool fadeOutCalled{};
    std::mutex mutex{};
    std::condition_variable condition{};
    auto future{
        setOnPlayTask(audioPlayer, [&](AudioPlayer::Observer *observer) {
            std::vector<float> fadeInBuffer(halfWindowLength);
            observer->fillAudioBuffer({fadeInBuffer}, {});
            {
                std::lock_guard<std::mutex> lock{mutex};
                fadeInComplete = true;
            }
            condition.notify_one();
            {
                std::unique_lock<std::mutex> lock{mutex};
                condition.wait(lock, [&] { return fadeOutCalled; });
            }
            return afterFadeOut(observer);
        })};
    fadeIn(player);
    {
        std::unique_lock<std::mutex> lock{mutex};
        condition.wait(lock, [&] { return fadeInComplete; });
    }
    callback(timer);
    fadeOut(player);
    {
        std::lock_guard<std::mutex> lock{mutex};
        fadeOutCalled = true;
    }
    condition.notify_one();
    assertion(std::move(future));
}

MASKER_PLAYER_TEST(fadesOutAccordingToHannFunctionMultipleFills) {
    // For this test:
    // halfWindowLength is determined by fade time and sample rate...
    // but must be divisible by framesPerBuffer.
    setFadeInOutSeconds(3);
    setSampleRateHz(audioPlayer, 5);
    auto halfWindowLength = 3 * 5 + 1;
    auto framesPerBuffer = 4;
    loadMonoAudio(player, audioReader, oneToN(halfWindowLength));
    assertOnPlayTaskAfterFadeOut(
        player, audioPlayer, timer, halfWindowLength,
        [=](AudioPlayer::Observer *observer) {
            std::vector<std::vector<float>> result;
            for (int i = 0; i < halfWindowLength / framesPerBuffer; ++i) {
                std::vector<float> mono(framesPerBuffer);
                observer->fillAudioBuffer({mono}, {});
                result.push_back(mono);
            }
            return result;
        },
        [=](std::future<std::vector<std::vector<float>>> future) {
            auto audioBuffers{future.get()};
            for (int i = 0; i < halfWindowLength / framesPerBuffer; ++i) {
                const auto offset = i * framesPerBuffer;
                assertChannelEqual(audioBuffers.at(i),
                    elementWiseProduct(
                        subvector(backHalfHannWindow(halfWindowLength), offset,
                            framesPerBuffer),
                        subvector(oneToN(halfWindowLength), offset,
                            framesPerBuffer)));
            }
        });
}

MASKER_PLAYER_TEST(fadesOutAccordingToHannFunctionOneFill) {
    setFadeInOutSeconds(2);
    setSampleRateHz(audioPlayer, 3);
    auto halfWindowLength = 2 * 3 + 1;

    loadMonoAudio(player, audioReader, oneToN(halfWindowLength));
    assertOnPlayTaskAfterFadeOut(
        player, audioPlayer, timer, halfWindowLength,
        [=](AudioPlayer::Observer *observer) {
            return av_speech_in_noise::fillAudioBuffer(
                observer, 1, halfWindowLength);
        },
        [=](std::future<std::vector<std::vector<float>>> future) {
            assertChannelEqual(future.get().at(0),
                elementWiseProduct(backHalfHannWindow(halfWindowLength),
                    oneToN(halfWindowLength)));
        });
}

MASKER_PLAYER_TEST(steadyLevelFollowingFadeOut) {
    setFadeInOutSeconds(2);
    setSampleRateHz(audioPlayer, 3);
    auto halfWindowLength = 2 * 3 + 1;

    loadMonoAudio(player, audioReader, {4, 5, 6});
    assertOnPlayTaskAfterFadeOut(
        player, audioPlayer, timer, halfWindowLength,
        [=](AudioPlayer::Observer *observer) {
            av_speech_in_noise::fillAudioBuffer(observer, 1, halfWindowLength);
            return av_speech_in_noise::fillAudioBuffer(observer, 1, 3);
        },
        [=](std::future<std::vector<std::vector<float>>> future) {
            assertChannelEqual(future.get().at(0), {0, 0, 0});
        });
}

MASKER_PLAYER_TEST(DISABLED_fadeInCompleteOnlyAfterFadeTime) {
    setFadeInOutSeconds(3);
    setSampleRateHz(audioPlayer, 4);

    loadMonoAudio(player, audioReader, {0});
    fadeIn();
    for (int i = 0; i < 3 * 4; ++i)
        assertFadeInNotCompletedAfterMonoFill(1);
    assertFadeInCompletedAfterMonoFill(1);
}

MASKER_PLAYER_TEST(DISABLED_fadeInCompletePassesSystemTimeAndSampleOffset) {
    setFadeInOutSeconds(3);
    setSampleRateHz(audioPlayer, 4);

    loadMonoAudio(player, audioReader, {0});
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
    setSampleRateHz(audioPlayer, 4);
    loadMonoAudio(player, audioReader, {0});
    fadeInFillAndCallback(3 * 4 + 1);

    fadeOut();
    for (int i = 0; i < 3 * 4; ++i)
        assertFadeOutNotCompletedAfterMonoFill(1);
    assertFadeOutCompletedAfterMonoFill(1);
}

MASKER_PLAYER_TEST(DISABLED_observerNotifiedOnceForFadeOut) {
    setFadeInOutSeconds(2);
    setSampleRateHz(audioPlayer, 3);
    auto halfWindowLength = 2 * 3 + 1;
    loadMonoAudio(player, audioReader, {0});
    fadeInFillAndCallback(halfWindowLength);
    fadeOutFillAndCallback(halfWindowLength);

    assertFadeOutCompletions(1);
    callbackAfterMonoFill();
    assertFadeOutCompletions(1);
}

MASKER_PLAYER_TEST(DISABLED_audioPlayerStoppedOnlyAtEndOfFadeOutTime) {
    setFadeInOutSeconds(3);
    setSampleRateHz(audioPlayer, 4);
    auto halfWindowLength = 3 * 4 + 1;
    loadMonoAudio(player, audioReader, {0});
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
    loadAudio(player, audioReader, {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        20 * std::log10(std::sqrt((1 * 1 + 2 * 2 + 3 * 3) / 3.F)),
        digitalLevel().dBov);
}

MASKER_PLAYER_TEST(digitalLevelPassesLoadedFileToVideoPlayer) {
    loadFile(player, "a");
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
        loadFile(player);
        FAIL() << "Expected av_coordinate_response_measure::InvalidAudioFile";
    } catch (const InvalidAudioFile &) {
    }
}
}
}
