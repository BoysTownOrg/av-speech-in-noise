#include "FixedLevelMethod.hpp"

namespace av_speech_in_noise {
FixedLevelMethodImpl::FixedLevelMethodImpl(ResponseEvaluator *evaluator)
    : evaluator{evaluator} {}

void FixedLevelMethodImpl::initialize(
    const FixedLevelTest &p, TargetList *list, TestConcluder *concluder_) {
    concluder = concluder_;
    targetList = list;
    test = &p;
    snr_dB_ = p.snr_dB;
    complete_ = false;
    targetList->loadFromDirectory(p.targetListDirectory);
    concluder->initialize(p);
}

bool FixedLevelMethodImpl::complete() {
    return complete_;
}

std::string FixedLevelMethodImpl::next() { return targetList->next(); }

int FixedLevelMethodImpl::snr_dB() { return snr_dB_; }

void FixedLevelMethodImpl::submitResponse(
    const coordinate_response_measure::Response &response) {
    auto current_ = current();
    lastTrial.subjectColor = response.color;
    lastTrial.subjectNumber = response.number;
    lastTrial.correctColor = evaluator->correctColor(current_);
    lastTrial.correctNumber = evaluator->correctNumber(current_);
    lastTrial.correct = evaluator->correct(current_, response);
    lastTrial.target = current_;
    concluder->submitResponse();
    complete_ = concluder->complete(targetList);
}

std::string FixedLevelMethodImpl::current() { return targetList->current(); }

void FixedLevelMethodImpl::writeTestingParameters(OutputFile *file) {
    file->writeTest(*test);
}

void FixedLevelMethodImpl::writeLastCoordinateResponse(OutputFile *file) {
    file->writeTrial(lastTrial);
}

void FixedLevelMethodImpl::submitIncorrectResponse() {}

void FixedLevelMethodImpl::submitCorrectResponse() {}

void FixedLevelMethodImpl::submitResponse(const FreeResponse &) {
    complete_ = concluder->complete(targetList);
}
}
