#include "MaskerPlayerImpl.hpp"
#include <gsl/gsl>
#include <cmath>
#include <vector>
#include <algorithm>
#include <limits>

namespace av_speech_in_noise {
static auto at(std::vector<double> &x, gsl::index n) -> double & {
    return x.at(n);
}

static auto at(const std::vector<double> &x, gsl::index n) -> double {
    return x.at(n);
}

static auto at(std::vector<sample_index_type> &x, gsl::index n)
    -> sample_index_type & {
    return x.at(n);
}

static auto channel(const audio_type &x, channel_index_type i)
    -> const channel_type & {
    return x.at(i);
}

static auto channel(const std::vector<channel_buffer_type> &x,
    channel_index_type i) -> channel_buffer_type {
    return x.at(i);
}

static auto firstChannel(const audio_type &x) -> const channel_type & {
    return x.front();
}

static auto samples(const channel_type &channel) -> sample_index_type {
    return channel.size();
}

static auto samples(const audio_type &x) -> sample_index_type {
    return samples(firstChannel(x));
}

static auto firstChannel(const std::vector<channel_buffer_type> &x)
    -> channel_buffer_type {
    return x.front();
}

static auto channels(const audio_type &x) -> channel_index_type {
    return x.size();
}

static auto channels(const std::vector<channel_buffer_type> &x)
    -> channel_index_type {
    return x.size();
}

static auto noChannels(const audio_type &x) -> bool { return x.empty(); }

static auto noChannels(const std::vector<channel_buffer_type> &x) -> bool {
    return x.empty();
}

static auto sampleRateHz(AudioPlayer *player) -> double {
    return player->sampleRateHz();
}

static void write(std::atomic<double> &to, double value) { to.store(value); }

static void write(std::atomic<int> &to, int value) { to.store(value); }

static auto read(std::atomic<double> &x) -> double { return x.load(); }

static auto read(std::atomic<int> &x) -> int { return x.load(); }

static void set(std::atomic<bool> &x) { x.store(true); }

static void clear(std::atomic<bool> &x) { x.store(false); }

static void set(bool &x) { x = true; }

static void clear(bool &x) { x = false; }

static auto thisCallClears(std::atomic<bool> &x) -> bool {
    auto expected{true};
    return x.compare_exchange_strong(expected, false);
}

static auto rms(const channel_type &channel) -> sample_type {
    return std::sqrt(
        std::accumulate(channel.begin(), channel.end(), sample_type{0},
            [](auto a, auto b) { return a += b * b; }) /
        samples(channel));
}

static auto pi() -> double { return std::acos(-1); }

static void mute(channel_buffer_type x) {
    std::fill(x.begin(), x.end(), sample_type{0});
}

static auto framesToFill(const std::vector<channel_buffer_type> &audioBuffer)
    -> sample_index_type {
    return noChannels(audioBuffer) ? 0 : firstChannel(audioBuffer).size();
}

static auto mathModulus(sample_index_type a, sample_index_type b)
    -> sample_index_type {
    auto result{a % b};
    return result > 0 ? result : result + b;
}

static void recalculateSamplesToWaitPerChannel(
    std::vector<sample_index_type> &samplesToWaitPerChannel,
    AudioPlayer *player, const std::vector<double> &channelDelaySeconds) {
    std::generate(samplesToWaitPerChannel.begin(),
        samplesToWaitPerChannel.end(), [&, n = 0]() mutable {
            return gsl::narrow_cast<channel_index_type>(
                av_speech_in_noise::sampleRateHz(player) *
                at(channelDelaySeconds, n++));
        });
}

static void scheduleCallbackAfterSeconds(Timer *timer, double x) {
    timer->scheduleCallbackAfterSeconds(x);
}

static auto audioDeviceDescriptions(AudioPlayer *player)
    -> std::vector<std::string> {
    std::vector<std::string> descriptions(
        gsl::narrow<std::size_t>(player->deviceCount()));
    std::generate(descriptions.begin(), descriptions.end(),
        [&, n = 0]() mutable { return player->deviceDescription(n++); });
    return descriptions;
}

static auto findDeviceIndex(AudioPlayer *player, const std::string &device)
    -> int {
    auto devices{audioDeviceDescriptions(player)};
    auto found{std::find(devices.begin(), devices.end(), device)};
    if (found == devices.end())
        throw InvalidAudioDevice{};
    return gsl::narrow<int>(std::distance(devices.begin(), found));
}

constexpr auto maxChannels{128};

MaskerPlayerImpl::MaskerPlayerImpl(
    AudioPlayer *player, AudioReader *reader, Timer *timer)
    : samplesToWaitPerChannel(maxChannels),
      audioFrameHeadsPerChannel(maxChannels), channelDelaySeconds(maxChannels),
      player{player}, reader{reader}, timer{timer} {
    player->attach(this);
    timer->attach(this);
    audioThread.setSharedState(this);
}

void MaskerPlayerImpl::attach(MaskerPlayer::Observer *e) { listener = e; }

auto MaskerPlayerImpl::duration() -> Duration {
    return Duration{
        samples(sourceAudio) / av_speech_in_noise::sampleRateHz(player)};
}

void MaskerPlayerImpl::seekSeconds(double x) {
    if (audioEnabled)
        return;

    recalculateSamplesToWaitPerChannel(
        samplesToWaitPerChannel, player, channelDelaySeconds);
    std::fill(audioFrameHeadsPerChannel.begin(),
        audioFrameHeadsPerChannel.end(),
        mathModulus(gsl::narrow_cast<sample_index_type>(
                        x * av_speech_in_noise::sampleRateHz(player)),
            samples(sourceAudio)));
}

auto MaskerPlayerImpl::fadeTime() -> Duration {
    return Duration{fadeInOutSeconds};
}

auto MaskerPlayerImpl::sampleRateHz() -> double {
    return av_speech_in_noise::sampleRateHz(player);
}

auto MaskerPlayerImpl::nanoseconds(PlayerTime t) -> std::uintmax_t {
    return player->nanoseconds(t);
}

auto MaskerPlayerImpl::currentSystemTime() -> PlayerTime {
    return player->currentSystemTime();
}

void MaskerPlayerImpl::loadFile(const LocalUrl &file) {
    if (audioEnabled)
        return;

    player->loadFile(file.path);
    recalculateSamplesToWaitPerChannel(
        samplesToWaitPerChannel, player, channelDelaySeconds);
    write(levelTransitionSamples,
        gsl::narrow_cast<int>(
            fadeInOutSeconds * av_speech_in_noise::sampleRateHz(player)));
    sourceAudio = readAudio(file.path);
    std::fill(
        audioFrameHeadsPerChannel.begin(), audioFrameHeadsPerChannel.end(), 0);
}

static_assert(std::numeric_limits<double>::is_iec559, "IEEE 754 required");

auto MaskerPlayerImpl::digitalLevel() -> DigitalLevel {
    return noChannels(sourceAudio)
        ? DigitalLevel{-std::numeric_limits<double>::infinity()}
        : DigitalLevel{20 * std::log10(rms(firstChannel(sourceAudio)))};
}

auto MaskerPlayerImpl::playing() -> bool { return player->playing(); }

void MaskerPlayerImpl::apply(LevelAmplification x) {
    write(levelScalar, std::pow(10, x.dB / 20));
}

void MaskerPlayerImpl::setFadeInOutSeconds(double x) { fadeInOutSeconds = x; }

void MaskerPlayerImpl::setAudioDevice(std::string device) {
    player->setDevice(findDeviceIndex(player, device));
}

auto MaskerPlayerImpl::readAudio(std::string filePath) -> audio_type {
    try {
        return reader->read(std::move(filePath));
    } catch (const AudioReader::InvalidFile &) {
        throw InvalidAudioFile{};
    }
}

auto MaskerPlayerImpl::outputAudioDeviceDescriptions()
    -> std::vector<std::string> {
    std::vector<std::string> descriptions{};
    for (int i = 0; i < player->deviceCount(); ++i)
        if (player->outputDevice(i))
            descriptions.push_back(player->deviceDescription(i));
    return descriptions;
}

void MaskerPlayerImpl::setChannelDelaySeconds(
    channel_index_type channel, double seconds) {
    if (audioEnabled)
        return;

    at(channelDelaySeconds, channel) = seconds;
    recalculateSamplesToWaitPerChannel(
        samplesToWaitPerChannel, player, channelDelaySeconds);
}

void MaskerPlayerImpl::clearChannelDelays() {
    if (audioEnabled)
        return;

    std::fill(channelDelaySeconds.begin(), channelDelaySeconds.end(), 0);
    recalculateSamplesToWaitPerChannel(
        samplesToWaitPerChannel, player, channelDelaySeconds);
}

void MaskerPlayerImpl::useFirstChannelOnly() {
    set(firstChannelOnly);
    clear(secondChannelOnly);
}

void MaskerPlayerImpl::useSecondChannelOnly() {
    set(secondChannelOnly);
    clear(firstChannelOnly);
}

void MaskerPlayerImpl::useAllChannels() {
    clear(firstChannelOnly);
    clear(secondChannelOnly);
}

void MaskerPlayerImpl::fadeIn() {
    if (fading())
        return;

    set(fadingIn);
    set(pleaseFadeIn);
    play();
    scheduleCallbackAfterSeconds(timer, 0.1);
}

void MaskerPlayerImpl::play() {
    if (!audioEnabled) {
        set(pleaseEnableAudio);
        audioEnabled = true;
    }
    player->play();
}

void MaskerPlayerImpl::stop() {
    if (audioEnabled) {
        set(pleaseDisableAudio);
        auto expected{true};
        while (!audioDisabledComplete.compare_exchange_weak(expected, false))
            expected = true;
        audioEnabled = false;
    }
    player->stop();
}

void MaskerPlayerImpl::fadeOut() {
    if (fading())
        return;

    set(fadingOut);
    set(pleaseFadeOut);
    scheduleCallbackAfterSeconds(timer, 0.1);
}

void MaskerPlayerImpl::callback() {
    if (thisCallClears(fadeInComplete)) {
        clear(fadingIn);
        listener->fadeInComplete({{fadeInCompleteSystemTime.load()},
            fadeInCompleteSystemTimeSampleOffset.load()});
        return;
    }

    if (thisCallClears(fadeOutComplete)) {
        clear(fadingOut);
        stop();
        listener->fadeOutComplete();
        return;
    }

    scheduleCallbackAfterSeconds(timer, 0.1);
}

auto MaskerPlayerImpl::fading() -> bool { return fadingIn || fadingOut; }

// real-time audio thread
void MaskerPlayerImpl::fillAudioBuffer(
    const std::vector<channel_buffer_type> &audioBuffer,
    player_system_time_type time) {
    audioThread.fillAudioBuffer(audioBuffer, time);
}

void MaskerPlayerImpl::AudioThread::setSharedState(MaskerPlayerImpl *p) {
    sharedState = p;
}

void MaskerPlayerImpl::AudioThread::fillAudioBuffer(
    const std::vector<channel_buffer_type> &audioBuffer,
    player_system_time_type time) {
    if (!enabled) {
        if (thisCallClears(sharedState->pleaseEnableAudio))
            enabled = true;
        else
            return;
    }

    systemTime = time;
    if (noChannels(sharedState->sourceAudio))
        for (auto channel : audioBuffer)
            mute(channel);
    else
        copySourceAudio(audioBuffer);
    checkForFadeIn();
    checkForFadeOut();
    applyLevel(audioBuffer);

    if (thisCallClears(sharedState->pleaseDisableAudio)) {
        enabled = false;
        set(sharedState->audioDisabledComplete);
    }
}

void MaskerPlayerImpl::AudioThread::copySourceAudio(
    const std::vector<channel_buffer_type> &audioBuffer) {
    auto usingFirstChannelOnly{sharedState->firstChannelOnly.load()};
    auto usingSecondChannelOnly{sharedState->secondChannelOnly.load()};
    for (channel_index_type i{0}; i < channels(audioBuffer); ++i) {
        const auto samplesToWait{at(sharedState->samplesToWaitPerChannel, i)};
        const auto framesToMute =
            std::min(samplesToWait, framesToFill(audioBuffer));
        mute(channel(audioBuffer, i).first(framesToMute));
        at(sharedState->samplesToWaitPerChannel, i) =
            samplesToWait - framesToMute;
        auto frameHead{at(sharedState->audioFrameHeadsPerChannel, i)};
        auto framesLeftToFill{framesToFill(audioBuffer) - framesToMute};
        at(sharedState->audioFrameHeadsPerChannel, i) =
            (frameHead + framesLeftToFill) % sourceFrames();
        while (framesLeftToFill != 0) {
            const auto framesAboutToFill =
                std::min(sourceFrames() - frameHead, framesLeftToFill);
            const auto &source = channels(sharedState->sourceAudio) > i
                ? channel(sharedState->sourceAudio, i)
                : firstChannel(sharedState->sourceAudio);
            const auto sourceBeginning{source.begin() + frameHead};
            std::copy(sourceBeginning, sourceBeginning + framesAboutToFill,
                channel(audioBuffer, i).begin() + framesToFill(audioBuffer) -
                    framesLeftToFill);
            frameHead = 0;
            framesLeftToFill -= framesAboutToFill;
        }
        if (usingFirstChannelOnly && i > 0)
            mute(channel(audioBuffer, i));
        if (usingSecondChannelOnly && i != 1)
            mute(channel(audioBuffer, i));
    }
}

auto MaskerPlayerImpl::AudioThread::sourceFrames() -> sample_index_type {
    return samples(firstChannel(sharedState->sourceAudio));
}

void MaskerPlayerImpl::AudioThread::applyLevel(
    const std::vector<channel_buffer_type> &audioBuffer) {
    const auto levelScalar_{read(sharedState->levelScalar)};
    for (auto i{sample_index_type{0}}; i < framesToFill(audioBuffer); ++i) {
        const auto fadeScalar{nextFadeScalar()};
        updateFadeState(i);
        for (auto channel : audioBuffer)
            at(channel, i) *=
                gsl::narrow_cast<sample_type>(fadeScalar * levelScalar_);
    }
}

void MaskerPlayerImpl::AudioThread::checkForFadeIn() {
    if (thisCallClears(sharedState->pleaseFadeIn))
        prepareToFadeIn();
}

void MaskerPlayerImpl::AudioThread::prepareToFadeIn() {
    updateWindowLength();
    hannCounter = 0;
    set(fadingIn);
}

void MaskerPlayerImpl::AudioThread::updateWindowLength() {
    halfWindowLength = read(sharedState->levelTransitionSamples);
}

void MaskerPlayerImpl::AudioThread::checkForFadeOut() {
    if (thisCallClears(sharedState->pleaseFadeOut))
        prepareToFadeOut();
}

void MaskerPlayerImpl::AudioThread::prepareToFadeOut() {
    updateWindowLength();
    hannCounter = halfWindowLength;
    set(fadingOut);
}

auto MaskerPlayerImpl::AudioThread::nextFadeScalar() -> double {
    const auto squareRoot = halfWindowLength != 0
        ? std::sin((pi() * hannCounter) / (2 * halfWindowLength))
        : 1;
    return squareRoot * squareRoot;
}

void MaskerPlayerImpl::AudioThread::updateFadeState(sample_index_type offset) {
    checkForFadeInComplete(offset);
    checkForFadeOutComplete();
    advanceCounterIfStillFading();
}

void MaskerPlayerImpl::AudioThread::checkForFadeInComplete(
    sample_index_type offset) {
    if (doneFadingIn()) {
        sharedState->fadeInCompleteSystemTime.store(systemTime);
        sharedState->fadeInCompleteSystemTimeSampleOffset.store(offset + 1);
        set(sharedState->fadeInComplete);
        clear(fadingIn);
    }
}

auto MaskerPlayerImpl::AudioThread::doneFadingIn() -> bool {
    return fadingIn && hannCounter == halfWindowLength;
}

auto MaskerPlayerImpl::AudioThread::doneFadingOut() -> bool {
    return fadingOut && hannCounter == 2 * halfWindowLength;
}

void MaskerPlayerImpl::AudioThread::checkForFadeOutComplete() {
    if (doneFadingOut()) {
        set(sharedState->fadeOutComplete);
        clear(fadingOut);
    }
}

void MaskerPlayerImpl::AudioThread::advanceCounterIfStillFading() {
    if (fadingIn || fadingOut)
        ++hannCounter;
}
}
