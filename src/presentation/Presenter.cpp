#include "Presenter.h"

namespace presentation {
    Presenter::Presenter(Model *model, View *view) :
        testSetup{view->setupView()},
        tester{view->testerView()},
        model{model},
        view{view}
    {
        testSetup.setParent(this);
        tester.setParent(this);
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
    
    void Presenter::initializeTest(Model::TestParameters p) {
        view->setupView()->hide();
        view->testerView()->show();
        model->initializeTest(std::move(p));
    }
    
    void Presenter::playTrial() { 
        model->playTrial();
    }

    Presenter::TestSetup::TestSetup(View::TestSetupView *view) :
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
        parent->initializeTest(std::move(p));
    }
    
    void Presenter::TestSetup::setParent(presentation::Presenter *p) {
        parent = p;
    }

    Presenter::Tester::Tester(View::TesterView *view) :
        view{view}
    {
        view->subscribe(this);
    }
    
    void Presenter::Tester::setParent(presentation::Presenter *p) {
        parent = p;
    }

    void Presenter::Tester::playTrial() {
        parent->playTrial();
    }
}
