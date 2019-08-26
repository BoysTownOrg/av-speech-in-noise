#include "FixedLevelMethod.hpp"

namespace av_speech_in_noise {
    FixedLevelMethod::FixedLevelMethod(
        TargetList *targetList,
        ResponseEvaluator *evaluator,
        TestConcluder *concluder
    ) :
        targetList{targetList},
        evaluator{evaluator},
        concluder{concluder} {}
    
    void FixedLevelMethod::initialize(const FixedLevelTest &p, TargetList *list) {
        if (list)
            targetList = list;
        test = &p;
        snr_dB_ = p.snr_dB;
        targetList->loadFromDirectory(p.common.targetListDirectory);
        concluder->initialize(p);
    }
    
    bool FixedLevelMethod::complete() {
        return concluder->complete(targetList);
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
        auto current_ = current();
        lastTrial.trial.subjectColor = response.color;
        lastTrial.trial.subjectNumber = response.number;
        lastTrial.trial.correctColor = evaluator->correctColor(current_);
        lastTrial.trial.correctNumber = evaluator->correctNumber(current_);
        lastTrial.trial.correct = evaluator->correct(current_, response);
        concluder->submitResponse();
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
}
