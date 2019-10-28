#include "Model.hpp"
#include <gsl/gsl>

namespace av_speech_in_noise {
ModelImpl::ModelImpl(AdaptiveMethod *adaptiveMethod,
    FixedLevelMethod *fixedLevelMethod, TargetList *infiniteTargetList,
    TestConcluder *fixedTrialTestConcluder, TargetList *finiteTargetList,
    TestConcluder *completesWhenTargetsEmpty,
    RecognitionTestModel *model)
    : adaptiveMethod{adaptiveMethod}, fixedLevelMethod{fixedLevelMethod},
      infiniteTargetList{infiniteTargetList},
      fixedTrialTestConcluder{fixedTrialTestConcluder},
      finiteTargetList{finiteTargetList},
      completesWhenTargetsEmpty{completesWhenTargetsEmpty}, model{model} {}

void ModelImpl::initializeTest(const FixedLevelTest &p) {
    fixedLevelMethod->initialize(
        p, infiniteTargetList, fixedTrialTestConcluder);
    model->initialize(fixedLevelMethod, p, p.identity);
}

void ModelImpl::initializeTestWithFiniteTargets(
    const FixedLevelTest &p) {
    fixedLevelMethod->initialize(
        p, finiteTargetList, completesWhenTargetsEmpty);
    model->initialize(fixedLevelMethod, p, p.identity);
}

void ModelImpl::initializeTest(const AdaptiveTest &p) {
    adaptiveMethod->initialize(p);
    model->initialize(adaptiveMethod, p, p.identity);
}

void ModelImpl::playTrial(const AudioSettings &settings) {
    model->playTrial(settings);
}

void ModelImpl::submitResponse(
    const coordinate_response_measure::Response &response) {
    model->submitResponse(response);
}

void ModelImpl::submitCorrectResponse() {
    model->submitCorrectResponse();
}

void ModelImpl::submitIncorrectResponse() {
    model->submitIncorrectResponse();
}

void ModelImpl::submitResponse(const FreeResponse &response) {
    model->submitResponse(response);
}

void ModelImpl::playCalibration(const Calibration &p) {
    model->playCalibration(p);
}

bool ModelImpl::testComplete() { return model->testComplete(); }

std::vector<std::string> ModelImpl::audioDevices() {
    return model->audioDevices();
}

void ModelImpl::subscribe(Model::EventListener *e) {
    model->subscribe(e);
}

int ModelImpl::trialNumber() { return model->trialNumber(); }
}
