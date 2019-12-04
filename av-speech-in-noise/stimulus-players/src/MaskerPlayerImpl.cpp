#include "MaskerPlayerImpl.hpp"
#include "AudioReader.hpp"
#include <gsl/gsl>
#include <cmath>
#include <vector>
#include <algorithm>

namespace stimulus_players {
static auto samples(const channel_type &channel) {
    return gsl::narrow<sample_index_type>(channel.size());
}

static auto channel(const audio_type &x, channel_index_type n) -> const auto & {
    return x.at(gsl::narrow<std::size_t>(n));
}

static auto channel(
    const std::vector<channel_buffer_type> &x, channel_index_type i) -> auto {
    return x.at(gsl::narrow<std::size_t>(i));
}

static auto firstChannel(const audio_type &x) -> const auto & {
    return x.front();
}

static auto firstChannel(const std::vector<channel_buffer_type> &x) -> auto {
    return x.front();
}

static auto channels(const audio_type &x) -> auto {
    return gsl::narrow<channel_index_type>(x.size());
}

static auto channels(const std::vector<channel_buffer_type> &x) -> auto {
    return gsl::narrow<channel_index_type>(x.size());
}

static auto noChannels(const audio_type &x) -> auto { return x.empty(); }

static auto noChannels(const std::vector<channel_buffer_type> &x) -> auto {
    return x.empty();
}

static auto sampleRateHz(AudioPlayer *player) -> auto {
    return player->sampleRateHz();
}

static void write(std::atomic<double> &to, double value) { to.store(value); }

static void write(std::atomic<int> &to, int value) { to.store(value); }

static void write(std::atomic<sample_index_type> &to, sample_index_type value) {
    to.store(value);
}

static auto read(std::atomic<double> &x) -> auto { return x.load(); }

static auto read(std::atomic<int> &x) -> auto { return x.load(); }

static auto read(std::atomic<sample_index_type> &x) -> auto { return x.load(); }

static void set(std::atomic<bool> &x) { x.store(true); }

static void set(bool &x) { x = true; }

static void clear(bool &x) { x = false; }

static auto thisCallClears(std::atomic<bool> &x) -> bool {
    auto expected = true;
    return x.compare_exchange_strong(expected, false);
}

static auto rms(const channel_type &channel) -> sample_type {
    return std::sqrt(
        std::accumulate(channel.begin(), channel.end(), sample_type{0},
            [](auto a, auto b) { return a += b * b; }) /
        samples(channel));
}

static auto dB(double x) -> double { return std::pow(10, x / 20); }

static auto pi() -> double { return std::acos(-1); }

static void mute(channel_buffer_type x) { std::fill(x.begin(), x.end(), 0); }

static auto framesToFill(const std::vector<channel_buffer_type> &audioBuffer)
    -> sample_index_type {
    return noChannels(audioBuffer) ? 0 : firstChannel(audioBuffer).size();
}

MaskerPlayerImpl::MaskerPlayerImpl(
    AudioPlayer *player, AudioReader *reader, Timer *timer)
    : mainThread{player, timer},
      audioFrameHeadsPerChannel__(128), player{player}, reader{reader} {
    player->subscribe(this);
    timer->subscribe(this);
    mainThread.setSharedAtomics(this);
    audioThread.setSharedAtomics(this);
}

void MaskerPlayerImpl::subscribe(MaskerPlayer::EventListener *e) {
    mainThread.subscribe(e);
}

auto MaskerPlayerImpl::durationSeconds() -> double {
    return samples(firstChannel(audio)) / sampleRateHz(player);
}

void MaskerPlayerImpl::seekSeconds(double x) {
    std::fill(audioFrameHeadsPerChannel__.begin(),
        audioFrameHeadsPerChannel__.end(),
        gsl::narrow_cast<sample_index_type>(x * sampleRateHz(player)));
}

auto MaskerPlayerImpl::fadeTimeSeconds() -> double {
    return mainThread.fadeTimeSeconds();
}

void MaskerPlayerImpl::loadFile(std::string filePath) {
    if (playing())
        return;

    player->loadFile(filePath);
    for (auto channel : mainThread.channelsWithDelay()) {
        write(samplesToWaitPerChannel_[channel],
            gsl::narrow_cast<sample_index_type>(sampleRateHz(player) *
                mainThread.channelDelaySeconds(channel)));
    }
    write(levelTransitionSamples_,
        gsl::narrow_cast<int>(
            mainThread.fadeTimeSeconds() * sampleRateHz(player)));
    audio = readAudio(std::move(filePath));
    write(audioFrameHead, 0);
    std::fill(audioFrameHeadsPerChannel__.begin(),
        audioFrameHeadsPerChannel__.end(), 0);
}

// real-time audio thread
void MaskerPlayerImpl::fillAudioBuffer(
    const std::vector<channel_buffer_type> &audioBuffer) {
    audioThread.fillAudioBuffer(audioBuffer);
}

auto MaskerPlayerImpl::rms() -> double {
    return noChannels(audio) ? 0 : stimulus_players::rms(firstChannel(audio));
}

auto MaskerPlayerImpl::playing() -> bool { return player->playing(); }

void MaskerPlayerImpl::setLevel_dB(double x) { write(levelScalar, dB(x)); }

void MaskerPlayerImpl::setFadeInOutSeconds(double x) {
    mainThread.setFadeInOutSeconds(x);
}

void MaskerPlayerImpl::setAudioDevice(std::string device) {
    player->setDevice(findDeviceIndex(device));
}

auto MaskerPlayerImpl::findDeviceIndex(const std::string &device) -> int {
    auto devices = audioDeviceDescriptions_();
    auto found = std::find(devices.begin(), devices.end(), device);
    if (found == devices.end())
        throw av_speech_in_noise::InvalidAudioDevice{};
    return gsl::narrow<int>(std::distance(devices.begin(), found));
}

auto MaskerPlayerImpl::readAudio(std::string filePath) -> audio_type {
    try {
        return reader->read(std::move(filePath));
    } catch (const AudioReader::InvalidFile &) {
        throw av_speech_in_noise::InvalidAudioFile{};
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
}

void MaskerPlayerImpl::fadeIn() { mainThread.fadeIn(); }

void MaskerPlayerImpl::fadeOut() { mainThread.fadeOut(); }

void MaskerPlayerImpl::callback() { mainThread.callback(); }

MaskerPlayerImpl::MainThread::MainThread(AudioPlayer *player, Timer *timer)
    : player{player}, timer{timer} {}

void MaskerPlayerImpl::MainThread::setSharedAtomics(MaskerPlayerImpl *p) {
    sharedAtomics = p;
}

void MaskerPlayerImpl::MainThread::subscribe(MaskerPlayer::EventListener *e) {
    listener = e;
}

void MaskerPlayerImpl::MainThread::setChannelDelaySeconds(
    channel_index_type channel, double seconds) {
    channelDelaySeconds_[channel] = seconds;
    channelsWithDelay_.insert(channel);
}

auto MaskerPlayerImpl::MainThread::channelsWithDelay()
    -> std::set<channel_index_type> {
    return channelsWithDelay_;
}

auto MaskerPlayerImpl::MainThread::channelDelaySeconds(
    channel_index_type channel) -> double {
    return channelDelaySeconds_.at(channel);
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

    set(fadingIn);
    set(sharedAtomics->pleaseFadeIn);
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

    set(fadingOut);
    set(sharedAtomics->pleaseFadeOut);
    scheduleCallbackAfterSeconds(0.1);
}

void MaskerPlayerImpl::MainThread::callback() {
    if (thisCallClears(sharedAtomics->fadeInComplete)) {
        clear(fadingIn);
        listener->fadeInComplete();
        return;
    }

    if (thisCallClears(sharedAtomics->fadeOutComplete)) {
        clear(fadingOut);
        listener->fadeOutComplete();
        player->stop();
        return;
    }

    scheduleCallbackAfterSeconds(0.1);
}

void MaskerPlayerImpl::AudioThread::setSharedAtomics(MaskerPlayerImpl *p) {
    sharedAtomics = p;
}

void MaskerPlayerImpl::AudioThread::fillAudioBuffer(
    const std::vector<channel_buffer_type> &audioBuffer) {
    checkForFadeIn();
    checkForFadeOut();
    if (noChannels(sharedAtomics->audio))
        for (auto channelBuffer : audioBuffer)
            mute(channelBuffer);
    else
        copySourceAudio(audioBuffer);
    applyLevel(audioBuffer);
}

void MaskerPlayerImpl::AudioThread::copySourceAudio(
    const std::vector<channel_buffer_type> &audioBuffer) {
    for (auto i = sample_index_type{0}; i < channels(audioBuffer); ++i) {
        const auto betterAudioFrameHead__ =
            read(sharedAtomics->audioFrameHeadsPerChannel__.at(i));
        auto framesFilled = sample_index_type{0};
        const auto samplesToWait =
            sharedAtomics->samplesToWaitPerChannel_.count(i) == 0U
            ? sample_index_type{0}
            : read(sharedAtomics->samplesToWaitPerChannel_.at(i));
        if (framesFilled < samplesToWait) {
            const auto framesAboutToFill =
                std::min(samplesToWait, framesToFill(audioBuffer));
            mute(channel(audioBuffer, i).first(framesAboutToFill));
            framesFilled += framesAboutToFill;
            write(sharedAtomics->samplesToWaitPerChannel_.at(i),
                samplesToWait - framesAboutToFill);
        }
        auto sourceFrameOffset = betterAudioFrameHead__;
        const auto framesLeftToFill = framesToFill(audioBuffer) - framesFilled;
        while (framesFilled < framesToFill(audioBuffer)) {
            const auto framesAboutToFill =
                std::min(sourceFrames() - sourceFrameOffset,
                    framesToFill(audioBuffer) - framesFilled);
            const auto &source = channels(sharedAtomics->audio) > i
                ? channel(sharedAtomics->audio, i)
                : firstChannel(sharedAtomics->audio);
            const auto sourceBeginning = source.begin() + sourceFrameOffset;
            std::copy(sourceBeginning, sourceBeginning + framesAboutToFill,
                channel(audioBuffer, i).begin() + framesFilled);
            sourceFrameOffset = 0;
            framesFilled += framesAboutToFill;
        }
        write(sharedAtomics->audioFrameHeadsPerChannel__.at(i),
            (betterAudioFrameHead__ + framesLeftToFill) % sourceFrames());
    }
}

auto MaskerPlayerImpl::AudioThread::sourceFrames() -> sample_index_type {
    return samples(firstChannel(sharedAtomics->audio));
}

void MaskerPlayerImpl::AudioThread::applyLevel(
    const std::vector<channel_buffer_type> &audioBuffer) {
    const auto levelScalar_ = read(sharedAtomics->levelScalar);
    for (auto i = sample_index_type{0}; i < framesToFill(audioBuffer); ++i) {
        const auto fadeScalar = nextFadeScalar();
        updateFadeState();
        for (auto channelBuffer : audioBuffer)
            channelBuffer.at(i) *=
                gsl::narrow_cast<sample_type>(fadeScalar * levelScalar_);
    }
}

void MaskerPlayerImpl::AudioThread::checkForFadeIn() {
    if (thisCallClears(sharedAtomics->pleaseFadeIn))
        prepareToFadeIn();
}

void MaskerPlayerImpl::AudioThread::prepareToFadeIn() {
    updateWindowLength();
    hannCounter = 0;
    set(fadingIn);
}

void MaskerPlayerImpl::AudioThread::updateWindowLength() {
    halfWindowLength = read(sharedAtomics->levelTransitionSamples_);
}

void MaskerPlayerImpl::AudioThread::checkForFadeOut() {
    if (thisCallClears(sharedAtomics->pleaseFadeOut))
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

void MaskerPlayerImpl::AudioThread::updateFadeState() {
    checkForFadeInComplete();
    checkForFadeOutComplete();
    advanceCounterIfStillFading();
}

void MaskerPlayerImpl::AudioThread::checkForFadeInComplete() {
    if (doneFadingIn()) {
        set(sharedAtomics->fadeInComplete);
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
        set(sharedAtomics->fadeOutComplete);
        clear(fadingOut);
    }
}

void MaskerPlayerImpl::AudioThread::advanceCounterIfStillFading() {
    if (fadingIn || fadingOut)
        ++hannCounter;
}
}
