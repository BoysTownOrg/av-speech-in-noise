#include "MaskerPlayerImpl.hpp"
#include <gsl/gsl>
#include <cmath>
#include <vector>
#include <algorithm>

namespace av_speech_in_noise {
static auto at(std::vector<double> &x, gsl::index n) -> double & {
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

constexpr auto maxChannels{128};

MaskerPlayerImpl::MaskerPlayerImpl(
    AudioPlayer *player, AudioReader *reader, Timer *timer)
    : mainThread{player, timer}, samplesToWaitPerChannel(maxChannels),
      audioFrameHeadsPerChannel(maxChannels), player{player}, reader{reader} {
    player->subscribe(this);
    timer->subscribe(this);
    mainThread.setSharedState(this);
    audioThread.setSharedState(this);
}

void MaskerPlayerImpl::subscribe(MaskerPlayer::EventListener *e) {
    mainThread.subscribe(e);
}

auto MaskerPlayerImpl::durationSeconds() -> double {
    return samples(sourceAudio) / av_speech_in_noise::sampleRateHz(player);
}

static auto mathModulus(sample_index_type a, sample_index_type b)
    -> sample_index_type {
    auto result{a % b};
    return result > 0 ? result : result + b;
}

void MaskerPlayerImpl::seekSeconds(double x) {
    if (playing())
        return;

    recalculateSamplesToWaitPerChannel();
    std::fill(audioFrameHeadsPerChannel.begin(),
        audioFrameHeadsPerChannel.end(),
        mathModulus(gsl::narrow_cast<sample_index_type>(
                        x * av_speech_in_noise::sampleRateHz(player)),
            samples(sourceAudio)));
}

void MaskerPlayerImpl::recalculateSamplesToWaitPerChannel() {
    std::generate(samplesToWaitPerChannel.begin(),
        samplesToWaitPerChannel.end(), [&, n = 0]() mutable {
            return gsl::narrow_cast<channel_index_type>(
                av_speech_in_noise::sampleRateHz(player) *
                mainThread.channelDelaySeconds(n++));
        });
}

auto MaskerPlayerImpl::fadeTimeSeconds() -> double {
    return mainThread.fadeTimeSeconds();
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
    if (playing())
        return;

    player->loadFile(file.path);
    recalculateSamplesToWaitPerChannel();
    write(levelTransitionSamples_,
        gsl::narrow_cast<int>(mainThread.fadeTimeSeconds() *
            av_speech_in_noise::sampleRateHz(player)));
    sourceAudio = readAudio(file.path);
    std::fill(
        audioFrameHeadsPerChannel.begin(), audioFrameHeadsPerChannel.end(), 0);
}

// real-time audio thread
void MaskerPlayerImpl::fillAudioBuffer(
    const std::vector<channel_buffer_type> &audioBuffer,
    player_system_time_type time) {
    audioThread.fillAudioBuffer(audioBuffer, time);
}

auto MaskerPlayerImpl::rms() -> double {
    return noChannels(sourceAudio)
        ? 0
        : av_speech_in_noise::rms(firstChannel(sourceAudio));
}

auto MaskerPlayerImpl::playing() -> bool { return player->playing(); }

void MaskerPlayerImpl::set(DigitalLevel x) {
    write(levelScalar, std::pow(10, x.dB / 20));
}

void MaskerPlayerImpl::setFadeInOutSeconds(double x) {
    mainThread.setFadeInOutSeconds(x);
}

void MaskerPlayerImpl::setAudioDevice(std::string device) {
    player->setDevice(findDeviceIndex(device));
}

auto MaskerPlayerImpl::findDeviceIndex(const std::string &device) -> int {
    auto devices{audioDeviceDescriptions_()};
    auto found{std::find(devices.begin(), devices.end(), device)};
    if (found == devices.end())
        throw InvalidAudioDevice{};
    return gsl::narrow<int>(std::distance(devices.begin(), found));
}

auto MaskerPlayerImpl::readAudio(std::string filePath) -> audio_type {
    try {
        return reader->read(std::move(filePath));
    } catch (const AudioReader::InvalidFile &) {
        throw InvalidAudioFile{};
    }
}

auto MaskerPlayerImpl::audioDeviceDescriptions_() -> std::vector<std::string> {
    std::vector<std::string> descriptions(
        gsl::narrow<std::size_t>(player->deviceCount()));
    std::generate(descriptions.begin(), descriptions.end(),
        [&, n = 0]() mutable { return player->deviceDescription(n++); });
    return descriptions;
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
    mainThread.setChannelDelaySeconds(channel, seconds);
    recalculateSamplesToWaitPerChannel();
}

void MaskerPlayerImpl::clearChannelDelays() {
    mainThread.clearChannelDelays();
    recalculateSamplesToWaitPerChannel();
}

void MaskerPlayerImpl::useFirstChannelOnly() {
    av_speech_in_noise::set(firstChannelOnly);
}

void MaskerPlayerImpl::useAllChannels() { clear(firstChannelOnly); }

void MaskerPlayerImpl::fadeIn() { mainThread.fadeIn(); }

void MaskerPlayerImpl::fadeOut() { mainThread.fadeOut(); }

void MaskerPlayerImpl::callback() { mainThread.callback(); }

MaskerPlayerImpl::MainThread::MainThread(AudioPlayer *player, Timer *timer)
    : channelDelaySeconds_(maxChannels), player{player}, timer{timer} {}

void MaskerPlayerImpl::MainThread::setSharedState(MaskerPlayerImpl *p) {
    sharedState = p;
}

void MaskerPlayerImpl::MainThread::subscribe(MaskerPlayer::EventListener *e) {
    listener = e;
}

void MaskerPlayerImpl::MainThread::setChannelDelaySeconds(
    channel_index_type channel, double seconds) {
    at(channelDelaySeconds_, channel) = seconds;
}

void MaskerPlayerImpl::MainThread::clearChannelDelays() {
    std::fill(channelDelaySeconds_.begin(), channelDelaySeconds_.end(), 0);
}

auto MaskerPlayerImpl::MainThread::channelDelaySeconds(
    channel_index_type channel) -> double {
    return at(channelDelaySeconds_, channel);
}

void MaskerPlayerImpl::MainThread::setFadeInOutSeconds(double x) {
    fadeInOutSeconds = x;
}

auto MaskerPlayerImpl::MainThread::fadeTimeSeconds() -> double {
    return fadeInOutSeconds;
}

void MaskerPlayerImpl::MainThread::fadeIn() {
    if (fading())
        return;

    av_speech_in_noise::set(fadingIn);
    av_speech_in_noise::set(sharedState->pleaseFadeIn);
    player->play();
    scheduleCallbackAfterSeconds(0.1);
}

void MaskerPlayerImpl::MainThread::scheduleCallbackAfterSeconds(double x) {
    timer->scheduleCallbackAfterSeconds(x);
}

auto MaskerPlayerImpl::MainThread::fading() -> bool {
    return fadingIn || fadingOut;
}

void MaskerPlayerImpl::MainThread::fadeOut() {
    if (fading())
        return;

    av_speech_in_noise::set(fadingOut);
    av_speech_in_noise::set(sharedState->pleaseFadeOut);
    scheduleCallbackAfterSeconds(0.1);
}

void MaskerPlayerImpl::MainThread::callback() {
    if (thisCallClears(sharedState->fadeInComplete)) {
        clear(fadingIn);
        listener->fadeInComplete(
            {{sharedState->fadeInCompleteSystemTime.load()},
                sharedState->fadeInCompleteSystemTimeSampleOffset.load()});
        return;
    }

    if (thisCallClears(sharedState->fadeOutComplete)) {
        clear(fadingOut);
        listener->fadeOutComplete();
        player->stop();
        return;
    }

    scheduleCallbackAfterSeconds(0.1);
}

void MaskerPlayerImpl::AudioThread::setSharedState(MaskerPlayerImpl *p) {
    sharedState = p;
}

void MaskerPlayerImpl::AudioThread::fillAudioBuffer(
    const std::vector<channel_buffer_type> &audioBuffer,
    player_system_time_type time) {
    systemTime = time;
    if (noChannels(sharedState->sourceAudio))
        for (auto channel : audioBuffer)
            mute(channel);
    else
        copySourceAudio(audioBuffer);
    checkForFadeIn();
    checkForFadeOut();
    applyLevel(audioBuffer);
}

void MaskerPlayerImpl::AudioThread::copySourceAudio(
    const std::vector<channel_buffer_type> &audioBuffer) {
    auto usingFirstChannelOnly{sharedState->firstChannelOnly.load()};
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
            channel.at(i) *=
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
    av_speech_in_noise::set(fadingIn);
}

void MaskerPlayerImpl::AudioThread::updateWindowLength() {
    halfWindowLength = read(sharedState->levelTransitionSamples_);
}

void MaskerPlayerImpl::AudioThread::checkForFadeOut() {
    if (thisCallClears(sharedState->pleaseFadeOut))
        prepareToFadeOut();
}

void MaskerPlayerImpl::AudioThread::prepareToFadeOut() {
    updateWindowLength();
    hannCounter = halfWindowLength;
    av_speech_in_noise::set(fadingOut);
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
        av_speech_in_noise::set(sharedState->fadeInComplete);
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
        av_speech_in_noise::set(sharedState->fadeOutComplete);
        clear(fadingOut);
    }
}

void MaskerPlayerImpl::AudioThread::advanceCounterIfStillFading() {
    if (fadingIn || fadingOut)
        ++hannCounter;
}
}
