#include "RecognitionTestModel.hpp"
#include <gsl/gsl>
#include <cmath>

namespace av_coordinate_response_measure {
    RecognitionTestModel::RecognitionTestModel(
        TargetList *targetList,
        TargetPlayer *targetPlayer,
        MaskerPlayer *maskerPlayer,
        Track *snrTrack,
        ResponseEvaluator *evaluator,
        OutputFile *outputFile,
        Randomizer *randomizer
    ) :
        maskerPlayer{maskerPlayer},
        targetList{targetList},
        targetPlayer{targetPlayer},
        snrTrack{snrTrack},
        evaluator{evaluator},
        outputFile{outputFile},
        randomizer{randomizer}
    {
        targetPlayer->subscribe(this);
        maskerPlayer->subscribe(this);
    }
    
    void RecognitionTestModel::subscribe(Model::EventListener *listener) {
        listener_ = listener;
    }
    
    void RecognitionTestModel::initializeTest(const Test &p) {
        fullScaleLevel_dB_SPL = p.fullScaleLevel_dB_SPL;
        maskerLevel_dB_SPL = p.maskerLevel_dB_SPL;
        
        prepareSnrTrack(p);
        prepareOutputFile(p);
        prepareMasker(p);
        prepareTargets(p);
        prepareVideo(p);
    }
    
    void RecognitionTestModel::prepareSnrTrack(const Test &p) {
        Track::Settings s;
        s.rule = p.targetLevelRule;
        s.startingX = p.startingSnr_dB;
        snrTrack->reset(s);
    }
    
    void RecognitionTestModel::prepareOutputFile(const Test &p) {
        outputFile->close();
        tryOpeningOutputFile(p);
        outputFile->writeTest(p);
        outputFile->writeTrialHeading();
    }
    
    void RecognitionTestModel::tryOpeningOutputFile(const Test &p) {
        try {
            outputFile->openNewFile(p);
        } catch (const OutputFile::OpenFailure &) {
            throw RequestFailure{"Unable to open output file."};
        }
    }
    
    void RecognitionTestModel::prepareMasker(const Test &p) {
        loadMaskerFile(p);
        maskerPlayer->setLevel_dB(maskerLevel_dB());
    }
    
    void RecognitionTestModel::loadMaskerFile(const Test &p) {
        maskerPlayer->loadFile(p.maskerFilePath);
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
    
    void RecognitionTestModel::prepareTargets(const Test &p) {
        targetList->loadFromDirectory(p.targetListDirectory);
    }
    
    void RecognitionTestModel::prepareVideo(const Test &p) {
        if (auditoryOnly(p))
            targetPlayer->hideVideo();
        else
            targetPlayer->showVideo();
    }

    bool RecognitionTestModel::auditoryOnly(const Test &p) {
        return p.condition == Condition::auditoryOnly;
    }
    
    void RecognitionTestModel::playTrial(const AudioSettings &settings) {
        if (noMoreTrials() || trialInProgress())
            return;
        
        preparePlayers(settings);
        startTrial();
    }
    
    bool RecognitionTestModel::noMoreTrials() {
        return snrTrack->complete();
    }
    
    bool RecognitionTestModel::trialInProgress() {
        return maskerPlayer->playing();
    }
    
    void RecognitionTestModel::preparePlayers(const AudioSettings &p) {
        trySettingAudioDevices(p);
        loadNextTarget();
        maskerPlayer->seekSeconds(randomizer->randomFloatBetween(
            0,
            maskerPlayer->durationSeconds() - 2 * maskerPlayer->fadeTimeSeconds() - targetPlayer->durationSeconds()));
    }
    
    void RecognitionTestModel::trySettingAudioDevices(const AudioSettings &p) {
        throwInvalidAudioDeviceOnErrorSettingDevice(
            &RecognitionTestModel::setAudioDevices,
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
    
    void RecognitionTestModel::setAudioDevices(const std::string &device) {
        maskerPlayer->setAudioDevice(device);
        setTargetPlayerDevice(device);
    }
    
    void RecognitionTestModel::setTargetPlayerDevice(const std::string &device) {
        targetPlayer->setAudioDevice(device);
    }
    
    void RecognitionTestModel::loadNextTarget() {
        loadTargetFile(targetList->next());
        setTargetLevel_dB(targetLevel_dB());
        targetPlayer->subscribeToPlaybackCompletion();
    }
    
    void RecognitionTestModel::loadTargetFile(std::string s) {
        targetPlayer->loadFile(std::move(s));;
    }
    
    void RecognitionTestModel::setTargetLevel_dB(double x) {
        targetPlayer->setLevel_dB(x);
    }
    
    double RecognitionTestModel::unalteredTargetLevel_dB() {
        return dB(targetPlayer->rms());
    }
    
    double RecognitionTestModel::targetLevel_dB() {
        return
            desiredMaskerLevel_dB() +
            SNR_dB() -
            unalteredTargetLevel_dB();
    }
    
    int RecognitionTestModel::SNR_dB() {
        return snrTrack->x();
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
    
    void RecognitionTestModel::submitResponse(const SubjectResponse &response) {
        updateSnr(response);
        writeTrial(response);
    }
    
    void RecognitionTestModel::updateSnr(const SubjectResponse &response) {
        if (correct(response))
            snrTrack->pushDown();
        else
            snrTrack->pushUp();
    }
    
    bool RecognitionTestModel::correct(const SubjectResponse &response) {
        return evaluator->correct(currentTarget(), response);
    }
    
    std::string RecognitionTestModel::currentTarget() {
        return targetList->current();
    }
    
    void RecognitionTestModel::writeTrial(const SubjectResponse &response) {
        Trial trial;
        trial.subjectColor = response.color;
        trial.subjectNumber = response.number;
        trial.reversals = snrTrack->reversals();
        trial.correctColor = evaluator->correctColor(currentTarget());
        trial.correctNumber = evaluator->correctNumber(currentTarget());
        trial.SNR_dB = SNR_dB();
        trial.correct = correct(response);
        outputFile->writeTrial(trial);
    }
    
    void RecognitionTestModel::fadeOutComplete() { 
        listener_->trialComplete();
    }
    
    void RecognitionTestModel::playCalibration(const Calibration &p) {
        if (targetPlayer->playing())
            return;
        
        playCalibration_(p);
    }
    
    void RecognitionTestModel::playCalibration_(const Calibration &p) {
        throwInvalidAudioDeviceOnErrorSettingDevice(
            &RecognitionTestModel::setTargetPlayerDevice,
            p.audioDevice
        );
        loadTargetFile(p.filePath);
        try {
            setTargetLevel_dB(
                p.level_dB_SPL -
                p.fullScaleLevel_dB_SPL -
                unalteredTargetLevel_dB()
            );
        } catch (const InvalidAudioFile &) {
            throw RequestFailure{"unable to read " + p.filePath};
        }
        playTarget();
    }

    bool RecognitionTestModel::testComplete() {
        return snrTrack->complete();
    }
    
    std::vector<std::string> RecognitionTestModel::audioDevices() {
        return maskerPlayer->outputAudioDeviceDescriptions();
    }
}

