#include "AdaptiveMethod.hpp"
#include <gsl/gsl>

namespace av_speech_in_noise {
static auto track(const TargetPlaylistWithTrack &t) -> Track * {
    return t.track.get();
}

static auto complete(const TargetPlaylistWithTrack &t) -> bool {
    return track(t)->complete();
}

static auto incomplete(const TargetPlaylistWithTrack &t) -> bool {
    return !complete(t);
}

static void assignReversals(AdaptiveProgress &trial, Track *track) {
    trial.reversals = track->reversals();
}

static auto correct(const CorrectKeywords &p) -> bool { return p.count >= 2; }

static void assignSnr(open_set::AdaptiveTrial &trial, Track *track) {
    trial.snr.dB = track->x();
}

static void assignCorrectness(open_set::AdaptiveTrial &trial, bool c) {
    trial.correct = c;
}

static auto fileName(ResponseEvaluator &evaluator, const LocalUrl &target)
    -> std::string {
    return evaluator.fileName(target);
}

static auto trackSettings(const AdaptiveTest &test) -> Track::Settings {
    Track::Settings trackSettings{};
    trackSettings.rule = &test.trackingRule;
    trackSettings.ceiling = test.ceilingSnr.dB;
    trackSettings.startingX = test.startingSnr.dB;
    trackSettings.floor = test.floorSnr.dB;
    trackSettings.bumpLimit = test.trackBumpLimit;
    return trackSettings;
}

static auto moveCompleteTracksToEnd(
    std::vector<TargetPlaylistWithTrack> &targetListsWithTracks) -> gsl::index {
    return std::distance(targetListsWithTracks.begin(),
        std::stable_partition(targetListsWithTracks.begin(),
            targetListsWithTracks.end(), incomplete));
}

static void down(Track *track) { track->down(); }

static void up(Track *track) { track->up(); }

static auto current(TargetPlaylist *list) -> LocalUrl { return list->current(); }

static void assignTarget(open_set::Trial &trial, ResponseEvaluator &evaluator,
    TargetPlaylist *targetList) {
    trial.target = fileName(evaluator, current(targetList));
}

static auto correct(ResponseEvaluator &evaluator, TargetPlaylist *targetList,
    const coordinate_response_measure::Response &response) -> bool {
    return evaluator.correct(current(targetList), response);
}

static void resetTrack(TargetPlaylistWithTrack &targetListWithTrack) {
    track(targetListWithTrack)->reset();
}

static auto x(Track *track) -> int { return track->x(); }

static auto testResults(
    const std::vector<TargetPlaylistWithTrack> &targetListsWithTracks,
    int thresholdReversals) -> AdaptiveTestResults {
    AdaptiveTestResults results;
    for (const auto &t : targetListsWithTracks)
        results.push_back(
            {t.list->directory(), t.track->threshold(thresholdReversals)});
    return results;
}

AdaptiveMethodImpl::AdaptiveMethodImpl(Track::Factory &snrTrackFactory,
    ResponseEvaluator &evaluator, Randomizer &randomizer)
    : snrTrackFactory{snrTrackFactory}, evaluator{evaluator}, randomizer{
                                                                  randomizer} {}

void AdaptiveMethodImpl::initialize(
    const AdaptiveTest &t, TargetPlaylistReader *targetListSetReader) {
    test = &t;
    thresholdReversals = t.thresholdReversals;
    targetListsWithTracks.clear();
    for (const auto &list : targetListSetReader->read(t.targetsUrl))
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

    const auto &next{targetListsWithTracks.at(
        randomizer.betweenInclusive(0, tracksInProgress - 1))};
    snrTrack = track(next);
    targetList = next.list.get();
}

auto AdaptiveMethodImpl::complete() -> bool {
    return std::all_of(targetListsWithTracks.begin(),
        targetListsWithTracks.end(), av_speech_in_noise::complete);
}

auto AdaptiveMethodImpl::nextTarget() -> LocalUrl { return targetList->next(); }

auto AdaptiveMethodImpl::snr() -> SNR {
    SNR snr;
    snr.dB = x(snrTrack);
    return snr;
}

auto AdaptiveMethodImpl::currentTarget() -> LocalUrl {
    return targetList->current();
}

void AdaptiveMethodImpl::submit(
    const coordinate_response_measure::Response &response) {
    const auto lastX{x(snrTrack)};
    if (correct(evaluator, targetList, response))
        down(snrTrack);
    else
        up(snrTrack);
    lastCoordinateResponseMeasureTrial.subjectColor = response.color;
    lastCoordinateResponseMeasureTrial.subjectNumber = response.number;
    assignReversals(lastCoordinateResponseMeasureTrial, snrTrack);
    lastCoordinateResponseMeasureTrial.correctColor =
        evaluator.correctColor(current(targetList));
    lastCoordinateResponseMeasureTrial.correctNumber =
        evaluator.correctNumber(current(targetList));
    lastCoordinateResponseMeasureTrial.snr.dB = lastX;
    lastCoordinateResponseMeasureTrial.correct =
        correct(evaluator, targetList, response);
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
    selectNextList();
}

void AdaptiveMethodImpl::submit(const CorrectKeywords &p) {
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

void AdaptiveMethodImpl::submit(const FreeResponse &) { selectNextList(); }

void AdaptiveMethodImpl::writeTestResult(OutputFile &file) {
    file.write(av_speech_in_noise::testResults(
        targetListsWithTracks, thresholdReversals));
}

void AdaptiveMethodImpl::writeTestingParameters(OutputFile &file) {
    file.write(*test);
}

void AdaptiveMethodImpl::writeLastCoordinateResponse(OutputFile &file) {
    file.write(lastCoordinateResponseMeasureTrial);
}

void AdaptiveMethodImpl::writeLastCorrectResponse(OutputFile &file) {
    file.write(lastOpenSetTrial);
}

void AdaptiveMethodImpl::writeLastIncorrectResponse(OutputFile &file) {
    file.write(lastOpenSetTrial);
}

void AdaptiveMethodImpl::writeLastCorrectKeywords(OutputFile &file) {
    file.write(lastCorrectKeywordsTrial);
}

auto AdaptiveMethodImpl::testResults() -> AdaptiveTestResults {
    return av_speech_in_noise::testResults(
        targetListsWithTracks, thresholdReversals);
}
}
