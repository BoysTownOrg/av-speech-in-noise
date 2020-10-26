#include "assert-utility.hpp"
#include "AudioReaderStub.hpp"
#include <stimulus-players/AudioReader.hpp>
#include <stimulus-players/MaskerPlayerImpl.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>
#include <cmath>
#include <algorithm>
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
        ++callbacksScheduled_;
    }

    [[nodiscard]] auto callbackScheduled() const { return callbackScheduled_; }

    void clearCallbackCount() {
        callbackScheduled_ = false;
        callbacksScheduled_ = 0;
    }

    void callback() { observer->callback(); }

    void attach(Observer *a) override { observer = a; }

    [[nodiscard]] auto callbacksScheduled() const -> int {
        return callbacksScheduled_;
    }

  private:
    Observer *observer{};
    int callbacksScheduled_{};
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

auto reverse(std::vector<float> x) -> std::vector<float> {
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

auto concatenate(std::vector<float> first, const std::vector<float> &second)
    -> std::vector<float> {
    first.insert(first.end(), second.begin(), second.end());
    return first;
}

auto zeros(int N) -> std::vector<float> { return std::vector<float>(N); }

auto NtoOne(int N) -> std::vector<float> { return reverse(oneToN(N)); }

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

auto fillAudioBufferMono(AudioPlayer::Observer *observer, gsl::index frames,
    player_system_time_type t = {}) -> std::vector<float> {
    return fillAudioBuffer(observer, 1, frames, t).front();
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

void assertAsyncFilledMonoAudioEquals(MaskerPlayerImpl &player,
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

void callback(TimerStub &timer) { timer.callback(); }

void setAudioDevice(MaskerPlayerImpl &player, std::string s) {
    player.setAudioDevice(std::move(s));
}

void setFadeInOutSeconds(MaskerPlayerImpl &player, double x) {
    player.setFadeInOutSeconds(x);
}

void set(std::mutex &mutex, bool &b) {
    std::lock_guard<std::mutex> lock{mutex};
    b = true;
}

void wait(std::mutex &mutex, std::condition_variable &condition, bool &b) {
    std::unique_lock<std::mutex> lock{mutex};
    condition.wait(lock, [&] { return b; });
}

void waitThenClear(
    std::mutex &mutex, std::condition_variable &condition, bool &b) {
    std::unique_lock<std::mutex> lock{mutex};
    condition.wait(lock, [&] { return b; });
    b = false;
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

    MaskerPlayerTests() { player.attach(&listener); }

    void fillAudioBuffer(const std::vector<gsl::span<float>> &audio,
        player_system_time_type t = {}) {
        audioPlayer.fillAudioBuffer(audio, t);
    }

    void fillAudioBufferMono(
        channel_index_type n, player_system_time_type t = {}) {
        resize(leftChannel, n);
        fillAudioBuffer({leftChannel}, t);
    }

    void setAudioDeviceDescriptions(std::vector<std::string> v) {
        audioPlayer.setAudioDeviceDescriptions(std::move(v));
    }

    void setAsOutputDevice(int i) { audioPlayer.setAsOutputDevice(i); }

    void assertCallbackScheduled() {
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(callbackScheduled());
    }

    auto callbackScheduled() -> bool { return timer.callbackScheduled(); }

    void assertCallbackNotScheduled() {
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(callbackScheduled());
    }

    void callbackAfterMonoFill(channel_index_type n = 0) {
        fillAudioBufferMono(n);
        callback(timer);
    }

    void clearCallbackCount() { timer.clearCallbackCount(); }

    void assertTimerCallbackDoesNotScheduleAdditionalCallback() {
        assertCallDoesNotScheduleAdditionalCallback([&] { callback(timer); });
    }

    void assertCallDoesNotScheduleAdditionalCallback(
        const std::function<void()> &f) {
        clearCallbackCount();
        f();
        assertCallbackNotScheduled();
    }

    void assertFadeInDoesNotScheduleAdditionalCallback() {
        assertCallDoesNotScheduleAdditionalCallback([&] { fadeIn(player); });
    }

    void assertFadeInSchedulesCallback() {
        clearCallbackCount();
        fadeIn(player);
        assertCallbackScheduled();
    }

    auto playerStopped() -> bool { return audioPlayer.stopped(); }

    void assertFadeInCompletions(int n) {
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(n, listener.fadeInCompletions());
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
    assertAsyncFilledMonoAudioEquals(player, audioPlayer, {4, 5, 6});
}

MASKER_PLAYER_TEST(fadeOutThenLoad) {
    loadMonoAudio(player, audioReader, std::vector<float>(9999999));
    std::vector<float> next(999);
    next.at(0) = 4;
    next.at(1) = 5;
    next.at(2) = 6;
    audioReader.set({next});
    audioPlayer.setRealisticExecution();
    fadeIn(player);
    while (!listener.fadeOutCompleted()) {
        callback(timer);
        std::this_thread::sleep_for(std::chrono::milliseconds{20});
    }
    loadFile(player);
    audioPlayer.clearRealisticExecution();
    audioPlayer.joinAudioThread();
    assertAsyncFilledMonoAudioEquals(player, audioPlayer, {4, 5, 6});
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
    assertAsyncFilledMonoAudioEquals(player, audioPlayer, {7, 8, 9, 1});
}

MASKER_PLAYER_TEST(seekNegativeTime) {
    setSampleRateHz(audioPlayer, 3);
    loadMonoAudio(player, audioReader, {1, 2, 3, 4, 5, 6, 7, 8, 9});
    seekSeconds(player, -2);
    assertAsyncFilledMonoAudioEquals(player, audioPlayer, {4, 5, 6, 7});
}

MASKER_PLAYER_TEST(setChannelDelayMono) {
    setSampleRateHz(audioPlayer, 3);
    setChannelDelaySeconds(player, 0, 1);
    loadMonoAudio(player, audioReader, {1, 2, 3});
    assertAsyncFilledMonoAudioEquals(player, audioPlayer, {0, 0, 0, 1, 2, 3});
}

MASKER_PLAYER_TEST(setChannelDelayAfterLoadMono) {
    setSampleRateHz(audioPlayer, 3);
    loadMonoAudio(player, audioReader, {1, 2, 3});
    setChannelDelaySeconds(player, 0, 1);
    assertAsyncFilledMonoAudioEquals(player, audioPlayer, {0, 0, 0, 1, 2, 3});
}

MASKER_PLAYER_TEST(setChannelDelayMono_Buffered) {
    setSampleRateHz(audioPlayer, 3);
    setChannelDelaySeconds(player, 0, 1);
    loadMonoAudio(player, audioReader, {4, 5, 6});
    auto future{
        setOnPlayTask(audioPlayer, [=](AudioPlayer::Observer *observer) {
            const auto first{
                av_speech_in_noise::fillAudioBufferMono(observer, 2)};
            const auto second{
                av_speech_in_noise::fillAudioBufferMono(observer, 4)};
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
    assertAsyncFilledMonoAudioEquals(player, audioPlayer, {0, 0, 0});
}

MASKER_PLAYER_TEST(setChannelDelayMonoWithSeek) {
    setSampleRateHz(audioPlayer, 3);
    setChannelDelaySeconds(player, 0, 1);
    loadMonoAudio(player, audioReader, {1, 2, 3, 4, 5, 6, 7, 8, 9});
    assertAsyncFilledMonoAudioEquals(player, audioPlayer, {0, 0, 0, 1, 2, 3});
}

MASKER_PLAYER_TEST(setChannelDelayMonoWithSeek2) {
    setSampleRateHz(audioPlayer, 3);
    setChannelDelaySeconds(player, 0, 1);
    loadMonoAudio(player, audioReader, {1, 2, 3, 4, 5, 6, 7, 8, 9});
    callInRealisticExecutionContext(
        player, audioPlayer, [&]() { seekSeconds(player, 2); });
    assertAsyncFilledMonoAudioEquals(player, audioPlayer, {0, 0, 0, 7, 8, 9});
}

MASKER_PLAYER_TEST(clearChannelDelaysMono) {
    setSampleRateHz(audioPlayer, 3);
    setChannelDelaySeconds(player, 0, 1);
    loadMonoAudio(player, audioReader, {4, 5, 6});
    callInRealisticExecutionContext(player, audioPlayer, [&]() {
        clearChannelDelays(player);
        loadMonoAudio(player, audioReader, {1, 2, 3});
    });
    assertAsyncFilledMonoAudioEquals(player, audioPlayer, {1, 2, 3});
}

MASKER_PLAYER_TEST(clearChannelDelaysAfterLoadMono) {
    setSampleRateHz(audioPlayer, 3);
    setChannelDelaySeconds(player, 0, 1);
    loadMonoAudio(player, audioReader, {4, 5, 6});
    callInRealisticExecutionContext(player, audioPlayer, [&]() {
        loadMonoAudio(player, audioReader, {1, 2, 3});
        clearChannelDelays(player);
    });
    assertAsyncFilledMonoAudioEquals(player, audioPlayer, {1, 2, 3});
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
            const auto first{
                av_speech_in_noise::fillAudioBuffer(observer, 2, 2)};
            const auto second{
                av_speech_in_noise::fillAudioBuffer(observer, 2, 2)};
            const auto third{
                av_speech_in_noise::fillAudioBuffer(observer, 2, 2)};
            return std::vector<std::vector<float>>{first.at(0), first.at(1),
                second.at(0), second.at(1), third.at(0), third.at(1)};
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
    assertAsyncFilledMonoAudioEquals(player, audioPlayer, {0, 0, 0});
}

MASKER_PLAYER_TEST(fadeTimeReturnsFadeTime) {
    setFadeInOutSeconds(player, 1);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1., player.fadeTime().seconds);
}

MASKER_PLAYER_TEST(loadFileLoadsAudioFile) {
    loadFile(player, "a");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, audioPlayer.filePath());
}

MASKER_PLAYER_TEST(fadeInPlaysAudioPlayer) {
    fadeIn(player);
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
    assertAsyncFilledMonoAudioEquals(player, audioPlayer, {10, 20, 30});
}

MASKER_PLAYER_TEST(loadFileResetsSampleIndex) {
    loadMonoAudio(player, audioReader, {1, 2, 3});
    callInRealisticExecutionContext(player, audioPlayer, [&]() {
        loadMonoAudio(player, audioReader, {4, 5, 6});
    });
    assertAsyncFilledMonoAudioEquals(player, audioPlayer, {4, 5, 6});
}

MASKER_PLAYER_TEST(fillAudioBufferWrapsMonoChannel) {
    loadMonoAudio(player, audioReader, {1, 2, 3});
    assertAsyncFilledMonoAudioEquals(player, audioPlayer, {1, 2, 3, 1});
}

MASKER_PLAYER_TEST(fillAudioBufferWrapsMonoChannel_Buffered) {
    loadMonoAudio(player, audioReader, {1, 2, 3});
    auto future{
        setOnPlayTask(audioPlayer, [=](AudioPlayer::Observer *observer) {
            const auto first{
                av_speech_in_noise::fillAudioBufferMono(observer, 4)};
            const auto second{
                av_speech_in_noise::fillAudioBufferMono(observer, 4)};
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
            const auto first{
                av_speech_in_noise::fillAudioBuffer(observer, 2, 2)};
            const auto second{
                av_speech_in_noise::fillAudioBuffer(observer, 2, 4)};
            return std::vector<std::vector<float>>{
                first.at(0), first.at(1), second.at(0), second.at(1)};
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
    setFadeInOutSeconds(player, 3);
    setSampleRateHz(audioPlayer, 5);
    auto halfWindowLength = 3 * 5 + 1;
    auto framesPerBuffer = 4;

    loadMonoAudio(player, audioReader, oneToN(halfWindowLength));
    const auto buffers = halfWindowLength / framesPerBuffer;
    auto future{
        setOnPlayTask(audioPlayer, [=](AudioPlayer::Observer *observer) {
            std::vector<std::vector<float>> result(buffers);
            for (int i = 0; i < buffers; ++i)
                result.at(i) = av_speech_in_noise::fillAudioBufferMono(
                    observer, framesPerBuffer);
            return result;
        })};
    fadeIn(player);
    const auto audioBuffers{future.get()};
    for (int i = 0; i < buffers; ++i) {
        const auto offset = i * framesPerBuffer;
        assertChannelEqual(audioBuffers.at(i),
            elementWiseProduct(subvector(halfHannWindow(halfWindowLength),
                                   offset, framesPerBuffer),
                subvector(oneToN(halfWindowLength), offset, framesPerBuffer)));
    }
}

MASKER_PLAYER_TEST(fadesInAccordingToHannFunctionOneFill) {
    setFadeInOutSeconds(player, 2);
    setSampleRateHz(audioPlayer, 3);
    auto halfWindowLength = 2 * 3 + 1;

    loadMonoAudio(player, audioReader, oneToN(halfWindowLength));
    auto future{
        setOnPlayTask(audioPlayer, [=](AudioPlayer::Observer *observer) {
            return av_speech_in_noise::fillAudioBuffer(
                observer, 1, halfWindowLength);
        })};
    fadeIn(player);
    assertChannelEqual(future.get().at(0),
        elementWiseProduct(
            halfHannWindow(halfWindowLength), oneToN(halfWindowLength)));
}

MASKER_PLAYER_TEST(fadesInAccordingToHannFunctionStereoMultipleFills) {
    // For this test:
    // halfWindowLength is determined by fade time and sample rate...
    // but must be divisible by framesPerBuffer.
    setFadeInOutSeconds(player, 3);
    setSampleRateHz(audioPlayer, 5);
    auto halfWindowLength = 3 * 5 + 1;
    auto framesPerBuffer = 4;

    loadStereoAudio(player, audioReader, oneToN(halfWindowLength),
        NtoOne(halfWindowLength));
    auto buffers = halfWindowLength / framesPerBuffer;
    auto future{
        setOnPlayTask(audioPlayer, [=](AudioPlayer::Observer *observer) {
            std::vector<std::vector<float>> result;
            for (int i = 0; i < buffers; ++i) {
                const auto stereo{av_speech_in_noise::fillAudioBuffer(
                    observer, 2, framesPerBuffer)};
                result.push_back(stereo.at(0));
                result.push_back(stereo.at(1));
            }
            return result;
        })};
    fadeIn(player);
    const auto audioBuffers{future.get()};
    for (int i = 0; i < buffers; ++i) {
        const auto offset = i * framesPerBuffer;
        assertChannelEqual(audioBuffers.at(2 * i),
            elementWiseProduct(subvector(halfHannWindow(halfWindowLength),
                                   offset, framesPerBuffer),
                subvector(oneToN(halfWindowLength), offset, framesPerBuffer)));
        assertChannelEqual(audioBuffers.at(2 * i + 1),
            elementWiseProduct(subvector(halfHannWindow(halfWindowLength),
                                   offset, framesPerBuffer),
                subvector(NtoOne(halfWindowLength), offset, framesPerBuffer)));
    }
}

MASKER_PLAYER_TEST(fadesInAccordingToHannFunctionStereoOneFill) {
    setFadeInOutSeconds(player, 2);
    setSampleRateHz(audioPlayer, 3);
    auto halfWindowLength = 2 * 3 + 1;

    loadStereoAudio(player, audioReader, oneToN(halfWindowLength),
        NtoOne(halfWindowLength));
    auto future{
        setOnPlayTask(audioPlayer, [=](AudioPlayer::Observer *observer) {
            return av_speech_in_noise::fillAudioBuffer(
                observer, 2, halfWindowLength);
        })};
    fadeIn(player);
    assertStereoChannelsEqual(future.get(),
        elementWiseProduct(
            halfHannWindow(halfWindowLength), oneToN(halfWindowLength)),
        elementWiseProduct(
            halfHannWindow(halfWindowLength), NtoOne(halfWindowLength)));
}

void setSteadyLevelSeconds(MaskerPlayerImpl &player, double seconds) {
    player.setSteadyLevelFor(Duration{seconds});
}

MASKER_PLAYER_TEST(steadyLevelFollowingFadeIn) {
    setFadeInOutSeconds(player, 2);
    setSampleRateHz(audioPlayer, 3);
    setSteadyLevelSeconds(player, 1);
    loadMonoAudio(player, audioReader, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    auto halfWindowLength = 2 * 3 + 1;
    auto future{
        setOnPlayTask(audioPlayer, [=](AudioPlayer::Observer *observer) {
            const auto first{av_speech_in_noise::fillAudioBufferMono(
                observer, halfWindowLength)};
            const auto second{
                av_speech_in_noise::fillAudioBufferMono(observer, 3)};
            return std::vector<std::vector<float>>{first, second};
        })};
    fadeIn(player);
    assertChannelEqual(future.get().at(1), {8, 9, 10});
}

void assertOnPlayTaskAfterFadeOut(MaskerPlayerImpl &player,
    AudioPlayerStub &audioPlayer, TimerStub &timer, gsl::index halfWindowLength,
    const std::function<std::vector<std::vector<float>>(
        AudioPlayer::Observer *)> &afterFadeOut,
    const std::function<void(const std::vector<std::vector<float>> &)>
        &assertion,
    gsl::index steadyLevelLength) {
    auto future{
        setOnPlayTask(audioPlayer, [&](AudioPlayer::Observer *observer) {
            std::vector<float> fadeInBuffer(halfWindowLength);
            observer->fillAudioBuffer({fadeInBuffer}, {});
            std::vector<float> steadyLevelBuffer(steadyLevelLength);
            observer->fillAudioBuffer({steadyLevelBuffer}, {});
            return afterFadeOut(observer);
        })};
    fadeIn(player);
    assertion(future.get());
}

MASKER_PLAYER_TEST(fadesOutAccordingToHannFunctionMultipleFills) {
    // For this test:
    // halfWindowLength is determined by fade time and sample rate...
    // but must be divisible by framesPerBuffer.
    setFadeInOutSeconds(player, 3);
    setSampleRateHz(audioPlayer, 5);
    setSteadyLevelSeconds(player, 7);
    auto steadyLevelLength = 5 * 7 + 1;
    auto halfWindowLength = 3 * 5 + 1;
    auto framesPerBuffer = 4;
    loadMonoAudio(player, audioReader,
        concatenate(oneToN(halfWindowLength), zeros(steadyLevelLength)));
    assertOnPlayTaskAfterFadeOut(
        player, audioPlayer, timer, halfWindowLength,
        [=](AudioPlayer::Observer *observer) {
            std::vector<std::vector<float>> result(
                halfWindowLength / framesPerBuffer);
            std::generate(result.begin(), result.end(), [=] {
                return av_speech_in_noise::fillAudioBufferMono(
                    observer, framesPerBuffer);
            });
            return result;
        },
        [=](const std::vector<std::vector<float>> &audioBuffers) {
            for (int i = 0; i < halfWindowLength / framesPerBuffer; ++i) {
                const auto offset = i * framesPerBuffer;
                assertChannelEqual(audioBuffers.at(i),
                    elementWiseProduct(
                        subvector(backHalfHannWindow(halfWindowLength), offset,
                            framesPerBuffer),
                        subvector(oneToN(halfWindowLength), offset,
                            framesPerBuffer)));
            }
        },
        steadyLevelLength);
}

MASKER_PLAYER_TEST(fadesOutAccordingToHannFunctionOneFill) {
    setFadeInOutSeconds(player, 2);
    setSampleRateHz(audioPlayer, 3);
    setSteadyLevelSeconds(player, 5);
    auto steadyLevelLength = 3 * 5 + 1;
    auto halfWindowLength = 2 * 3 + 1;

    loadMonoAudio(player, audioReader,
        concatenate(oneToN(halfWindowLength), zeros(steadyLevelLength)));
    assertOnPlayTaskAfterFadeOut(
        player, audioPlayer, timer, halfWindowLength,
        [=](AudioPlayer::Observer *observer) {
            return av_speech_in_noise::fillAudioBuffer(
                observer, 1, halfWindowLength);
        },
        [=](const std::vector<std::vector<float>> &audioBuffers) {
            assertChannelEqual(audioBuffers.at(0),
                elementWiseProduct(backHalfHannWindow(halfWindowLength),
                    oneToN(halfWindowLength)));
        },
        steadyLevelLength);
}

MASKER_PLAYER_TEST(steadyLevelFollowingFadeOut) {
    setFadeInOutSeconds(player, 2);
    setSampleRateHz(audioPlayer, 3);
    setSteadyLevelSeconds(player, 5);
    auto steadyLevelLength = 3 * 5 + 1;
    auto halfWindowLength = 2 * 3 + 1;

    loadMonoAudio(player, audioReader, {4, 5, 6});
    assertOnPlayTaskAfterFadeOut(
        player, audioPlayer, timer, halfWindowLength,
        [=](AudioPlayer::Observer *observer) {
            av_speech_in_noise::fillAudioBuffer(observer, 1, halfWindowLength);
            return av_speech_in_noise::fillAudioBuffer(observer, 1, 3);
        },
        [=](const std::vector<std::vector<float>> &audioBuffers) {
            assertChannelEqual(audioBuffers.at(0), {0, 0, 0});
        },
        steadyLevelLength);
}

MASKER_PLAYER_TEST(fadeInCompleteOnlyAfterFadeTime) {
    setFadeInOutSeconds(player, 3);
    setSampleRateHz(audioPlayer, 4);

    loadMonoAudio(player, audioReader, {0});
    bool fillOnce{};
    bool filledOnce{};
    std::mutex mutex{};
    std::condition_variable condition{};
    auto future{
        setOnPlayTask(audioPlayer, [&](AudioPlayer::Observer *observer) {
            for (int i = 0; i < 3 * 4 + 1; ++i) {
                waitThenClear(mutex, condition, fillOnce);
                av_speech_in_noise::fillAudioBuffer(observer, 1, 1);
                set(mutex, filledOnce);
                condition.notify_one();
            }
            return std::vector<std::vector<float>>{};
        })};
    fadeIn(player);
    for (int i = 0; i < 3 * 4; ++i) {
        set(mutex, fillOnce);
        condition.notify_one();
        waitThenClear(mutex, condition, filledOnce);
        callback(timer);
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(listener.fadeInCompleted());
    }
    set(mutex, fillOnce);
    condition.notify_one();
    wait(mutex, condition, filledOnce);
    callback(timer);
    future.get();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(listener.fadeInCompleted());
}

MASKER_PLAYER_TEST(fadeInCompletePassesSystemTimeAndSampleOffset) {
    setFadeInOutSeconds(player, 3);
    setSampleRateHz(audioPlayer, 4);

    loadMonoAudio(player, audioReader, {0});
    fadeIn(player);
    fillAudioBufferMono(5);
    fillAudioBufferMono(3 * 4 - 5 + 1, 6);
    callback(timer);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        player_system_time_type{6}, listener.fadeInCompleteSystemTime());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(gsl::index{3 * 4 - 5 + 1},
        listener.fadeInCompleteSystemTimeSampleOffset());
}

MASKER_PLAYER_TEST(observerNotifiedOnlyOnceForFadeIn) {
    setFadeInOutSeconds(player, 2);
    setSampleRateHz(audioPlayer, 3);
    loadMonoAudio(player, audioReader, {0});
    fadeIn(player);
    fillAudioBufferMono(2 * 3 + 1);
    callback(timer);
    assertFadeInCompletions(1);
    callbackAfterMonoFill();
    assertFadeInCompletions(1);
}

MASKER_PLAYER_TEST(fadeOutCompleteOnlyAfterFadeTime) {
    setFadeInOutSeconds(player, 3);
    setSampleRateHz(audioPlayer, 4);
    setSteadyLevelSeconds(player, 5);
    loadMonoAudio(player, audioReader, {0});
    bool fillOnce{};
    bool filledOnce{};
    bool fadeInComplete{};
    bool finish{};
    std::mutex mutex{};
    std::condition_variable condition{};
    auto future{
        setOnPlayTask(audioPlayer, [&](AudioPlayer::Observer *observer) {
            std::vector<float> fadeInBuffer(3 * 4 + 1);
            observer->fillAudioBuffer({fadeInBuffer}, {});
            set(mutex, fadeInComplete);
            condition.notify_one();
            std::vector<float> steadyLevelBuffer(4 * 5 + 1);
            observer->fillAudioBuffer({steadyLevelBuffer}, {});
            for (int i = 0; i < 3 * 4 + 1; ++i) {
                waitThenClear(mutex, condition, fillOnce);
                av_speech_in_noise::fillAudioBuffer(observer, 1, 1);
                set(mutex, filledOnce);
                condition.notify_one();
            }
            bool done{};
            while (!done) {
                av_speech_in_noise::fillAudioBuffer(observer, 1, 1);
                {
                    std::lock_guard<std::mutex> lock{mutex};
                    done = finish;
                }
            }
            return std::vector<std::vector<float>>{};
        })};
    fadeIn(player);
    wait(mutex, condition, fadeInComplete);
    callback(timer);
    for (int i = 0; i < 3 * 4; ++i) {
        set(mutex, fillOnce);
        condition.notify_one();
        waitThenClear(mutex, condition, filledOnce);
        callback(timer);
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(listener.fadeOutCompleted());
    }
    set(mutex, fillOnce);
    condition.notify_one();
    wait(mutex, condition, filledOnce);
    callback(timer);
    set(mutex, finish);
    future.get();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(listener.fadeOutCompleted());
}

MASKER_PLAYER_TEST(audioPlayerStoppedOnlyAtEndOfFadeOutTime) {
    setFadeInOutSeconds(player, 3);
    setSampleRateHz(audioPlayer, 4);
    setSteadyLevelSeconds(player, 5);
    auto halfWindowLength = 3 * 4 + 1;
    auto steadyLevelLength = 4 * 5 + 1;
    loadMonoAudio(player, audioReader, {0});
    bool fillOnce{};
    bool filledOnce{};
    bool fadeInComplete{};
    bool finish{};
    std::mutex mutex{};
    std::condition_variable condition{};
    auto future{
        setOnPlayTask(audioPlayer, [&](AudioPlayer::Observer *observer) {
            std::vector<float> fadeInBuffer(halfWindowLength);
            observer->fillAudioBuffer({fadeInBuffer}, {});
            set(mutex, fadeInComplete);
            condition.notify_one();
            std::vector<float> steadyLevelBuffer(steadyLevelLength);
            observer->fillAudioBuffer({steadyLevelBuffer}, {});
            for (int i = 0; i < halfWindowLength; ++i) {
                waitThenClear(mutex, condition, fillOnce);
                av_speech_in_noise::fillAudioBuffer(observer, 1, 1);
                set(mutex, filledOnce);
                condition.notify_one();
            }
            bool done{};
            while (!done) {
                av_speech_in_noise::fillAudioBuffer(observer, 1, 1);
                {
                    std::lock_guard<std::mutex> lock{mutex};
                    done = finish;
                }
            }
            return std::vector<std::vector<float>>{};
        })};
    fadeIn(player);
    wait(mutex, condition, fadeInComplete);
    callback(timer);
    for (int i = 0; i < 3 * 4; ++i) {
        set(mutex, fillOnce);
        condition.notify_one();
        waitThenClear(mutex, condition, filledOnce);
        callback(timer);
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(playerStopped());
    }
    set(mutex, fillOnce);
    condition.notify_one();
    wait(mutex, condition, filledOnce);
    callback(timer);
    set(mutex, finish);
    future.get();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(playerStopped());
}

MASKER_PLAYER_TEST(fadeInSchedulesCallback) { assertFadeInSchedulesCallback(); }

MASKER_PLAYER_TEST(fadeInTwiceDoesNotScheduleAdditionalCallback) {
    fadeIn(player);
    assertFadeInDoesNotScheduleAdditionalCallback();
}

MASKER_PLAYER_TEST(fadeInWhileFadingOutDoesNotScheduleAdditionalCallback) {
    setFadeInOutSeconds(player, 2);
    setSampleRateHz(audioPlayer, 3);
    setSteadyLevelSeconds(player, 5);
    auto steadyLevelLength = 3 * 5 + 1;
    loadMonoAudio(player, audioReader, {0});
    assertOnPlayTaskAfterFadeOut(
        player, audioPlayer, timer, 2 * 3 + 1,
        [=](AudioPlayer::Observer *observer) {
            return av_speech_in_noise::fillAudioBuffer(observer, 1, 2 * 3 + 1);
        },
        [=](const std::vector<std::vector<float>> &) {
            assertFadeInDoesNotScheduleAdditionalCallback();
        },
        steadyLevelLength);
}

MASKER_PLAYER_TEST(fadeInAfterFadingOutSchedulesCallback) {
    setFadeInOutSeconds(player, 2);
    setSampleRateHz(audioPlayer, 3);
    setSteadyLevelSeconds(player, 5);
    loadMonoAudio(player, audioReader, {0});
    bool fadeInComplete{};
    bool fadeOutComplete{};
    bool fadeOutCalled{};
    bool finish{};
    std::mutex mutex{};
    std::condition_variable condition{};
    auto future{
        setOnPlayTask(audioPlayer, [&](AudioPlayer::Observer *observer) {
            av_speech_in_noise::fillAudioBuffer(observer, 1, 2 * 3 + 1);
            set(mutex, fadeInComplete);
            condition.notify_one();
            av_speech_in_noise::fillAudioBuffer(observer, 1, 3 * 5 + 1);
            wait(mutex, condition, fadeOutCalled);
            av_speech_in_noise::fillAudioBuffer(observer, 1, 2 * 3 + 1);
            set(mutex, fadeOutComplete);
            condition.notify_one();
            while (true) {
                av_speech_in_noise::fillAudioBuffer(observer, 1, 1);
                std::lock_guard<std::mutex> lock{mutex};
                if (finish)
                    return std::vector<std::vector<float>>{};
            }
        })};
    fadeIn(player);
    wait(mutex, condition, fadeInComplete);
    callback(timer);
    set(mutex, fadeOutCalled);
    condition.notify_one();
    wait(mutex, condition, fadeOutComplete);
    callback(timer);
    set(mutex, finish);
    future.get();
    assertFadeInSchedulesCallback();
}

MASKER_PLAYER_TEST(callbackSchedulesAdditionalCallback) {
    callback(timer);
    assertCallbackScheduled();
}

MASKER_PLAYER_TEST(callbackSchedulesAdditionalCallbackEvenWhenFadeInComplete) {
    setFadeInOutSeconds(player, 2);
    setSampleRateHz(audioPlayer, 3);
    loadMonoAudio(player, audioReader, {0});
    fadeIn(player);
    fillAudioBufferMono(2 * 3 + 1);
    clearCallbackCount();
    callback(timer);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(callbackScheduled());
}

MASKER_PLAYER_TEST(
    callbackDoesNotScheduleAdditionalCallbackWhenFadeOutComplete) {
    setFadeInOutSeconds(player, 2);
    setSampleRateHz(audioPlayer, 3);
    setSteadyLevelSeconds(player, 5);
    loadMonoAudio(player, audioReader, {0});
    bool fadeInComplete{};
    bool fadeOutComplete{};
    bool fadeOutCalled{};
    bool finish{};
    std::mutex mutex{};
    std::condition_variable condition{};
    auto future{
        setOnPlayTask(audioPlayer, [&](AudioPlayer::Observer *observer) {
            av_speech_in_noise::fillAudioBuffer(observer, 1, 2 * 3 + 1);
            set(mutex, fadeInComplete);
            condition.notify_one();
            av_speech_in_noise::fillAudioBuffer(observer, 1, 3 * 5 + 1);
            wait(mutex, condition, fadeOutCalled);
            av_speech_in_noise::fillAudioBuffer(observer, 1, 2 * 3 + 1);
            set(mutex, fadeOutComplete);
            condition.notify_one();
            while (true) {
                av_speech_in_noise::fillAudioBuffer(observer, 1, 1);
                std::lock_guard<std::mutex> lock{mutex};
                if (finish)
                    return std::vector<std::vector<float>>{};
            }
        })};
    fadeIn(player);
    wait(mutex, condition, fadeInComplete);
    callback(timer);
    set(mutex, fadeOutCalled);
    condition.notify_one();
    wait(mutex, condition, fadeOutComplete);
    clearCallbackCount();
    callback(timer);
    set(mutex, finish);
    future.get();
    assertCallbackNotScheduled();
}

MASKER_PLAYER_TEST(setAudioDeviceFindsIndex) {
    setAudioDeviceDescriptions({"zeroth", "first", "second", "third"});
    setAudioDevice(player, "second");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(2, audioPlayer.deviceIndex());
}

MASKER_PLAYER_TEST(setAudioDeviceThrowsInvalidAudioDeviceIfDoesntExist) {
    setAudioDeviceDescriptions({"zeroth", "first", "second"});
    try {
        setAudioDevice(player, "third");
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
