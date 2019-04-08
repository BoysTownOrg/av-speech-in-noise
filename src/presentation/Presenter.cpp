#include "Presenter.h"

namespace presentation {
    Presenter::Presenter(
        av_coordinated_response_measure::Model *model,
        View *view
    ) :
        testSetup{model, view->testSetup()},
        tester{model, view->tester()},
        model{model},
        view{view}
    {
        model->subscribe(this);
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
        if (userCancels())
            return;
        
        tester.tuneOut();
    }

    bool Presenter::userCancels() {
        return view->showConfirmationDialog() == View::DialogResponse::cancel;
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
        view->subject()->showNextTrialButton();
    }
    
    void Presenter::playTrial() {
        view->subject()->hideNextTrialButton();
        tester.playTrial();
    }
    
    void Presenter::submitResponse() {
        model->submitResponse(subjectResponse());
        view->subject()->hideResponseButtons();
        proceedToNextTrial();
    }
    
    void Presenter::proceedToNextTrial() {
        if (model->testComplete()) {
            tester.tuneOut();
            testSetup.listen();
        }
        else {
            view->subject()->showNextTrialButton();
        }
    }
    
    av_coordinated_response_measure::Model::SubjectResponse
        Presenter::subjectResponse()
    {
        av_coordinated_response_measure::Model::SubjectResponse p;
        p.color = colorResponse();
        p.number = std::stoi(view->subject()->numberResponse());
        return p;
    }
    
    av_coordinated_response_measure::Color Presenter::colorResponse() {
        av_coordinated_response_measure::Color color;
        if (view->subject()->greenResponse())
            color = av_coordinated_response_measure::Color::green;
        else if (view->subject()->blueResponse())
            color = av_coordinated_response_measure::Color::blue;
        else if (view->subject()->grayResponse())
            color = av_coordinated_response_measure::Color::gray;
        else
            color = av_coordinated_response_measure::Color::red;
        return color;
    }
    
    void Presenter::browseForStimulusList() {
        auto result = view->browseForDirectory();
        if (!view->browseCancelled())
            view->testSetup()->setStimulusList(result);
    }
    
    void Presenter::browseForMasker() {
        auto result = view->browseForOpeningFile();
        if (!view->browseCancelled())
            view->testSetup()->setMasker(result);
    }
    
    void Presenter::trialComplete() { 
        view->subject()->showResponseButtons();
    }
    
    void Presenter::playCalibration() {
        try {
            testSetup.playCalibration();
        } catch (const std::runtime_error &e) {
            view->showErrorMessage(e.what());
        }
    }
    

    Presenter::TestSetup::TestSetup(
        av_coordinated_response_measure::Model *model,
        View::TestSetup *view
    ) :
        model{model},
        view{view}
    {
        using av_coordinated_response_measure::Condition;
        view->populateConditionMenu({
            conditionName(Condition::audioVisual),
            conditionName(Condition::auditoryOnly)
        });
    }
    
    void Presenter::TestSetup::listen() {
        view->show();
    }
    
    void Presenter::TestSetup::initializeTest() {
        model->initializeTest(testParameters());
    }
    
    av_coordinated_response_measure::Model::Test
        Presenter::TestSetup::testParameters()
    {
        av_coordinated_response_measure::Model::Test p;
        p.startingSnr_dB =
            readInteger(view->startingSnr_dB(), "SNR");
        p.signalLevel_dB_SPL =
            readInteger(view->signalLevel_dB_SPL(), "signal level");
        p.maskerFilePath = view->maskerFilePath();
        p.targetListDirectory = view->stimulusListDirectory();
        p.subjectId = view->subjectId();
        p.testerId = view->testerId();
        p.session = view->session();
        p.condition = auditoryOnly()
            ? av_coordinated_response_measure::Condition::auditoryOnly
            : av_coordinated_response_measure::Condition::audioVisual;
        p.fullScaleLevel_dB_SPL = 119;
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
            av_coordinated_response_measure::Condition::auditoryOnly
        );
    }
    
    void Presenter::TestSetup::tuneOut() {
        view->hide();
    }
    
    void Presenter::TestSetup::playCalibration() { 
        av_coordinated_response_measure::Model::Calibration p;
        p.filePath = view->calibrationFilePath();
        p.level_dB_SPL =
            readInteger(view->signalLevel_dB_SPL(), "signal level");
        model->playCalibration(p);
    }
    
    
    Presenter::Tester::Tester(
        av_coordinated_response_measure::Model *model,
        View::Tester *view
    ) :
        model{model},
        view{view}
    {
        view->populateAudioDeviceMenu(model->audioDevices());
    }
    
    void Presenter::Tester::listen() {
        view->show();
    }
    
    void Presenter::Tester::playTrial() {
        av_coordinated_response_measure::Model::AudioSettings p;
        p.audioDevice = view->audioDevice();
        model->playTrial(p);
    }

    void Presenter::Tester::tuneOut() {
        view->hide();
    }
}
