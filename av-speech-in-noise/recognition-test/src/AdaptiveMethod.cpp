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

void AdaptiveMethodImpl::initialize(const AdaptiveTest &test_) {
    test = &test_;
    trackSettings.rule = trackSettingsReader->read(test_.trackSettingsFile);
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
    auto remainingLists{gsl::narrow<int>(targetListsWithTracks.size())};
    auto index{randomizer->randomIntBetween(0, remainingLists - 1)};
    auto targetListsWithTrack{targetListsWithTracks.at(index)};
    currentSnrTrack = track(targetListsWithTrack);
    currentTargetList = targetListsWithTrack.list;
}

void AdaptiveMethodImpl::removeCompleteTracks() {
    auto end{targetListsWithTracks.end()};
    targetListsWithTracks.erase(
        std::remove_if(targetListsWithTracks.begin(), end,
            [&](const TargetListWithTrack &t) { return complete(t); }),
        end);
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

void AdaptiveMethodImpl::submitResponse(
    const coordinate_response_measure::Response &response) {
    auto lastSnr_dB_{snr_dB()};
    auto current_{currentTarget()};
    auto correct_{correct(current_, response)};
    if (correct_)
        correct();
    else
        incorrect();
    auto updatedReversals{currentSnrTrack->reversals()};
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
    lastOpenSetTrial.target = evaluator->fileName(currentTarget());
    incorrect();
    assignReversals(lastOpenSetTrial, currentSnrTrack);
    selectNextList();
}

void AdaptiveMethodImpl::submitCorrectResponse() {
    assignCorrectness(lastOpenSetTrial, true);
    assignSnr(lastOpenSetTrial, currentSnrTrack);
    lastOpenSetTrial.target = evaluator->fileName(currentTarget());
    correct();
    assignReversals(lastOpenSetTrial, currentSnrTrack);
    selectNextList();
}

void AdaptiveMethodImpl::submitResponse(const open_set::FreeResponse &) {
    selectNextList();
}
}
