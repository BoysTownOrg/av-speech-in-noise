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
    targetList->loadFromDirectory(p.targetListDirectory);
    concluder->initialize(p);
    complete_ = concluder->complete(targetList);
}

auto FixedLevelMethodImpl::complete() -> bool { return complete_; }

auto FixedLevelMethodImpl::next() -> std::string { return targetList->next(); }

auto FixedLevelMethodImpl::snr_dB() -> int { return snr_dB_; }

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

auto FixedLevelMethodImpl::current() -> std::string {
    return targetList->current();
}

void FixedLevelMethodImpl::writeTestingParameters(OutputFile *file) {
    file->writeTest(*test);
}

void FixedLevelMethodImpl::writeLastCoordinateResponse(OutputFile *file) {
    file->writeTrial(lastTrial);
}

void FixedLevelMethodImpl::submitIncorrectResponse() {}

void FixedLevelMethodImpl::submitCorrectResponse() {}

void FixedLevelMethodImpl::submitResponse(
    const open_set::FreeResponse &response) {
    concluder->submitResponse();
    if (response.flagged)
        targetList->reinsertCurrent();
    complete_ = concluder->complete(targetList);
}
}
