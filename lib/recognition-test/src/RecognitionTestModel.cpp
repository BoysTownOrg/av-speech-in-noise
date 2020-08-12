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
    auto snr() -> SNR override { return SNR{}; }
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

static void useAllChannels(MaskerPlayer &player) { player.useAllChannels(); }

static void clearChannelDelays(MaskerPlayer &player) {
    player.clearChannelDelays();
}

static void useAllChannels(TargetPlayer &player) { player.useAllChannels(); }

static auto totalFadeTime(MaskerPlayer &player) -> Duration {
    return Duration{2 * player.fadeTime().seconds};
}

static constexpr auto operator+(const Duration &a, const Duration &b)
    -> Duration {
    return Duration{a.seconds + b.seconds};
}

static auto trialDuration(TargetPlayer &target, MaskerPlayer &masker)
    -> Duration {
    return totalFadeTime(masker) + target.duration();
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

static void apply(TargetPlayer &player, LevelAmplification x) {
    player.apply(x);
}

static void loadFile(TargetPlayer &player, const LocalUrl &s) {
    player.loadFile(s);
}

static void play(TargetPlayer &player) { player.play(); }

static auto targetName(ResponseEvaluator &evaluator, TestMethod *testMethod)
    -> std::string {
    return evaluator.fileName(testMethod->currentTarget());
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

constexpr auto operator-(const RealLevel &a, const RealLevel &b)
    -> RealLevelDifference {
    return RealLevelDifference{a.dB_SPL - b.dB_SPL};
}

static constexpr auto operator-(
    const RealLevelDifference &a, const DigitalLevel &b) -> DigitalLevel {
    return DigitalLevel{a.dB - b.dBov};
}

static auto levelAmplification(TargetPlayer &player, const Calibration &p)
    -> LevelAmplification {
    return LevelAmplification{
        DigitalLevel{p.level - p.fullScaleLevel - player.digitalLevel()}.dBov};
}

static void show(TargetPlayer &player) { player.showVideo(); }

static void hide(TargetPlayer &player) { player.hideVideo(); }

static auto maskerFileUrl(const Test &test) -> LocalUrl {
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
        [&](auto file) { maskerPlayer.loadFile(file); }, maskerFileUrl(test));

    testMethod = testMethod_;
    fullScaleLevel_ = test.fullScaleLevel;
    maskerLevel_ = test.maskerLevel;
    condition = test.condition;

    hide(targetPlayer);
    maskerPlayer.apply(maskerLevelAmplification());
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
    maskerPlayer.setChannelDelaySeconds(0, maskerChannelDelay.seconds);
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

auto RecognitionTestModelImpl::maskerLevelAmplification()
    -> LevelAmplification {
    return LevelAmplification{DigitalLevel{
        maskerLevel_ - fullScaleLevel_ - maskerPlayer.digitalLevel()}
                                  .dBov};
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

static auto offsetDuration(
    MaskerPlayer &player, const AudioSampleTimeWithOffset &t) -> Duration {
    return Duration{t.sampleOffset / player.sampleRateHz()};
}

void RecognitionTestModelImpl::fadeInComplete(
    const AudioSampleTimeWithOffset &t) {
    if (eyeTracking) {
        PlayerTimeWithDelay timeToPlayWithDelay{};
        timeToPlayWithDelay.playerTime = t.playerTime;
        timeToPlayWithDelay.delay = Delay{
            Duration{offsetDuration(maskerPlayer, t) + additionalTargetDelay}
                .seconds};
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
    apply(targetPlayer, targetLevelAmplification());
    targetPlayer.subscribeToPlaybackCompletion();
    seekRandomMaskerPosition();
}

auto RecognitionTestModelImpl::targetLevelAmplification()
    -> LevelAmplification {
    return LevelAmplification{
        maskerLevelAmplification().dB + testMethod->snr().dB};
}

static constexpr auto operator-(const Duration &a, const Duration &b)
    -> Duration {
    return Duration{a.seconds - b.seconds};
}

void RecognitionTestModelImpl::seekRandomMaskerPosition() {
    const auto upperLimit{
        maskerPlayer.duration() - trialDuration(targetPlayer, maskerPlayer)};
    maskerPlayer.seekSeconds(
        randomizer.betweenInclusive(0., upperLimit.seconds));
}

void RecognitionTestModelImpl::playTrial(const AudioSettings &settings) {
    throwRequestFailureIfTrialInProgress(maskerPlayer);

    throwRequestFailureOnInvalidAudioDevice(
        [&](auto device) {
            setAudioDevices(maskerPlayer, targetPlayer, device);
        },
        settings.audioDevice);

    if (eyeTracking) {
        eyeTracker.allocateRecordingTimeSeconds(Duration{
            trialDuration(targetPlayer, maskerPlayer) + additionalTargetDelay}
                                                    .seconds);
        eyeTracker.start();
    }
    if (condition == Condition::audioVisual)
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
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::submit(const ConsonantResponse &) {
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
            apply(targetPlayer, levelAmplification(targetPlayer, calibration));
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
