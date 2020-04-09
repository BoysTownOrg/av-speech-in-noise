#include "Model.hpp"
#include "TestMethod.hpp"
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
ModelImpl::ModelImpl(AdaptiveMethod &adaptiveMethod,
    FixedLevelMethod &fixedLevelMethod,
    TargetListReader &targetsWithReplacementReader,
    TargetListReader &cyclicTargetsReader, TargetList &targetsWithReplacement,
    FiniteTargetList &silentIntervalTargets, FiniteTargetList &everyTargetOnce,
    RecognitionTestModel &model)
    : adaptiveMethod{adaptiveMethod}, fixedLevelMethod{fixedLevelMethod},
      targetsWithReplacementReader{targetsWithReplacementReader},
      cyclicTargetsReader{cyclicTargetsReader},
      targetsWithReplacement{targetsWithReplacement},
      silentIntervalTargets{silentIntervalTargets},
      everyTargetOnce{everyTargetOnce}, model{model} {}

static void initialize(
    RecognitionTestModel &model, TestMethod &method, const Test &test) {
    model.initialize(&method, test);
}

static void initialize(
    FixedLevelMethod &method, const FixedLevelTest &test, TargetList &targets) {
    method.initialize(test, &targets);
}

static void initialize(FixedLevelMethod &method, const FixedLevelTest &test,
    FiniteTargetList &targets) {
    method.initialize(test, &targets);
}

static void initialize(AdaptiveMethod &method, const AdaptiveTest &test,
    TargetListReader &reader) {
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

void ModelImpl::initializeWithTargetReplacement(const FixedLevelTest &test) {
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

void ModelImpl::initializeWithCyclicTargets(const AdaptiveTest &test) {
    av_speech_in_noise::initialize(adaptiveMethod, test, cyclicTargetsReader);
    av_speech_in_noise::initialize(model, adaptiveMethod, test);
}

void ModelImpl::restartAdaptiveTestWhilePreservingCyclicTargets() {
    adaptiveMethod.resetTracks();
    model.prepareNextTrialIfNeeded();
}

void ModelImpl::playTrial(const AudioSettings &settings) {
    model.playTrial(settings);
}

void ModelImpl::submit(const coordinate_response_measure::Response &response) {
    model.submit(response);
}

void ModelImpl::submitCorrectResponse() { model.submitCorrectResponse(); }

void ModelImpl::submitIncorrectResponse() { model.submitIncorrectResponse(); }

void ModelImpl::submit(const FreeResponse &response) { model.submit(response); }

void ModelImpl::submit(const CorrectKeywords &k) { model.submit(k); }

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

void ModelImpl::subscribe(Model::EventListener *e) { model.subscribe(e); }

auto ModelImpl::trialNumber() -> int { return model.trialNumber(); }

auto ModelImpl::targetFileName() -> std::string {
    return model.targetFileName();
}
}
