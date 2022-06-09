#include "RecognitionTestModel.hpp"
#include "IOutputFile.hpp"
#include "av-speech-in-noise/Model.hpp"
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
    void writeLastCoordinateResponse(OutputFile &) override {}
    void writeTestingParameters(OutputFile &) override {}
    void writeTestResult(OutputFile &) override {}
};
}

static NullTestMethod nullTestMethod;

static void useAllChannels(MaskerPlayer &player) { player.useAllChannels(); }

static void clearChannelDelays(MaskerPlayer &player) {
    player.clearChannelDelays();
}

static void useAllChannels(TargetPlayer &player) { player.useAllChannels(); }

static auto totalrampDuration(MaskerPlayer &player) -> Duration {
    return Duration{2 * player.rampDuration().seconds};
}

static constexpr auto operator+(const Duration &a, const Duration &b)
    -> Duration {
    return Duration{a.seconds + b.seconds};
}

static auto steadyLevelDuration(TargetPlayer &player) -> Duration {
    return player.duration() +
        RecognitionTestModelImpl::targetOnsetFringeDuration +
        RecognitionTestModelImpl::targetOffsetFringeDuration;
}

static auto trialDuration(TargetPlayer &target, MaskerPlayer &masker)
    -> Duration {
    return totalrampDuration(masker) + steadyLevelDuration(target);
}

static void turnOff(bool &b) { b = false; }

static void useFirstChannelOnly(TargetPlayer &player) {
    player.useFirstChannelOnly();
}

static void setAudioDevice(TargetPlayer &player, const std::string &device) {
    player.setAudioDevice(device);
}

static void setAudioDevice(MaskerPlayer &player, const std::string &device) {
    player.setAudioDevice(device);
}

static void setAudioDevices(MaskerPlayer &maskerPlayer,
    TargetPlayer &targetPlayer, const std::string &device) {
    setAudioDevice(maskerPlayer, device);
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

static void throwRequestFailureIfTrialInProgress(bool f) {
    if (f)
        throw Model::RequestFailure{"Trial in progress."};
}

static void apply(TargetPlayer &player, LevelAmplification x) {
    player.apply(x);
}

static void apply(MaskerPlayer &player, LevelAmplification x) {
    player.apply(x);
}

static void loadFile(TargetPlayer &player, const LocalUrl &s) {
    player.loadFile(s);
}

static void loadFile(MaskerPlayer &player, const LocalUrl &s) {
    player.loadFile(s);
}

static void play(TargetPlayer &player) { player.play(); }

static void play(MaskerPlayer &player) { player.play(); }

static auto currentTarget(TestMethod *testMethod) -> LocalUrl {
    return testMethod->currentTarget();
}

static auto targetName(ResponseEvaluator &evaluator, TestMethod *testMethod)
    -> std::string {
    return evaluator.fileName(currentTarget(testMethod));
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

static auto levelAmplification(MaskerPlayer &player, const Calibration &p)
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

static auto nanoseconds(Delay x) -> std::uintmax_t {
    return gsl::narrow_cast<std::uintmax_t>(x.seconds * 1e9);
}

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

static constexpr auto operator-(const Duration &a, const Duration &b)
    -> Duration {
    return Duration{a.seconds - b.seconds};
}

static void play(TargetPlayer &targetPlayer, const Calibration &calibration) {
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

static void play(MaskerPlayer &maskerPlayer, const Calibration &calibration) {
    maskerPlayer.stop();
    throwRequestFailureOnInvalidAudioDevice(
        [&](auto device) { setAudioDevice(maskerPlayer, device); },
        calibration.audioDevice);
    throwRequestFailureOnInvalidAudioFile(
        [&](auto file) {
            loadFile(maskerPlayer, file);
            apply(maskerPlayer, levelAmplification(maskerPlayer, calibration));
        },
        calibration.fileUrl);
    play(maskerPlayer);
}

static auto maskerLevelAmplification(MaskerPlayer &maskerPlayer,
    RealLevel maskerLevel, RealLevel fullScaleLevel) -> LevelAmplification {
    return LevelAmplification{
        DigitalLevel{maskerLevel - fullScaleLevel - maskerPlayer.digitalLevel()}
            .dBov};
}

static auto targetLevelAmplification(TestMethod *testMethod,
    MaskerPlayer &maskerPlayer, RealLevel maskerLevel,
    RealLevel fullScaleLevel) {
    return LevelAmplification{
        maskerLevelAmplification(maskerPlayer, maskerLevel, fullScaleLevel).dB +
        testMethod->snr().dB};
}

static void preparePlayersForNextTrial(TestMethod *testMethod,
    Randomizer &randomizer, TargetPlayer &targetPlayer,
    MaskerPlayer &maskerPlayer, RealLevel maskerLevel,
    RealLevel fullScaleLevel) {
    loadFile(targetPlayer, testMethod->nextTarget());
    apply(targetPlayer,
        targetLevelAmplification(
            testMethod, maskerPlayer, maskerLevel, fullScaleLevel));
    const auto maskerPlayerSeekTimeUpperLimit{
        maskerPlayer.duration() - trialDuration(targetPlayer, maskerPlayer)};
    maskerPlayer.seekSeconds(randomizer.betweenInclusive(
        0., maskerPlayerSeekTimeUpperLimit.seconds));
    maskerPlayer.setSteadyLevelFor(steadyLevelDuration(targetPlayer));
}

static void prepareNextTrialIfNeeded(TestMethod *testMethod, int &trialNumber_,
    OutputFile &outputFile, Randomizer &randomizer, TargetPlayer &targetPlayer,
    MaskerPlayer &maskerPlayer, EyeTracker &eyeTracker, RealLevel maskerLevel,
    RealLevel fullScaleLevel, bool eyeTracking,
    TargetStartTime lastTargetStartTime,
    EyeTrackerTargetPlayerSynchronization
        lastEyeTrackerTargetPlayerSynchronization) {
    if (eyeTracking) {
        outputFile.write(lastTargetStartTime);
        outputFile.write(lastEyeTrackerTargetPlayerSynchronization);
        outputFile.write(eyeTracker.gazeSamples());
        save(outputFile);
    }
    if (!testMethod->complete()) {
        ++trialNumber_;
        preparePlayersForNextTrial(testMethod, randomizer, targetPlayer,
            maskerPlayer, maskerLevel, fullScaleLevel);
    } else {
        testMethod->writeTestResult(outputFile);
        save(outputFile);
    }
}

static void saveOutputFileAndPrepareNextTrialAfter(
    const std::function<void()> &f, TestMethod *testMethod, int &trialNumber_,
    OutputFile &outputFile, Randomizer &randomizer, TargetPlayer &targetPlayer,
    MaskerPlayer &maskerPlayer, EyeTracker &eyeTracker, RealLevel maskerLevel,
    RealLevel fullScaleLevel, bool eyeTracking,
    TargetStartTime lastTargetStartTime,
    EyeTrackerTargetPlayerSynchronization
        lastEyeTrackerTargetPlayerSynchronization) {
    f();
    save(outputFile);
    prepareNextTrialIfNeeded(testMethod, trialNumber_, outputFile, randomizer,
        targetPlayer, maskerPlayer, eyeTracker, maskerLevel, fullScaleLevel,
        eyeTracking, lastTargetStartTime,
        lastEyeTrackerTargetPlayerSynchronization);
}

RecognitionTestModelImpl::RecognitionTestModelImpl(TargetPlayer &targetPlayer,
    MaskerPlayer &maskerPlayer, AudioRecorder &audioRecorder,
    ResponseEvaluator &evaluator, OutputFile &outputFile,
    Randomizer &randomizer, EyeTracker &eyeTracker, Clock &clock)
    : maskerPlayer{maskerPlayer}, targetPlayer{targetPlayer},
      audioRecorder{audioRecorder}, evaluator{evaluator},
      outputFile{outputFile}, randomizer{randomizer},
      eyeTracker{eyeTracker}, clock{clock}, testMethod{&nullTestMethod} {
    targetPlayer.attach(this);
    maskerPlayer.attach(this);
}

void RecognitionTestModelImpl::attach(Model::Observer *listener) {
    listener_ = listener;
}

void RecognitionTestModelImpl::initialize(
    TestMethod *testMethod_, const Test &test) {
    initialize_(testMethod_, test);
}

void RecognitionTestModelImpl::initialize_(
    TestMethod *testMethod_, const Test &test) {
    throwRequestFailureIfTrialInProgress(trialInProgress_);

    if (testMethod_->complete())
        return;

    tryOpening(outputFile, test.identity);
    maskerPlayer.stop();
    throwRequestFailureOnInvalidAudioFile(
        [&](auto file) { maskerPlayer.loadFile(file); }, maskerFileUrl(test));

    testMethod = testMethod_;
    fullScaleLevel_ = test.fullScaleLevel;
    maskerLevel_ = test.maskerLevel;
    condition = test.condition;

    hide(targetPlayer);
    maskerPlayer.apply(
        maskerLevelAmplification(maskerPlayer, maskerLevel_, fullScaleLevel_));
    preparePlayersForNextTrial(testMethod, randomizer, targetPlayer,
        maskerPlayer, maskerLevel_, fullScaleLevel_);
    testMethod->writeTestingParameters(outputFile);
    trialNumber_ = 1;

    useAllChannels(targetPlayer);
    useAllChannels(maskerPlayer);
    clearChannelDelays(maskerPlayer);
    turnOff(eyeTracking);
    audioRecordingEnabled = false;
}

void RecognitionTestModelImpl::initializeWithSingleSpeaker(
    TestMethod *testMethod_, const Test &test) {
    initialize_(testMethod_, test);
    useFirstChannelOnly(targetPlayer);
    maskerPlayer.useFirstChannelOnly();
}

void RecognitionTestModelImpl::initializeWithDelayedMasker(
    TestMethod *testMethod_, const Test &test) {
    initialize_(testMethod_, test);
    useFirstChannelOnly(targetPlayer);
    maskerPlayer.setChannelDelaySeconds(0, maskerChannelDelay.seconds);
}

void RecognitionTestModelImpl::initializeWithEyeTracking(
    TestMethod *method, const Test &test) {
    initialize_(method, test);
    eyeTracking = true;
}

void RecognitionTestModelImpl::initializeWithAudioRecording(
    TestMethod *method, const Test &test) {
    initialize_(method, test);
    audioRecordingEnabled = true;
}

void RecognitionTestModelImpl::playTrial(const AudioSettings &settings) {
    throwRequestFailureIfTrialInProgress(trialInProgress_);

    throwRequestFailureOnInvalidAudioDevice(
        [&](auto device) {
            setAudioDevices(maskerPlayer, targetPlayer, device);
        },
        settings.audioDevice);

    playTrialTime_ = clock.time();
    if (audioRecordingEnabled)
        audioRecorder.initialize(LocalUrl{"tbd.wav"});
    if (eyeTracking) {
        eyeTracker.allocateRecordingTimeSeconds(
            Duration{trialDuration(targetPlayer, maskerPlayer)}.seconds);
        eyeTracker.start();
    }
    if (condition == Condition::audioVisual)
        show(targetPlayer);
    targetPlayer.preRoll();
    trialInProgress_ = true;
}

void RecognitionTestModelImpl::notifyThatPreRollHasCompleted() {
    maskerPlayer.fadeIn();
}

void RecognitionTestModelImpl::fadeInComplete(
    const AudioSampleTimeWithOffset &t) {
    if (eyeTracking) {
        PlayerTimeWithDelay timeToPlayWithDelay{};
        timeToPlayWithDelay.playerTime = t.playerTime;
        timeToPlayWithDelay.delay = Delay{Duration{
            offsetDuration(maskerPlayer, t) + targetOnsetFringeDuration}
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
        PlayerTimeWithDelay timeToPlayWithDelay{};
        timeToPlayWithDelay.playerTime = t.playerTime;
        timeToPlayWithDelay.delay = Delay{Duration{
            offsetDuration(maskerPlayer, t) + targetOnsetFringeDuration}
                                              .seconds};
        targetPlayer.playAt(timeToPlayWithDelay);
    }
}

void RecognitionTestModelImpl::fadeOutComplete() {
    hide(targetPlayer);
    if (eyeTracking)
        eyeTracker.stop();
    listener_->trialComplete();
    trialInProgress_ = false;
    if (audioRecordingEnabled)
        audioRecorder.start();
}

void RecognitionTestModelImpl::submit(
    const coordinate_response_measure::Response &response) {
    if (audioRecordingEnabled)
        audioRecorder.stop();
    saveOutputFileAndPrepareNextTrialAfter(
        [&]() {
            testMethod->submit(response);
            testMethod->writeLastCoordinateResponse(outputFile);
        },
        testMethod, trialNumber_, outputFile, randomizer, targetPlayer,
        maskerPlayer, eyeTracker, maskerLevel_, fullScaleLevel_, eyeTracking,
        lastTargetStartTime, lastEyeTrackerTargetPlayerSynchronization);
}

void RecognitionTestModelImpl::submit(const ConsonantResponse &) {
    saveOutputFileAndPrepareNextTrialAfter([]() {}, testMethod, trialNumber_,
        outputFile, randomizer, targetPlayer, maskerPlayer, eyeTracker,
        maskerLevel_, fullScaleLevel_, eyeTracking, lastTargetStartTime,
        lastEyeTrackerTargetPlayerSynchronization);
}

void RecognitionTestModelImpl::prepareNextTrialIfNeeded() {
    av_speech_in_noise::prepareNextTrialIfNeeded(testMethod, trialNumber_,
        outputFile, randomizer, targetPlayer, maskerPlayer, eyeTracker,
        maskerLevel_, fullScaleLevel_, eyeTracking, lastTargetStartTime,
        lastEyeTrackerTargetPlayerSynchronization);
}

void RecognitionTestModelImpl::playCalibration(const Calibration &calibration) {
    throwRequestFailureIfTrialInProgress(trialInProgress_);
    targetPlayer.useAllChannels();
    play(targetPlayer, calibration);
}

void RecognitionTestModelImpl::playLeftSpeakerCalibration(
    const Calibration &calibration) {
    throwRequestFailureIfTrialInProgress(trialInProgress_);
    maskerPlayer.useFirstChannelOnly();
    play(maskerPlayer, calibration);
}

void RecognitionTestModelImpl::playRightSpeakerCalibration(
    const Calibration &calibration) {
    throwRequestFailureIfTrialInProgress(trialInProgress_);
    maskerPlayer.useSecondChannelOnly();
    play(maskerPlayer, calibration);
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

auto RecognitionTestModelImpl::playTrialTime() -> std::string {
    return playTrialTime_;
}
}
