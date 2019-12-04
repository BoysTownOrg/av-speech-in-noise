#include "MaskerPlayerImpl.hpp"
#include "AudioReader.hpp"
#include <gsl/gsl>
#include <cmath>
#include <vector>
#include <algorithm>

namespace stimulus_players {
MaskerPlayerImpl::MaskerPlayerImpl(
    AudioPlayer *player, AudioReader *reader, Timer *timer)
    : mainThread{player, timer}, player{player}, reader{reader} {
    player->subscribe(this);
    timer->subscribe(this);
    mainThread.setSharedAtomics(this);
    audioThread.setSharedAtomics(this);
}

MaskerPlayerImpl::MainThread::MainThread(AudioPlayer *player, Timer *timer)
    : player{player}, timer{timer} {}

void MaskerPlayerImpl::MainThread::setSharedAtomics(MaskerPlayerImpl *p) {
    sharedAtomics = p;
}

void MaskerPlayerImpl::AudioThread::setSharedAtomics(MaskerPlayerImpl *p) {
    sharedAtomics = p;
}

void MaskerPlayerImpl::subscribe(MaskerPlayer::EventListener *e) {
    mainThread.subscribe(e);
}

void MaskerPlayerImpl::MainThread::subscribe(MaskerPlayer::EventListener *e) {
    listener = e;
}

void MaskerPlayerImpl::setChannelDelaySeconds(
    channel_index_type channel, double seconds) {
    mainThread.setChannelDelaySeconds(channel, seconds);
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

static auto samples(const channel_type &channel) -> std::size_t {
    return channel.size();
}

static auto channel(const audio_type &x, channel_index_type n)
    -> const channel_type & {
    return x.at(n);
}

static auto firstChannel(const audio_type &x) -> const channel_type & {
    return x.front();
}

static auto channels(const audio_type &x) -> std::size_t { return x.size(); }

static auto sampleRateHz(AudioPlayer *player) -> double {
    return player->sampleRateHz();
}

static void write(std::atomic<double> &to, double value) { to.store(value); }

static void write(std::atomic<std::size_t> &to, std::size_t value) {
    to.store(value);
}

static void set(std::atomic<bool> &x) { x.store(true); }

static void set(bool &x) { x = true; }

static auto read(std::atomic<double> &x) -> double { return x.load(); }

static auto read(std::atomic<std::size_t> &x) -> std::size_t {
    return x.load();
}

auto MaskerPlayerImpl::durationSeconds() -> double {
    return samples(firstChannel(audio)) / read(sampleRateHz_);
}

void MaskerPlayerImpl::seekSeconds(double x) {
    write(audioFrameHead, x * read(sampleRateHz_));
}

auto MaskerPlayerImpl::fadeTimeSeconds() -> double {
    return fadeInOutSeconds.load();
}

static auto rms(const channel_type &channel) -> sample_type {
    return std::sqrt(
        std::accumulate(channel.begin(), channel.end(), sample_type{0},
            [](auto a, auto b) { return a += b * b; }) /
        samples(channel));
}

static auto noChannels(const audio_type &x) -> bool { return x.empty(); }

// calling this while player is playing yields undefined behavior
void MaskerPlayerImpl::loadFile(std::string filePath) {
    if (playing())
        return;

    player->loadFile(filePath);
    write(sampleRateHz_, sampleRateHz(player));
    for (auto channel : mainThread.channelsWithDelay())
        write(samplesToWaitPerChannel_[channel],
            read(sampleRateHz_) * mainThread.channelDelaySeconds(channel));
    audio = readAudio(std::move(filePath));
    write(audioFrameHead, 0);
}

auto MaskerPlayerImpl::rms() -> double {
    return noChannels(audio) ? 0 : stimulus_players::rms(firstChannel(audio));
}

auto MaskerPlayerImpl::playing() -> bool { return player->playing(); }

static auto dB(double x) -> double { return std::pow(10, x / 20); }

void MaskerPlayerImpl::setLevel_dB(double x) { write(levelScalar, dB(x)); }

void MaskerPlayerImpl::setFadeInOutSeconds(double x) {
    write(fadeInOutSeconds, x);
}

void MaskerPlayerImpl::setAudioDevice(std::string device) {
    player->setDevice(findDeviceIndex(device));
}

auto MaskerPlayerImpl::findDeviceIndex(const std::string &device) -> int {
    auto devices = audioDeviceDescriptions_();
    auto found = std::find(devices.begin(), devices.end(), device);
    if (found == devices.end())
        throw av_speech_in_noise::InvalidAudioDevice{};
    return std::distance(devices.begin(), found);
}

auto MaskerPlayerImpl::readAudio(std::string filePath) -> audio_type {
    try {
        return reader->read(std::move(filePath));
    } catch (const AudioReader::InvalidFile &) {
        throw av_speech_in_noise::InvalidAudioFile{};
    }
}

auto MaskerPlayerImpl::audioDeviceDescriptions_() -> std::vector<std::string> {
    std::vector<std::string> descriptions(player->deviceCount());
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

void MaskerPlayerImpl::fadeIn() { mainThread.fadeIn(); }

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

void MaskerPlayerImpl::fadeOut() { mainThread.fadeOut(); }

void MaskerPlayerImpl::MainThread::fadeOut() {
    if (fading())
        return;

    set(fadingOut);
    set(sharedAtomics->pleaseFadeOut);
    scheduleCallbackAfterSeconds(0.1);
}

void MaskerPlayerImpl::callback() { mainThread.callback(); }

static void clear(bool &x) { x = false; }

static auto thisCallClears(std::atomic<bool> &x) -> bool {
    auto expected = true;
    return x.compare_exchange_strong(expected, false);
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

using channel_buffer_type = gsl::span<float>;

// real-time audio thread
void MaskerPlayerImpl::fillAudioBuffer(
    const std::vector<channel_buffer_type> &audioBuffer) {
    audioThread.fillAudioBuffer(audioBuffer);
}

static void mute(channel_buffer_type x) { std::fill(x.begin(), x.end(), 0); }

auto firstChannel(const std::vector<channel_buffer_type> &x)
    -> channel_buffer_type {
    return x.front();
}

auto noChannels(const std::vector<channel_buffer_type> &x) -> bool {
    return x.empty();
}

auto channels(const std::vector<channel_buffer_type> &x) -> std::size_t {
    return x.size();
}

void MaskerPlayerImpl::AudioThread::fillAudioBuffer(
    const std::vector<channel_buffer_type> &audioBuffer) {
    checkForFadeIn();
    checkForFadeOut();

    const std::size_t framesToFill =
        noChannels(audioBuffer) ? 0 : firstChannel(audioBuffer).size();
    if (noChannels(sharedAtomics->audio))
        for (auto channelBuffer : audioBuffer)
            mute(channelBuffer);
    else {
        const auto sourceFrames = samples(firstChannel(sharedAtomics->audio));
        const auto audioFrameHead_ = read(sharedAtomics->audioFrameHead);
        for (std::size_t i = 0; i < channels(audioBuffer); ++i) {
            const auto &source = channels(sharedAtomics->audio) > i
                ? channel(sharedAtomics->audio, i)
                : firstChannel(sharedAtomics->audio);
            auto destination = audioBuffer.at(i);
            auto sourceFrameOffset = audioFrameHead_;
            auto framesFilled = 0UL;
            auto samplesToWait =
                sharedAtomics->samplesToWaitPerChannel_.count(i) == 0U
                ? 0
                : read(sharedAtomics->samplesToWaitPerChannel_.at(i));
            if (framesFilled < samplesToWait) {
                const auto framesAboutToFill =
                    std::min(samplesToWait, framesToFill);
                mute(destination.first(framesAboutToFill));
                framesFilled += framesAboutToFill;
                write(sharedAtomics->samplesToWaitPerChannel_.at(i),
                    samplesToWait - framesAboutToFill);
            }
            while (framesFilled < framesToFill) {
                const auto framesAboutToFill =
                    std::min(sourceFrames - sourceFrameOffset,
                        framesToFill - framesFilled);
                const auto sourceBeginning = source.begin() + sourceFrameOffset;
                std::copy(sourceBeginning, sourceBeginning + framesAboutToFill,
                    destination.begin() + framesFilled);
                sourceFrameOffset = 0;
                framesFilled += framesAboutToFill;
            }
        }
        write(sharedAtomics->audioFrameHead,
            (audioFrameHead_ + framesToFill) % sourceFrames);
    }
    const auto levelScalar_ = read(sharedAtomics->levelScalar);
    for (std::size_t i = 0; i < framesToFill; ++i) {
        const auto fadeScalar = nextFadeScalar();
        updateFadeState();
        for (auto channelBuffer : audioBuffer)
            channelBuffer.at(i) *= fadeScalar * levelScalar_;
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
    halfWindowLength = levelTransitionSamples();
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

auto MaskerPlayerImpl::AudioThread::levelTransitionSamples() -> int {
    return gsl::narrow_cast<int>(
        read(sharedAtomics->fadeInOutSeconds) * sharedAtomics->sampleRateHz_);
}

static const auto pi = std::acos(-1);

auto MaskerPlayerImpl::AudioThread::nextFadeScalar() -> double {
    const auto squareRoot = halfWindowLength != 0
        ? std::sin((pi * hannCounter) / (2 * halfWindowLength))
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
