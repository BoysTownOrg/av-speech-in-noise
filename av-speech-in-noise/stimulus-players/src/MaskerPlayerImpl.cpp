#include "MaskerPlayerImpl.hpp"
#include <cmath>
#include <gsl/gsl>

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

auto MaskerPlayerImpl::durationSeconds() -> double {
    return audio_.front().size() / player->sampleRateHz();
}

void MaskerPlayerImpl::seekSeconds(double x) {
    audioSampleIndex_ = x * player->sampleRateHz();
}

auto MaskerPlayerImpl::fadeTimeSeconds() -> double {
    return fadeInOutSeconds.load();
}

template <typename T> auto rms(const std::vector<T> &x) -> T {
    return std::sqrt(std::accumulate(x.begin(), x.end(), T{0}, [](T a, T b) {
        return a += b * b;
    }) / x.size());
}

void MaskerPlayerImpl::loadFile(std::string filePath) {
    player->loadFile(filePath_ = std::move(filePath));
    audio_ = readAudio_();
    if (audio_.empty())
        rms_ = 0;
    else {
        auto firstChannel = audio_.front();
        rms_ = ::stimulus_players::rms(firstChannel);
    }
    audioSampleIndex_ = 0;
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

auto MaskerPlayerImpl::readAudio_() -> audio_type {
    try {
        return reader->read(filePath_);
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
    const std::vector<gsl::span<float>> &audio) {
    audioThread.fillAudioBuffer(audio);
}

void MaskerPlayerImpl::AudioThread::fillAudioBuffer(
    const std::vector<gsl::span<float>> &audio) {
    if (audio.empty())
        return;

    std::size_t framesToFill = audio.front().size();
    if (!sharedAtomics->audio_.empty()) {
        auto framesAvailable = sharedAtomics->audio_.front().size();
        for (std::size_t i = 0; i < audio.size(); ++i) {
            auto source = sharedAtomics->audio_.size() > i
                ? sharedAtomics->audio_.at(i)
                : sharedAtomics->audio_.front();
            if (source.empty())
                continue;
            auto destination = audio.at(i);
            auto sourceFrameOffset = sharedAtomics->audioSampleIndex_;
            auto framesFilled = 0UL;
            while (framesFilled < framesToFill) {
                auto framesAboutToFill =
                    std::min(framesAvailable - sourceFrameOffset,
                        framesToFill - framesFilled);
                std::copy(source.begin() + sourceFrameOffset,
                    source.begin() + sourceFrameOffset + framesAboutToFill,
                    destination.begin() + framesFilled);
                sourceFrameOffset = 0;
                framesFilled += framesAboutToFill;
            }
        }
    }
    sharedAtomics->audioSampleIndex_ += framesToFill;
    checkForFadeIn();
    checkForFadeOut();
    scaleAudio(audio);
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

void MaskerPlayerImpl::AudioThread::scaleAudio(
    const std::vector<gsl::span<float>> &audio) {
    if (audio.empty())
        return;

    auto firstChannel = audio.front();
    auto levelScalar_ = sharedAtomics->levelScalar.load();
    for (int i = 0; i < firstChannel.size(); ++i) {
        auto fadeScalar_ = fadeScalar();
        updateFadeState();
        for (auto channel : audio)
            channel.at(i) *= fadeScalar_ * levelScalar_;
    }
}

static const auto pi = std::acos(-1);

auto MaskerPlayerImpl::AudioThread::fadeScalar() -> double {
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
