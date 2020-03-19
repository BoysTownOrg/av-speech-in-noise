#include "FixedLevelMethod.hpp"

namespace av_speech_in_noise {
FixedLevelMethodImpl::FixedLevelMethodImpl(ResponseEvaluator *evaluator)
    : evaluator{evaluator} {}

static void loadFromDirectory(TargetList *list, const FixedLevelTest &test) {
    list->loadFromDirectory(test.targetListDirectory);
}

void FixedLevelMethodImpl::initialize(
    const FixedLevelTest &p, TargetList *list) {
    usingFiniteTargetList_ = false;
    targetList = list;
    test = &p;
    trials_ = p.trials;
    snr_dB_ = p.snr_dB;
    loadFromDirectory(targetList, p);
}

void FixedLevelMethodImpl::initialize(
    const FixedLevelTest &p, FiniteTargetList *list) {
    usingFiniteTargetList_ = true;
    targetList = list;
    finiteTargetList = list;
    test = &p;
    snr_dB_ = p.snr_dB;
    loadFromDirectory(targetList, p);
    finiteTargetsExhausted_ = finiteTargetList->empty();
}

auto FixedLevelMethodImpl::complete() -> bool {
    return usingFiniteTargetList_ ? finiteTargetsExhausted_ : trials_ == 0;
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
    if (usingFiniteTargetList_)
        finiteTargetsExhausted_ = finiteTargetList->empty();
    else
        --trials_;
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

void FixedLevelMethodImpl::submit(const FreeResponse &response) {
    if (usingFiniteTargetList_) {
        if (response.flagged)
            finiteTargetList->reinsertCurrent();
        finiteTargetsExhausted_ = finiteTargetList->empty();
    } else
        --trials_;
}
}
