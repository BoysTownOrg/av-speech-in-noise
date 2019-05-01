#include "Presenter.h"
#include <sstream>

namespace av_speech_in_noise {
    int Presenter::fullScaleLevel_dB_SPL = 119;
    
    TrackingRule Presenter::targetLevelRule = {
        { 2, 4, 1, 1 },
        { 6, 2, 1, 1 }
    };
    
    Presenter::Presenter(
        Model *model,
        View *view,
        TestSetup *testSetup,
        Subject *subject,
        Experimenter *experimenter
    ) :
        model{model},
        view{view},
        testSetup{testSetup},
        subject{subject},
        experimenter{experimenter}
    {
        model->subscribe(this);
        testSetup->becomeChild(this);
        subject->becomeChild(this);
        experimenter->becomeChild(this);
        view->populateAudioDeviceMenu(model->audioDevices());
    }

    void Presenter::run() {
        view->eventLoop();
    }
    
    void Presenter::confirmTestSetup() {
        try {
            initializeTest_();
        } catch (const std::runtime_error &e) {
            showErrorMessage(e.what());
        }
    }
    
    void Presenter::initializeTest_() {
        model->initializeTest(testSetup->testParameters());
        hideTestSetup();
        experimenter->show();
        if (testSetup->adaptiveClosedSet())
            subject->show();
        showNextTrialButton();
    }
    
    void Presenter::hideTestSetup() {
        testSetup->hide();
    }
    
    void Presenter::showNextTrialButton() {
        subject->showNextTrialButton();
        experimenter->showNextTrialButton();
    }
    
    void Presenter::showErrorMessage(std::string e) {
        view->showErrorMessage(std::move(e));
    }
    
    void Presenter::playTrial() {
        hideNextTrialButton();
        AudioSettings p;
        p.audioDevice = view->audioDevice();
        model->playTrial(p);
    }
    
    void Presenter::hideNextTrialButton() {
        subject->hideNextTrialButton();
    }
    
    void Presenter::trialComplete() {
        showResponseButtons();
    }
    
    void Presenter::showResponseButtons() {
        subject->showResponseButtons();
    }
    
    void Presenter::submitResponse() {
        model->submitResponse(subject->subjectResponse());
        hideResponseButtons();
        proceedToNextTrial();
    }
    
    void Presenter::hideResponseButtons() {
        subject->hideResponseButtons();
    }
    
    void Presenter::proceedToNextTrial() {
        if (model->testComplete())
            showTestSetup();
        else
            showNextTrialButton();
    }
    
    void Presenter::showTestSetup() {
        testSetup->show();
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
    
    void Presenter::browseForTargetList() {
        applyIfBrowseNotCancelled(
            view->browseForDirectory(),
            &TestSetup::setStimulusList
        );
    }

    void Presenter::applyIfBrowseNotCancelled(
        std::string s,
        void(TestSetup::*f)(std::string)
    ) {
        if (!view->browseCancelled())
            (testSetup->*f)(std::move(s));
    }
    
    void Presenter::browseForMasker() {
        applyIfBrowseNotCancelled(
            view->browseForOpeningFile(),
            &TestSetup::setMasker
        );
    }
    
    void Presenter::browseForCalibration() {
        applyIfBrowseNotCancelled(
            view->browseForOpeningFile(),
            &TestSetup::setCalibrationFilePath
        );
    }
    

    Presenter::TestSetup::TestSetup(View::TestSetup *view) : view{view} {
        view->populateConditionMenu({
            conditionName(Condition::audioVisual),
            conditionName(Condition::auditoryOnly)
        });
        view->subscribe(this);
    }
    
    void Presenter::TestSetup::show() {
        view->show();
    }
    
    Test Presenter::TestSetup::testParameters() {
        Test p;
        p.startingSnr_dB = readInteger(view->startingSnr_dB(), "SNR");
        p.maskerLevel_dB_SPL = readMaskerLevel();
        p.maskerFilePath = view->maskerFilePath();
        p.targetListDirectory = view->targetListDirectory();
        p.subjectId = view->subjectId();
        p.testerId = view->testerId();
        p.session = view->session();
        p.condition = readCondition();
        p.fullScaleLevel_dB_SPL = fullScaleLevel_dB_SPL;
        p.targetLevelRule = &targetLevelRule;
        return p;
    }
    
    Condition Presenter::TestSetup::readCondition() {
        return auditoryOnly()
            ? Condition::auditoryOnly
            : Condition::audioVisual;
    }
    
    int Presenter::TestSetup::readInteger(std::string x, std::string identifier) {
        try {
            return std::stoi(x);
        }
        catch (const std::invalid_argument &) {
            std::stringstream stream;
            stream << '\'' << std::move(x) << '\'';
            stream << " is not a valid ";
            stream << std::move(identifier);
            stream << '.';
            throw BadInput{stream.str()};
        }
    }
    
    bool Presenter::TestSetup::auditoryOnly() {
        return view->condition() == conditionName(Condition::auditoryOnly);
    }
    
    void Presenter::TestSetup::hide() {
        view->hide();
    }
    
    void Presenter::TestSetup::playCalibration() {
        parent->playCalibration();
    }
    
    Calibration Presenter::TestSetup::calibrationParameters() {
        Calibration p;
        p.filePath = view->calibrationFilePath();
        p.level_dB_SPL = readCalibrationLevel();
        p.fullScaleLevel_dB_SPL = fullScaleLevel_dB_SPL;
        p.condition = readCondition();
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
    
    bool Presenter::TestSetup::adaptiveClosedSet() { 
        return view->method() == methodName(Method::adaptiveClosedSet);
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
    
    coordinate_response_measure::SubjectResponse Presenter::Subject::subjectResponse() {
        coordinate_response_measure::SubjectResponse p;
        p.color = colorResponse();
        p.number = std::stoi(view->numberResponse());
        return p;
    }
    
    coordinate_response_measure::Color Presenter::Subject::colorResponse() {
        if (view->greenResponse())
            return coordinate_response_measure::Color::green;
        else if (view->blueResponse())
            return coordinate_response_measure::Color::blue;
        else if (view->whiteResponse())
            return coordinate_response_measure::Color::white;
        else
            return coordinate_response_measure::Color::red;
    }
    
    void Presenter::Subject::show() { 
        view->show();
    }
    

    Presenter::Experimenter::Experimenter(View::Experimenter *view) :
        view{view}
    {
        view->subscribe(this);
    }
    
    void Presenter::Experimenter::showNextTrialButton() {
        view->showNextTrialButton();
    }
    
    void Presenter::Experimenter::show() { 
        view->show();
    }
    
    void Presenter::Experimenter::playTrial() { 
        view->hideNextTrialButton();
        parent->playTrial();
    }
    
    void Presenter::Experimenter::becomeChild(Presenter *p) {
        parent = p;
    }
}
