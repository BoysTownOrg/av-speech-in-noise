#include "RecognitionTestModel.hpp"
#include <gsl/gsl>
#include <cmath>
#include <functional>

namespace av_speech_in_noise {
namespace {
class NullTestMethod : public TestMethod {
    auto complete() -> bool override { return {}; }
    auto nextTarget() -> LocalUrl override { return {}; }
    auto currentTarget() -> LocalUrl override { return {}; }
    auto snr() -> SNR override { return {}; }
    void submit(const coordinate_response_measure::Response &) override {}
    void submit(const FreeResponse &) override {}
    void submit(const CorrectKeywords &) override {}
    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
    void writeLastCoordinateResponse(OutputFile &) override {}
    void writeLastCorrectResponse(OutputFile &) override {}
    void writeLastIncorrectResponse(OutputFile &) override {}
    void writeTestingParameters(OutputFile &) override {}
    void writeLastCorrectKeywords(OutputFile &) override {}
    void writeTestResult(OutputFile &) override {}
};
}

static NullTestMethod nullTestMethod;

static auto auditoryOnly(const Condition &c) -> bool {
    return c == Condition::auditoryOnly;
}

static void useAllChannels(MaskerPlayer &player) { player.useAllChannels(); }

static void clearChannelDelays(MaskerPlayer &player) {
    player.clearChannelDelays();
}

static void useAllChannels(TargetPlayer &player) { player.useAllChannels(); }

static auto fadeTimeSeconds(MaskerPlayer &player) -> double {
    return player.fadeTimeSeconds();
}

static auto totalFadeTimeSeconds(MaskerPlayer &player) -> double {
    return 2 * fadeTimeSeconds(player);
}

static auto durationSeconds(TargetPlayer &player) -> double {
    return player.durationSeconds();
}

static auto trialDurationSeconds(TargetPlayer &target, MaskerPlayer &masker)
    -> double {
    return totalFadeTimeSeconds(masker) + durationSeconds(target);
}

static void turnOff(bool &b) { b = false; }

static void useFirstChannelOnly(TargetPlayer &player) {
    player.useFirstChannelOnly();
}

static auto trialInProgress(MaskerPlayer &player) -> bool {
    return player.playing();
}

static void setAudioDevice(TargetPlayer &player, const std::string &device) {
    player.setAudioDevice(device);
}

static void setAudioDevices(MaskerPlayer &maskerPlayer,
    TargetPlayer &targetPlayer, const std::string &device) {
    maskerPlayer.setAudioDevice(device);
    setAudioDevice(targetPlayer, device);
}

static void throwRequestFailureOnInvalidAudioDevice(
    const std::function<void(const std::string &)> &f,
    const std::string &device) {
    try {
        f(device);
    } catch (const InvalidAudioDevice &) {
        throw Model::RequestFailure{
            "'" + device + "' is not a valid audio device."};
    }
}

static void throwRequestFailureIfTrialInProgress(MaskerPlayer &player) {
    if (trialInProgress(player))
        throw Model::RequestFailure{"Trial in progress."};
}

static void setLevel_dB(TargetPlayer &player, double x) {
    player.setLevel_dB(x);
}

static void loadFile(TargetPlayer &player, const LocalUrl &s) {
    player.loadFile(s);
}

static void play(TargetPlayer &player) { player.play(); }

static auto dB(double x) -> double { return 20 * std::log10(x); }

static auto targetName(ResponseEvaluator &evaluator, TestMethod *testMethod)
    -> std::string {
    return evaluator.fileName({testMethod->currentTarget()});
}

static void save(OutputFile &file) { file.save(); }

static void tryOpening(OutputFile &file, const TestIdentity &p) {
    file.close();
    try {
        file.openNewFile(p);
    } catch (const OutputFile::OpenFailure &) {
        throw Model::RequestFailure{"Unable to open output file."};
    }
}

static auto level_dB(TargetPlayer &player, const Calibration &p) -> double {
    return gsl::narrow_cast<double>(p.level.dB_SPL - p.fullScaleLevel.dB_SPL) -
        dB(player.rms());
}

static void show(TargetPlayer &player) { player.showVideo(); }

static void hide(TargetPlayer &player) { player.hideVideo(); }

static auto maskerFilePath(const Test &test) -> LocalUrl {
    return test.maskerFileUrl;
}

static void throwRequestFailureOnInvalidAudioFile(
    const std::function<void(const LocalUrl &)> &f, const LocalUrl &s) {
    try {
        f(s);
    } catch (const InvalidAudioFile &) {
        throw Model::RequestFailure{"unable to read " + s.path};
    }
}

RecognitionTestModelImpl::RecognitionTestModelImpl(TargetPlayer &targetPlayer,
    MaskerPlayer &maskerPlayer, ResponseEvaluator &evaluator,
    OutputFile &outputFile, Randomizer &randomizer, EyeTracker &eyeTracker)
    : maskerPlayer{maskerPlayer}, targetPlayer{targetPlayer},
      evaluator{evaluator}, outputFile{outputFile}, randomizer{randomizer},
      eyeTracker{eyeTracker}, testMethod{&nullTestMethod} {
    targetPlayer.subscribe(this);
    maskerPlayer.subscribe(this);
}

void RecognitionTestModelImpl::subscribe(Model::EventListener *listener) {
    listener_ = listener;
}

void RecognitionTestModelImpl::initialize(
    TestMethod *testMethod_, const Test &test) {
    initialize_(testMethod_, test);
    targetPlayer.useAllChannels();
    useAllChannels(maskerPlayer);
    clearChannelDelays(maskerPlayer);
    turnOff(eyeTracking);
}

void RecognitionTestModelImpl::initialize_(
    TestMethod *testMethod_, const Test &test) {
    throwRequestFailureIfTrialInProgress(maskerPlayer);

    if (testMethod_->complete())
        return;

    tryOpening(outputFile, test.identity);
    throwRequestFailureOnInvalidAudioFile(
        [&](auto file) { maskerPlayer.loadFile(file); }, maskerFilePath(test));

    testMethod = testMethod_;
    fullScaleLevel_dB_SPL = test.fullScaleLevel.dB_SPL;
    maskerLevel_dB_SPL = test.maskerLevel.dB_SPL;
    condition = test.condition;

    hide(targetPlayer);
    maskerPlayer.setLevel_dB(maskerLevel_dB());
    preparePlayersForNextTrial();
    testMethod->writeTestingParameters(outputFile);
    trialNumber_ = 1;
}

void RecognitionTestModelImpl::initializeWithSingleSpeaker(
    TestMethod *testMethod_, const Test &test) {
    initialize_(testMethod_, test);
    useFirstChannelOnly(targetPlayer);
    maskerPlayer.useFirstChannelOnly();
    clearChannelDelays(maskerPlayer);
    turnOff(eyeTracking);
}

void RecognitionTestModelImpl::initializeWithDelayedMasker(
    TestMethod *testMethod_, const Test &test) {
    initialize_(testMethod_, test);
    useFirstChannelOnly(targetPlayer);
    useAllChannels(maskerPlayer);
    maskerPlayer.setChannelDelaySeconds(0, maskerChannelDelaySeconds);
    turnOff(eyeTracking);
}

void RecognitionTestModelImpl::initializeWithEyeTracking(
    TestMethod *method, const Test &test) {
    initialize_(method, test);
    useAllChannels(targetPlayer);
    useAllChannels(maskerPlayer);
    clearChannelDelays(maskerPlayer);
    eyeTracking = true;
}

auto RecognitionTestModelImpl::maskerLevel_dB() -> double {
    return desiredMaskerLevel_dB() - dB(maskerPlayer.rms());
}

auto RecognitionTestModelImpl::desiredMaskerLevel_dB() -> int {
    return maskerLevel_dB_SPL - fullScaleLevel_dB_SPL;
}

static auto nanoseconds(Delay x) -> std::uintmax_t { return x.seconds * 1e9; }

static auto nanoseconds(MaskerPlayer &player, const PlayerTime &t)
    -> std::uintmax_t {
    return player.nanoseconds(t);
}

static auto nanoseconds(MaskerPlayer &player, const PlayerTimeWithDelay &t)
    -> std::uintmax_t {
    return nanoseconds(player, t.playerTime) + nanoseconds(t.delay);
}

static auto offsetSeconds(
    MaskerPlayer &player, const AudioSampleTimeWithOffset &t) -> double {
    return t.sampleOffset / player.sampleRateHz();
}

void RecognitionTestModelImpl::fadeInComplete(
    const AudioSampleTimeWithOffset &t) {
    if (eyeTracking) {
        PlayerTimeWithDelay timeToPlayWithDelay{};
        timeToPlayWithDelay.playerTime = t.playerTime;
        timeToPlayWithDelay.delay = Delay{
            offsetSeconds(maskerPlayer, t) + additionalTargetDelaySeconds};
        targetPlayer.playAt(timeToPlayWithDelay);

        lastTargetStartTime.nanoseconds =
            nanoseconds(maskerPlayer, timeToPlayWithDelay);

        lastEyeTrackerTargetPlayerSynchronization.eyeTrackerSystemTime =
            eyeTracker.currentSystemTime();
        lastEyeTrackerTargetPlayerSynchronization.targetPlayerSystemTime =
            TargetPlayerSystemTime{
                nanoseconds(maskerPlayer, maskerPlayer.currentSystemTime())};
    } else {
        play(targetPlayer);
    }
}

void RecognitionTestModelImpl::playbackComplete() { maskerPlayer.fadeOut(); }

void RecognitionTestModelImpl::fadeOutComplete() {
    hide(targetPlayer);
    if (eyeTracking)
        eyeTracker.stop();
    listener_->trialComplete();
}

void RecognitionTestModelImpl::preparePlayersForNextTrial() {
    loadFile(targetPlayer, testMethod->nextTarget());
    setLevel_dB(targetPlayer, targetLevel_dB());
    targetPlayer.subscribeToPlaybackCompletion();
    seekRandomMaskerPosition();
}

auto RecognitionTestModelImpl::targetLevel_dB() -> double {
    return maskerLevel_dB() + testMethod->snr().dB;
}

void RecognitionTestModelImpl::seekRandomMaskerPosition() {
    auto upperLimit = maskerPlayer.durationSeconds() -
        2 * maskerPlayer.fadeTimeSeconds() - targetPlayer.durationSeconds();
    maskerPlayer.seekSeconds(randomizer.betweenInclusive(0., upperLimit));
}

void RecognitionTestModelImpl::playTrial(const AudioSettings &settings) {
    throwRequestFailureIfTrialInProgress(maskerPlayer);

    throwRequestFailureOnInvalidAudioDevice(
        [&](auto device) {
            setAudioDevices(maskerPlayer, targetPlayer, device);
        },
        settings.audioDevice);

    if (eyeTracking) {
        eyeTracker.allocateRecordingTimeSeconds(
            trialDurationSeconds(targetPlayer, maskerPlayer) +
            additionalTargetDelaySeconds);
        eyeTracker.start();
    }
    if (!auditoryOnly(condition))
        show(targetPlayer);
    maskerPlayer.fadeIn();
}

void RecognitionTestModelImpl::submitCorrectResponse() {
    testMethod->submitCorrectResponse();
    testMethod->writeLastCorrectResponse(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::submitIncorrectResponse() {
    testMethod->submitIncorrectResponse();
    testMethod->writeLastIncorrectResponse(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::submit(
    const coordinate_response_measure::Response &response) {
    testMethod->submit(response);
    testMethod->writeLastCoordinateResponse(outputFile);
    if (eyeTracking) {
        outputFile.write(lastTargetStartTime);
        outputFile.write(lastEyeTrackerTargetPlayerSynchronization);
        outputFile.write(eyeTracker.gazeSamples());
    }
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::submit(const FreeResponse &response) {
    testMethod->submit(response);
    FreeResponseTrial trial;
    trial.response = response.response;
    trial.target = targetName(evaluator, testMethod);
    trial.flagged = response.flagged;
    outputFile.write(trial);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::submit(const CorrectKeywords &correctKeywords) {
    testMethod->submit(correctKeywords);
    testMethod->writeLastCorrectKeywords(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::prepareNextTrialIfNeeded() {
    if (!testMethod->complete()) {
        ++trialNumber_;
        preparePlayersForNextTrial();
    } else {
        testMethod->writeTestResult(outputFile);
        save(outputFile);
    }
}

void RecognitionTestModelImpl::playCalibration(const Calibration &calibration) {
    throwRequestFailureIfTrialInProgress(maskerPlayer);

    throwRequestFailureOnInvalidAudioDevice(
        [&](auto device) { setAudioDevice(targetPlayer, device); },
        calibration.audioDevice);
    throwRequestFailureOnInvalidAudioFile(
        [&](auto file) {
            loadFile(targetPlayer, file);
            setLevel_dB(targetPlayer, level_dB(targetPlayer, calibration));
        },
        calibration.fileUrl);
    show(targetPlayer);
    play(targetPlayer);
}

auto RecognitionTestModelImpl::testComplete() -> bool {
    return testMethod->complete();
}

auto RecognitionTestModelImpl::audioDevices() -> AudioDevices {
    return maskerPlayer.outputAudioDeviceDescriptions();
}

auto RecognitionTestModelImpl::trialNumber() -> int { return trialNumber_; }

auto RecognitionTestModelImpl::targetFileName() -> std::string {
    return targetName(evaluator, testMethod);
}
}
