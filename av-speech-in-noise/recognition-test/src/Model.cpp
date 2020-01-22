#include "Model.hpp"
#include "av-speech-in-noise/Model.hpp"

namespace av_speech_in_noise {
ModelImpl::ModelImpl(AdaptiveMethod *adaptiveMethod,
    FixedLevelMethod *fixedLevelMethod, TargetList *infiniteTargetList,
    TestConcluder *fixedTrialTestConcluder, TargetList *silentIntervals,
    TestConcluder *completesWhenTargetsEmpty, TargetList *allStimuli,
    RecognitionTestModel *model)
    : adaptiveMethod{adaptiveMethod}, fixedLevelMethod{fixedLevelMethod},
      infiniteTargetList{infiniteTargetList},
      fixedTrialTestConcluder{fixedTrialTestConcluder},
      silentIntervals{silentIntervals},
      completesWhenTargetsEmpty{completesWhenTargetsEmpty},
      allStimuli{allStimuli}, model{model} {}

static void initialize(RecognitionTestModel *model, class TestMethod *method,
    const struct Test &test) {
        model->initialize(method, test);
}

void ModelImpl::initialize(const FixedLevelTest &p) {
    fixedLevelMethod->initialize(
        p, infiniteTargetList, fixedTrialTestConcluder);
    av_speech_in_noise::initialize(model, fixedLevelMethod, p);
}

void ModelImpl::initializeWithSilentIntervalTargets(const FixedLevelTest &p) {
    fixedLevelMethod->initialize(
        p, silentIntervals, completesWhenTargetsEmpty);
    av_speech_in_noise::initialize(model, fixedLevelMethod, p);
}

void ModelImpl::initializeWithAllTargets(const FixedLevelTest &p) {
    fixedLevelMethod->initialize(
        p, allStimuli, completesWhenTargetsEmpty);
    av_speech_in_noise::initialize(model, fixedLevelMethod, p);
}

void ModelImpl::initialize(const AdaptiveTest &p) {
    initializeTest_(p);
}

void ModelImpl::initializeTest_(const AdaptiveTest &p) {
    adaptiveMethod->initialize(p);
    av_speech_in_noise::initialize(model, adaptiveMethod, p);
}

void ModelImpl::initializeWithSingleSpeaker(const AdaptiveTest &p) {
    adaptiveMethod->initialize(p);
    model->initializeWithSingleSpeaker(adaptiveMethod, p);
}

void ModelImpl::initializeWithDelayedMasker(const AdaptiveTest &p) {
    adaptiveMethod->initialize(p);
    model->initializeWithDelayedMasker(adaptiveMethod, p);
}

void ModelImpl::initializeEyeTrackingTest(const FixedLevelTest &p) {
    fixedLevelMethod->initialize(
        p, infiniteTargetList, fixedTrialTestConcluder);
    model->initializeWithEyeTracking(fixedLevelMethod, p);
}

void ModelImpl::initializeEyeTrackingTest(const AdaptiveTest &p) {
    adaptiveMethod->initialize(p);
    model->initializeWithEyeTracking(adaptiveMethod, p);
}

void ModelImpl::playTrial(const AudioSettings &settings) {
    model->playTrial(settings);
}

void ModelImpl::submitResponse(
    const coordinate_response_measure::Response &response) {
    model->submitResponse(response);
}

void ModelImpl::submitCorrectResponse() { model->submitCorrectResponse(); }

void ModelImpl::submitIncorrectResponse() { model->submitIncorrectResponse(); }

void ModelImpl::submitResponse(const open_set::FreeResponse &response) {
    model->submitResponse(response);
}

void ModelImpl::playCalibration(const Calibration &p) {
    model->playCalibration(p);
}

auto ModelImpl::testComplete() -> bool { return model->testComplete(); }

auto ModelImpl::audioDevices() -> std::vector<std::string> {
    return model->audioDevices();
}

void ModelImpl::subscribe(Model::EventListener *e) { model->subscribe(e); }

auto ModelImpl::trialNumber() -> int { return model->trialNumber(); }
}
