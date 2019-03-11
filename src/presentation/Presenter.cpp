#include "Presenter.h"

namespace presentation {
    Presenter::Presenter(Model *model, View *view) :
        testSetup{view->testSetup()},
        tester{view->tester()},
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
        testSetup.run();
    }

    void Presenter::openTest() {
        tester.run();
    }

    void Presenter::closeTest() {
        if (view->showConfirmationDialog() == View::DialogResponse::cancel)
            return;
        tester.close();
    }
    
    void Presenter::initializeTest(Model::TestParameters p) {
        tester.run();
        try {
            model->initializeTest(std::move(p));
            testSetup.close();
        } catch (const Model::RequestFailure &e) {
            view->showErrorMessage(e.what());
        }
    }
    
    void Presenter::playTrial() { 
        model->playTrial();
    }

    Presenter::TestSetup::TestSetup(View::TestSetup *view) :
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
    
    void Presenter::TestSetup::run() { 
        view->show();
    }
    
    void Presenter::TestSetup::close() { 
        view->hide();
    }
    
    Presenter::Tester::Tester(View::Tester *view) :
        view{view}
    {
        view->subscribe(this);
    }
    
    void Presenter::Tester::run() {
        view->show();
    }

    void Presenter::Tester::close() {
        view->hide();
    }
    
    void Presenter::Tester::setParent(presentation::Presenter *p) {
        parent = p;
    }

    void Presenter::Tester::playTrial() {
        parent->playTrial();
    }
}
