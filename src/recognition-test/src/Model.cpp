#include "Model.hpp"
#include "TestMethod.hpp"
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
ModelImpl::ModelImpl(AdaptiveMethod &adaptiveMethod,
    FixedLevelMethod &fixedLevelMethod, TargetList &targetsWithReplacement,
    TestConcluder &fixedTrialTestConcluder, TargetList &silentIntervalTargets,
    TestConcluder &completesWhenTargetsEmpty, TargetList &everyTargetOnce,
    RecognitionTestModel &model)
    : adaptiveMethod{adaptiveMethod}, fixedLevelMethod{fixedLevelMethod},
      targetsWithReplacement{targetsWithReplacement},
      fixedTrialTestConcluder{fixedTrialTestConcluder},
      silentIntervalTargets{silentIntervalTargets},
      completesWhenTargetsEmpty{completesWhenTargetsEmpty},
      everyTargetOnce{everyTargetOnce}, model{model} {}

static void initialize(
    RecognitionTestModel &model, TestMethod &method, const struct Test &test) {
    model.initialize(&method, test);
}

static void initialize(FixedLevelMethod &method, const FixedLevelTest &test,
    TargetList &targets, TestConcluder &concluder) {
    method.initialize(test, &targets, &concluder);
}

static void initialize(AdaptiveMethod &method, const AdaptiveTest &test) {
    method.initialize(test);
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

void ModelImpl::initializeWithTargetReplacement(const FixedLevelTest &test) {
    av_speech_in_noise::initialize(fixedLevelMethod, test,
        targetsWithReplacement, fixedTrialTestConcluder);
    av_speech_in_noise::initialize(model, fixedLevelMethod, test);
}

void ModelImpl::initialize(const AdaptiveTest &test) {
    av_speech_in_noise::initialize(adaptiveMethod, test);
    av_speech_in_noise::initialize(model, adaptiveMethod, test);
}

void ModelImpl::initializeWithSilentIntervalTargets(
    const FixedLevelTest &test) {
    av_speech_in_noise::initialize(fixedLevelMethod, test,
        silentIntervalTargets, completesWhenTargetsEmpty);
    av_speech_in_noise::initialize(model, fixedLevelMethod, test);
}

void ModelImpl::initializeWithAllTargets(const FixedLevelTest &test) {
    av_speech_in_noise::initialize(
        fixedLevelMethod, test, everyTargetOnce, completesWhenTargetsEmpty);
    av_speech_in_noise::initialize(model, fixedLevelMethod, test);
}

void ModelImpl::initializeWithSingleSpeaker(const AdaptiveTest &test) {
    av_speech_in_noise::initialize(adaptiveMethod, test);
    av_speech_in_noise::initializeWithSingleSpeaker(
        model, adaptiveMethod, test);
}

void ModelImpl::initializeWithDelayedMasker(const AdaptiveTest &test) {
    av_speech_in_noise::initialize(adaptiveMethod, test);
    av_speech_in_noise::initializeWithDelayedMasker(
        model, adaptiveMethod, test);
}

void ModelImpl::initializeWithTargetReplacementAndEyeTracking(
    const FixedLevelTest &test) {
    av_speech_in_noise::initialize(fixedLevelMethod, test,
        targetsWithReplacement, fixedTrialTestConcluder);
    av_speech_in_noise::initializeWithEyeTracking(
        model, fixedLevelMethod, test);
}

void ModelImpl::initializeWithSilentIntervalTargetsAndEyeTracking(
    const FixedLevelTest &test) {
    av_speech_in_noise::initialize(
        fixedLevelMethod, test, silentIntervalTargets, fixedTrialTestConcluder);
}

void ModelImpl::initializeWithEyeTracking(const AdaptiveTest &test) {
    av_speech_in_noise::initialize(adaptiveMethod, test);
    av_speech_in_noise::initializeWithEyeTracking(model, adaptiveMethod, test);
}

void ModelImpl::playTrial(const AudioSettings &settings) {
    model.playTrial(settings);
}

void ModelImpl::submit(const coordinate_response_measure::Response &response) {
    model.submit(response);
}

void ModelImpl::submitCorrectResponse() { model.submitCorrectResponse(); }

void ModelImpl::submitIncorrectResponse() { model.submitIncorrectResponse(); }

void ModelImpl::submit(const open_set::FreeResponse &response) {
    model.submit(response);
}

void ModelImpl::submit(const open_set::CorrectKeywords &k) { model.submit(k); }

void ModelImpl::playCalibration(const Calibration &p) {
    model.playCalibration(p);
}

auto ModelImpl::testComplete() -> bool { return model.testComplete(); }

auto ModelImpl::audioDevices() -> std::vector<std::string> {
    return model.audioDevices();
}

void ModelImpl::subscribe(Model::EventListener *e) { model.subscribe(e); }

auto ModelImpl::trialNumber() -> int { return model.trialNumber(); }
}
