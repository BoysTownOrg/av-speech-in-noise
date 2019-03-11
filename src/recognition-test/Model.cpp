#include "Model.hpp"

namespace recognition_test {
    Model::Model(
        MaskerPlayer *maskerPlayer,
        StimulusList *list,
        SubjectView *view
    ) :
        maskerPlayer{maskerPlayer},
        list{list},
        view{view}
    {
        maskerPlayer->subscribe(this);
        view->stimulusPlayer()->subscribe(this);
    }
    
    void Model::playTrial(const TrialParameters &trial) {
        if (list->empty())
            return;
        
        for (int i = 0; i < maskerPlayer->deviceCount(); ++i)
            if (trial.audioDevice == maskerPlayer->deviceDescription(i)) {
                maskerPlayer->setDevice(i);
                view->stimulusPlayer()->setDevice(i);
                break;
            }
        view->stimulusPlayer()->loadFile(list->next());
        maskerPlayer->fadeIn();
    }

    void Model::initializeTest(const TestParameters &p) {
        maskerPlayer->loadFile(p.maskerFilePath);
        list->initialize(p.stimulusListDirectory);
    }

    bool Model::testComplete() {
        return list->empty();
    }
    
    void Model::fadeInComplete() {
        view->stimulusPlayer()->play();
    }
    
    void Model::playbackComplete() {
        maskerPlayer->fadeOut();
    }
}

