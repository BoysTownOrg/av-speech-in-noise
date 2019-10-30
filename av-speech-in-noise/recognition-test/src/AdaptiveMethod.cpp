#include "AdaptiveMethod.hpp"
#include <gsl/gsl>

namespace av_speech_in_noise {
AdaptiveMethodImpl::AdaptiveMethodImpl(TargetListReader *targetListSetReader,
    TrackSettingsReader *trackSettingsReader, Track::Factory *snrTrackFactory,
    ResponseEvaluator *evaluator, Randomizer *randomizer)
    : targetListSetReader{targetListSetReader},
      trackSettingsReader{trackSettingsReader},
      snrTrackFactory{snrTrackFactory}, evaluator{evaluator}, randomizer{
                                                                  randomizer} {}

void AdaptiveMethodImpl::initialize(const AdaptiveTest &p) {
    test = &p;
    trackSettings.rule = trackSettingsReader->read(p.trackSettingsFile);
    trackSettings.ceiling = p.ceilingSnr_dB;
    trackSettings.startingX = p.startingSnr_dB;
    trackSettings.floor = p.floorSnr_dB;
    trackSettings.bumpLimit = p.trackBumpLimit;
    lists = targetListSetReader->read(p.targetListDirectory);

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

void AdaptiveMethodImpl::selectNextList() {
    removeCompleteTracks();
    auto remainingLists = gsl::narrow<int>(targetListsWithTracks.size());
    if (remainingLists == 0)
        return;
    auto targetListsWithTrack_ = targetListsWithTracks.at(
        randomizer->randomIntBetween(0, remainingLists - 1));
    currentSnrTrack = targetListsWithTrack_.track.get();
    currentTargetList = targetListsWithTrack_.list;
}

void AdaptiveMethodImpl::removeCompleteTracks() {
    auto end = targetListsWithTracks.end();
    targetListsWithTracks.erase(
        std::remove_if(targetListsWithTracks.begin(), end,
            [&](const TargetListWithTrack &t) { return complete(t); }),
        end);
}

auto AdaptiveMethodImpl::complete(const TargetListWithTrack &t) -> bool {
    return t.track->complete();
}

auto AdaptiveMethodImpl::complete() -> bool {
    return std::all_of(targetListsWithTracks.begin(),
        targetListsWithTracks.end(),
        [&](const TargetListWithTrack &t) { return complete(t); });
}

auto AdaptiveMethodImpl::next() -> std::string {
    return currentTargetList->next();
}

void AdaptiveMethodImpl::submitResponse(
    const coordinate_response_measure::Response &response) {
    auto lastSnr_dB_ = snr_dB();
    auto current_ = current();
    auto correct_ = correct(current_, response);
    if (correct_)
        correct();
    else
        incorrect();
    auto updatedReversals = currentSnrTrack->reversals();
    lastTrial.subjectColor = response.color;
    lastTrial.subjectNumber = response.number;
    lastTrial.reversals = updatedReversals;
    lastTrial.correctColor = evaluator->correctColor(current_);
    lastTrial.correctNumber = evaluator->correctNumber(current_);
    lastTrial.SNR_dB = lastSnr_dB_;
    lastTrial.correct = correct_;
    selectNextList();
}

auto AdaptiveMethodImpl::snr_dB() -> int { return currentSnrTrack->x(); }

auto AdaptiveMethodImpl::current() -> std::string {
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
    file->writeTrial(lastTrial);
}

void AdaptiveMethodImpl::writeLastCorrectResponse(OutputFile *file) {
    file->writeTrial(lastOpenSetTrial);
}

void AdaptiveMethodImpl::writeLastIncorrectResponse(OutputFile *file) {
    file->writeTrial(lastOpenSetTrial);
}

static void assignReversals(open_set::AdaptiveTrial &trial, Track *track) {
    trial.reversals = track->reversals();
}

static void assignSnr(open_set::AdaptiveTrial &trial, Track *track) {
    trial.SNR_dB = track->x();
}

static void assignCorrectness(open_set::AdaptiveTrial &trial, bool c) {
    trial.correct = c;
}

void AdaptiveMethodImpl::submitIncorrectResponse() {
    assignCorrectness(lastOpenSetTrial, false);
    assignSnr(lastOpenSetTrial, currentSnrTrack);
    lastOpenSetTrial.target = evaluator->fileName(current());
    incorrect();
    assignReversals(lastOpenSetTrial, currentSnrTrack);
    selectNextList();
}

void AdaptiveMethodImpl::submitCorrectResponse() {
    assignCorrectness(lastOpenSetTrial, true);
    assignSnr(lastOpenSetTrial, currentSnrTrack);
    lastOpenSetTrial.target = evaluator->fileName(current());
    correct();
    assignReversals(lastOpenSetTrial, currentSnrTrack);
    selectNextList();
}

void AdaptiveMethodImpl::submitResponse(const open_set::FreeResponse &) {
    selectNextList();
}
}
