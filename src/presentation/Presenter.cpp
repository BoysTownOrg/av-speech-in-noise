#include "Presenter.h"

namespace presentation {
    int Presenter::fullScaleLevel_dB_SPL = 119;
    
    av_coordinated_response_measure::Rule
        Presenter::targetLevelRule = {
            { 2, 4, 1, 1 },
            { 6, 2, 1, 1 }
        };
    
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
        testSetup->becomeChild(this);
        subject->becomeChild(this);
        view->populateAudioDeviceMenu(model->audioDevices());
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
        testSetup->hide();
    }
    
    void Presenter::showTesterView() {
        tester->show();
    }
    
    void Presenter::playTrial() {
        hideNextTrialButton();
        av_coordinated_response_measure::Model::AudioSettings p;
        p.audioDevice = view->audioDevice();
        model->playTrial(p);
    }
    
    void Presenter::showNextTrialButton() {
        subject->showNextTrialButton();
    }
    
    void Presenter::hideNextTrialButton() {
        subject->hideNextTrialButton();
    }
    
    void Presenter::hideResponseButtons() {
        subject->hideResponseButtons();
    }
    
    void Presenter::showResponseButtons() {
        subject->showResponseButtons();
    }
    
    void Presenter::submitResponse() {
        model->submitResponse(subject->subjectResponse());
        hideResponseButtons();
        proceedToNextTrial();
    }
    
    void Presenter::proceedToNextTrial() {
        if (model->testComplete()) {
            hideTesterView();
            showTestSetup();
        }
        else
            showNextTrialButton();
    }
    
    void Presenter::hideTesterView() {
        tester->hide();
    }
    
    void Presenter::showTestSetup() {
        testSetup->show();
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
    
    void Presenter::browseForCalibration() {
        auto result = view->browseForOpeningFile();
        if (!view->browseCancelled())
            testSetup->setCalibrationFilePath(result);
    }
    
    void Presenter::trialComplete() {
        showResponseButtons();
    }
    
    void Presenter::playCalibration() {
        try {
            playCalibration_();
        } catch (const std::runtime_error &e) {
            showErrorMessage(e.what());
        }
    }
    
    void Presenter::playCalibration_() {
        auto p = testSetup->calibrationParameters();
        p.audioDevice = view->audioDevice();
        model->playCalibration(p);
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
    
    void Presenter::TestSetup::show() {
        view->show();
    }
    
    av_coordinated_response_measure::Test
        Presenter::TestSetup::testParameters()
    {
        av_coordinated_response_measure::Test p;
        p.startingSnr_dB =
            readInteger(view->startingSnr_dB(), "SNR");
        p.maskerLevel_dB_SPL = readMaskerLevel();
        p.maskerFilePath = view->maskerFilePath();
        p.targetListDirectory = view->targetListDirectory();
        p.subjectId = view->subjectId();
        p.testerId = view->testerId();
        p.session = view->session();
        p.condition = auditoryOnly()
            ? av_coordinated_response_measure::Condition::auditoryOnly
            : av_coordinated_response_measure::Condition::audioVisual;
        p.fullScaleLevel_dB_SPL = fullScaleLevel_dB_SPL;
        p.targetLevelRule = &targetLevelRule;
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
    
    void Presenter::TestSetup::hide() {
        view->hide();
    }
    
    void Presenter::TestSetup::playCalibration() {
        parent->playCalibration();
    }
    
    av_coordinated_response_measure::Calibration
        Presenter::TestSetup::calibrationParameters()
    {
        av_coordinated_response_measure::Calibration p;
        p.filePath = view->calibrationFilePath();
        p.level_dB_SPL = readCalibrationLevel();
        p.fullScaleLevel_dB_SPL = fullScaleLevel_dB_SPL;
        return p;
    }
    
    int Presenter::TestSetup::readCalibrationLevel() {
        return readInteger(view->calibrationLevel_dB_SPL(), "calibration level");
    }
    
    int Presenter::TestSetup::readMaskerLevel() {
        return readInteger(view->maskerLevel_dB_SPL(), "masker level");
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
        view->setTargetListDirectory(std::move(s));
    }
    
    void Presenter::TestSetup::browseForTargetList() { 
        parent->browseForTargetList();
    }
    
    void Presenter::TestSetup::browseForMasker() { 
        parent->browseForMasker();
    }
    
    void Presenter::TestSetup::browseForCalibration() {
        parent->browseForCalibration();
    }
    
    void Presenter::TestSetup::setCalibrationFilePath(std::string s) {
        view->setCalibrationFilePath(std::move(s));
    }
    
    
    Presenter::Tester::Tester(View::Tester *view) :
        view{view}
    {
    }
    
    void Presenter::Tester::show() {
        view->show();
    }

    void Presenter::Tester::hide() {
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
    
    void Presenter::Subject::showNextTrialButton() {
        view->showNextTrialButton();
    }
    
    void Presenter::Subject::hideNextTrialButton() {
        view->hideNextTrialButton();
    }
    
    void Presenter::Subject::hideResponseButtons() {
        view->hideResponseButtons();
    }
    
    void Presenter::Subject::showResponseButtons() {
        view->showResponseButtons();
    }
    
    av_coordinated_response_measure::SubjectResponse
        Presenter::Subject::subjectResponse()
    {
        av_coordinated_response_measure::SubjectResponse p;
        p.color = colorResponse();
        p.number = std::stoi(view->numberResponse());
        return p;
    }
    
    av_coordinated_response_measure::Color Presenter::Subject::colorResponse() {
        av_coordinated_response_measure::Color color;
        if (view->greenResponse())
            color = av_coordinated_response_measure::Color::green;
        else if (view->blueResponse())
            color = av_coordinated_response_measure::Color::blue;
        else if (view->grayResponse())
            color = av_coordinated_response_measure::Color::gray;
        else
            color = av_coordinated_response_measure::Color::red;
        return color;
    }
}
