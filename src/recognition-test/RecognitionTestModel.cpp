#include "RecognitionTestModel.hpp"
#include <gsl/gsl>

namespace av_speech_in_noise {
    AdaptiveMethod::AdaptiveMethod(
        TargetListReader *targetListSetReader,
        TrackFactory *snrTrackFactory,
        ResponseEvaluator *evaluator,
        Randomizer *randomizer
    ) :
        targetListSetReader{targetListSetReader},
        snrTrackFactory{snrTrackFactory},
        evaluator{evaluator},
        randomizer{randomizer} {}
    
    void AdaptiveMethod::initialize(const AdaptiveTest &p) {
        test = &p;
        trackSettings.ceiling = p.ceilingSnr_dB;
        trackSettings.rule = p.targetLevelRule;
        trackSettings.startingX = p.startingSnr_dB;
        trackSettings.floor = p.floorSnr_dB;
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
        currentSnrTrack->pushDown();
    }
    
    void AdaptiveMethod::incorrect() {
        currentSnrTrack->pushUp();
    }
    
    void AdaptiveMethod::writeTestingParameters(OutputFile *file) {
        file->writeTest(*test);
    }
    
    void AdaptiveMethod::writeLastCoordinateResponse(OutputFile *file) {
        file->writeTrial(lastTrial);
    }
    
    void AdaptiveMethod::submitIncorrectResponse() {
        selectNextListAfter(&AdaptiveMethod::incorrect);
    }
    
    void AdaptiveMethod::submitCorrectResponse() {
        selectNextListAfter(&AdaptiveMethod::correct);
    }
    
    void AdaptiveMethod::submitResponse(const FreeResponse &) {
        selectNextList();
    }
    
    
    FixedLevelMethod::FixedLevelMethod(
        TargetList *targetList,
        ResponseEvaluator *evaluator
    ) :
        targetList{targetList},
        evaluator{evaluator} {}
    
    void FixedLevelMethod::initialize(const FixedLevelTest &p) {
        test = &p;
        snr_dB_ = p.snr_dB;
        trials_ = p.trials;
        targetList->loadFromDirectory(p.common.targetListDirectory);
        updateCompletion();
    }
    
    void FixedLevelMethod::updateCompletion() {
        complete_ = trials_ == 0;
    }
    
    bool FixedLevelMethod::complete() {
        return complete_;
    }
    
    std::string FixedLevelMethod::next() {
        return targetList->next();
    }
    
    int FixedLevelMethod::snr_dB() {
        return snr_dB_;
    }
    
    void FixedLevelMethod::submitResponse(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        --trials_;
        auto current_ = current();
        lastTrial.trial.subjectColor = response.color;
        lastTrial.trial.subjectNumber = response.number;
        lastTrial.trial.correctColor = evaluator->correctColor(current_);
        lastTrial.trial.correctNumber = evaluator->correctNumber(current_);
        lastTrial.trial.correct = evaluator->correct(current_, response);
        updateCompletion();
    }
    
    std::string FixedLevelMethod::current() {
        return targetList->current();
    }
    
    void FixedLevelMethod::writeTestingParameters(OutputFile *file) {
        file->writeTest(*test);
    }
    
    void FixedLevelMethod::writeLastCoordinateResponse(OutputFile *file) {
        file->writeTrial(lastTrial);
    }
    
    void FixedLevelMethod::submitIncorrectResponse() {
        
    }
    
    void FixedLevelMethod::submitCorrectResponse() {
        
    }
    
    void FixedLevelMethod::submitResponse(const FreeResponse &) {
    
    }
    
    RecognitionTestModel::RecognitionTestModel(
        IAdaptiveMethod *adaptiveMethod,
        IFixedLevelMethod *fixedLevelMethod,
        IRecognitionTestModel_Internal *model
    ) :
        adaptiveMethod{adaptiveMethod},
        fixedLevelMethod{fixedLevelMethod},
        model{model}
    {
    }
    
    void RecognitionTestModel::initializeTest(const FixedLevelTest &p) {
        fixedLevelMethod->initialize(p);
        model->initialize(fixedLevelMethod, p.common, p.information);
    }
    
    void RecognitionTestModel::initializeTest(const AdaptiveTest &p) {
        adaptiveMethod->initialize(p);
        model->initialize(adaptiveMethod, p.common, p.information);
    }
    
    void RecognitionTestModel::playTrial(const AudioSettings &settings) {
        model->playTrial(settings);
    }
    
    void RecognitionTestModel::submitResponse(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        model->submitResponse(response);
    }
    
    void RecognitionTestModel::submitCorrectResponse() {
        model->submitCorrectResponse();
    }
    
    void RecognitionTestModel::submitIncorrectResponse() {
        model->submitIncorrectResponse();
    }
    
    void RecognitionTestModel::submitResponse(const FreeResponse &response) {
        model->submitResponse(response);
    }
    
    void RecognitionTestModel::playCalibration(const Calibration &p) {
        model->playCalibration(p);
    }

    bool RecognitionTestModel::testComplete() {
        return model->testComplete();
    }
    
    std::vector<std::string> RecognitionTestModel::audioDevices() {
        return model->audioDevices();
    }
    
    void RecognitionTestModel::subscribe(Model::EventListener *e) {
        model->subscribe(e);
    }
}

