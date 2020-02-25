#include "FixedLevelMethod.hpp"

namespace av_speech_in_noise {
FixedLevelMethodImpl::FixedLevelMethodImpl(ResponseEvaluator *evaluator)
    : evaluator{evaluator} {}

static void loadFromDirectory(TargetList *list, const FixedLevelTest &test) {
    list->loadFromDirectory(test.targetListDirectory);
}

void FixedLevelMethodImpl::initialize(
    const FixedLevelTest &p, TargetList *list, TestConcluder *concluder_) {
    usingFiniteTargetList_ = false;
    concluder = concluder_;
    targetList = list;
    test = &p;
    snr_dB_ = p.snr_dB;
    loadFromDirectory(targetList, p);
    concluder->initialize(p);
    complete_ = concluder->complete(targetList);
}

void FixedLevelMethodImpl::initialize(const FixedLevelTest &p,
    FiniteTargetList *list, TestConcluder *concluder_) {
    usingFiniteTargetList_ = true;
    concluder = concluder_;
    targetList = list;
    finiteTargetList = list;
    test = &p;
    snr_dB_ = p.snr_dB;
    loadFromDirectory(targetList, p);
    concluder_->initialize(p);
    complete_ = concluder->complete(targetList);
}

auto FixedLevelMethodImpl::complete() -> bool {
    return usingFiniteTargetList_ ? finiteTargetList->empty() : complete_;
}

auto FixedLevelMethodImpl::nextTarget() -> std::string {
    return targetList->next();
}

auto FixedLevelMethodImpl::snr_dB() -> int { return snr_dB_; }

void FixedLevelMethodImpl::submit(
    const coordinate_response_measure::Response &response) {
    auto current_ = currentTarget();
    lastTrial.subjectColor = response.color;
    lastTrial.subjectNumber = response.number;
    lastTrial.correctColor = evaluator->correctColor(current_);
    lastTrial.correctNumber = evaluator->correctNumber(current_);
    lastTrial.correct = evaluator->correct(current_, response);
    lastTrial.target = current_;
    concluder->submitResponse();
    complete_ = concluder->complete(targetList);
}

auto FixedLevelMethodImpl::currentTarget() -> std::string {
    return targetList->current();
}

void FixedLevelMethodImpl::writeTestingParameters(OutputFile *file) {
    file->writeTest(*test);
}

void FixedLevelMethodImpl::writeLastCoordinateResponse(OutputFile *file) {
    file->write(lastTrial);
}

void FixedLevelMethodImpl::submitIncorrectResponse() {}

void FixedLevelMethodImpl::submitCorrectResponse() {}

void FixedLevelMethodImpl::submit(const open_set::FreeResponse &response) {
    concluder->submitResponse();
    if (response.flagged)
        targetList->reinsertCurrent();
    complete_ = concluder->complete(targetList);
}
}
