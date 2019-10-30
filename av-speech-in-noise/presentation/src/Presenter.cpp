#include "Presenter.hpp"
#include <string>
#include <sstream>

namespace av_speech_in_noise {
namespace {
class BadInput : public std::runtime_error {
  public:
    explicit BadInput(const std::string &s) : std::runtime_error{s} {}
};
}

int Presenter::fullScaleLevel_dB_SPL = 119;
int Presenter::ceilingSnr_dB = 20;
int Presenter::floorSnr_dB = -40;
int Presenter::trackBumpLimit = 10;

static FixedLevelTest fixedLevelTest(Presenter::TestSetup *testSetup) {
    return testSetup->fixedLevelTest();
}

static void displayTrialNumber(
    Presenter::Experimenter *experimenter, Model *model) {
    experimenter->display("Trial " + std::to_string(model->trialNumber()));
}

Presenter::Presenter(Model *model, View *view, TestSetup *testSetup,
    Subject *subject, Experimenter *experimenter, Testing *testing)
    : fixedLevelOpenSetTrialCompletionHandler{testing},
      fixedLevelClosedSetTrialCompletionHandler{subject},
      adaptiveOpenSetTrialCompletionHandler{testing},
      adaptiveClosedSetTrialCompletionHandler{subject}, model{model},
      view{view}, testSetup{testSetup}, subject{subject},
      experimenter{experimenter}, testing{testing},
      trialCompletionHandler_{&adaptiveClosedSetTrialCompletionHandler} {
    model->subscribe(this);
    testSetup->becomeChild(this);
    subject->becomeChild(this);
    experimenter->becomeChild(this);
    testing->becomeChild(this);
    view->populateAudioDeviceMenu(model->audioDevices());
}

void Presenter::run() { view->eventLoop(); }

void Presenter::confirmTestSetup() {
    try {
        confirmTestSetup_();
    } catch (const std::runtime_error &e) {
        showErrorMessage(e.what());
    }
}

void Presenter::confirmTestSetup_() {
    initializeTest();
    if (!testComplete())
        switchToTestView();
    trialCompletionHandler_ = trialCompletionHandler();
}

void Presenter::initializeTest() {
    if (adaptiveTest())
        model->initializeTest(testSetup->adaptiveTest());
    else if (finiteTargets())
        model->initializeTestWithFiniteTargets(fixedLevelTest(testSetup));
    else
        model->initializeTest(fixedLevelTest(testSetup));
}

bool Presenter::adaptiveTest() {
    return adaptiveClosedSet() || adaptiveOpenSet();
}

bool Presenter::adaptiveClosedSet() { return testSetup->adaptiveClosedSet(); }

bool Presenter::adaptiveOpenSet() { return testSetup->adaptiveOpenSet(); }

bool Presenter::finiteTargets() { return testSetup->finiteTargets(); }

bool Presenter::testComplete() { return model->testComplete(); }

void Presenter::switchToTestView() {
    hideTestSetup();
    showTestView();
}

void Presenter::hideTestSetup() { testSetup->hide(); }

void Presenter::showTestView() {
    experimenter->show();
    displayTrialNumber(experimenter, model);
    if (closedSet())
        subject->show();
    else
        testing->show();
}

bool Presenter::closedSet() {
    return adaptiveClosedSet() || fixedLevelClosedSet();
}

bool Presenter::fixedLevelClosedSet() {
    return testSetup->fixedLevelClosedSet();
}

auto Presenter::trialCompletionHandler() -> TrialCompletionHandler * {
    if (adaptiveClosedSet())
        return &adaptiveClosedSetTrialCompletionHandler;
    if (adaptiveOpenSet())
        return &adaptiveOpenSetTrialCompletionHandler;
    if (fixedLevelClosedSet())
        return &fixedLevelClosedSetTrialCompletionHandler;
    return &fixedLevelOpenSetTrialCompletionHandler;
}

void Presenter::showErrorMessage(std::string e) {
    view->showErrorMessage(std::move(e));
}

void Presenter::playTrial() {
    AudioSettings p;
    p.audioDevice = view->audioDevice();
    model->playTrial(p);
    experimenter->hideExitTestButton();
}

void Presenter::trialComplete() {
    trialCompletionHandler_->showResponseView();
    experimenter->showExitTestButton();
}

void Presenter::submitSubjectResponse() {
    submitSubjectResponse_();
    displayTrialNumber(experimenter, model);
    if (testComplete())
        switchToSetupView();
    else
        playTrial();
}

void Presenter::submitSubjectResponse_() {
    model->submitResponse(subject->subjectResponse());
}

void Presenter::submitExperimenterResponse() {
    proceedToNextTrialAfter(&Presenter::submitExperimenterResponse_);
}

void Presenter::submitExperimenterResponse_() {
    model->submitResponse(testing->openSetResponse());
}

void Presenter::submitPassedTrial() {
    proceedToNextTrialAfter(&Presenter::submitPassedTrial_);
}

void Presenter::submitPassedTrial_() { model->submitCorrectResponse(); }

void Presenter::submitFailedTrial() {
    proceedToNextTrialAfter(&Presenter::submitFailedTrial_);
}

void Presenter::submitFailedTrial_() { model->submitIncorrectResponse(); }

void Presenter::proceedToNextTrialAfter(void (Presenter::*f)()) {
    (this->*f)();
    proceedToNextTrial();
}

void Presenter::proceedToNextTrial() {
    displayTrialNumber(experimenter, model);
    if (testComplete())
        switchToSetupView();
}

void Presenter::exitTest() { switchToSetupView(); }

void Presenter::switchToSetupView() {
    showTestSetup();
    hideTestView();
}

void Presenter::showTestSetup() { testSetup->show(); }

void Presenter::hideTestView() {
    testing->hide();
    experimenter->hide();
    subject->hide();
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
    p.audioSettings.audioDevice = view->audioDevice();
    model->playCalibration(p);
}

void Presenter::browseForTargetList() {
    applyIfBrowseNotCancelled(
        view->browseForDirectory(), &TestSetup::setStimulusList);
}

void Presenter::applyIfBrowseNotCancelled(
    std::string s, void (TestSetup::*f)(std::string)) {
    if (!view->browseCancelled())
        (testSetup->*f)(std::move(s));
}

void Presenter::browseForMasker() {
    applyIfBrowseNotCancelled(
        view->browseForOpeningFile(), &TestSetup::setMasker);
}

void Presenter::browseForCalibration() {
    applyIfBrowseNotCancelled(
        view->browseForOpeningFile(), &TestSetup::setCalibrationFilePath);
}

void Presenter::browseForTrackSettingsFile() {
    applyIfBrowseNotCancelled(
        view->browseForOpeningFile(), &TestSetup::setTrackSettingsFile);
}

Presenter::TestSetup::TestSetup(View::TestSetup *view) : view{view} {
    view->populateConditionMenu({conditionName(Condition::audioVisual),
        conditionName(Condition::auditoryOnly)});
    view->populateMethodMenu({methodName(Method::adaptiveClosedSet),
        methodName(Method::adaptiveOpenSet),
        methodName(Method::fixedLevelClosedSet),
        methodName(Method::fixedLevelOpenSet)});
    view->subscribe(this);
}

void Presenter::TestSetup::show() { view->show(); }

void Presenter::TestSetup::hide() { view->hide(); }

static int readInteger(const std::string &x, const std::string &identifier) {
    try {
        return std::stoi(x);
    } catch (const std::invalid_argument &) {
        std::stringstream stream;
        stream << '\'' << x << '\'';
        stream << " is not a valid ";
        stream << identifier;
        stream << '.';
        throw BadInput{stream.str()};
    }
}

FixedLevelTest Presenter::TestSetup::fixedLevelTest() {
    FixedLevelTest p;
    commonTest(p);
    p.snr_dB = readInteger(view->startingSnr_dB(), "SNR");
    p.identity = testIdentity();
    return p;
}

TestIdentity Presenter::TestSetup::testIdentity() {
    TestIdentity p;
    p.subjectId = view->subjectId();
    p.testerId = view->testerId();
    p.session = view->session();
    return p;
}

void Presenter::TestSetup::commonTest(Test &p) {
    p.maskerLevel_dB_SPL = readMaskerLevel();
    p.targetListDirectory = view->targetListDirectory();
    p.maskerFilePath = view->maskerFilePath();
    p.fullScaleLevel_dB_SPL = fullScaleLevel_dB_SPL;
    p.condition = readCondition();
}

AdaptiveTest Presenter::TestSetup::adaptiveTest() {
    AdaptiveTest p;
    commonTest(p);
    p.startingSnr_dB = readInteger(view->startingSnr_dB(), "SNR");
    p.identity = testIdentity();
    p.ceilingSnr_dB = ceilingSnr_dB;
    p.floorSnr_dB = floorSnr_dB;
    p.trackBumpLimit = trackBumpLimit;
    p.trackSettingsFile = view->trackSettingsFile();
    return p;
}

Condition Presenter::TestSetup::readCondition() {
    return auditoryOnly() ? Condition::auditoryOnly : Condition::audioVisual;
}

bool Presenter::TestSetup::auditoryOnly() {
    return view->condition() == conditionName(Condition::auditoryOnly);
}

void Presenter::TestSetup::playCalibration() { parent->playCalibration(); }

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

void Presenter::TestSetup::confirmTestSetup() { parent->confirmTestSetup(); }

void Presenter::TestSetup::becomeChild(Presenter *p) { parent = p; }

void Presenter::TestSetup::setMasker(std::string s) {
    view->setMasker(std::move(s));
}

void Presenter::TestSetup::setStimulusList(std::string s) {
    view->setTargetListDirectory(std::move(s));
}

void Presenter::TestSetup::browseForTargetList() {
    parent->browseForTargetList();
}

void Presenter::TestSetup::browseForMasker() { parent->browseForMasker(); }

void Presenter::TestSetup::browseForCalibration() {
    parent->browseForCalibration();
}

void Presenter::TestSetup::browseForTrackSettingsFile() {
    parent->browseForTrackSettingsFile();
}

void Presenter::TestSetup::setCalibrationFilePath(std::string s) {
    view->setCalibrationFilePath(std::move(s));
}

void Presenter::TestSetup::setTrackSettingsFile(std::string s) {
    view->setTrackSettingsFile(std::move(s));
}

bool Presenter::TestSetup::adaptiveClosedSet() {
    return method(Method::adaptiveClosedSet);
}

bool Presenter::TestSetup::method(Method m) {
    return view->method() == methodName(m);
}

bool Presenter::TestSetup::adaptiveOpenSet() {
    return method(Method::adaptiveOpenSet);
}

bool Presenter::TestSetup::fixedLevelOpenSet() {
    return method(Method::fixedLevelOpenSet);
}

bool Presenter::TestSetup::fixedLevelClosedSet() {
    return method(Method::fixedLevelClosedSet);
}

bool Presenter::TestSetup::finiteTargets() {
    return view->usingTargetsWithoutReplacement();
}

Presenter::Subject::Subject(View::Subject *view) : view{view} {
    view->subscribe(this);
}

void Presenter::Subject::show() {
    view->show();
    showNextTrialButton();
}

void Presenter::Subject::hide() {
    hideResponseButtons();
    view->hide();
}

void Presenter::Subject::playTrial() {
    parent->playTrial();
    view->hideNextTrialButton();
}

void Presenter::Subject::submitResponse() {
    parent->submitSubjectResponse();
    hideResponseButtons();
}

void Presenter::Subject::becomeChild(Presenter *p) { parent = p; }

void Presenter::Subject::showNextTrialButton() { view->showNextTrialButton(); }

void Presenter::Subject::hideResponseButtons() { view->hideResponseButtons(); }

void Presenter::Subject::showResponseButtons() { view->showResponseButtons(); }

coordinate_response_measure::Response Presenter::Subject::subjectResponse() {
    coordinate_response_measure::Response p{};
    p.color = colorResponse();
    p.number = std::stoi(view->numberResponse());
    return p;
}

coordinate_response_measure::Color Presenter::Subject::colorResponse() {
    if (view->greenResponse())
        return coordinate_response_measure::Color::green;
    if (view->blueResponse())
        return coordinate_response_measure::Color::blue;
    if (view->whiteResponse())
        return coordinate_response_measure::Color::white;

    return coordinate_response_measure::Color::red;
}

Presenter::Testing::Testing(View::Testing *view) : view{view} {
    view->subscribe(this);
}

void Presenter::Testing::show() {
    view->show();
    showNextTrialButton();
}

void Presenter::Testing::showNextTrialButton() { view->showNextTrialButton(); }

void Presenter::Testing::playTrial() {
    parent->playTrial();
    view->hideNextTrialButton();
}

void Presenter::Testing::becomeChild(Presenter *p) { parent = p; }

void Presenter::Testing::submitPassedTrial() {
    parent->submitPassedTrial();
    prepareNextEvaluatedTrial();
}

void Presenter::Testing::prepareNextEvaluatedTrial() {
    view->hideEvaluationButtons();
    showNextTrialButton();
}

void Presenter::Testing::submitFailedTrial() {
    parent->submitFailedTrial();
    prepareNextEvaluatedTrial();
}

void Presenter::Testing::hide() {
    view->hideEvaluationButtons();
    view->hideResponseSubmission();
    view->hide();
}

void Presenter::Testing::showEvaluationButtons() {
    view->showEvaluationButtons();
}

void Presenter::Testing::showResponseSubmission() {
    view->showResponseSubmission();
}

void Presenter::Testing::submitResponse() {
    parent->submitExperimenterResponse();
    view->hideResponseSubmission();
    showNextTrialButton();
}

FreeResponse Presenter::Testing::openSetResponse() {
    return {view->response(), view->flagged()};
}

Presenter::Experimenter::Experimenter(View::Experimenter *view) : view{view} {
    view->subscribe(this);
}

void Presenter::Experimenter::show() { view->show(); }

void Presenter::Experimenter::hideExitTestButton() {
    view->hideExitTestButton();
}

void Presenter::Experimenter::showExitTestButton() {
    view->showExitTestButton();
}

void Presenter::Experimenter::display(std::string s) {
    view->display(std::move(s));
}

void Presenter::Experimenter::becomeChild(Presenter *p) { parent = p; }

void Presenter::Experimenter::exitTest() { parent->exitTest(); }

void Presenter::Experimenter::hide() { view->hide(); }
}
