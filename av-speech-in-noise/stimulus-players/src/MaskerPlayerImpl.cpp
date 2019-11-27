#include "MaskerPlayerImpl.hpp"
#include "AudioReader.hpp"
#include <cmath>
#include <gsl/gsl>
#include <vector>

namespace stimulus_players {
MaskerPlayerImpl::MaskerPlayerImpl(
    AudioPlayer *player, AudioReader *reader, Timer *timer)
    : audioThread{player},
      mainThread{player, timer}, player{player}, reader{reader} {
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

MaskerPlayerImpl::AudioThread::AudioThread(AudioPlayer *player)
    : player{player} {}

void MaskerPlayerImpl::AudioThread::setSharedAtomics(MaskerPlayerImpl *p) {
    sharedAtomics = p;
}

void MaskerPlayerImpl::subscribe(MaskerPlayer::EventListener *e) {
    mainThread.subscribe(e);
}

void MaskerPlayerImpl::MainThread::subscribe(MaskerPlayer::EventListener *e) {
    listener = e;
}

static auto samples(const channel_type &channel) -> std::size_t {
    return channel.size();
}

static auto firstChannel(const audio_type &x) -> const channel_type & {
    return x.front();
}

static auto sampleRateHz(AudioPlayer *player) -> double {
    return player->sampleRateHz();
}

auto MaskerPlayerImpl::durationSeconds() -> double {
    return samples(firstChannel(audio)) / sampleRateHz(player);
}

void MaskerPlayerImpl::seekSeconds(double x) {
    audioFrameHead.store(x * sampleRateHz(player));
}

auto MaskerPlayerImpl::fadeTimeSeconds() -> double {
    return fadeInOutSeconds.load();
}

static auto rms(const channel_type &x) -> sample_type {
    return std::sqrt(std::accumulate(x.begin(), x.end(), sample_type{0},
                         [](auto a, auto b) { return a += b * b; }) /
        samples(x));
}

static auto noChannels(const audio_type &x) -> bool { return x.empty(); }

void MaskerPlayerImpl::loadFile(std::string filePath) {
    player->loadFile(filePath);
    audio = readAudio(std::move(filePath));
    rms_ = noChannels(audio) ? 0 : ::stimulus_players::rms(firstChannel(audio));
    audioFrameHead.store(0);
}

auto MaskerPlayerImpl::playing() -> bool { return player->playing(); }

void MaskerPlayerImpl::setLevel_dB(double x) {
    levelScalar.store(std::pow(10, x / 20));
}

void MaskerPlayerImpl::setFadeInOutSeconds(double x) {
    fadeInOutSeconds.store(x);
}

void MaskerPlayerImpl::setAudioDevice(std::string device) {
    player->setDevice(findDeviceIndex(device));
}

auto MaskerPlayerImpl::findDeviceIndex(const std::string &device) -> int {
    auto devices_ = audioDeviceDescriptions_();
    auto found = std::find(devices_.begin(), devices_.end(), device);
    if (found == devices_.end())
        throw av_speech_in_noise::InvalidAudioDevice{};
    return gsl::narrow<int>(found - devices_.begin());
}

auto MaskerPlayerImpl::rms() -> double { return rms_; }

auto MaskerPlayerImpl::readAudio(std::string filePath) -> audio_type {
    try {
        return reader->read(std::move(filePath));
    } catch (const AudioReader::InvalidFile &) {
        throw av_speech_in_noise::InvalidAudioFile{};
    }
}

auto MaskerPlayerImpl::audioDeviceDescriptions_() -> std::vector<std::string> {
    std::vector<std::string> descriptions{};
    for (int i = 0; i < player->deviceCount(); ++i)
        descriptions.push_back(player->deviceDescription(i));
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

    fadingIn = true;
    sharedAtomics->pleaseFadeIn.store(true);
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

    fadingOut = true;
    sharedAtomics->pleaseFadeOut.store(true);
    scheduleCallbackAfterSeconds(0.1);
}

void MaskerPlayerImpl::callback() { mainThread.callback(); }

void MaskerPlayerImpl::MainThread::callback() {
    auto expectedFadeInComplete = true;
    if (sharedAtomics->fadeInComplete.compare_exchange_strong(
            expectedFadeInComplete, false)) {
        fadingIn = false;
        listener->fadeInComplete();
        return;
    }

    auto expectedFadeOutComplete = true;
    if (sharedAtomics->fadeOutComplete.compare_exchange_strong(
            expectedFadeOutComplete, false)) {
        fadingOut = false;
        listener->fadeOutComplete();
        player->stop();
        return;
    }

    scheduleCallbackAfterSeconds(0.1);
}

// real-time audio thread
void MaskerPlayerImpl::fillAudioBuffer(
    const std::vector<gsl::span<float>> &audioBuffer) {
    audioThread.fillAudioBuffer(audioBuffer);
}

void MaskerPlayerImpl::AudioThread::fillAudioBuffer(
    const std::vector<gsl::span<float>> &audioBuffer) {
    checkForFadeIn();
    checkForFadeOut();

    const std::size_t framesToFill =
        audioBuffer.empty() ? 0 : audioBuffer.front().size();
    const auto framesAvailable = noChannels(sharedAtomics->audio)
        ? 0
        : samples(firstChannel(sharedAtomics->audio));
    const auto audioFrameHead_ = sharedAtomics->audioFrameHead.load();
    for (std::size_t i = 0; i < audioBuffer.size(); ++i) {
        auto destination = audioBuffer.at(i);
        if (framesAvailable == 0)
            std::fill(destination.begin(), destination.end(), 0);
        else {
            const auto &source = sharedAtomics->audio.size() > i
                ? sharedAtomics->audio.at(i)
                : sharedAtomics->audio.front();
            auto sourceFrameOffset = audioFrameHead_;
            auto framesFilled = 0UL;
            while (framesFilled < framesToFill) {
                const auto framesAboutToFill =
                    std::min(framesAvailable - sourceFrameOffset,
                        framesToFill - framesFilled);
                const auto sourceBeginning = source.begin() + sourceFrameOffset;
                std::copy(sourceBeginning, sourceBeginning + framesAboutToFill,
                    destination.begin() + framesFilled);
                sourceFrameOffset = 0;
                framesFilled += framesAboutToFill;
            }
        }
    }
    if (framesAvailable != 0)
        sharedAtomics->audioFrameHead.store(
            (audioFrameHead_ + framesToFill) % framesAvailable);

    const auto levelScalar_ = sharedAtomics->levelScalar.load();
    for (std::size_t i = 0; i < framesToFill; ++i) {
        const auto fadeScalar = nextFadeScalar();
        updateFadeState();
        for (auto channel : audioBuffer)
            channel.at(i) *= fadeScalar * levelScalar_;
    }
}

void MaskerPlayerImpl::AudioThread::checkForFadeIn() {
    auto expected = true;
    if (sharedAtomics->pleaseFadeIn.compare_exchange_strong(expected, false))
        prepareToFadeIn();
}

void MaskerPlayerImpl::AudioThread::prepareToFadeIn() {
    updateWindowLength();
    hannCounter = 0;
    fadingIn = true;
}

void MaskerPlayerImpl::AudioThread::updateWindowLength() {
    halfWindowLength = levelTransitionSamples();
}

void MaskerPlayerImpl::AudioThread::checkForFadeOut() {
    auto expected = true;
    if (sharedAtomics->pleaseFadeOut.compare_exchange_strong(expected, false))
        prepareToFadeOut();
}

void MaskerPlayerImpl::AudioThread::prepareToFadeOut() {
    updateWindowLength();
    hannCounter = halfWindowLength;
    fadingOut = true;
}

auto MaskerPlayerImpl::AudioThread::levelTransitionSamples() -> int {
    return gsl::narrow_cast<int>(
        sharedAtomics->fadeInOutSeconds.load() * player->sampleRateHz());
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
        sharedAtomics->fadeInComplete.store(true);
        fadingIn = false;
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
        sharedAtomics->fadeOutComplete.store(true);
        fadingOut = false;
    }
}

void MaskerPlayerImpl::AudioThread::advanceCounterIfStillFading() {
    if (fadingIn || fadingOut)
        ++hannCounter;
}
}
