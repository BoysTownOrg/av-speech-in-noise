#include "FixedLevelMethod.hpp"

namespace av_speech_in_noise {
FixedLevelMethodImpl::FixedLevelMethodImpl(ResponseEvaluator &evaluator)
    : evaluator{evaluator} {}

static void loadFromDirectory(TargetList *list, const FixedLevelTest &test) {
    list->loadFromDirectory(test.targetsUrl);
}

void FixedLevelMethodImpl::initialize(
    const FixedLevelTest &test, TargetList *list) {
    usingFiniteTargetList_ = false;
    targetList = list;
    test_ = &test;
    trials_ = test.trials;
    snr_ = test.snr;
    loadFromDirectory(targetList, test);
}

void FixedLevelMethodImpl::initialize(
    const FixedLevelTest &p, FiniteTargetList *list) {
    usingFiniteTargetList_ = true;
    targetList = list;
    finiteTargetList = list;
    test_ = &p;
    snr_ = p.snr;
    loadFromDirectory(targetList, p);
    finiteTargetsExhausted_ = finiteTargetList->empty();
}

auto FixedLevelMethodImpl::complete() -> bool {
    return usingFiniteTargetList_ ? finiteTargetsExhausted_ : trials_ == 0;
}

auto FixedLevelMethodImpl::nextTarget() -> LocalUrl {
    return targetList->next();
}

auto FixedLevelMethodImpl::snr() -> SNR { return snr_; }

static auto current(TargetList *list) -> LocalUrl { return list->current(); }

void FixedLevelMethodImpl::submit(
    const coordinate_response_measure::Response &response) {
    lastTrial.subjectColor = response.color;
    lastTrial.subjectNumber = response.number;
    lastTrial.correctColor = evaluator.correctColor(current(targetList));
    lastTrial.correctNumber = evaluator.correctNumber(current(targetList));
    lastTrial.correct = evaluator.correct(current(targetList), response);
    lastTrial.target = current(targetList).path;
    if (usingFiniteTargetList_)
        finiteTargetsExhausted_ = finiteTargetList->empty();
    else
        --trials_;
}

auto FixedLevelMethodImpl::currentTarget() -> LocalUrl {
    return current(targetList);
}

void FixedLevelMethodImpl::writeTestingParameters(OutputFile &file) {
    file.write(*test_);
}

void FixedLevelMethodImpl::writeLastCoordinateResponse(OutputFile &file) {
    file.write(lastTrial);
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
