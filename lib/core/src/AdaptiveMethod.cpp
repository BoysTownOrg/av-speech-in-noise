#include "AdaptiveMethod.hpp"
#include "Configuration.hpp"

#include <gsl/gsl>

namespace av_speech_in_noise {
static auto track(const TargetPlaylistWithTrack &t) -> AdaptiveTrack * {
    return t.track.get();
}

static auto complete(const TargetPlaylistWithTrack &t) -> bool {
    return track(t)->complete();
}

static auto incomplete(const TargetPlaylistWithTrack &t) -> bool {
    return !complete(t);
}

static void assignProgress(AdaptiveProgress &trial, AdaptiveTrack *track) {
    trial.phi = track->phi();
    trial.reversals = track->reversals();
}

static auto correct(const CorrectKeywords &p) -> bool { return p.count >= 2; }

static void assignSnr(open_set::AdaptiveTrial &trial, AdaptiveTrack *track) {
    trial.snr.dB = track->x();
}

static void assignCorrectness(open_set::AdaptiveTrial &trial, bool c) {
    trial.correct = c;
}

static auto fileName(ResponseEvaluator &evaluator, const LocalUrl &target)
    -> std::string {
    return evaluator.fileName(target);
}

static auto trackSettings(FloatSNR startingSNR) -> AdaptiveTrack::Settings {
    AdaptiveTrack::Settings trackSettings{};
    trackSettings.startingX = startingSNR.dB;
    trackSettings.ceiling = 20;
    trackSettings.floor = -40;
    trackSettings.bumpLimit = 10;
    return trackSettings;
}

static auto moveCompleteTracksToEnd(
    std::vector<TargetPlaylistWithTrack> &targetListsWithTracks) -> gsl::index {
    return std::distance(targetListsWithTracks.begin(),
        std::stable_partition(targetListsWithTracks.begin(),
            targetListsWithTracks.end(), incomplete));
}

static void down(AdaptiveTrack *track) { track->down(); }

static void up(AdaptiveTrack *track) { track->up(); }

static auto current(TargetPlaylist *list) -> LocalUrl {
    return list->current();
}

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

static auto x(AdaptiveTrack *track) -> double { return track->x(); }

static auto testResults(
    const std::vector<TargetPlaylistWithTrack> &targetListsWithTracks)
    -> AdaptiveTestResults {
    AdaptiveTestResults results;
    for (const auto &t : targetListsWithTracks)
        results.push_back({t.list->directory(), t.track->result()});
    return results;
}

AdaptiveMethodImpl::AdaptiveMethodImpl(ConfigurationRegistry &registry,
    ResponseEvaluator &evaluator, Randomizer &randomizer,
    AdaptiveTrack::Factory &adaptiveTrackFactory)
    : evaluator{evaluator}, randomizer{randomizer},
      adaptiveTrackFactory{adaptiveTrackFactory} {
    registry.subscribe(*this, "targets");
    registry.subscribe(*this, "starting SNR (dB)");
}

void AdaptiveMethodImpl::initialize(TargetPlaylistReader *targetListSetReader) {
    targetListsWithTracks.clear();
    for (const auto &list : targetListSetReader->read(targetsUrl))
        targetListsWithTracks.push_back(
            {list, adaptiveTrackFactory.make(trackSettings(startingSNR))});
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

auto AdaptiveMethodImpl::snr() -> FloatSNR {
    FloatSNR snr{};
    snr.dB = x(snrTrack);
    return snr;
}

auto AdaptiveMethodImpl::currentTarget() -> LocalUrl {
    return targetList->current();
}

void AdaptiveMethodImpl::submit(
    const coordinate_response_measure::Response &response) {
    lastCoordinateResponseMeasureTrial.subjectColor = response.color;
    lastCoordinateResponseMeasureTrial.subjectNumber = response.number;
    lastCoordinateResponseMeasureTrial.correctColor =
        evaluator.correctColor(current(targetList));
    lastCoordinateResponseMeasureTrial.correctNumber =
        evaluator.correctNumber(current(targetList));
    lastCoordinateResponseMeasureTrial.snr.dB = x(snrTrack);

    if ((lastCoordinateResponseMeasureTrial.correct =
                correct(evaluator, targetList, response)))
        down(snrTrack);
    else
        up(snrTrack);
    assignProgress(lastCoordinateResponseMeasureTrial, snrTrack);
    selectNextList();
}

void AdaptiveMethodImpl::submitIncorrectResponse() {
    assignCorrectness(lastOpenSetTrial, false);
    assignSnr(lastOpenSetTrial, snrTrack);
    assignTarget(lastOpenSetTrial, evaluator, targetList);

    up(snrTrack);
    assignProgress(lastOpenSetTrial, snrTrack);
    selectNextList();
}

void AdaptiveMethodImpl::submitCorrectResponse() {
    assignCorrectness(lastOpenSetTrial, true);
    assignSnr(lastOpenSetTrial, snrTrack);
    assignTarget(lastOpenSetTrial, evaluator, targetList);

    down(snrTrack);
    assignProgress(lastOpenSetTrial, snrTrack);
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
    assignProgress(lastCorrectKeywordsTrial, snrTrack);
    selectNextList();
}

void AdaptiveMethodImpl::writeTestResult(OutputFile &file) {
    file.write(av_speech_in_noise::testResults(targetListsWithTracks));
}

void AdaptiveMethodImpl::write(std::ostream &stream) {
    insertLabeledLine(stream, "targets", targetsUrl.path);
    insertLabeledLine(stream, "starting SNR (dB)", startingSNR.dB);
    adaptiveTrackFactory.write(stream);
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
    return av_speech_in_noise::testResults(targetListsWithTracks);
}

void AdaptiveMethodImpl::configure(
    const std::string &key, const std::string &value) {
    if (key == "targets")
        targetsUrl.path = value;
    else if (key == "starting SNR (dB)")
        startingSNR.dB = integer(value);
}
}
