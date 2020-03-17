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

static void assignTarget(open_set::Trial &trial, std::string s) {
    trial.target = std::move(s);
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

static void correct(Track *track) { track->down(); }

static void incorrect(Track *track) { track->up(); }

static auto correct(ResponseEvaluator &evaluator, const std::string &target,
    const coordinate_response_measure::Response &response) -> bool {
    return evaluator.correct(target, response);
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
    for (const auto &t : targetListsWithTracks)
        t.track->reset();
    selectNextList();
}

void AdaptiveMethodImpl::selectNextList() {
    const auto tracksInProgress{moveCompleteTracksToEnd(targetListsWithTracks)};
    if (tracksInProgress == 0)
        return;
    const auto &targetListsWithTrack{targetListsWithTracks.at(
        randomizer.betweenInclusive(0, tracksInProgress - 1))};
    currentSnrTrack = track(targetListsWithTrack);
    currentTargetList = targetListsWithTrack.list.get();
}

auto AdaptiveMethodImpl::complete() -> bool {
    return std::all_of(targetListsWithTracks.begin(),
        targetListsWithTracks.end(), av_speech_in_noise::complete);
}

auto AdaptiveMethodImpl::nextTarget() -> std::string {
    return currentTargetList->next();
}

auto AdaptiveMethodImpl::snr_dB() -> int { return currentSnrTrack->x(); }

auto AdaptiveMethodImpl::currentTarget() -> std::string {
    return currentTargetList->current();
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

void AdaptiveMethodImpl::submit(
    const coordinate_response_measure::Response &response) {
    const auto lastSnr_dB{snr_dB()};
    if (correct(evaluator, currentTarget(), response))
        correct(currentSnrTrack);
    else
        incorrect(currentSnrTrack);
    lastTrial.subjectColor = response.color;
    lastTrial.subjectNumber = response.number;
    assignReversals(lastTrial, currentSnrTrack);
    lastTrial.correctColor = evaluator.correctColor(currentTarget());
    lastTrial.correctNumber = evaluator.correctNumber(currentTarget());
    lastTrial.SNR_dB = lastSnr_dB;
    lastTrial.correct = correct(evaluator, currentTarget(), response);
    selectNextList();
}

void AdaptiveMethodImpl::submitIncorrectResponse() {
    assignCorrectness(lastOpenSetTrial, false);
    assignSnr(lastOpenSetTrial, currentSnrTrack);
    assignTarget(lastOpenSetTrial, fileName(evaluator, currentTarget()));
    incorrect(currentSnrTrack);
    assignReversals(lastOpenSetTrial, currentSnrTrack);
    selectNextList();
}

void AdaptiveMethodImpl::submitCorrectResponse() {
    assignCorrectness(lastOpenSetTrial, true);
    assignSnr(lastOpenSetTrial, currentSnrTrack);
    assignTarget(lastOpenSetTrial, fileName(evaluator, currentTarget()));
    correct(currentSnrTrack);
    assignReversals(lastOpenSetTrial, currentSnrTrack);
    lastAdaptiveTestResult.threshold = currentSnrTrack->threshold({});
    lastAdaptiveTestResult.targetListDirectory = currentTargetList->directory();
    selectNextList();
}

void AdaptiveMethodImpl::submit(const open_set::CorrectKeywords &p) {
    lastCorrectKeywordsTrial.count = p.count;
    assignCorrectness(lastCorrectKeywordsTrial, correct(p));
    assignSnr(lastCorrectKeywordsTrial, currentSnrTrack);
    assignTarget(
        lastCorrectKeywordsTrial, fileName(evaluator, currentTarget()));
    if (correct(p))
        correct(currentSnrTrack);
    else
        incorrect(currentSnrTrack);
    assignReversals(lastCorrectKeywordsTrial, currentSnrTrack);
    selectNextList();
}

void AdaptiveMethodImpl::submit(const open_set::FreeResponse &) {
    selectNextList();
}
}
