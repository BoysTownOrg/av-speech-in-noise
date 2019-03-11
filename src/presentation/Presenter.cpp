#include "Presenter.h"

namespace presentation {
    Presenter::Presenter(Model *model, View *view) :
        testSetup{model, view->setupView()},
        tester{model, view->testerView()},
        model{model},
        view{view}
    {
        view->subscribe(this);
    }

    void Presenter::run() {
        view->eventLoop();
    }

    void Presenter::newTest() {
        view->setupView()->show();
    }

    void Presenter::openTest() {
        view->testerView()->show();
    }

    void Presenter::closeTest() {
        if (view->showConfirmationDialog() == View::DialogResponse::cancel)
            return;
        view->testerView()->hide();
    }

    Presenter::TestSetup::TestSetup(Model *model, View::TestSetupView *view) :
        model{model},
        view{view}
    {
        view->subscribe(this);
    }

    void Presenter::TestSetup::confirmTestSetup() {
        Model::TestParameters p;
        try {
            p.maskerLevel_dB_SPL =
                std::stoi(view->maskerLevel_dB_SPL());
            p.signalLevel_dB_SPL =
                std::stoi(view->signalLevel_dB_SPL());
        }
        catch (const std::invalid_argument &) {
            return;
        }
        p.maskerFilePath = view->maskerFilePath();
        p.stimulusListDirectory =
            view->stimulusListDirectory();
        p.subjectId = view->subjectId();
        p.testerId = view->testerId();
        p.condition =
            view->condition() == "Auditory-only"
            ? Model::TestParameters::Condition::auditoryOnly
            : Model::TestParameters::Condition::audioVisual;
        view->hide();
        model->initializeTest(p);
    }

    Presenter::Tester::Tester(presentation::Model *model, View::TesterView *view) :
        model{model},
        view{view}
    {
        view->subscribe(this);
    }

    void Presenter::Tester::playTrial() {
        model->playTrial();
    }
}
