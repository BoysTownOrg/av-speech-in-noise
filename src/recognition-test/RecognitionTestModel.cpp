#include "RecognitionTestModel.hpp"
#include <gsl/gsl>
#include <cmath>

namespace recognition_test {
    RecognitionTestModel::RecognitionTestModel(
        TargetList *targetList,
        TargetPlayer *targetPlayer,
        MaskerPlayer *maskerPlayer,
        OutputFile *outputFile
    ) :
        maskerPlayer{maskerPlayer},
        targetList{targetList},
        targetPlayer{targetPlayer},
        outputFile{outputFile}
    {
        targetPlayer->subscribe(this);
        maskerPlayer->subscribe(this);
    }
    
    void RecognitionTestModel::playTrial(const AudioSettings &settings) {
        if (noMoreTrials() || trialInProgress())
            return;
        
        preparePlayers(settings);
        startTrial();
    }
    
    bool RecognitionTestModel::noMoreTrials() {
        return targetList->empty();
    }
    
    bool RecognitionTestModel::trialInProgress() {
        return maskerPlayer->playing();
    }
    
    void RecognitionTestModel::preparePlayers(const AudioSettings &p) {
        trySettingAudioDevices(p);
        loadNextTarget();
    }
    
    void RecognitionTestModel::trySettingAudioDevices(
        const AudioSettings &p
    ) {
        auto device = p.audioDevice;
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
    
    static double dB(double x) {
        return 20 * std::log10(x);
    }
    
    double RecognitionTestModel::signalLevel_dB() {
        return
            desiredSignalLevel_dB() -
            dB(targetPlayer->rms());
    }
    
    int RecognitionTestModel::desiredSignalLevel_dB() {
        return
            test.signalLevel_dB_SPL -
            test.fullScaleLevel_dB_SPL;
    }
    
    void RecognitionTestModel::initializeTest(const Test &p) {
        test = p;
        
        prepareOutputFile(p);
        prepareMasker(p);
        loadStimulusList(p);
        prepareVideo(p);
    }
    
    void RecognitionTestModel::prepareOutputFile(const Test &p) {
        outputFile->close();
        tryOpeningOutputFile(p);
        outputFile->writeTest(p);
        outputFile->writeTrialHeading();
    }
    
    void RecognitionTestModel::prepareMasker(const Test &p) {
        loadMaskerFile(p);
        maskerPlayer->setLevel_dB(maskerLevel_dB());
    }
    
    double RecognitionTestModel::maskerLevel_dB() {
        return
            desiredSignalLevel_dB() -
            dB(maskerPlayer->rms()) -
            test.startingSnr_dB;
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
        targetList->loadFromDirectory(p.targetListDirectory);
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
    
    void RecognitionTestModel::playCalibration(const Calibration &p) {
        auto device = p.audioDevice;
        try {
            targetPlayer->setAudioDevice(device);
        } catch (const InvalidAudioDevice &) {
            throw RequestFailure{
                "'" + device + "' is not a valid audio device."
            };
        }
    }
}

