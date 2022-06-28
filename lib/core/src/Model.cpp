#include "Model.hpp"
#include "AudioRecording.hpp"
#include "IRecognitionTestModel.hpp"

namespace av_speech_in_noise {
ModelImpl::ModelImpl(AdaptiveMethod &adaptiveMethod,
    FixedLevelMethod &fixedLevelMethod,
    TargetPlaylistReader &targetsWithReplacementReader,
    TargetPlaylistReader &cyclicTargetsReader,
    TargetPlaylist &targetsWithReplacement,
    FiniteTargetPlaylistWithRepeatables &silentIntervalTargets,
    FiniteTargetPlaylistWithRepeatables &everyTargetOnce,
    RepeatableFiniteTargetPlaylist &eachTargetNTimes,
    FiniteTargetPlaylistWithRepeatables &predeterminedTargets,
    RunningATest &model, OutputFile &outputFile, AudioRecorder &audioRecorder)
    : adaptiveMethod{adaptiveMethod}, fixedLevelMethod{fixedLevelMethod},
      targetsWithReplacementReader{targetsWithReplacementReader},
      cyclicTargetsReader{cyclicTargetsReader},
      targetsWithReplacement{targetsWithReplacement},
      silentIntervalTargets{silentIntervalTargets},
      everyTargetOnce{everyTargetOnce},
      predeterminedTargets{predeterminedTargets},
      eachTargetNTimes{eachTargetNTimes}, model{model}, outputFile{outputFile},
      audioRecording{audioRecorder, outputFile} {}

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

static void initializeWithEyeTracking(
    RunningATest &model, TestMethod &method, const Test &test) {
    model.initializeWithEyeTracking(&method, test);
}

static void initialize(RunningATest &model, TestMethod &method,
    const Test &test, RunningATest::Observer *observer) {
    model.initialize(&method, test, observer);
}

void ModelImpl::initializeWithTargetReplacement(
    const FixedLevelFixedTrialsTest &test) {
    av_speech_in_noise::initialize(
        fixedLevelMethod, test, targetsWithReplacement);
    av_speech_in_noise::initialize(model, fixedLevelMethod, test);
}

void ModelImpl::initialize(const AdaptiveTest &test) {
    av_speech_in_noise::initialize(
        adaptiveMethod, test, targetsWithReplacementReader);
    av_speech_in_noise::initialize(model, adaptiveMethod, test);
}

void ModelImpl::initializeWithSilentIntervalTargets(
    const FixedLevelTest &test) {
    av_speech_in_noise::initialize(
        fixedLevelMethod, test, silentIntervalTargets);
    av_speech_in_noise::initialize(model, fixedLevelMethod, test);
}

void ModelImpl::initializeWithAllTargets(const FixedLevelTest &test) {
    av_speech_in_noise::initialize(fixedLevelMethod, test, everyTargetOnce);
    av_speech_in_noise::initialize(model, fixedLevelMethod, test);
}

void ModelImpl::initialize(const FixedLevelTestWithEachTargetNTimes &test) {
    eachTargetNTimes.setRepeats(test.timesEachTargetIsPlayed - 1);
    av_speech_in_noise::initialize(fixedLevelMethod, test, eachTargetNTimes);
    av_speech_in_noise::initialize(model, fixedLevelMethod, test);
}

void ModelImpl::initializeWithAllTargetsAndEyeTracking(
    const FixedLevelTest &test) {
    av_speech_in_noise::initialize(fixedLevelMethod, test, everyTargetOnce);
    av_speech_in_noise::initializeWithEyeTracking(
        model, fixedLevelMethod, test);
}

void ModelImpl::initializeWithAllTargetsAndAudioRecording(
    const FixedLevelTest &test) {
    av_speech_in_noise::initialize(fixedLevelMethod, test, everyTargetOnce);
    av_speech_in_noise::initialize(
        model, fixedLevelMethod, test, &audioRecording);
}

void ModelImpl::initializeWithPredeterminedTargetsAndAudioRecording(
    const FixedLevelTest &test) {
    av_speech_in_noise::initialize(
        fixedLevelMethod, test, predeterminedTargets);
    av_speech_in_noise::initialize(
        model, fixedLevelMethod, test, &audioRecording);
}

void ModelImpl::initializeWithSingleSpeaker(const AdaptiveTest &test) {
    av_speech_in_noise::initialize(
        adaptiveMethod, test, targetsWithReplacementReader);
    av_speech_in_noise::initializeWithSingleSpeaker(
        model, adaptiveMethod, test);
}

void ModelImpl::initializeWithDelayedMasker(const AdaptiveTest &test) {
    av_speech_in_noise::initialize(
        adaptiveMethod, test, targetsWithReplacementReader);
    av_speech_in_noise::initializeWithDelayedMasker(
        model, adaptiveMethod, test);
}

void ModelImpl::initializeWithTargetReplacementAndEyeTracking(
    const FixedLevelFixedTrialsTest &test) {
    av_speech_in_noise::initialize(
        fixedLevelMethod, test, targetsWithReplacement);
    av_speech_in_noise::initializeWithEyeTracking(
        model, fixedLevelMethod, test);
}

void ModelImpl::initializeWithSilentIntervalTargetsAndEyeTracking(
    const FixedLevelTest &test) {
    av_speech_in_noise::initialize(
        fixedLevelMethod, test, silentIntervalTargets);
    av_speech_in_noise::initializeWithEyeTracking(
        model, fixedLevelMethod, test);
}

void ModelImpl::initializeWithEyeTracking(const AdaptiveTest &test) {
    av_speech_in_noise::initialize(
        adaptiveMethod, test, targetsWithReplacementReader);
    av_speech_in_noise::initializeWithEyeTracking(model, adaptiveMethod, test);
}

void ModelImpl::initializeWithCyclicTargets(const AdaptiveTest &test) {
    av_speech_in_noise::initialize(adaptiveMethod, test, cyclicTargetsReader);
    av_speech_in_noise::initialize(model, adaptiveMethod, test);
}

void ModelImpl::initializeWithCyclicTargetsAndEyeTracking(
    const AdaptiveTest &test) {
    av_speech_in_noise::initialize(adaptiveMethod, test, cyclicTargetsReader);
    av_speech_in_noise::initializeWithEyeTracking(model, adaptiveMethod, test);
}

void ModelImpl::restartAdaptiveTestWhilePreservingTargets() {
    adaptiveMethod.resetTracks();
    model.prepareNextTrialIfNeeded();
}

void ModelImpl::playTrial(const AudioSettings &settings) {
    model.playTrial(settings);
}

void ModelImpl::submit(const coordinate_response_measure::Response &response) {
    model.submit(response);
}

void ModelImpl::playCalibration(const Calibration &p) {
    model.playCalibration(p);
}

void ModelImpl::playLeftSpeakerCalibration(const Calibration &p) {
    model.playLeftSpeakerCalibration(p);
}

void ModelImpl::playRightSpeakerCalibration(const Calibration &p) {
    model.playRightSpeakerCalibration(p);
}

auto ModelImpl::testComplete() -> bool { return model.testComplete(); }

auto ModelImpl::audioDevices() -> std::vector<std::string> {
    return model.audioDevices();
}

auto ModelImpl::adaptiveTestResults() -> AdaptiveTestResults {
    return adaptiveMethod.testResults();
}

void ModelImpl::attach(Model::Observer *e) { model.attach(e); }

auto ModelImpl::trialNumber() -> int { return model.trialNumber(); }

auto ModelImpl::targetFileName() -> std::string {
    return model.targetFileName();
}

auto ModelImpl::keywordsTestResults() -> KeywordsTestResults {
    return fixedLevelMethod.keywordsTestResults();
}
}
