#include "Model.hpp"
#include "IRecognitionTestModel.hpp"

namespace av_speech_in_noise {
RunningATestFacadeImpl::RunningATestFacadeImpl(AdaptiveMethod &adaptiveMethod,
    FixedLevelMethod &fixedLevelMethod,
    TargetPlaylistReader &targetsWithReplacementReader,
    TargetPlaylistReader &cyclicTargetsReader,
    TargetPlaylist &targetsWithReplacement,
    FiniteTargetPlaylistWithRepeatables &silentIntervalTargets,
    FiniteTargetPlaylistWithRepeatables &everyTargetOnce,
    RepeatableFiniteTargetPlaylist &eachTargetNTimes,
    FiniteTargetPlaylistWithRepeatables &predeterminedTargets,
    RunningATest &model, OutputFile &outputFile,
    RunningATest::Observer &audioRecording, RunningATest::Observer &eyeTracking)
    : audioRecording{audioRecording}, eyeTracking{eyeTracking},
      adaptiveMethod{adaptiveMethod}, fixedLevelMethod{fixedLevelMethod},
      targetsWithReplacementReader{targetsWithReplacementReader},
      cyclicTargetsReader{cyclicTargetsReader},
      targetsWithReplacement{targetsWithReplacement},
      silentIntervalTargets{silentIntervalTargets},
      everyTargetOnce{everyTargetOnce},
      predeterminedTargets{predeterminedTargets},
      eachTargetNTimes{eachTargetNTimes}, runningATest{model},
      outputFile{outputFile} {}

static void initialize(
    RunningATest &model, TestMethod &method, const Test &test) {
    model.initialize(&method, test);
}

static void initialize(FixedLevelMethod &method,
    const FixedLevelFixedTrialsTest &test, TargetPlaylist &targets) {
    method.initialize(test, &targets);
}

static void initialize(FixedLevelMethod &method,
    const FixedLevelTestWithEachTargetNTimes &test,
    FiniteTargetPlaylist &targets) {
    method.initialize(test, &targets);
}

static void initialize(FixedLevelMethod &method, const FixedLevelTest &test,
    FiniteTargetPlaylistWithRepeatables &targets) {
    method.initialize(test, &targets);
}

static void initialize(AdaptiveMethod &method, const AdaptiveTest &test,
    TargetPlaylistReader &reader) {
    method.initialize(test, &reader);
}

static void initializeWithSingleSpeaker(
    RunningATest &model, AdaptiveMethod &method, const AdaptiveTest &test) {
    model.initializeWithSingleSpeaker(&method, test);
}

static void initializeWithDelayedMasker(
    RunningATest &model, TestMethod &method, const Test &test) {
    model.initializeWithDelayedMasker(&method, test);
}

static void initialize(RunningATest &model, TestMethod &method,
    const Test &test, RunningATest::Observer *observer) {
    model.initialize(&method, test, observer);
}

void RunningATestFacadeImpl::initializeWithTargetReplacement(
    const FixedLevelFixedTrialsTest &test) {
    av_speech_in_noise::initialize(
        fixedLevelMethod, test, targetsWithReplacement);
    av_speech_in_noise::initialize(runningATest, fixedLevelMethod, test);
}

void RunningATestFacadeImpl::initialize(const AdaptiveTest &test) {
    av_speech_in_noise::initialize(
        adaptiveMethod, test, targetsWithReplacementReader);
    av_speech_in_noise::initialize(runningATest, adaptiveMethod, test);
}

void RunningATestFacadeImpl::initializeWithSilentIntervalTargets(
    const FixedLevelTest &test) {
    av_speech_in_noise::initialize(
        fixedLevelMethod, test, silentIntervalTargets);
    av_speech_in_noise::initialize(runningATest, fixedLevelMethod, test);
}

void RunningATestFacadeImpl::initializeWithAllTargets(
    const FixedLevelTest &test) {
    av_speech_in_noise::initialize(fixedLevelMethod, test, everyTargetOnce);
    av_speech_in_noise::initialize(runningATest, fixedLevelMethod, test);
}

void RunningATestFacadeImpl::initialize(
    const FixedLevelTestWithEachTargetNTimes &test) {
    eachTargetNTimes.setRepeats(test.timesEachTargetIsPlayed - 1);
    av_speech_in_noise::initialize(fixedLevelMethod, test, eachTargetNTimes);
    av_speech_in_noise::initialize(runningATest, fixedLevelMethod, test);
}

void RunningATestFacadeImpl::initializeWithAllTargetsAndEyeTracking(
    const FixedLevelTest &test) {
    av_speech_in_noise::initialize(fixedLevelMethod, test, everyTargetOnce);
    av_speech_in_noise::initialize(
        runningATest, fixedLevelMethod, test, &eyeTracking);
}

void RunningATestFacadeImpl::initializeWithAllTargetsAndAudioRecording(
    const FixedLevelTest &test) {
    av_speech_in_noise::initialize(fixedLevelMethod, test, everyTargetOnce);
    av_speech_in_noise::initialize(
        runningATest, fixedLevelMethod, test, &audioRecording);
}

void RunningATestFacadeImpl::
    initializeWithPredeterminedTargetsAndAudioRecording(
        const FixedLevelTest &test) {
    av_speech_in_noise::initialize(
        fixedLevelMethod, test, predeterminedTargets);
    av_speech_in_noise::initialize(
        runningATest, fixedLevelMethod, test, &audioRecording);
}

void RunningATestFacadeImpl::initializeWithPredeterminedTargetsAndEyeTracking(
    const FixedLevelTest &test) {
    av_speech_in_noise::initialize(
        fixedLevelMethod, test, predeterminedTargets);
}

void RunningATestFacadeImpl::initializeWithSingleSpeaker(
    const AdaptiveTest &test) {
    av_speech_in_noise::initialize(
        adaptiveMethod, test, targetsWithReplacementReader);
    av_speech_in_noise::initializeWithSingleSpeaker(
        runningATest, adaptiveMethod, test);
}

void RunningATestFacadeImpl::initializeWithDelayedMasker(
    const AdaptiveTest &test) {
    av_speech_in_noise::initialize(
        adaptiveMethod, test, targetsWithReplacementReader);
    av_speech_in_noise::initializeWithDelayedMasker(
        runningATest, adaptiveMethod, test);
}

void RunningATestFacadeImpl::initializeWithTargetReplacementAndEyeTracking(
    const FixedLevelFixedTrialsTest &test) {
    av_speech_in_noise::initialize(
        fixedLevelMethod, test, targetsWithReplacement);
    av_speech_in_noise::initialize(
        runningATest, fixedLevelMethod, test, &eyeTracking);
}

void RunningATestFacadeImpl::initializeWithSilentIntervalTargetsAndEyeTracking(
    const FixedLevelTest &test) {
    av_speech_in_noise::initialize(
        fixedLevelMethod, test, silentIntervalTargets);
    av_speech_in_noise::initialize(
        runningATest, fixedLevelMethod, test, &eyeTracking);
}

void RunningATestFacadeImpl::initializeWithEyeTracking(
    const AdaptiveTest &test) {
    av_speech_in_noise::initialize(
        adaptiveMethod, test, targetsWithReplacementReader);
    av_speech_in_noise::initialize(
        runningATest, adaptiveMethod, test, &eyeTracking);
}

void RunningATestFacadeImpl::initializeWithCyclicTargets(
    const AdaptiveTest &test) {
    av_speech_in_noise::initialize(adaptiveMethod, test, cyclicTargetsReader);
    av_speech_in_noise::initialize(runningATest, adaptiveMethod, test);
}

void RunningATestFacadeImpl::initializeWithCyclicTargetsAndEyeTracking(
    const AdaptiveTest &test) {
    av_speech_in_noise::initialize(adaptiveMethod, test, cyclicTargetsReader);
    av_speech_in_noise::initialize(
        runningATest, adaptiveMethod, test, &eyeTracking);
}

void RunningATestFacadeImpl::restartAdaptiveTestWhilePreservingTargets() {
    adaptiveMethod.resetTracks();
    runningATest.prepareNextTrialIfNeeded();
}

void RunningATestFacadeImpl::playTrial(const AudioSettings &settings) {
    runningATest.playTrial(settings);
}

void RunningATestFacadeImpl::submit(
    const coordinate_response_measure::Response &response) {
    runningATest.submit(response);
}

void RunningATestFacadeImpl::playCalibration(const Calibration &p) {
    runningATest.playCalibration(p);
}

void RunningATestFacadeImpl::playLeftSpeakerCalibration(const Calibration &p) {
    runningATest.playLeftSpeakerCalibration(p);
}

void RunningATestFacadeImpl::playRightSpeakerCalibration(const Calibration &p) {
    runningATest.playRightSpeakerCalibration(p);
}

auto RunningATestFacadeImpl::testComplete() -> bool {
    return runningATest.testComplete();
}

auto RunningATestFacadeImpl::audioDevices() -> std::vector<std::string> {
    return runningATest.audioDevices();
}

auto RunningATestFacadeImpl::adaptiveTestResults() -> AdaptiveTestResults {
    return adaptiveMethod.testResults();
}

void RunningATestFacadeImpl::attach(RunningATestFacade::Observer *e) {
    runningATest.attach(e);
}

auto RunningATestFacadeImpl::trialNumber() -> int {
    return runningATest.trialNumber();
}

auto RunningATestFacadeImpl::targetFileName() -> std::string {
    return runningATest.targetFileName();
}

auto RunningATestFacadeImpl::keywordsTestResults() -> KeywordsTestResults {
    return fixedLevelMethod.keywordsTestResults();
}
}
