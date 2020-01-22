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

void ModelImpl::initialize(const FixedLevelTest &test) {
    fixedLevelMethod->initialize(
        test, infiniteTargetList, fixedTrialTestConcluder);
    av_speech_in_noise::initialize(model, fixedLevelMethod, test);
}

void ModelImpl::initializeWithSilentIntervalTargets(
    const FixedLevelTest &test) {
    fixedLevelMethod->initialize(
        test, silentIntervals, completesWhenTargetsEmpty);
    av_speech_in_noise::initialize(model, fixedLevelMethod, test);
}

void ModelImpl::initializeWithAllTargets(const FixedLevelTest &test) {
    fixedLevelMethod->initialize(test, allStimuli, completesWhenTargetsEmpty);
    av_speech_in_noise::initialize(model, fixedLevelMethod, test);
}

void ModelImpl::initialize(const AdaptiveTest &test) { initializeTest_(test); }

void ModelImpl::initializeTest_(const AdaptiveTest &test) {
    adaptiveMethod->initialize(test);
    av_speech_in_noise::initialize(model, adaptiveMethod, test);
}

void ModelImpl::initializeWithSingleSpeaker(const AdaptiveTest &test) {
    adaptiveMethod->initialize(test);
    model->initializeWithSingleSpeaker(adaptiveMethod, test);
}

void ModelImpl::initializeWithDelayedMasker(const AdaptiveTest &test) {
    adaptiveMethod->initialize(test);
    model->initializeWithDelayedMasker(adaptiveMethod, test);
}

void ModelImpl::initializeWithEyeTracking(const FixedLevelTest &test) {
    fixedLevelMethod->initialize(
        test, infiniteTargetList, fixedTrialTestConcluder);
    model->initializeWithEyeTracking(fixedLevelMethod, test);
}

void ModelImpl::initializeWithEyeTracking(const AdaptiveTest &test) {
    adaptiveMethod->initialize(test);
    model->initializeWithEyeTracking(adaptiveMethod, test);
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
