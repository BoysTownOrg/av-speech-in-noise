#include "FixedLevelMethod.hpp"

namespace av_speech_in_noise {
FixedLevelMethodImpl::FixedLevelMethodImpl(ResponseEvaluator &evaluator)
    : evaluator{evaluator} {}

static void loadFromDirectory(
    TargetPlaylist *list, const FixedLevelTest &test) {
    list->loadFromDirectory(test.targetsUrl);
}

void FixedLevelMethodImpl::initialize(
    const FixedLevelFixedTrialsTest &test, TargetPlaylist *list) {
    usingFiniteTargetPlaylist_ = false;
    targetList = list;
    test_ = &test;
    trials_ = test.trials;
    snr_ = test.snr;
    loadFromDirectory(targetList, test);
}

void FixedLevelMethodImpl::initialize(
    const FixedLevelTest &p, FiniteTargetPlaylistWithRepeatables *list) {
    usingFiniteTargetPlaylist_ = true;
    targetList = list;
    finiteTargetPlaylist = list;
    finiteTargetPlaylistWithRepeatables = list;
    test_ = &p;
    snr_ = p.snr;
    loadFromDirectory(targetList, p);
    finiteTargetsExhausted_ = finiteTargetPlaylist->empty();
}

void FixedLevelMethodImpl::initialize(
    const FixedLevelTest &p, FiniteTargetPlaylist *list) {
    usingFiniteTargetPlaylist_ = true;
    targetList = list;
    finiteTargetPlaylist = list;
    test_ = &p;
    snr_ = p.snr;
    loadFromDirectory(targetList, p);
    finiteTargetsExhausted_ = finiteTargetPlaylist->empty();
}

auto FixedLevelMethodImpl::complete() -> bool {
    return usingFiniteTargetPlaylist_ ? finiteTargetsExhausted_ : trials_ == 0;
}

auto FixedLevelMethodImpl::nextTarget() -> LocalUrl {
    return targetList->next();
}

auto FixedLevelMethodImpl::snr() -> SNR { return snr_; }

static auto current(TargetPlaylist *list) -> LocalUrl {
    return list->current();
}

void FixedLevelMethodImpl::submit(const ConsonantResponse &response) {
    lastConsonantTrial.subjectConsonant = response.consonant;
    lastConsonantTrial.correctConsonant =
        evaluator.correctConsonant(current(targetList));
    lastConsonantTrial.target = current(targetList).path;
    lastConsonantTrial.correct =
        evaluator.correct(current(targetList), response);
    finiteTargetsExhausted_ = finiteTargetPlaylist->empty();
}

void FixedLevelMethodImpl::submit(
    const coordinate_response_measure::Response &response) {
    lastCoordinateResponseMeasureTrial.subjectColor = response.color;
    lastCoordinateResponseMeasureTrial.subjectNumber = response.number;
    lastCoordinateResponseMeasureTrial.correctColor =
        evaluator.correctColor(current(targetList));
    lastCoordinateResponseMeasureTrial.correctNumber =
        evaluator.correctNumber(current(targetList));
    lastCoordinateResponseMeasureTrial.correct =
        evaluator.correct(current(targetList), response);
    lastCoordinateResponseMeasureTrial.target = current(targetList).path;
    if (usingFiniteTargetPlaylist_)
        finiteTargetsExhausted_ = finiteTargetPlaylist->empty();
    else
        --trials_;
}

auto FixedLevelMethodImpl::currentTarget() -> LocalUrl {
    return current(targetList);
}

void FixedLevelMethodImpl::writeTestingParameters(OutputFile &file) {
    file.write(*test_);
}

void FixedLevelMethodImpl::writeLastConsonant(OutputFile &file) {
    file.write(lastConsonantTrial);
}

void FixedLevelMethodImpl::writeLastCoordinateResponse(OutputFile &file) {
    file.write(lastCoordinateResponseMeasureTrial);
}

static void update(
    FiniteTargetPlaylistWithRepeatables *finiteTargetPlaylistWithRepeatables,
    FiniteTargetPlaylist *finiteTargetPlaylist, bool flagged,
    bool &finiteTargetsExhausted_) {
    if (flagged)
        finiteTargetPlaylistWithRepeatables->reinsertCurrent();
    finiteTargetsExhausted_ = finiteTargetPlaylist->empty();
}

void FixedLevelMethodImpl::submit(const FreeResponse &response) {
    if (usingFiniteTargetPlaylist_)
        update(finiteTargetPlaylistWithRepeatables, finiteTargetPlaylist,
            response.flagged, finiteTargetsExhausted_);
    else
        --trials_;
}

void FixedLevelMethodImpl::submit(const ThreeKeywords &response) {
    update(finiteTargetPlaylistWithRepeatables, finiteTargetPlaylist,
        response.flagged, finiteTargetsExhausted_);
}
}
