#include "AdaptiveMethod.hpp"
#include "Model.hpp"
#include "av-speech-in-noise/Model.hpp"
#include <gsl/gsl>

namespace av_speech_in_noise {
AdaptiveMethodImpl::AdaptiveMethodImpl(Track::Factory *snrTrackFactory,
    ResponseEvaluator *evaluator, Randomizer *randomizer)
    : snrTrackFactory{snrTrackFactory}, evaluator{evaluator}, randomizer{
                                                                  randomizer} {}

void AdaptiveMethodImpl::resetTracks() {
    for (const auto &t : targetListsWithTracks)
        t.track->reset();
}

void AdaptiveMethodImpl::initialize(
    const AdaptiveTest &test_, TargetListReader *targetListSetReader) {
    test = &test_;
    trackSettings.rule = &test_.trackingRule;
    trackSettings.ceiling = test_.ceilingSnr_dB;
    trackSettings.startingX = test_.startingSnr_dB;
    trackSettings.floor = test_.floorSnr_dB;
    trackSettings.bumpLimit = test_.trackBumpLimit;
    lists = targetListSetReader->read(test_.targetListDirectory);

    selectNextListAfter(&AdaptiveMethodImpl::makeSnrTracks);
}

void AdaptiveMethodImpl::selectNextListAfter(void (AdaptiveMethodImpl::*f)()) {
    (this->*f)();
    selectNextList();
}

void AdaptiveMethodImpl::makeSnrTracks() {
    targetListsWithTracks.clear();
    for (const auto &list : lists)
        makeTrackWithList(list.get());
}

void AdaptiveMethodImpl::makeTrackWithList(TargetList *list) {
    targetListsWithTracks.push_back(
        {list, snrTrackFactory->make(trackSettings)});
}

auto AdaptiveMethodImpl::track(const TargetListWithTrack &t) -> Track * {
    return t.track.get();
}

void AdaptiveMethodImpl::selectNextList() {
    removeCompleteTracks();
    if (targetListsWithTracks.empty())
        return;
    auto index{randomizer->betweenInclusive(0, tracksInProgress - 1)};
    auto targetListsWithTrack{targetListsWithTracks.at(index)};
    currentSnrTrack = track(targetListsWithTrack);
    currentTargetList = targetListsWithTrack.list;
}

void AdaptiveMethodImpl::removeCompleteTracks() {
    tracksInProgress = std::distance(targetListsWithTracks.begin(),
        std::stable_partition(targetListsWithTracks.begin(),
            targetListsWithTracks.end(),
            [&](const TargetListWithTrack &t) { return !complete(t); }));
}

auto AdaptiveMethodImpl::complete(const TargetListWithTrack &t) -> bool {
    return track(t)->complete();
}

auto AdaptiveMethodImpl::complete() -> bool {
    return std::all_of(targetListsWithTracks.begin(),
        targetListsWithTracks.end(),
        [&](const TargetListWithTrack &t) { return complete(t); });
}

auto AdaptiveMethodImpl::nextTarget() -> std::string {
    return currentTargetList->next();
}

static void assignReversals(Adaptive &trial, Track *track) {
    trial.reversals = track->reversals();
}

void AdaptiveMethodImpl::submit(
    const coordinate_response_measure::Response &response) {
    auto lastSnr_dB_{snr_dB()};
    auto current_{currentTarget()};
    auto correct_{correct(current_, response)};
    if (correct_)
        correct();
    else
        incorrect();
    lastTrial.subjectColor = response.color;
    lastTrial.subjectNumber = response.number;
    assignReversals(lastTrial, currentSnrTrack);
    lastTrial.correctColor = evaluator->correctColor(current_);
    lastTrial.correctNumber = evaluator->correctNumber(current_);
    lastTrial.SNR_dB = lastSnr_dB_;
    lastTrial.correct = correct_;
    selectNextList();
}

auto AdaptiveMethodImpl::snr_dB() -> int { return currentSnrTrack->x(); }

auto AdaptiveMethodImpl::currentTarget() -> std::string {
    return currentTargetList->current();
}

auto AdaptiveMethodImpl::correct(const std::string &target,
    const coordinate_response_measure::Response &response) -> bool {
    return evaluator->correct(target, response);
}

void AdaptiveMethodImpl::correct() { currentSnrTrack->down(); }

void AdaptiveMethodImpl::incorrect() { currentSnrTrack->up(); }

void AdaptiveMethodImpl::writeTestingParameters(OutputFile *file) {
    file->writeTest(*test);
}

void AdaptiveMethodImpl::writeLastCoordinateResponse(OutputFile *file) {
    file->write(lastTrial);
}

void AdaptiveMethodImpl::writeLastCorrectResponse(OutputFile *file) {
    file->write(lastOpenSetTrial);
}

void AdaptiveMethodImpl::writeLastIncorrectResponse(OutputFile *file) {
    file->write(lastOpenSetTrial);
}

void AdaptiveMethodImpl::writeLastCorrectKeywords(OutputFile *file) {
    file->write(lastCorrectKeywordsTrial);
}

static void assignSnr(open_set::AdaptiveTrial &trial, Track *track) {
    trial.SNR_dB = track->x();
}

static void assignCorrectness(open_set::AdaptiveTrial &trial, bool c) {
    trial.correct = c;
}

static auto fileName(ResponseEvaluator *evaluator, const std::string &target)
    -> std::string {
    return evaluator->fileName(target);
}

static void assignTarget(open_set::Trial &trial, std::string s) {
    trial.target = std::move(s);
}

void AdaptiveMethodImpl::submitIncorrectResponse() {
    assignCorrectness(lastOpenSetTrial, false);
    assignSnr(lastOpenSetTrial, currentSnrTrack);
    assignTarget(lastOpenSetTrial, fileName(evaluator, currentTarget()));
    incorrect();
    assignReversals(lastOpenSetTrial, currentSnrTrack);
    selectNextList();
}

void AdaptiveMethodImpl::submitCorrectResponse() {
    assignCorrectness(lastOpenSetTrial, true);
    assignSnr(lastOpenSetTrial, currentSnrTrack);
    assignTarget(lastOpenSetTrial, fileName(evaluator, currentTarget()));
    correct();
    assignReversals(lastOpenSetTrial, currentSnrTrack);
    selectNextList();
}

static auto correct(const open_set::CorrectKeywords &p) -> bool {
    return p.count >= 2;
}

void AdaptiveMethodImpl::submit(const open_set::CorrectKeywords &p) {
    lastCorrectKeywordsTrial.count = p.count;
    assignCorrectness(lastCorrectKeywordsTrial, av_speech_in_noise::correct(p));
    assignSnr(lastCorrectKeywordsTrial, currentSnrTrack);
    assignTarget(
        lastCorrectKeywordsTrial, fileName(evaluator, currentTarget()));
    if (av_speech_in_noise::correct(p))
        correct();
    else
        incorrect();
    assignReversals(lastCorrectKeywordsTrial, currentSnrTrack);
    selectNextList();
}

void AdaptiveMethodImpl::submit(const open_set::FreeResponse &) {
    selectNextList();
}
}
