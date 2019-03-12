#include "Presenter.h"

namespace presentation {
    Presenter::Presenter(Model *model, View *view) :
        testSetup{view->testSetup()},
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
    
    void Presenter::initializeTest() {
        try {
            initializeTest_();
        } catch (const std::runtime_error &e) {
            view->showErrorMessage(e.what());
        }
    }
    
    void Presenter::initializeTest_() {
        testSetup.submitRequest(model);
        testSetup.close();
        tester.run();
    }
    
    void Presenter::playTrial() {
        tester.playTrial();
    }
    
    void Presenter::confirmTestSetup() { 
        initializeTest();
    }
    

    Presenter::TestSetup::TestSetup(View::TestSetup *view) :
        view{view}
    {
    }
    
    void Presenter::TestSetup::run() {
        view->show();
    }
    
    void Presenter::TestSetup::submitRequest(Model *model) {
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
    
    void Presenter::TestSetup::close() {
        view->hide();
    }
    
    Presenter::Tester::Tester(Model *model, View::Tester *view) :
        model{model},
        view{view}
    {
        view->populateAudioDeviceMenu(model->audioDevices());
    }
    
    void Presenter::Tester::run() {
        view->show();
    }

    void Presenter::Tester::close() {
        view->hide();
    }
    
    void Presenter::Tester::playTrial() {
        Model::TrialParameters p;
        p.audioDevice = view->audioDevice();
        model->playTrial(p);
    }
}
