#include "Model.hpp"
#include <gsl/gsl>
#include <cmath>

namespace recognition_test {
    Model::Model(
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
    
    void Model::playTrial(const Trial &trial) {
        if (list->empty())
            return;
        if (maskerPlayer->playing())
            return;
        
        int deviceIndex = findDeviceIndex(trial);
        maskerPlayer->setDevice(deviceIndex);
        stimulusPlayer->setDevice(deviceIndex);
        stimulusPlayer->loadFile(list->next());
        stimulusPlayer->setLevel_dB(
            20 * std::log10(1.0/stimulusPlayer->rms()) -
            testParameters.signalLevel_dB_SPL +
            testParameters.fullScaleLevel_dB_SPL
        );
        maskerPlayer->fadeIn();
    }
    
    int Model::findDeviceIndex(const Trial &trial) {
        auto devices_ = audioDevices();
        auto deviceIndex = gsl::narrow<int>(
            std::find(
                devices_.begin(),
                devices_.end(),
                trial.audioDevice
            ) - devices_.begin()
        );
        if (deviceIndex == maskerPlayer->deviceCount())
            throw RequestFailure{
                "'" + trial.audioDevice + "' is not a valid audio device."
            };
        return deviceIndex;
    }

    void Model::initializeTest(const Test &p) {
        maskerPlayer->loadFile(p.maskerFilePath);
        list->loadFromDirectory(p.stimulusListDirectory);
        if (p.condition == Test::Condition::auditoryOnly)
            stimulusPlayer->hideVideo();
        else
            stimulusPlayer->showVideo();
        testParameters = p;
    }

    bool Model::testComplete() {
        return list->empty();
    }
    
    void Model::fadeInComplete() {
        stimulusPlayer->play();
    }
    
    void Model::playbackComplete() {
        maskerPlayer->fadeOut();
    }
    
    std::vector<std::string> Model::audioDevices() {
        std::vector<std::string> descriptions{};
        for (int i = 0; i < maskerPlayer->deviceCount(); ++i)
            descriptions.push_back(maskerPlayer->deviceDescription(i));
        return descriptions;
    }
    
    void Model::submitResponse(const SubjectResponse &) { 
        ;
    }
}

