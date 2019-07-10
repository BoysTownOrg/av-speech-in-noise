#include "RecognitionTestModel.hpp"
#include <cmath>

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
        randomizer{randomizer},
        currentSnrTrack{},
        currentTargetList{} {}
    
    void AdaptiveMethod::initialize(const AdaptiveTest &p) {
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
    
    void FixedLevelMethod::writeLastCoordinateResponse(OutputFile *file) {
        file->writeTrial(lastTrial);
    }
    
    void FixedLevelMethod::submitIncorrectResponse() {
        
    }
    
    void FixedLevelMethod::submitCorrectResponse() {
        
    }
    
    void FixedLevelMethod::submitResponse(const FreeResponse &) {
    
    }

    class NullTestMethod : public TestMethod {
        bool complete() override { return {}; }
        std::string next() override { return {}; }
        std::string current() override { return {}; }
        int snr_dB() override { return {}; }
        void submitCorrectResponse() override {}
        void submitIncorrectResponse() override {}
        void writeLastCoordinateResponse(OutputFile *) override {}
        void submitResponse(
            const coordinate_response_measure::SubjectResponse &
        ) override {}
        void submitResponse(const FreeResponse &) override {}
    };
    
    static NullTestMethod nullTestMethod;
    
    RecognitionTestModel::RecognitionTestModel(
        AdaptiveMethod *adaptiveMethod,
        FixedLevelMethod *fixedLevelMethod,
        TargetPlayer *targetPlayer,
        MaskerPlayer *maskerPlayer,
        ResponseEvaluator *evaluator,
        OutputFile *outputFile,
        Randomizer *randomizer
    ) :
        adaptiveMethod{adaptiveMethod},
        fixedLevelMethod{fixedLevelMethod},
        maskerPlayer{maskerPlayer},
        targetPlayer{targetPlayer},
        evaluator{evaluator},
        outputFile{outputFile},
        randomizer{randomizer},
        testMethod{&nullTestMethod}
    {
        targetPlayer->subscribe(this);
        maskerPlayer->subscribe(this);
    }
    
    void RecognitionTestModel::subscribe(Model::EventListener *listener) {
        listener_ = listener;
    }
    
    void RecognitionTestModel::initializeTest(const FixedLevelTest &p) {
        throwIfTrialInProgress();
        
        fixedLevelMethod->initialize(p);
        testMethod = fixedLevelMethod;
        prepareCommonTest(p.common);
        tryOpeningOutputFile(p.information);
        outputFile->writeTest(p);
    }
    
    void RecognitionTestModel::initializeTest(const AdaptiveTest &p) {
        throwIfTrialInProgress();
        
        adaptiveMethod->initialize(p);
        testMethod = adaptiveMethod;
        prepareCommonTest(p.common);
        tryOpeningOutputFile(p.information);
        outputFile->writeTest(p);
    }
    
    void RecognitionTestModel::throwIfTrialInProgress() {
        if (trialInProgress())
            throw RequestFailure{"Trial in progress."};
    }
    
    bool RecognitionTestModel::trialInProgress() {
        return maskerPlayer->playing();
    }
    
    void RecognitionTestModel::prepareCommonTest(const CommonTest &common) {
        storeLevels(common);
        prepareMasker(common.maskerFilePath);
        condition = common.condition;
        preparePlayersForNextTrial();
    }
    
    void RecognitionTestModel::storeLevels(const CommonTest &common) {
        fullScaleLevel_dB_SPL = common.fullScaleLevel_dB_SPL;
        maskerLevel_dB_SPL = common.maskerLevel_dB_SPL;
    }
    
    void RecognitionTestModel::prepareMasker(const std::string &p) {
        loadMaskerFile(p);
        maskerPlayer->setLevel_dB(maskerLevel_dB());
    }
    
    void RecognitionTestModel::loadMaskerFile(const std::string &p) {
        maskerPlayer->loadFile(p);
    }
    
    static double dB(double x) {
        return 20 * std::log10(x);
    }
    
    double RecognitionTestModel::maskerLevel_dB() {
        return desiredMaskerLevel_dB() - dB(maskerPlayer->rms());
    }
    
    int RecognitionTestModel::desiredMaskerLevel_dB() {
        return maskerLevel_dB_SPL - fullScaleLevel_dB_SPL;
    }
    
    void RecognitionTestModel::prepareVideo(const Condition &p) {
        if (auditoryOnly(p))
            targetPlayer->hideVideo();
        else
            targetPlayer->showVideo();
    }

    bool RecognitionTestModel::auditoryOnly(const Condition &c) {
        return c == Condition::auditoryOnly;
    }
    
    void RecognitionTestModel::preparePlayersForNextTrial() {
        prepareTargetPlayer();
        seekRandomMaskerPosition();
    }
    
    void RecognitionTestModel::prepareTargetPlayer() {
        loadTargetFile(testMethod->next());
        setTargetLevel_dB(targetLevel_dB());
        targetPlayer->subscribeToPlaybackCompletion();
    }
    
    void RecognitionTestModel::loadTargetFile(std::string s) {
        targetPlayer->loadFile(std::move(s));;
    }
    
    void RecognitionTestModel::setTargetLevel_dB(double x) {
        targetPlayer->setLevel_dB(x);
    }
    
    double RecognitionTestModel::targetLevel_dB() {
        return maskerLevel_dB() + testMethod->snr_dB();
    }
    
    double RecognitionTestModel::unalteredTargetLevel_dB() {
        return dB(targetPlayer->rms());
    }
    
    void RecognitionTestModel::seekRandomMaskerPosition() {
        auto upperLimit =
            maskerPlayer->durationSeconds() -
            2 * maskerPlayer->fadeTimeSeconds() -
            targetPlayer->durationSeconds();
        maskerPlayer->seekSeconds(randomizer->randomFloatBetween(0, upperLimit));
    }
    
    void RecognitionTestModel::tryOpeningOutputFile(const TestInformation &p) {
        outputFile->close();
        tryOpeningOutputFile_(p);
    }
    
    void RecognitionTestModel::tryOpeningOutputFile_(const TestInformation &p) {
        try {
            outputFile->openNewFile(p);
        } catch (const OutputFile::OpenFailure &) {
            throw RequestFailure{"Unable to open output file."};
        }
    }
    
    void RecognitionTestModel::playTrial(const AudioSettings &settings) {
        throwIfTrialInProgress();
        
        preparePlayersToPlay(settings);
        startTrial();
    }
    
    void RecognitionTestModel::preparePlayersToPlay(const AudioSettings &p) {
        setAudioDevices(p);
    }
    
    void RecognitionTestModel::setAudioDevices(const AudioSettings &p) {
        throwInvalidAudioDeviceOnErrorSettingDevice(
            &RecognitionTestModel::setAudioDevices_,
            p.audioDevice
        );
    }
    
    void RecognitionTestModel::throwInvalidAudioDeviceOnErrorSettingDevice(
        void (RecognitionTestModel::*f)(const std::string &),
        const std::string &device
    ) {
        try {
            (this->*f)(device);
        } catch (const InvalidAudioDevice &) {
            throw RequestFailure{
                "'" + device + "' is not a valid audio device."
            };
        }
    }
    
    void RecognitionTestModel::setAudioDevices_(const std::string &device) {
        maskerPlayer->setAudioDevice(device);
        setTargetPlayerDevice_(device);
    }
    
    void RecognitionTestModel::setTargetPlayerDevice_(const std::string &device) {
        targetPlayer->setAudioDevice(device);
    }
    
    void RecognitionTestModel::startTrial() {
        if (!auditoryOnly(condition))
            targetPlayer->showVideo();
        maskerPlayer->fadeIn();
    }
    
    void RecognitionTestModel::fadeInComplete() {
        playTarget();
    }
    
    void RecognitionTestModel::playTarget() {
        targetPlayer->play();
    }
    
    void RecognitionTestModel::playbackComplete() {
        maskerPlayer->fadeOut();
    }
    
    void RecognitionTestModel::fadeOutComplete() {
        targetPlayer->hideVideo();
        listener_->trialComplete();
    }
    
    void RecognitionTestModel::submitResponse(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        submitResponse_(response);
        testMethod->writeLastCoordinateResponse(outputFile);
        outputFile->save();
    }
    
    void RecognitionTestModel::submitResponse_(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        testMethod->submitResponse(response);
        if (!testMethod->complete())
            preparePlayersForNextTrial();
    }
    
    void RecognitionTestModel::submitCorrectResponse() {
        submitCorrectResponse_();
    }
    
    void RecognitionTestModel::submitCorrectResponse_() {
        testMethod->submitCorrectResponse();
        preparePlayersForNextTrial();
    }
    
    void RecognitionTestModel::submitIncorrectResponse() {
        submitIncorrectResponse_();
    }
    
    void RecognitionTestModel::submitIncorrectResponse_() {
        testMethod->submitIncorrectResponse();
        preparePlayersForNextTrial();
    }
    
    void RecognitionTestModel::submitResponse(const FreeResponse &response) {
        writeTrial(response);
        testMethod->submitResponse(response);
        preparePlayersForNextTrial();
    }
    
    void RecognitionTestModel::writeTrial(const FreeResponse &p) {
        FreeResponseTrial trial;
        trial.response = p.response;
        trial.target = evaluator->fileName(testMethod->current());
        outputFile->writeTrial(trial);
        outputFile->save();
    }
    
    void RecognitionTestModel::playCalibration(const Calibration &p) {
        throwIfTrialInProgress();
        
        playCalibration_(p);
    }
    
    void RecognitionTestModel::playCalibration_(const Calibration &p) {
        setTargetPlayerDevice(p);
        loadTargetFile(p.filePath);
        trySettingTargetLevel(p);
        prepareVideo(p.condition);
        playTarget();
    }
    
    void RecognitionTestModel::setTargetPlayerDevice(const Calibration &p) {
        throwInvalidAudioDeviceOnErrorSettingDevice(
            &RecognitionTestModel::setTargetPlayerDevice_,
            p.audioSettings.audioDevice
        );
    }
    
    void RecognitionTestModel::trySettingTargetLevel(const Calibration &p) {
        try {
            setTargetLevel_dB(calibrationLevel_dB(p));
        } catch (const InvalidAudioFile &) {
            throw RequestFailure{"unable to read " + p.filePath};
        }
    }
    
    double RecognitionTestModel::calibrationLevel_dB(const Calibration &p) {
        return
            p.level_dB_SPL -
            p.fullScaleLevel_dB_SPL -
            unalteredTargetLevel_dB();
    }

    bool RecognitionTestModel::testComplete() {
        return testMethod->complete();
    }
    
    std::vector<std::string> RecognitionTestModel::audioDevices() {
        return maskerPlayer->outputAudioDeviceDescriptions();
    }
}

