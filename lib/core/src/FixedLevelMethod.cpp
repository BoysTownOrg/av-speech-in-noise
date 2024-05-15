#include "FixedLevelMethod.hpp"

#include <sstream>
#include <stdexcept>

namespace av_speech_in_noise {
FixedLevelMethodImpl::FixedLevelMethodImpl(ResponseEvaluator &evaluator)
    : evaluator{evaluator} {}

static void load(TargetPlaylist *list, const FixedLevelTest &test) {
    list->load(test.targetsUrl);
}

static void initialize(TargetPlaylist *&targetList,
    const FixedLevelTest *&test_, SNR &snr_, const FixedLevelTest &test,
    TargetPlaylist *list) {
    targetList = list;
    test_ = &test;
    snr_ = test.snr;
    try {
        load(targetList, test);
    } catch (const FiniteTargetPlaylist::LoadFailure &) {
        std::stringstream stream;
        stream << "Unable to load targets from " << test.targetsUrl.path;
        throw std::runtime_error{stream.str()};
    }
}

static void initialize(bool &usingFiniteTargetPlaylist_,
    FiniteTargetPlaylist *&finiteTargetPlaylist, bool &finiteTargetsExhausted_,
    FiniteTargetPlaylist *list) {
    usingFiniteTargetPlaylist_ = true;
    finiteTargetPlaylist = list;
    finiteTargetsExhausted_ = finiteTargetPlaylist->empty();
}

void FixedLevelMethodImpl::initialize(
    const FixedLevelFixedTrialsTest &test, TargetPlaylist *list) {
    usingFiniteTargetPlaylist_ = false;
    av_speech_in_noise::initialize(targetList, test_, snr_, test, list);
    trials_ = test.trials;
}

void FixedLevelMethodImpl::initialize(
    const FixedLevelTest &test, FiniteTargetPlaylistWithRepeatables *list) {
    av_speech_in_noise::initialize(targetList, test_, snr_, test, list);
    av_speech_in_noise::initialize(usingFiniteTargetPlaylist_,
        finiteTargetPlaylist, finiteTargetsExhausted_, list);
    finiteTargetPlaylistWithRepeatables = list;
    totalKeywordsSubmitted_ = 0;
    totalKeywordsCorrect_ = 0;
}

void FixedLevelMethodImpl::initialize(
    const FixedLevelTest &test, FiniteTargetPlaylist *list) {
    av_speech_in_noise::initialize(targetList, test_, snr_, test, list);
    av_speech_in_noise::initialize(usingFiniteTargetPlaylist_,
        finiteTargetPlaylist, finiteTargetsExhausted_, list);
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

void FixedLevelMethodImpl::submit(const Flaggable &response) {
    if (usingFiniteTargetPlaylist_)
        update(finiteTargetPlaylistWithRepeatables, finiteTargetPlaylist,
            response.flagged, finiteTargetsExhausted_);
    else
        --trials_;
}

void FixedLevelMethodImpl::submit(const ThreeKeywordsResponse &response) {
    update(finiteTargetPlaylistWithRepeatables, finiteTargetPlaylist,
        response.flagged, finiteTargetsExhausted_);
    totalKeywordsSubmitted_ += 3;
    if (response.firstCorrect)
        ++totalKeywordsCorrect_;
    if (response.secondCorrect)
        ++totalKeywordsCorrect_;
    if (response.thirdCorrect)
        ++totalKeywordsCorrect_;
}

auto FixedLevelMethodImpl::keywordsTestResults() -> KeywordsTestResults {
    return KeywordsTestResults{totalKeywordsSubmitted_ == 0
            ? 0
            : totalKeywordsCorrect_ * 100. / totalKeywordsSubmitted_,
        totalKeywordsCorrect_};
}
}
