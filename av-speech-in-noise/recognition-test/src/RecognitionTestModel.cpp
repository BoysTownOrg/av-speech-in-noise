#include "RecognitionTestModel.hpp"
#include <gsl/gsl>

namespace av_speech_in_noise {
    RecognitionTestModel::RecognitionTestModel(
        IAdaptiveMethod *adaptiveMethod,
        IFixedLevelMethod *fixedLevelMethod,
        TargetList *infiniteTargetList,
        TargetList *finiteTargetList,
        IRecognitionTestModel_Internal *model
    ) :
        adaptiveMethod{adaptiveMethod},
        fixedLevelMethod{fixedLevelMethod},
        infiniteTargetList{infiniteTargetList},
        finiteTargetList{finiteTargetList},
        model{model}
    {
    }
    
    void RecognitionTestModel::initializeTest(const FixedLevelTest &p) {
        fixedLevelMethod->initialize(p, infiniteTargetList);
        model->initialize(fixedLevelMethod, p.common, p.information);
    }
    
    void RecognitionTestModel::initializeTestWithFiniteTargets(const FixedLevelTest &p) {
        fixedLevelMethod->initialize(p, finiteTargetList);
        model->initialize(fixedLevelMethod, p.common, p.information);
    }
    
    void RecognitionTestModel::initializeTest(const AdaptiveTest &p) {
        adaptiveMethod->initialize(p);
        model->initialize(adaptiveMethod, p.common, p.information);
    }
    
    void RecognitionTestModel::playTrial(const AudioSettings &settings) {
        model->playTrial(settings);
    }
    
    void RecognitionTestModel::submitResponse(
        const coordinate_response_measure::SubjectResponse &response
    ) {
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

    bool RecognitionTestModel::testComplete() {
        return model->testComplete();
    }
    
    std::vector<std::string> RecognitionTestModel::audioDevices() {
        return model->audioDevices();
    }
    
    void RecognitionTestModel::subscribe(Model::EventListener *e) {
        model->subscribe(e);
    }
}

