#include "Model.hpp"

namespace av_speech_in_noise {
ModelImpl::ModelImpl(AdaptiveMethod &adaptiveMethod,
    FixedLevelMethod &fixedLevelMethod,
    TargetPlaylistReader &targetsWithReplacementReader,
    TargetPlaylistReader &cyclicTargetsReader,
    TargetPlaylist &targetsWithReplacement,
    FiniteTargetPlaylistWithRepeatables &silentIntervalTargets,
    FiniteTargetPlaylistWithRepeatables &everyTargetOnce,
    RepeatableFiniteTargetPlaylist &eachTargetNTimes,
    RecognitionTestModel &model, OutputFile &outputFile)
    : adaptiveMethod{adaptiveMethod}, fixedLevelMethod{fixedLevelMethod},
      targetsWithReplacementReader{targetsWithReplacementReader},
      cyclicTargetsReader{cyclicTargetsReader},
      targetsWithReplacement{targetsWithReplacement},
      silentIntervalTargets{silentIntervalTargets},
      everyTargetOnce{everyTargetOnce},
      eachTargetNTimes{eachTargetNTimes}, model{model}, outputFile{outputFile} {
}

static void initialize(
    RecognitionTestModel &model, TestMethod &method, const Test &test) {
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

static void initializeWithSingleSpeaker(RecognitionTestModel &model,
    AdaptiveMethod &method, const AdaptiveTest &test) {
    model.initializeWithSingleSpeaker(&method, test);
}

static void initializeWithDelayedMasker(
    RecognitionTestModel &model, TestMethod &method, const Test &test) {
    model.initializeWithDelayedMasker(&method, test);
}

static void initializeWithEyeTracking(
    RecognitionTestModel &model, TestMethod &method, const Test &test) {
    model.initializeWithEyeTracking(&method, test);
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

void ModelImpl::initialize(
    const FixedLevelTestWithEachTargetNTimesAndFiltering &test) {
    av_speech_in_noise::initialize(fixedLevelMethod, test, eachTargetNTimes);
}

void ModelImpl::initializeWithAllTargetsAndEyeTracking(
    const FixedLevelTest &test) {
    av_speech_in_noise::initialize(fixedLevelMethod, test, everyTargetOnce);
    av_speech_in_noise::initialize(model, fixedLevelMethod, test);
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

void ModelImpl::submitCorrectResponse() {
    adaptiveMethod.submitCorrectResponse();
    adaptiveMethod.writeLastCorrectResponse(outputFile);
    model.submitCorrectResponse();
}

void ModelImpl::submitIncorrectResponse() {
    adaptiveMethod.submitIncorrectResponse();
    adaptiveMethod.writeLastIncorrectResponse(outputFile);
    model.submitIncorrectResponse();
}

void ModelImpl::submit(const FreeResponse &response) { model.submit(response); }

void ModelImpl::submit(const CorrectKeywords &k) {
    adaptiveMethod.submit(k);
    adaptiveMethod.writeLastCorrectKeywords(outputFile);
    model.submit(k);
}

void ModelImpl::submit(const ConsonantResponse &r) {
    fixedLevelMethod.submit(r);
    fixedLevelMethod.writeLastConsonant(outputFile);
    model.submit(r);
}

void ModelImpl::playCalibration(const Calibration &p) {
    model.playCalibration(p);
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
}
