#include "RecognitionTestModel.hpp"
#include <cmath>

namespace av_speech_in_noise {
    class NullTestMethod : public TestMethod {
        bool complete() override { return {}; }
        std::string next() override { return {}; }
        std::string current() override { return {}; }
        void loadTargets(const std::string &) override {}
        int snr_dB() override { return {}; }
        void correct() override {}
        void incorrect() override {}
    };
    
    static NullTestMethod nullTestMethod;
    
    RecognitionTestModel::RecognitionTestModel(
        AdaptiveMethod *adaptiveMethod,
        FiniteTargetList *finiteTargetList,
        TargetPlayer *targetPlayer,
        MaskerPlayer *maskerPlayer,
        ResponseEvaluator *evaluator,
        OutputFile *outputFile,
        Randomizer *randomizer
    ) :
        adaptiveMethod{adaptiveMethod},
        fixedLevelMethod{finiteTargetList},
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
        
        fixedLevelMethod.store(p);
        testMethod = &fixedLevelMethod;
        prepareCommonTest(p.common);
        tryOpeningOutputFile(p.information);
        outputFile->writeTest(p);
    }
    
    void RecognitionTestModel::initializeTest(const AdaptiveTest &p) {
        throwIfTrialInProgress();
        
        adaptiveMethod->store(p);
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
        prepareVideo(common.condition);
        testMethod->loadTargets(common.targetListDirectory);
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
        return
            desiredMaskerLevel_dB() +
            testMethod->snr_dB() -
            unalteredTargetLevel_dB();
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
        listener_->trialComplete();
    }
    
    void RecognitionTestModel::submitResponse(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        writeTrial(response);
        submitResponse_(response);
    }
    
    void RecognitionTestModel::writeTrial(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        if (!justWroteCoordinateResponseTrial)
            outputFile->writeCoordinateResponseTrialHeading();
        coordinate_response_measure::Trial trial;
        trial.subjectColor = response.color;
        trial.subjectNumber = response.number;
        trial.reversals = adaptiveMethod->reversals();
        trial.correctColor = evaluator->correctColor(currentTarget());
        trial.correctNumber = evaluator->correctNumber(currentTarget());
        trial.SNR_dB = testMethod->snr_dB();
        trial.correct = correct(response);
        outputFile->writeTrial(trial);
        outputFile->save();
        justWroteCoordinateResponseTrial = true;
        justWroteFreeResponseTrial = false;
    }
    
    void RecognitionTestModel::submitResponse_(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        if (correct(response))
            submitCorrectResponse_();
        else
            submitIncorrectResponse_();
    }
    
    bool RecognitionTestModel::correct(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        return evaluator->correct(currentTarget(), response);
    }
    
    std::string RecognitionTestModel::currentTarget() {
        return testMethod->current();
    }
    
    void RecognitionTestModel::submitCorrectResponse() {
        submitCorrectResponse_();
    }
    
    void RecognitionTestModel::submitCorrectResponse_() {
        testMethod->correct();
        preparePlayersForNextTrial();
    }
    
    void RecognitionTestModel::submitIncorrectResponse() {
        submitIncorrectResponse_();
    }
    
    void RecognitionTestModel::submitIncorrectResponse_() {
        testMethod->incorrect();
        preparePlayersForNextTrial();
    }
    
    void RecognitionTestModel::submitResponse(const FreeResponse &p) {
        writeTrial(p);
        preparePlayersForNextTrial();
    }
    
    void RecognitionTestModel::writeTrial(const FreeResponse &p) {
        if (!justWroteFreeResponseTrial)
            outputFile->writeFreeResponseTrialHeading();
        FreeResponseTrial trial;
        trial.response = p.response;
        trial.target = evaluator->fileName(testMethod->current());
        outputFile->writeTrial(trial);
        outputFile->save();
        justWroteFreeResponseTrial = true;
        justWroteCoordinateResponseTrial = false;
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

