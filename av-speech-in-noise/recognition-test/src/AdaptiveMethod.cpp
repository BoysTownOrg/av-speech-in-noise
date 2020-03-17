#include "AdaptiveMethod.hpp"
#include <gsl/gsl>

namespace av_speech_in_noise {
static auto track(const TargetListWithTrack &t) -> Track * {
    return t.track.get();
}

static auto complete(const TargetListWithTrack &t) -> bool {
    return track(t)->complete();
}

static auto incomplete(const TargetListWithTrack &t) -> bool {
    return !complete(t);
}

static void assignReversals(Adaptive &trial, Track *track) {
    trial.reversals = track->reversals();
}

static auto correct(const open_set::CorrectKeywords &p) -> bool {
    return p.count >= 2;
}

static void assignSnr(open_set::AdaptiveTrial &trial, Track *track) {
    trial.SNR_dB = track->x();
}

static void assignCorrectness(open_set::AdaptiveTrial &trial, bool c) {
    trial.correct = c;
}

static auto fileName(ResponseEvaluator &evaluator, const std::string &target)
    -> std::string {
    return evaluator.fileName(target);
}

static auto trackSettings(const AdaptiveTest &test) -> Track::Settings {
    Track::Settings trackSettings{};
    trackSettings.rule = &test.trackingRule;
    trackSettings.ceiling = test.ceilingSnr_dB;
    trackSettings.startingX = test.startingSnr_dB;
    trackSettings.floor = test.floorSnr_dB;
    trackSettings.bumpLimit = test.trackBumpLimit;
    return trackSettings;
}

static auto moveCompleteTracksToEnd(
    std::vector<TargetListWithTrack> &targetListsWithTracks) -> gsl::index {
    return std::distance(targetListsWithTracks.begin(),
        std::stable_partition(targetListsWithTracks.begin(),
            targetListsWithTracks.end(), incomplete));
}

static void down(Track *track) { track->down(); }

static void up(Track *track) { track->up(); }

static auto current(TargetList *list) -> std::string { return list->current(); }

static void assignTarget(open_set::Trial &trial, ResponseEvaluator &evaluator,
    TargetList *targetList) {
    trial.target = fileName(evaluator, current(targetList));
}

static auto correct(ResponseEvaluator &evaluator, TargetList *targetList,
    const coordinate_response_measure::Response &response) -> bool {
    return evaluator.correct(current(targetList), response);
}

static void resetTrack(TargetListWithTrack &targetListWithTrack) {
    track(targetListWithTrack)->reset();
}

AdaptiveMethodImpl::AdaptiveMethodImpl(Track::Factory &snrTrackFactory,
    ResponseEvaluator &evaluator, Randomizer &randomizer)
    : snrTrackFactory{snrTrackFactory}, evaluator{evaluator}, randomizer{
                                                                  randomizer} {}

void AdaptiveMethodImpl::initialize(
    const AdaptiveTest &t, TargetListReader *targetListSetReader) {
    test = &t;
    targetListsWithTracks.clear();
    for (auto &&list : targetListSetReader->read(t.targetListDirectory))
        targetListsWithTracks.push_back(
            {list, snrTrackFactory.make(trackSettings(t))});
    selectNextList();
}

void AdaptiveMethodImpl::resetTracks() {
    std::for_each(
        targetListsWithTracks.begin(), targetListsWithTracks.end(), resetTrack);
    selectNextList();
}

void AdaptiveMethodImpl::selectNextList() {
    const auto tracksInProgress{moveCompleteTracksToEnd(targetListsWithTracks)};
    if (tracksInProgress == 0)
        return;
    const auto &targetListsWithTrack{targetListsWithTracks.at(
        randomizer.betweenInclusive(0, tracksInProgress - 1))};
    snrTrack = track(targetListsWithTrack);
    targetList = targetListsWithTrack.list.get();
}

auto AdaptiveMethodImpl::complete() -> bool {
    return std::all_of(targetListsWithTracks.begin(),
        targetListsWithTracks.end(), av_speech_in_noise::complete);
}

auto AdaptiveMethodImpl::nextTarget() -> std::string {
    return targetList->next();
}

auto AdaptiveMethodImpl::snr_dB() -> int { return snrTrack->x(); }

auto AdaptiveMethodImpl::currentTarget() -> std::string {
    return targetList->current();
}

void AdaptiveMethodImpl::submit(
    const coordinate_response_measure::Response &response) {
    const auto lastSnr_dB{snr_dB()};
    if (correct(evaluator, targetList, response))
        down(snrTrack);
    else
        up(snrTrack);
    lastTrial.subjectColor = response.color;
    lastTrial.subjectNumber = response.number;
    assignReversals(lastTrial, snrTrack);
    lastTrial.correctColor = evaluator.correctColor(current(targetList));
    lastTrial.correctNumber = evaluator.correctNumber(current(targetList));
    lastTrial.SNR_dB = lastSnr_dB;
    lastTrial.correct = correct(evaluator, targetList, response);
    selectNextList();
}

void AdaptiveMethodImpl::submitIncorrectResponse() {
    assignCorrectness(lastOpenSetTrial, false);
    assignSnr(lastOpenSetTrial, snrTrack);
    assignTarget(lastOpenSetTrial, evaluator, targetList);
    up(snrTrack);
    assignReversals(lastOpenSetTrial, snrTrack);
    selectNextList();
}

void AdaptiveMethodImpl::submitCorrectResponse() {
    assignCorrectness(lastOpenSetTrial, true);
    assignSnr(lastOpenSetTrial, snrTrack);
    assignTarget(lastOpenSetTrial, evaluator, targetList);
    down(snrTrack);
    assignReversals(lastOpenSetTrial, snrTrack);
    lastAdaptiveTestResult.threshold = snrTrack->threshold({});
    lastAdaptiveTestResult.targetListDirectory = targetList->directory();
    selectNextList();
}

void AdaptiveMethodImpl::submit(const open_set::CorrectKeywords &p) {
    lastCorrectKeywordsTrial.count = p.count;
    assignCorrectness(lastCorrectKeywordsTrial, correct(p));
    assignSnr(lastCorrectKeywordsTrial, snrTrack);
    assignTarget(lastCorrectKeywordsTrial, evaluator, targetList);
    if (correct(p))
        down(snrTrack);
    else
        up(snrTrack);
    assignReversals(lastCorrectKeywordsTrial, snrTrack);
    selectNextList();
}

void AdaptiveMethodImpl::submit(const open_set::FreeResponse &) {
    selectNextList();
}

void AdaptiveMethodImpl::writeTestResult(OutputFile *file) {
    file->write(lastAdaptiveTestResult);
}

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
}
