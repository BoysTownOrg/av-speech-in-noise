#include "RecognitionTestModel.hpp"
#include <gsl/gsl>

namespace av_speech_in_noise {
    
    
    FixedLevelMethod::FixedLevelMethod(
        TargetList *targetList,
        ResponseEvaluator *evaluator
    ) :
        targetList{targetList},
        evaluator{evaluator} {}
    
    void FixedLevelMethod::initialize(const FixedLevelTest &p) {
        test = &p;
        snr_dB_ = p.snr_dB;
        trials_ = p.trials;
        targetList->loadFromDirectory(p.common.targetListDirectory);
        updateCompletion();
    }
    
    void FixedLevelMethod::updateCompletion() {
        complete_ = trials_ == 0;
    }
    
    bool FixedLevelMethod::complete() {
        return complete_;
    }
    
    std::string FixedLevelMethod::next() {
        return targetList->next();
    }
    
    int FixedLevelMethod::snr_dB() {
        return snr_dB_;
    }
    
    void FixedLevelMethod::submitResponse(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        --trials_;
        auto current_ = current();
        lastTrial.trial.subjectColor = response.color;
        lastTrial.trial.subjectNumber = response.number;
        lastTrial.trial.correctColor = evaluator->correctColor(current_);
        lastTrial.trial.correctNumber = evaluator->correctNumber(current_);
        lastTrial.trial.correct = evaluator->correct(current_, response);
        updateCompletion();
    }
    
    std::string FixedLevelMethod::current() {
        return targetList->current();
    }
    
    void FixedLevelMethod::writeTestingParameters(OutputFile *file) {
        file->writeTest(*test);
    }
    
    void FixedLevelMethod::writeLastCoordinateResponse(OutputFile *file) {
        file->writeTrial(lastTrial);
    }
    
    void FixedLevelMethod::submitIncorrectResponse() {
        
    }
    
    void FixedLevelMethod::submitCorrectResponse() {
        
    }
    
    void FixedLevelMethod::submitResponse(const FreeResponse &) {
    
    }
    
    RecognitionTestModel::RecognitionTestModel(
        IAdaptiveMethod *adaptiveMethod,
        IFixedLevelMethod *fixedLevelMethod,
        IRecognitionTestModel_Internal *model
    ) :
        adaptiveMethod{adaptiveMethod},
        fixedLevelMethod{fixedLevelMethod},
        model{model}
    {
    }
    
    void RecognitionTestModel::initializeTest(const FixedLevelTest &p) {
        fixedLevelMethod->initialize(p);
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

