#include "Presenter.h"

namespace presentation {
    std::string conditionName(Model::Test::Condition c) {
        switch (c) {
        case Model::Test::Condition::auditoryOnly:
            return "auditory-only";
        case Model::Test::Condition::audioVisual:
            return "audio-visual";
        }
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
        tester.listen();
    }
    
    void Presenter::playTrial() {
        tester.playTrial();
    }
    
    void Presenter::submitResponse() { 
        Model::SubjectResponse p;
        if (view->subject()->greenResponse())
            p.color = Model::SubjectResponse::Color::green;
        p.number = std::stoi(view->subject()->numberResponse());
        model->submitResponse(p);
    }

    Presenter::TestSetup::TestSetup(Model *model, View::TestSetup *view) :
        model{model},
        view{view}
    {
        view->populateConditionMenu({
            conditionName(Model::Test::Condition::auditoryOnly),
            conditionName(Model::Test::Condition::audioVisual)
        });
    }
    
    void Presenter::TestSetup::listen() {
        view->show();
    }
    
    void Presenter::TestSetup::initializeTest() {
        model->initializeTest(testParameters());
    }
    
    Model::Test Presenter::TestSetup::testParameters() {
        Model::Test p;
        p.startingSnr_dB =
            readInteger(view->startingSnr_dB(), "SNR");
        p.signalLevel_dB_SPL =
            readInteger(view->signalLevel_dB_SPL(), "signal level");
        p.maskerFilePath = view->maskerFilePath();
        p.stimulusListDirectory = view->stimulusListDirectory();
        p.subjectId = view->subjectId();
        p.testerId = view->testerId();
        p.condition = auditoryOnly()
            ? Model::Test::Condition::auditoryOnly
            : Model::Test::Condition::audioVisual;
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
    
    bool Presenter::TestSetup::auditoryOnly() {
        return view->condition() == conditionName(
            Model::Test::Condition::auditoryOnly
        );
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
        Model::Trial p;
        p.audioDevice = view->audioDevice();
        model->playTrial(p);
    }

    void Presenter::Tester::tuneOut() {
        view->hide();
    }
}
