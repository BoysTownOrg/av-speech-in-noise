#include "RecognitionTestModel.hpp"
#include <gsl/gsl>
#include <cmath>

namespace recognition_test {
    RecognitionTestModel::RecognitionTestModel(
        TargetList *targetList,
        TargetPlayer *stimulusPlayer,
        MaskerPlayer *maskerPlayer,
        OutputFile *outputFile
    ) :
        maskerPlayer{maskerPlayer},
        targetList{targetList},
        targetPlayer{stimulusPlayer},
        outputFile{outputFile}
    {
        maskerPlayer->subscribe(this);
        stimulusPlayer->subscribe(this);
    }
    
    void RecognitionTestModel::playTrial(const AudioSettings &trial) {
        if (noMoreTrials() || trialInProgress())
            return;
        
        preparePlayers(trial);
        startTrial();
    }
    
    bool RecognitionTestModel::noMoreTrials() {
        return targetList->empty();
    }
    
    bool RecognitionTestModel::trialInProgress() {
        return maskerPlayer->playing();
    }
    
    void RecognitionTestModel::preparePlayers(const AudioSettings &trial) {
        trySettingAudioDevices(trial);
        loadNextTarget();
    }
    
    void RecognitionTestModel::trySettingAudioDevices(const AudioSettings &trial) {
        auto device = trial.audioDevice;
        try {
            setAudioDevices(device);
        } catch (const InvalidAudioDevice &) {
            throw RequestFailure{
                "'" + device + "' is not a valid audio device."
            };
        }
    }
    
    void RecognitionTestModel::setAudioDevices(const std::string &device) {
        maskerPlayer->setAudioDevice(device);
        targetPlayer->setAudioDevice(device);
    }
    
    void RecognitionTestModel::loadNextTarget() {
        targetPlayer->loadFile(targetList->next());
        targetPlayer->setLevel_dB(signalLevel_dB());
    }
    
    void RecognitionTestModel::startTrial() {
        maskerPlayer->fadeIn();
    }
    
    double RecognitionTestModel::signalLevel_dB() {
        return
            -20 * std::log10(targetPlayer->rms()) +
            test.signalLevel_dB_SPL -
            test.fullScaleLevel_dB_SPL;
    }
    
    void RecognitionTestModel::initializeTest(const Test &p) {
        tryOpeningOutputFile(p);
        loadMaskerFile(p);
        loadStimulusList(p);
        prepareVideo(p);
        test = p;
    }
    
    void RecognitionTestModel::tryOpeningOutputFile(const Test &p) {
        try {
            outputFile->openNewFile(p);
        } catch (const OutputFile::OpenFailure &) {
            throw RequestFailure{"Unable to open output file."};
        }
    }
    
    void RecognitionTestModel::loadMaskerFile(const Test &p) {
        maskerPlayer->loadFile(p.maskerFilePath);
    }
    
    void RecognitionTestModel::loadStimulusList(const Test &p) {
        targetList->loadFromDirectory(p.stimulusListDirectory);
    }
    
    void RecognitionTestModel::prepareVideo(const Test &p) {
        if (auditoryOnly(p))
            targetPlayer->hideVideo();
        else
            targetPlayer->showVideo();
    }

    bool RecognitionTestModel::auditoryOnly(const Test &p) {
        return p.condition ==
            av_coordinated_response_measure::Condition::auditoryOnly;
    }

    bool RecognitionTestModel::testComplete() {
        return targetList->empty();
    }
    
    void RecognitionTestModel::fadeInComplete() {
        targetPlayer->play();
    }
    
    void RecognitionTestModel::playbackComplete() {
        maskerPlayer->fadeOut();
    }
    
    std::vector<std::string> RecognitionTestModel::audioDevices() {
        return maskerPlayer->outputAudioDeviceDescriptions();
    }
    
    void RecognitionTestModel::submitResponse(const SubjectResponse &response) {
        av_coordinated_response_measure::Trial trial{};
        trial.subjectColor = response.color;
        trial.subjectNumber = response.number;
        outputFile->writeTrial(trial);
    }
    
    void RecognitionTestModel::subscribe(Model::EventListener *listener) {
        listener_ = listener;
    }
    
    void RecognitionTestModel::fadeOutComplete() { 
        listener_->trialComplete();
    }
}

