#include "AdaptiveMethod.hpp"
#include <gsl/gsl>

namespace av_speech_in_noise {
    AdaptiveMethod::AdaptiveMethod(
        TargetListReader *targetListSetReader,
        ITrackSettingsReader *trackSettingsReader,
        TrackFactory *snrTrackFactory,
        ResponseEvaluator *evaluator,
        Randomizer *randomizer
    ) :
        targetListSetReader{targetListSetReader},
        trackSettingsReader{trackSettingsReader},
        snrTrackFactory{snrTrackFactory},
        evaluator{evaluator},
        randomizer{randomizer} {}
    
    void AdaptiveMethod::initialize(const AdaptiveTest &p) {
        test = &p;
        trackSettings.rule = trackSettingsReader->read(p.trackSettingsFile);
        trackSettings.ceiling = p.ceilingSnr_dB;
        trackSettings.startingX = p.startingSnr_dB;
        trackSettings.floor = p.floorSnr_dB;
        trackSettings.bumpLimit = p.trackBumpLimit;
        lists = targetListSetReader->read(p.common.targetListDirectory);
        
        selectNextListAfter(&AdaptiveMethod::makeSnrTracks);
    }
    
    void AdaptiveMethod::selectNextListAfter(void (AdaptiveMethod::*f)()) {
        (this->*f)();
        selectNextList();
    }

    void AdaptiveMethod::makeSnrTracks() {
        targetListsWithTracks.clear();
        for (auto list : lists)
            makeTrackWithList(list.get());
    }
    
    void AdaptiveMethod::makeTrackWithList(
        TargetList *list
    ) {
        targetListsWithTracks.push_back({
            list,
            snrTrackFactory->make(trackSettings)
        });
    }

    void AdaptiveMethod::selectNextList() {
        removeCompleteTracks();
        auto remainingLists = gsl::narrow<int>(targetListsWithTracks.size());
        if (remainingLists == 0)
            return;
        auto targetListsWithTrack_ = targetListsWithTracks.at(
            randomizer->randomIntBetween(0, remainingLists - 1)
        );
        currentSnrTrack = targetListsWithTrack_.track.get();
        currentTargetList = targetListsWithTrack_.list;
    }
    
    void AdaptiveMethod::removeCompleteTracks() {
        auto end = targetListsWithTracks.end();
        targetListsWithTracks.erase(
            std::remove_if(
                targetListsWithTracks.begin(),
                end,
                [&](const TargetListWithTrack &t) {
                    return complete(t);
                }
            ),
            end
        );
    }
    
    bool AdaptiveMethod::complete(const TargetListWithTrack &t) {
        return t.track->complete();
    }
    
    bool AdaptiveMethod::complete() {
        return std::all_of(
            targetListsWithTracks.begin(),
            targetListsWithTracks.end(),
            [&](const TargetListWithTrack &t) {
                return complete(t);
            }
        );
    }
    
    std::string AdaptiveMethod::next() {
        return currentTargetList->next();
    }
    
    void AdaptiveMethod::submitResponse(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        auto lastSnr_dB_ = snr_dB();
        auto current_ = current();
        auto correct_ = correct(current_, response);
        if (correct_)
            correct();
        else
            incorrect();
        auto updatedReversals = currentSnrTrack->reversals();
        lastTrial.trial.subjectColor = response.color;
        lastTrial.trial.subjectNumber = response.number;
        lastTrial.reversals = updatedReversals;
        lastTrial.trial.correctColor = evaluator->correctColor(current_);
        lastTrial.trial.correctNumber = evaluator->correctNumber(current_);
        lastTrial.SNR_dB = lastSnr_dB_;
        lastTrial.trial.correct = correct_;
        selectNextList();
    }
    
    int AdaptiveMethod::snr_dB() {
        return currentSnrTrack->x();
    }
    
    std::string AdaptiveMethod::current() {
        return currentTargetList->current();
    }

    bool AdaptiveMethod::correct(
        const std::string &target,
        const coordinate_response_measure::SubjectResponse &response
    ) {
        return evaluator->correct(target, response);
    }
    
    void AdaptiveMethod::correct() {
        currentSnrTrack->down();
    }
    
    void AdaptiveMethod::incorrect() {
        currentSnrTrack->up();
    }
    
    void AdaptiveMethod::writeTestingParameters(OutputFile *file) {
        file->writeTest(*test);
    }
    
    void AdaptiveMethod::writeLastCoordinateResponse(OutputFile *file) {
        file->writeTrial(lastTrial);
    }
    
    void AdaptiveMethod::writeLastCorrectResponse(OutputFile *file) {
        file->writeTrial(lastOpenSetTrial);
    }
    
    void AdaptiveMethod::writeLastIncorrectResponse(OutputFile *file) {
        file->writeTrial(lastOpenSetTrial);
    }
    
    void AdaptiveMethod::submitIncorrectResponse() {
        incorrect();
        lastOpenSetTrial.reversals = currentSnrTrack->reversals();
        selectNextList();
    }
    
    void AdaptiveMethod::submitCorrectResponse() {
        correct();
        lastOpenSetTrial.reversals = currentSnrTrack->reversals();
        selectNextList();
    }
    
    void AdaptiveMethod::submitResponse(const FreeResponse &) {
        selectNextList();
    }
}
