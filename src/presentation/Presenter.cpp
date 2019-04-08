#include "Presenter.h"

namespace presentation {
    Presenter::Presenter(
        av_coordinated_response_measure::Model *model,
        View *view,
        TestSetup *testSetup,
        Tester *tester,
        Subject *subject
    ) :
        model{model},
        view{view},
        testSetup{testSetup},
        tester{tester},
        subject{subject}
    {
        model->subscribe(this);
        view->subscribe(this);
        view->populateAudioDeviceMenu(model->audioDevices());
        testSetup->becomeChild(this);
        subject->becomeChild(this);
    }

    void Presenter::run() {
        view->eventLoop();
    }

    void Presenter::newTest() {
        showTestSetup();
    }

    void Presenter::openTest() {
        showTesterView();
    }
    
    void Presenter::closeTest() {
        if (userCancels())
            return;
        
        hideTesterView();
    }

    bool Presenter::userCancels() {
        return view->showConfirmationDialog() == View::DialogResponse::cancel;
    }
    
    void Presenter::confirmTestSetup() {
        try {
            initializeTest_();
        } catch (const std::runtime_error &e) {
            showErrorMessage(e.what());
        }
    }
    
    void Presenter::showErrorMessage(std::string e) {
        view->showErrorMessage(std::move(e));
    }
    
    void Presenter::initializeTest_() {
        model->initializeTest(testSetup->testParameters());
        hideTestSetup();
        showTesterView();
        showNextTrialButton();
    }
    
    void Presenter::hideTestSetup() {
        testSetup->tuneOut();
    }
    
    void Presenter::showTesterView() {
        tester->listen();
    }
    
    void Presenter::showNextTrialButton() {
        view->subject()->showNextTrialButton();
    }
    
    void Presenter::playTrial() {
        hideNextTrialButton();
        av_coordinated_response_measure::Model::AudioSettings p;
        p.audioDevice = view->audioDevice();
        model->playTrial(p);
    }
    
    void Presenter::hideNextTrialButton() {
        view->subject()->hideNextTrialButton();
    }
    
    void Presenter::submitResponse() {
        model->submitResponse(subjectResponse());
        hideResponseButtons();
        proceedToNextTrial();
    }
    
    void Presenter::hideResponseButtons() {
        view->subject()->hideResponseButtons();
    }
    
    void Presenter::proceedToNextTrial() {
        if (model->testComplete()) {
            hideTesterView();
            showTestSetup();
        }
        else {
            showNextTrialButton();
        }
    }
    
    void Presenter::hideTesterView() {
        tester->tuneOut();
    }
    
    void Presenter::showTestSetup() {
        testSetup->listen();
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
    
    void Presenter::browseForTargetList() {
        auto result = view->browseForDirectory();
        if (!view->browseCancelled())
            testSetup->setStimulusList(result);
    }
    
    void Presenter::browseForMasker() {
        auto result = view->browseForOpeningFile();
        if (!view->browseCancelled())
            testSetup->setMasker(result);
    }
    
    void Presenter::trialComplete() {
        showResponseButtons();
    }
    
    void Presenter::showResponseButtons() {
        view->subject()->showResponseButtons();
    }
    
    void Presenter::playCalibration() {
        try {
            auto p = testSetup->calibrationParameters();
            p.audioDevice = view->audioDevice();
            model->playCalibration(p);
        } catch (const std::runtime_error &e) {
            showErrorMessage(e.what());
        }
    }
    

    Presenter::TestSetup::TestSetup(View::TestSetup *view) :
        view{view}
    {
        using av_coordinated_response_measure::Condition;
        view->populateConditionMenu({
            conditionName(Condition::audioVisual),
            conditionName(Condition::auditoryOnly)
        });
        view->subscribe(this);
    }
    
    void Presenter::TestSetup::listen() {
        view->show();
    }
    
    av_coordinated_response_measure::Model::Test
        Presenter::TestSetup::testParameters()
    {
        av_coordinated_response_measure::Model::Test p;
        p.startingSnr_dB =
            readInteger(view->startingSnr_dB(), "SNR");
        p.signalLevel_dB_SPL = readSignalLevel();
        p.maskerFilePath = view->maskerFilePath();
        p.targetListDirectory = view->targetListDirectory();
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
        parent->playCalibration();
    }
    
    av_coordinated_response_measure::Model::Calibration
        Presenter::TestSetup::calibrationParameters()
    {
        av_coordinated_response_measure::Model::Calibration p;
        p.filePath = view->calibrationFilePath();
        p.level_dB_SPL = readSignalLevel();
        return p;
    }
    
    int Presenter::TestSetup::readSignalLevel() {
        return readInteger(view->signalLevel_dB_SPL(), "signal level");
    }
    
    void Presenter::TestSetup::confirmTestSetup() { 
        parent->confirmTestSetup();
    }
    
    void Presenter::TestSetup::becomeChild(Presenter *p) {
        parent = p;
    }
    
    void Presenter::TestSetup::setMasker(std::string s) {
        view->setMasker(std::move(s));
    }
    
    void Presenter::TestSetup::setStimulusList(std::string s) {
        view->setStimulusList(std::move(s));
    }
    
    void Presenter::TestSetup::browseForTargetList() { 
        parent->browseForTargetList();
    }
    
    void Presenter::TestSetup::browseForMasker() { 
        parent->browseForMasker();
    }
    
    
    Presenter::Tester::Tester(View::Tester *view) :
        view{view}
    {
    }
    
    void Presenter::Tester::listen() {
        view->show();
    }

    void Presenter::Tester::tuneOut() {
        view->hide();
    }

    Presenter::Subject::Subject(View::Subject *view) :
        view{view}
    {
        view->subscribe(this);
    }

    void Presenter::Subject::playTrial() {
        parent->playTrial();
    }

    void Presenter::Subject::submitResponse() {
        parent->submitResponse();
    }
    
    void Presenter::Subject::becomeChild(Presenter *p) {
        parent = p;
    }
}
