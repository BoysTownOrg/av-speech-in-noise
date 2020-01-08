#include "Model.hpp"

namespace av_speech_in_noise {
ModelImpl::ModelImpl(AdaptiveMethod *adaptiveMethod,
    FixedLevelMethod *fixedLevelMethod, TargetList *infiniteTargetList,
    TestConcluder *fixedTrialTestConcluder, TargetList *finiteTargetList,
    TestConcluder *completesWhenTargetsEmpty, RecognitionTestModel *model)
    : adaptiveMethod{adaptiveMethod}, fixedLevelMethod{fixedLevelMethod},
      infiniteTargetList{infiniteTargetList},
      fixedTrialTestConcluder{fixedTrialTestConcluder},
      finiteTargetList{finiteTargetList},
      completesWhenTargetsEmpty{completesWhenTargetsEmpty}, model{model} {}

static void initialize(RecognitionTestModel *model, class TestMethod *method,
    const struct Test &test) {
        model->initialize(method, test);
}

void ModelImpl::initializeTest(const FixedLevelTest &p) {
    fixedLevelMethod->initialize(
        p, infiniteTargetList, fixedTrialTestConcluder);
    initialize(model, fixedLevelMethod, p);
}

void ModelImpl::initializeSilentIntervalsTest(const FixedLevelTest &p) {
    fixedLevelMethod->initialize(
        p, finiteTargetList, completesWhenTargetsEmpty);
    initialize(model, fixedLevelMethod, p);
}

void ModelImpl::initializeTest(const AdaptiveTest &p) {
    initializeTest_(p);
}

void ModelImpl::initializeTest_(const AdaptiveTest &p) {
    adaptiveMethod->initialize(p);
    initialize(model, adaptiveMethod, p);
}

void ModelImpl::initializeTestWithSingleSpeaker(const AdaptiveTest &p) {
    adaptiveMethod->initialize(p);
    model->initializeWithSingleSpeaker(adaptiveMethod, p);
}

void ModelImpl::initializeTestWithDelayedMasker(const AdaptiveTest &p) {
    adaptiveMethod->initialize(p);
    model->initializeWithDelayedMasker(adaptiveMethod, p);
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
