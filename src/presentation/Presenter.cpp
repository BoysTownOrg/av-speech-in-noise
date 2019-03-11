#include "Presenter.h"

namespace presentation {
    Presenter::Presenter(Model *model, View *view) :
        model{model},
        view{view}
    {
        view->subscribe(this);
        view->setupView()->subscribe(this);
        view->testerView()->subscribe(this);
    }

    void Presenter::run() {
        view->eventLoop();
    }

    void Presenter::newTest() {
        view->setupView()->show();
    }

    void Presenter::confirmTestSetup() {
        Model::TestParameters p;
        try {
            p.maskerLevel_dB_SPL =
                std::stoi(view->setupView()->maskerLevel_dB_SPL());
            p.signalLevel_dB_SPL =
                std::stoi(view->setupView()->signalLevel_dB_SPL());
        }
        catch (const std::invalid_argument &) {
            return;
        }
        p.maskerFilePath = view->setupView()->maskerFilePath();
        p.stimulusListDirectory =
            view->setupView()->stimulusListDirectory();
        p.subjectId = view->setupView()->subjectId();
        p.testerId = view->setupView()->testerId();
        p.condition =
            view->setupView()->condition() == "Auditory-only"
            ? Model::TestParameters::Condition::auditoryOnly
            : Model::TestParameters::Condition::audioVisual;
        view->setupView()->hide();
        view->testerView()->show();
        model->initializeTest(p);
    }

    void Presenter::openTest() {
        view->testerView()->show();
    }

    void Presenter::playTrial() {
        model->playTrial();
    }

    void Presenter::closeTest() {
        if (view->showConfirmationDialog() == View::DialogResponse::cancel)
            return;
        view->testerView()->hide();
    }
}
