#include "RecognitionTestModel.hpp"
#include <gsl/gsl>
#include <cmath>

namespace recognition_test {
    RecognitionTestModel::RecognitionTestModel(
        MaskerPlayer *maskerPlayer,
        StimulusList *list,
        StimulusPlayer *stimulusPlayer,
        OutputFile *outputFile
    ) :
        maskerPlayer{maskerPlayer},
        list{list},
        stimulusPlayer{stimulusPlayer},
        outputFile{outputFile}
    {
        maskerPlayer->subscribe(this);
        stimulusPlayer->subscribe(this);
    }
    
    void RecognitionTestModel::playTrial(const Trial &trial) {
        if (list->empty())
            return;
        if (maskerPlayer->playing())
            return;
        
        setAudioDevices(trial);
        stimulusPlayer->loadFile(list->next());
        stimulusPlayer->setLevel_dB(signalLevel_dB());
        maskerPlayer->fadeIn();
    }
    
    void RecognitionTestModel::setAudioDevices(const Trial &trial) {
        auto device = trial.audioDevice;
        try {
            maskerPlayer->setAudioDevice(device);
            stimulusPlayer->setAudioDevice(device);
        } catch (const InvalidAudioDevice &) {
            throw RequestFailure{
                "'" + device + "' is not a valid audio device."
            };
        }
    }
    
    double RecognitionTestModel::signalLevel_dB() {
        return
            -20 * std::log10(stimulusPlayer->rms()) +
            testParameters.signalLevel_dB_SPL -
            testParameters.fullScaleLevel_dB_SPL;
    }
    
    void RecognitionTestModel::initializeTest(const Test &p) {
        maskerPlayer->loadFile(p.maskerFilePath);
        list->loadFromDirectory(p.stimulusListDirectory);
        prepareVideo(p);
        outputFile->openNewFile(p);
        testParameters = p;
    }
    
    void RecognitionTestModel::prepareVideo(const Test &p) {
        if (auditoryOnly(p))
            stimulusPlayer->hideVideo();
        else
            stimulusPlayer->showVideo();
    }

    bool RecognitionTestModel::auditoryOnly(const Test &p) {
        return p.condition ==
            av_coordinated_response_measure::Condition::auditoryOnly;
    }

    bool RecognitionTestModel::testComplete() {
        return list->empty();
    }
    
    void RecognitionTestModel::fadeInComplete() {
        stimulusPlayer->play();
    }
    
    void RecognitionTestModel::playbackComplete() {
        maskerPlayer->fadeOut();
    }
    
    std::vector<std::string> RecognitionTestModel::audioDevices() {
        return maskerPlayer->audioDeviceDescriptions();
    }
    
    void RecognitionTestModel::submitResponse(const SubjectResponse &response) {
        av_coordinated_response_measure::Trial trial;
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

