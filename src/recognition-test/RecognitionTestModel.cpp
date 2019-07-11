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
        model->throwIfTrialInProgress();
        
        fixedLevelMethod->initialize(p);
        model->initialize(fixedLevelMethod, p.common, p.information);
    }
    
    void RecognitionTestModel::initializeTest(const AdaptiveTest &p) {
        model->throwIfTrialInProgress();
        
        adaptiveMethod->initialize(p);
        model->initialize(adaptiveMethod, p.common, p.information);
    }
    
    void RecognitionTestModel::playTrial(const AudioSettings &settings) {
        model->throwIfTrialInProgress();
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

    class NullTestMethod : public TestMethod {
        bool complete() override { return {}; }
        std::string next() override { return {}; }
        std::string current() override { return {}; }
        int snr_dB() override { return {}; }
        void submitCorrectResponse() override {}
        void submitIncorrectResponse() override {}
        void writeLastCoordinateResponse(OutputFile *) override {}
        void writeTestingParameters(OutputFile *) override {}
        void submitResponse(
            const coordinate_response_measure::SubjectResponse &
        ) override {}
        void submitResponse(const FreeResponse &) override {}
    };
    
    static NullTestMethod nullTestMethod;
    
    
    RecognitionTestModel_Internal::RecognitionTestModel_Internal(
        TargetPlayer *targetPlayer,
        MaskerPlayer *maskerPlayer,
        ResponseEvaluator *evaluator,
        OutputFile *outputFile,
        Randomizer *randomizer
    ) :
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
    
    void RecognitionTestModel_Internal::subscribe(Model::EventListener *listener) {
        listener_ = listener;
    }
    
    void RecognitionTestModel_Internal::throwIfTrialInProgress() {
        if (trialInProgress())
            throw Model::RequestFailure{"Trial in progress."};
    }
    
    void RecognitionTestModel_Internal::initialize(
        TestMethod *testMethod_,
        const CommonTest &common,
        const TestInformation &information
    ) {
        throwIfTrialInProgress();
        testMethod = testMethod_;
        prepareCommonTest(common, information);
    }
    
    bool RecognitionTestModel_Internal::trialInProgress() {
        return maskerPlayer->playing();
    }
    
    void RecognitionTestModel_Internal::prepareCommonTest(
        const CommonTest &common,
        const TestInformation &information
    ) {
        storeLevels(common);
        prepareMasker(common.maskerFilePath);
        targetPlayer->hideVideo();
        condition = common.condition;
        preparePlayersForNextTrial();
        tryOpeningOutputFile(information);
        testMethod->writeTestingParameters(outputFile);
    }
    
    void RecognitionTestModel_Internal::storeLevels(const CommonTest &common) {
        fullScaleLevel_dB_SPL = common.fullScaleLevel_dB_SPL;
        maskerLevel_dB_SPL = common.maskerLevel_dB_SPL;
    }
    
    void RecognitionTestModel_Internal::prepareMasker(const std::string &p) {
        throwInvalidAudioFileOnErrorLoading(&RecognitionTestModel_Internal::loadMaskerFile, p);
        maskerPlayer->setLevel_dB(maskerLevel_dB());
    }
    
    void RecognitionTestModel_Internal::throwInvalidAudioFileOnErrorLoading(
        void (RecognitionTestModel_Internal::*f)(const std::string &),
        const std::string &file
    ) {
        try {
            (this->*f)(file);
        } catch (const InvalidAudioFile &) {
            throw Model::RequestFailure{"unable to read " + file};
        }
    }
    
    void RecognitionTestModel_Internal::loadMaskerFile(const std::string &p) {
        maskerPlayer->loadFile(p);
    }
    
    static double dB(double x) {
        return 20 * std::log10(x);
    }
    
    double RecognitionTestModel_Internal::maskerLevel_dB() {
        return desiredMaskerLevel_dB() - dB(maskerPlayer->rms());
    }
    
    int RecognitionTestModel_Internal::desiredMaskerLevel_dB() {
        return maskerLevel_dB_SPL - fullScaleLevel_dB_SPL;
    }
    
    void RecognitionTestModel_Internal::prepareVideo(const Condition &p) {
        if (auditoryOnly(p))
            targetPlayer->hideVideo();
        else
            targetPlayer->showVideo();
    }

    bool RecognitionTestModel_Internal::auditoryOnly(const Condition &c) {
        return c == Condition::auditoryOnly;
    }
    
    void RecognitionTestModel_Internal::preparePlayersForNextTrial() {
        prepareTargetPlayer();
        seekRandomMaskerPosition();
    }
    
    void RecognitionTestModel_Internal::prepareTargetPlayer() {
        loadTargetFile(testMethod->next());
        setTargetLevel_dB(targetLevel_dB());
        targetPlayer->subscribeToPlaybackCompletion();
    }
    
    void RecognitionTestModel_Internal::loadTargetFile(std::string s) {
        targetPlayer->loadFile(std::move(s));;
    }
    
    void RecognitionTestModel_Internal::setTargetLevel_dB(double x) {
        targetPlayer->setLevel_dB(x);
    }
    
    double RecognitionTestModel_Internal::targetLevel_dB() {
        return maskerLevel_dB() + testMethod->snr_dB();
    }
    
    void RecognitionTestModel_Internal::seekRandomMaskerPosition() {
        auto upperLimit =
            maskerPlayer->durationSeconds() -
            2 * maskerPlayer->fadeTimeSeconds() -
            targetPlayer->durationSeconds();
        maskerPlayer->seekSeconds(randomizer->randomFloatBetween(0, upperLimit));
    }
    
    void RecognitionTestModel_Internal::tryOpeningOutputFile(const TestInformation &p) {
        outputFile->close();
        tryOpeningOutputFile_(p);
    }
    
    void RecognitionTestModel_Internal::tryOpeningOutputFile_(const TestInformation &p) {
        try {
            outputFile->openNewFile(p);
        } catch (const OutputFile::OpenFailure &) {
            throw Model::RequestFailure{"Unable to open output file."};
        }
    }
    
    void RecognitionTestModel_Internal::playTrial(const AudioSettings &settings) {
        throwIfTrialInProgress();
        
        preparePlayersToPlay(settings);
        startTrial();
    }
    
    void RecognitionTestModel_Internal::preparePlayersToPlay(const AudioSettings &p) {
        setAudioDevices(p);
    }
    
    void RecognitionTestModel_Internal::setAudioDevices(const AudioSettings &p) {
        throwInvalidAudioDeviceOnErrorSettingDevice(
            &RecognitionTestModel_Internal::setAudioDevices_,
            p.audioDevice
        );
    }
    
    void RecognitionTestModel_Internal::throwInvalidAudioDeviceOnErrorSettingDevice(
        void (RecognitionTestModel_Internal::*f)(const std::string &),
        const std::string &device
    ) {
        try {
            (this->*f)(device);
        } catch (const InvalidAudioDevice &) {
            throw Model::RequestFailure{
                "'" + device + "' is not a valid audio device."
            };
        }
    }
    
    void RecognitionTestModel_Internal::setAudioDevices_(const std::string &device) {
        maskerPlayer->setAudioDevice(device);
        setTargetPlayerDevice_(device);
    }
    
    void RecognitionTestModel_Internal::setTargetPlayerDevice_(const std::string &device) {
        targetPlayer->setAudioDevice(device);
    }
    
    void RecognitionTestModel_Internal::startTrial() {
        if (!auditoryOnly(condition))
            targetPlayer->showVideo();
        maskerPlayer->fadeIn();
    }
    
    void RecognitionTestModel_Internal::fadeInComplete() {
        playTarget();
    }
    
    void RecognitionTestModel_Internal::playTarget() {
        targetPlayer->play();
    }
    
    void RecognitionTestModel_Internal::playbackComplete() {
        maskerPlayer->fadeOut();
    }
    
    void RecognitionTestModel_Internal::fadeOutComplete() {
        targetPlayer->hideVideo();
        listener_->trialComplete();
    }
    
    void RecognitionTestModel_Internal::submitResponse(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        submitResponse_(response);
        testMethod->writeLastCoordinateResponse(outputFile);
        outputFile->save();
    }
    
    void RecognitionTestModel_Internal::submitResponse_(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        testMethod->submitResponse(response);
        if (!testMethod->complete())
            preparePlayersForNextTrial();
    }
    
    void RecognitionTestModel_Internal::submitCorrectResponse() {
        submitCorrectResponse_();
    }
    
    void RecognitionTestModel_Internal::submitCorrectResponse_() {
        testMethod->submitCorrectResponse();
        preparePlayersForNextTrial();
    }
    
    void RecognitionTestModel_Internal::submitIncorrectResponse() {
        submitIncorrectResponse_();
    }
    
    void RecognitionTestModel_Internal::submitIncorrectResponse_() {
        testMethod->submitIncorrectResponse();
        preparePlayersForNextTrial();
    }
    
    void RecognitionTestModel_Internal::submitResponse(const FreeResponse &response) {
        writeTrial(response);
        testMethod->submitResponse(response);
        preparePlayersForNextTrial();
    }
    
    void RecognitionTestModel_Internal::writeTrial(const FreeResponse &p) {
        FreeResponseTrial trial;
        trial.response = p.response;
        trial.target = evaluator->fileName(testMethod->current());
        outputFile->writeTrial(trial);
        outputFile->save();
    }
    
    void RecognitionTestModel_Internal::playCalibration(const Calibration &p) {
        throwIfTrialInProgress();
        
        playCalibration_(p);
    }
    
    void RecognitionTestModel_Internal::playCalibration_(const Calibration &p) {
        setTargetPlayerDevice(p);
        loadTargetFile(p.filePath);
        trySettingTargetLevel(p);
        prepareVideo(p.condition);
        playTarget();
    }
    
    void RecognitionTestModel_Internal::setTargetPlayerDevice(const Calibration &p) {
        throwInvalidAudioDeviceOnErrorSettingDevice(
            &RecognitionTestModel_Internal::setTargetPlayerDevice_,
            p.audioSettings.audioDevice
        );
    }
    
    void RecognitionTestModel_Internal::trySettingTargetLevel(const Calibration &p) {
        try {
            setTargetLevel_dB(calibrationLevel_dB(p));
        } catch (const InvalidAudioFile &) {
            throw Model::RequestFailure{"unable to read " + p.filePath};
        }
    }
    
    double RecognitionTestModel_Internal::calibrationLevel_dB(const Calibration &p) {
        return
            p.level_dB_SPL -
            p.fullScaleLevel_dB_SPL -
            unalteredTargetLevel_dB();
    }
    
    double RecognitionTestModel_Internal::unalteredTargetLevel_dB() {
        return dB(targetPlayer->rms());
    }

    bool RecognitionTestModel_Internal::testComplete() {
        return testMethod->complete();
    }
    
    std::vector<std::string> RecognitionTestModel_Internal::audioDevices() {
        return maskerPlayer->outputAudioDeviceDescriptions();
    }
}

