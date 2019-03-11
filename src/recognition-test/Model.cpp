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
    
    void Model::playTrial() {
        if (list->empty())
            return;
        view->stimulusPlayer()->loadFile(list->next());
        maskerPlayer->fadeIn();
    }

    void Model::initializeTest(TestParameters p) {
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

