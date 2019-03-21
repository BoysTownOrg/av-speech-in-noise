#include "RecognitionTestModel.hpp"
#include <gsl/gsl>
#include <cmath>

namespace recognition_test {
    RecognitionTestModel::RecognitionTestModel(
        MaskerPlayer *maskerPlayer,
        StimulusList *list,
        StimulusPlayer *stimulusPlayer
    ) :
        maskerPlayer{maskerPlayer},
        list{list},
        stimulusPlayer{stimulusPlayer}
    {
        maskerPlayer->subscribe(this);
        stimulusPlayer->subscribe(this);
    }
    
    void RecognitionTestModel::playTrial(const Trial &trial) {
        if (list->empty())
            return;
        if (maskerPlayer->playing())
            return;
        
        try {
            maskerPlayer->setAudioDevice(trial.audioDevice);
            stimulusPlayer->setAudioDevice(trial.audioDevice);
        } catch (const InvalidAudioDevice &) {
            throw RequestFailure{
                "'" + trial.audioDevice + "' is not a valid audio device."
            };
        }
        stimulusPlayer->loadFile(list->next());
        stimulusPlayer->setLevel_dB(
            20 * std::log10(1.0/stimulusPlayer->rms()) -
            testParameters.signalLevel_dB_SPL +
            testParameters.fullScaleLevel_dB_SPL
        );
        maskerPlayer->fadeIn();
    }

    void RecognitionTestModel::initializeTest(const Test &p) {
        maskerPlayer->loadFile(p.maskerFilePath);
        list->loadFromDirectory(p.stimulusListDirectory);
        if (p.condition == Test::Condition::auditoryOnly)
            stimulusPlayer->hideVideo();
        else
            stimulusPlayer->showVideo();
        testParameters = p;
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
    
    void RecognitionTestModel::submitResponse(const SubjectResponse &) { 
        ;
    }
}

