#include "RecognitionTestModel.hpp"
#include <gsl/gsl>

namespace av_speech_in_noise {
RecognitionTestModel::RecognitionTestModel(AdaptiveMethod *adaptiveMethod,
    FixedLevelMethod *fixedLevelMethod, TargetList *infiniteTargetList,
    TestConcluder *fixedTrialTestConcluder, TargetList *finiteTargetList,
    TestConcluder *completesWhenTargetsEmpty,
    RecognitionTestModel_ *model)
    : adaptiveMethod{adaptiveMethod}, fixedLevelMethod{fixedLevelMethod},
      infiniteTargetList{infiniteTargetList},
      fixedTrialTestConcluder{fixedTrialTestConcluder},
      finiteTargetList{finiteTargetList},
      completesWhenTargetsEmpty{completesWhenTargetsEmpty}, model{model} {}

void RecognitionTestModel::initializeTest(const FixedLevelTest &p) {
    fixedLevelMethod->initialize(
        p, infiniteTargetList, fixedTrialTestConcluder);
    model->initialize(fixedLevelMethod, p, p.identity);
}

void RecognitionTestModel::initializeTestWithFiniteTargets(
    const FixedLevelTest &p) {
    fixedLevelMethod->initialize(
        p, finiteTargetList, completesWhenTargetsEmpty);
    model->initialize(fixedLevelMethod, p, p.identity);
}

void RecognitionTestModel::initializeTest(const AdaptiveTest &p) {
    adaptiveMethod->initialize(p);
    model->initialize(adaptiveMethod, p, p.identity);
}

void RecognitionTestModel::playTrial(const AudioSettings &settings) {
    model->playTrial(settings);
}

void RecognitionTestModel::submitResponse(
    const coordinate_response_measure::Response &response) {
    model->submitResponse(response);
}

void RecognitionTestModel::submitCorrectResponse() {
    model->submitCorrectResponse();
}

void RecognitionTestModel::submitIncorrectResponse() {
    model->submitIncorrectResponse();
}

void RecognitionTestModel::submitResponse(const FreeResponse &response) {
    model->submitResponse(response);
}

void RecognitionTestModel::playCalibration(const Calibration &p) {
    model->playCalibration(p);
}

bool RecognitionTestModel::testComplete() { return model->testComplete(); }

std::vector<std::string> RecognitionTestModel::audioDevices() {
    return model->audioDevices();
}

void RecognitionTestModel::subscribe(Model::EventListener *e) {
    model->subscribe(e);
}

int RecognitionTestModel::trialNumber() { return model->trialNumber(); }
}
