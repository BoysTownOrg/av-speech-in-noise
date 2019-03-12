#include "Presenter.h"

namespace presentation {
    std::string conditionName(Model::TestParameters::Condition c) {
        if (c == Model::TestParameters::Condition::auditoryOnly)
            return "auditory-only";
        else
            return "audio-visual";
    }
    
    Presenter::Presenter(Model *model, View *view) :
        testSetup{model, view->testSetup()},
        tester{model, view->tester()},
        model{model},
        view{view}
    {
        view->subscribe(this);
    }

    void Presenter::run() {
        view->eventLoop();
    }

    void Presenter::newTest() {
        testSetup.listen();
        view->showConfirmTestSetupButton();
    }

    void Presenter::openTest() {
        tester.listen();
    }

    void Presenter::closeTest() {
        if (view->showConfirmationDialog() == View::DialogResponse::cancel)
            return;
        tester.tuneOut();
    }
    
    void Presenter::confirmTestSetup() {
        try {
            initializeTest_();
        } catch (const std::runtime_error &e) {
            view->showErrorMessage(e.what());
        }
    }
    
    void Presenter::initializeTest_() {
        testSetup.initializeTest();
        testSetup.tuneOut();
        view->hideConfirmTestSetupButton();
        tester.listen();
    }
    
    void Presenter::playTrial() {
        tester.playTrial();
    }
    
    void Presenter::submitResponse() { 
        Model::ResponseParameters p;
        if (view->subject()->greenResponse())
            p.color = Model::ResponseParameters::Color::green;
        p.number = view->subject()->numberResponse();
        model->submitResponse(p);
    }
    
    

    Presenter::TestSetup::TestSetup(Model *model, View::TestSetup *view) :
        model{model},
        view{view}
    {
        view->populateConditionMenu({
            conditionName(Model::TestParameters::Condition::auditoryOnly),
            conditionName(Model::TestParameters::Condition::audioVisual)
        });
    }
    
    void Presenter::TestSetup::listen() {
        view->show();
    }
    
    void Presenter::TestSetup::initializeTest() {
        model->initializeTest(testParameters());
    }
    
    Model::TestParameters Presenter::TestSetup::testParameters() {
        Model::TestParameters p;
        p.maskerLevel_dB_SPL =
            readInteger(view->maskerLevel_dB_SPL(), "masker level");
        p.signalLevel_dB_SPL =
            readInteger(view->signalLevel_dB_SPL(), "signal level");
        p.maskerFilePath = view->maskerFilePath();
        p.stimulusListDirectory = view->stimulusListDirectory();
        p.subjectId = view->subjectId();
        p.testerId = view->testerId();
        p.condition =
            view->condition() == "Auditory-only"
            ? Model::TestParameters::Condition::auditoryOnly
            : Model::TestParameters::Condition::audioVisual;
        return p;
    }
    
    int Presenter::TestSetup::readInteger(std::string x, std::string identifier) {
        try {
            return std::stoi(x);
        }
        catch (const std::invalid_argument &) {
            throw BadInput{"'" + x + "' is not a valid " + identifier + "."};
        }
    }
    
    void Presenter::TestSetup::tuneOut() {
        view->hide();
    }
    
    
    Presenter::Tester::Tester(Model *model, View::Tester *view) :
        model{model},
        view{view}
    {
        view->populateAudioDeviceMenu(model->audioDevices());
    }
    
    void Presenter::Tester::listen() {
        view->show();
    }
    
    void Presenter::Tester::playTrial() {
        Model::TrialParameters p;
        p.audioDevice = view->audioDevice();
        model->playTrial(p);
    }

    void Presenter::Tester::tuneOut() {
        view->hide();
    }
}
