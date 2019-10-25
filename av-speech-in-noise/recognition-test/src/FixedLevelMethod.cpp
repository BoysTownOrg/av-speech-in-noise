#include "FixedLevelMethod.hpp"

namespace av_speech_in_noise {
FixedLevelMethod::FixedLevelMethod(ResponseEvaluator *evaluator)
    : evaluator{evaluator} {}

void FixedLevelMethod::initialize(
    const FixedLevelTest &p, TargetList *list, TestConcluder *concluder_) {
    concluder = concluder_;
    targetList = list;
    test = &p;
    snr_dB_ = p.snr_dB;
    targetList->loadFromDirectory(p.targetListDirectory);
    concluder->initialize(p);
}

bool FixedLevelMethod::complete() { return concluder->complete(targetList); }

std::string FixedLevelMethod::next() { return targetList->next(); }

int FixedLevelMethod::snr_dB() { return snr_dB_; }

void FixedLevelMethod::submitResponse(
    const coordinate_response_measure::Response &response) {
    auto current_ = current();
    lastTrial.subjectColor = response.color;
    lastTrial.subjectNumber = response.number;
    lastTrial.correctColor = evaluator->correctColor(current_);
    lastTrial.correctNumber = evaluator->correctNumber(current_);
    lastTrial.correct = evaluator->correct(current_, response);
    lastTrial.target = current_;
    concluder->submitResponse();
}

std::string FixedLevelMethod::current() { return targetList->current(); }

void FixedLevelMethod::writeTestingParameters(OutputFile *file) {
    file->writeTest(*test);
}

void FixedLevelMethod::writeLastCoordinateResponse(OutputFile *file) {
    file->writeTrial(lastTrial);
}

void FixedLevelMethod::submitIncorrectResponse() {}

void FixedLevelMethod::submitCorrectResponse() {}

void FixedLevelMethod::submitResponse(const FreeResponse &) {}
}
