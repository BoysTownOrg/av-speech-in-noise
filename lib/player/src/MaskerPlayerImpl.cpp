#include "MaskerPlayerImpl.hpp"

#include <gsl/gsl>

#include <cmath>
#include <exception>
#include <iostream>
#include <string_view>
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

static auto at(
    std::vector<sample_index_type> &x, gsl::index n) -> sample_index_type & {
    return x.at(n);
}

static auto channel(
    const audio_type &x, channel_index_type i) -> const channel_type & {
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

static auto firstChannel(
    const std::vector<channel_buffer_type> &x) -> channel_buffer_type {
    return x.front();
}

static auto channels(const audio_type &x) -> channel_index_type {
    return x.size();
}

static auto channels(
    const std::vector<channel_buffer_type> &x) -> channel_index_type {
    return x.size();
}

static auto noChannels(const audio_type &x) -> bool { return x.empty(); }

static auto noChannels(const std::vector<channel_buffer_type> &x) -> bool {
    return x.empty();
}

static auto sampleRateHz(AudioPlayer &player) -> double {
    return player.sampleRateHz();
}

static void set(std::atomic<bool> &x) { x.store(true); }

static void postForExecution(LockFreeMessage &x) { set(x.execute); }

static void postCompletion(LockFreeMessage &x) { set(x.complete); }

static void set(bool &x) { x = true; }

static void clear(bool &x) { x = false; }

static auto thisCallClears(std::atomic<bool> &x) -> bool {
    auto expected{true};
    return x.compare_exchange_strong(expected, false);
}

static auto thisCallConsumesExecutionMessage(LockFreeMessage &x) -> bool {
    return thisCallClears(x.execute);
}

static auto thisCallConsumesCompletionMessage(LockFreeMessage &x) -> bool {
    return thisCallClears(x.complete);
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

static auto framesToFill(
    const std::vector<channel_buffer_type> &audioBuffer) -> sample_index_type {
    return noChannels(audioBuffer) ? 0 : firstChannel(audioBuffer).size();
}

static auto mathModulus(
    sample_index_type a, sample_index_type b) -> sample_index_type {
    auto result{a % b};
    return result > 0 ? result : result + b;
}

static void recalculateSamplesToWaitPerChannel(
    std::vector<sample_index_type> &samplesToWaitPerChannel,
    AudioPlayer &player, const std::vector<double> &channelDelaySeconds) {
    std::generate(samplesToWaitPerChannel.begin(),
        samplesToWaitPerChannel.end(), [&, n = 0]() mutable {
            return gsl::narrow_cast<channel_index_type>(
                av_speech_in_noise::sampleRateHz(player) *
                at(channelDelaySeconds, n++));
        });
}

static void scheduleCallback(Timer &timer, Delay x) {
    timer.scheduleCallbackAfterSeconds(x.seconds);
}

static auto audioDeviceDescriptions(
    AudioPlayer &player) -> std::vector<std::string> {
    std::vector<std::string> descriptions(
        gsl::narrow<std::size_t>(player.deviceCount()));
    std::generate(descriptions.begin(), descriptions.end(),
        [&, n = 0]() mutable { return player.deviceDescription(n++); });
    return descriptions;
}

static auto findDeviceIndex(
    AudioPlayer &player, const std::string &device) -> int {
    auto devices{audioDeviceDescriptions(player)};
    auto found{std::find(devices.begin(), devices.end(), device)};
    if (found == devices.end())
        throw InvalidAudioDevice{};
    return gsl::narrow<int>(std::distance(devices.begin(), found));
}

static void panic(std::string_view message) {
    std::cerr << message << '\n';
    std::terminate();
}

constexpr auto maxChannels{128};

MaskerPlayerImpl::MaskerPlayerImpl(
    AudioPlayer &player, AudioReader &reader, Timer &timer)
    : audioThreadContext{sharedState}, channelDelaySeconds(maxChannels),
      player{player}, reader{reader}, timer{timer} {
    sharedState.samplesToWaitPerChannel.resize(maxChannels);
    sharedState.audioFrameHeadsPerChannel.resize(maxChannels);
    player.attach(this);
    timer.attach(this);
}

void MaskerPlayerImpl::attach(MaskerPlayer::Observer *e) { observer = e; }

auto MaskerPlayerImpl::duration() -> Duration {
    if (audioEnabled)
        panic("Audio is currently enabled. Can't safely determine duration.");

    return Duration{samples(sharedState.sourceAudio) /
        av_speech_in_noise::sampleRateHz(player)};
}

void MaskerPlayerImpl::seekSeconds(double x) {
    if (audioEnabled)
        panic("Audio is currently enabled. Can't safely seek.");

    recalculateSamplesToWaitPerChannel(
        sharedState.samplesToWaitPerChannel, player, channelDelaySeconds);
    std::fill(sharedState.audioFrameHeadsPerChannel.begin(),
        sharedState.audioFrameHeadsPerChannel.end(),
        mathModulus(gsl::narrow_cast<sample_index_type>(
                        x * av_speech_in_noise::sampleRateHz(player)),
            samples(sharedState.sourceAudio)));
}

auto MaskerPlayerImpl::rampDuration() -> Duration { return rampDuration_; }

auto MaskerPlayerImpl::sampleRateHz() -> double {
    return av_speech_in_noise::sampleRateHz(player);
}

auto MaskerPlayerImpl::nanoseconds(PlayerTime t) -> std::uintmax_t {
    return player.nanoseconds(t);
}

auto MaskerPlayerImpl::currentSystemTime() -> PlayerTime {
    return player.currentSystemTime();
}

void MaskerPlayerImpl::loadFile(const LocalUrl &file) {
    if (audioEnabled)
        panic("Audio is currently enabled. Can't safely load file.");

    player.loadFile(file.path);
    recalculateSamplesToWaitPerChannel(
        sharedState.samplesToWaitPerChannel, player, channelDelaySeconds);
    sharedState.rampSamples = gsl::narrow_cast<gsl::index>(
        rampDuration_.seconds * av_speech_in_noise::sampleRateHz(player));
    sharedState.sourceAudio = readAudio(file.path);
    std::fill(sharedState.audioFrameHeadsPerChannel.begin(),
        sharedState.audioFrameHeadsPerChannel.end(), 0);
}

void MaskerPlayerImpl::prepareVibrotactileStimulus(
    VibrotactileStimulus stimulus) {
    if (audioEnabled)
        panic("Audio is currently enabled. Can't safely "
              "prepare vibrotactile stimulus.");

    const auto sampleRateHz{av_speech_in_noise::sampleRateHz(player)};
    sharedState.vibrotactileSamplesToWait = gsl::narrow_cast<gsl::index>(
        (stimulus.targetStartRelativeDelay.seconds +
            stimulus.additionalPostFadeInDelay.seconds) *
        sampleRateHz);
    sharedState.vibrotactileStimulus.clear();
    for (auto i{0}; i < stimulus.vibrations.size(); ++i) {
        if (i > 0)
            for (auto j{0}; j < gsl::narrow_cast<gsl::index>(
                                    stimulus.gap.seconds * sampleRateHz);
                 ++j)
                sharedState.vibrotactileStimulus.push_back(0);
        for (auto j{0};
             j < gsl::narrow_cast<gsl::index>(
                     stimulus.vibrations.at(i).duration.seconds * sampleRateHz);
             ++j)
            sharedState.vibrotactileStimulus.push_back(
                gsl::narrow_cast<sample_type>(std::sin(
                    2 * pi() * stimulus.frequency.Hz * j / sampleRateHz)));
    }
}

void MaskerPlayerImpl::enableVibrotactileStimulus() {
    if (audioEnabled)
        panic("Audio is currently enabled. Can't safely "
              "enable vibrotactile stimulus.");

    sharedState.vibrotactileEnabled = true;
}

void MaskerPlayerImpl::disableVibrotactileStimulus() {
    if (audioEnabled)
        panic("Audio is currently enabled. Can't safely "
              "disable vibrotactile stimulus.");

    sharedState.vibrotactileEnabled = false;
}

static_assert(std::numeric_limits<double>::is_iec559, "IEEE 754 required");

auto MaskerPlayerImpl::digitalLevel() -> DigitalLevel {
    if (audioEnabled)
        panic("Audio is currently enabled. Can't safely determine level.");

    return noChannels(sharedState.sourceAudio)
        ? DigitalLevel{-std::numeric_limits<double>::infinity()}
        : DigitalLevel{
              20 * std::log10(rms(firstChannel(sharedState.sourceAudio)))};
}

void MaskerPlayerImpl::apply(LevelAmplification x) {
    if (audioEnabled)
        panic("Audio is currently enabled. Can't safely change level.");

    sharedState.levelScalar = std::pow(10, x.dB / 20);
}

void MaskerPlayerImpl::setRampFor(Duration x) { rampDuration_ = x; }

void MaskerPlayerImpl::setSteadyLevelFor(Duration x) {
    if (audioEnabled)
        panic("Audio is currently enabled. Can't safely "
              "change steady level duration.");

    sharedState.steadyLevelSamples = gsl::narrow_cast<gsl::index>(
        x.seconds * av_speech_in_noise::sampleRateHz(player));
}

void MaskerPlayerImpl::setAudioDevice(std::string device) {
    player.setDevice(findDeviceIndex(player, device));
}

auto MaskerPlayerImpl::readAudio(std::string filePath) -> audio_type {
    try {
        return reader.read(std::move(filePath));
    } catch (const AudioReader::InvalidFile &) {
        throw InvalidAudioFile{};
    }
}

auto MaskerPlayerImpl::outputAudioDeviceDescriptions()
    -> std::vector<std::string> {
    std::vector<std::string> descriptions{};
    for (int i = 0; i < player.deviceCount(); ++i)
        if (player.outputDevice(i))
            descriptions.push_back(player.deviceDescription(i));
    return descriptions;
}

void MaskerPlayerImpl::setChannelDelaySeconds(
    channel_index_type channel, double seconds) {
    if (audioEnabled)
        panic("Audio is currently enabled. Can't safely set channel delay.");

    at(channelDelaySeconds, channel) = seconds;
    recalculateSamplesToWaitPerChannel(
        sharedState.samplesToWaitPerChannel, player, channelDelaySeconds);
}

void MaskerPlayerImpl::clearChannelDelays() {
    if (audioEnabled)
        panic("Audio is currently enabled. Can't safely clear channel delays.");

    std::fill(channelDelaySeconds.begin(), channelDelaySeconds.end(), 0);
    recalculateSamplesToWaitPerChannel(
        sharedState.samplesToWaitPerChannel, player, channelDelaySeconds);
}

void MaskerPlayerImpl::useFirstChannelOnly() {
    if (audioEnabled)
        panic(
            "Audio is currently enabled. Can't safely use first channel only.");

    set(sharedState.firstChannelOnly);
    clear(sharedState.secondChannelOnly);
}

void MaskerPlayerImpl::useSecondChannelOnly() {
    if (audioEnabled)
        panic("Audio is currently enabled. Can't safely use "
              "second channel only.");

    set(sharedState.secondChannelOnly);
    clear(sharedState.firstChannelOnly);
}

void MaskerPlayerImpl::useAllChannels() {
    if (audioEnabled)
        panic("Audio is currently enabled. Can't safely use all channels.");

    clear(sharedState.firstChannelOnly);
    clear(sharedState.secondChannelOnly);
}

void MaskerPlayerImpl::fadeIn() {
    if (playingFiniteSection)
        return;

    set(playingFiniteSection);
    postForExecution(sharedState.fadeInMessage);
    play();
    scheduleCallback(timer, callbackDelay);
}

void MaskerPlayerImpl::play() {
    if (!audioEnabled) {
        postForExecution(sharedState.enableAudioMessage);
        audioEnabled = true;
    }
    player.play();
}

void MaskerPlayerImpl::stop() {
    if (audioEnabled) {
        postForExecution(sharedState.disableAudioMessage);
        auto expected{true};
        while (!sharedState.disableAudioMessage.complete.compare_exchange_weak(
            expected, false))
            expected = true;
        audioEnabled = false;
    }
    player.stop();
}

void MaskerPlayerImpl::callback() {
    if (thisCallConsumesCompletionMessage(sharedState.fadeInMessage))
        observer->fadeInComplete({{sharedState.fadeInCompleteSystemTime.load()},
            sharedState.fadeInCompleteSystemTimeSampleOffset.load()});

    if (thisCallConsumesCompletionMessage(sharedState.fadeOutMessage)) {
        clear(playingFiniteSection);
        stop();
        observer->fadeOutComplete();
        return;
    }

    scheduleCallback(timer, callbackDelay);
}

// real-time audio thread
void MaskerPlayerImpl::fillAudioBuffer(
    const std::vector<channel_buffer_type> &audioBuffer,
    player_system_time_type time) {
    audioThreadContext.fillAudioBuffer(audioBuffer, time);
}

static auto squared(double x) -> double { return x * x; }

static auto sourceFrames(
    MaskerPlayerImpl::SharedState &sharedState) -> sample_index_type {
    return samples(firstChannel(sharedState.sourceAudio));
}

static void copySourceAudio(const std::vector<channel_buffer_type> &audioBuffer,
    MaskerPlayerImpl::SharedState &sharedState) {
    const auto usingFirstChannelOnly{sharedState.firstChannelOnly};
    const auto usingSecondChannelOnly{sharedState.secondChannelOnly};
    for (channel_index_type i{0}; i < std::min(2L, channels(audioBuffer));
         ++i) {
        const auto samplesToWait{at(sharedState.samplesToWaitPerChannel, i)};
        const auto framesToMute =
            std::min(samplesToWait, framesToFill(audioBuffer));
        mute(channel(audioBuffer, i).first(framesToMute));
        at(sharedState.samplesToWaitPerChannel, i) =
            samplesToWait - framesToMute;
        auto frameHead{at(sharedState.audioFrameHeadsPerChannel, i)};
        auto framesLeftToFill{framesToFill(audioBuffer) - framesToMute};
        at(sharedState.audioFrameHeadsPerChannel, i) =
            (frameHead + framesLeftToFill) % sourceFrames(sharedState);
        while (framesLeftToFill != 0) {
            const auto framesAboutToFill = std::min(
                sourceFrames(sharedState) - frameHead, framesLeftToFill);
            const auto &source = channels(sharedState.sourceAudio) > i
                ? channel(sharedState.sourceAudio, i)
                : firstChannel(sharedState.sourceAudio);
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

void MaskerPlayerImpl::AudioThreadContext::fillAudioBuffer(
    const std::vector<channel_buffer_type> &audioBuffer,
    player_system_time_type time) {
    if (!enabled) {
        if (thisCallConsumesExecutionMessage(sharedState.enableAudioMessage))
            enabled = true;
        else
            return;
    }
    if (noChannels(sharedState.sourceAudio)) {
        for (channel_index_type i{0}; i < std::min(2L, channels(audioBuffer));
             ++i)
            mute(channel(audioBuffer, i));
    } else
        copySourceAudio(audioBuffer, sharedState);
    if (thisCallConsumesExecutionMessage(sharedState.fadeInMessage)) {
        rampCounter = 0;
        state = State::fadingIn;
    }
    for (auto i{sample_index_type{0}}; i < framesToFill(audioBuffer); ++i) {
        for (channel_index_type j{0}; j < std::min(2L, channels(audioBuffer));
             ++j)
            at(channel(audioBuffer, j), i) *= gsl::narrow_cast<sample_type>(
                (sharedState.rampSamples != 0
                        ? squared(std::sin((pi() * rampCounter) /
                              (2 * sharedState.rampSamples)))
                        : 1) *
                sharedState.levelScalar);
        if (sharedState.vibrotactileEnabled && channels(audioBuffer) > 2)
            at(channel(audioBuffer, 2), i) = playingVibrotactile &&
                    vibrotactileCounter >= sharedState.vibrotactileSamplesToWait
                ? sharedState.vibrotactileStimulus.at(vibrotactileCounter -
                      sharedState.vibrotactileSamplesToWait)
                : sample_type{0.};
        bool stateTransition{};
        if (state == State::fadingIn &&
            rampCounter == sharedState.rampSamples) {
            sharedState.fadeInCompleteSystemTime.store(time);
            sharedState.fadeInCompleteSystemTimeSampleOffset.store(i + 1);
            postCompletion(sharedState.fadeInMessage);
            state = State::steadyLevel;
            steadyLevelCounter = 0;
            vibrotactileCounter = 0;
            set(playingVibrotactile);
            stateTransition = true;
        }
        if (playingVibrotactile &&
            vibrotactileCounter + 1 ==
                sharedState.vibrotactileStimulus.size() +
                    sharedState.vibrotactileSamplesToWait) {
            clear(playingVibrotactile);
        }
        if (state == State::steadyLevel &&
            steadyLevelCounter == sharedState.steadyLevelSamples) {
            state = State::fadingOut;
            stateTransition = true;
        }
        if (state == State::fadingOut &&
            rampCounter == 2 * sharedState.rampSamples) {
            postCompletion(sharedState.fadeOutMessage);
            state = State::idle;
            clear(playingVibrotactile);
        }
        if (!stateTransition &&
            (state == State::fadingIn || state == State::fadingOut))
            ++rampCounter;
        if (!stateTransition && state == State::steadyLevel)
            ++steadyLevelCounter;
        if (!stateTransition && playingVibrotactile)
            ++vibrotactileCounter;
    }
    if (thisCallConsumesExecutionMessage(sharedState.disableAudioMessage)) {
        enabled = false;
        postCompletion(sharedState.disableAudioMessage);
    }
}
}
