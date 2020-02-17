#include "Presenter.hpp"
#include "av-speech-in-noise/Model.hpp"
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

static auto fixedLevelTest(Presenter::TestSetup &testSetup) -> FixedLevelTest {
    return testSetup.fixedLevelTest();
}

static auto adaptiveTest(Presenter::TestSetup &testSetup) -> AdaptiveTest {
    return testSetup.adaptiveTest();
}

static void displayTrialNumber(
    Presenter::Experimenter &experimenter, Model &model) {
    experimenter.display("Trial " + std::to_string(model.trialNumber()));
}

static void displayTarget(
    Presenter::Experimenter &experimenter, Model &model) {
    experimenter.secondaryDisplay(model.targetFileName());
}

Presenter::Presenter(Model &model, View &view, TestSetup &testSetup,
    Subject &subject, Experimenter &experimenter, Testing &testing)
    : fixedLevelOpenSetTrialCompletionHandler{testing},
      fixedLevelClosedSetTrialCompletionHandler{subject},
      adaptiveOpenSetTrialCompletionHandler{testing},
      adaptiveOpenSetKeywordsTrialCompletionHandler{testing},
      adaptiveClosedSetTrialCompletionHandler{subject}, model{model},
      view{view}, testSetup{testSetup}, subject{subject},
      experimenter{experimenter}, testing{testing},
      trialCompletionHandler_{&adaptiveClosedSetTrialCompletionHandler} {
    model.subscribe(this);
    testSetup.becomeChild(this);
    subject.becomeChild(this);
    experimenter.becomeChild(this);
    testing.becomeChild(this);
    view.populateAudioDeviceMenu(model.audioDevices());
}

void Presenter::run() { view.eventLoop(); }

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
    if (adaptiveClosedSetDelayedMasker())
        model.initializeTestWithDelayedMasker(adaptiveTest(testSetup));
    else if (adaptiveClosedSetSingleSpeaker())
        model.initializeTestWithSingleSpeaker(adaptiveTest(testSetup));
    else if (defaultAdaptive() || adaptiveCorrectKeywords())
        model.initializeTest(adaptiveTest(testSetup));
    else if (fixedLevelSilentIntervals())
        model.initializeSilentIntervalsTest(fixedLevelTest(testSetup));
    else if (fixedLevelAllStimuli())
        model.initializeAllStimuliTest(fixedLevelTest(testSetup));
    else
        model.initializeTest(fixedLevelTest(testSetup));
}

auto Presenter::adaptiveClosedSet() -> bool {
    return testSetup.adaptiveClosedSet();
}

auto Presenter::adaptivePassFail() -> bool {
    return testSetup.adaptivePassFail();
}

auto Presenter::fixedLevelSilentIntervals() -> bool {
    return testSetup.fixedLevelSilentIntervals();
}

auto Presenter::fixedLevelAllStimuli() -> bool {
    return testSetup.fixedLevelAllStimuli();
}

auto Presenter::adaptiveClosedSetDelayedMasker() -> bool {
    return testSetup.adaptiveClosedSetDelayedMasker();
}

auto Presenter::adaptiveClosedSetSingleSpeaker() -> bool {
    return testSetup.adaptiveClosedSetSingleSpeaker();
}

auto Presenter::adaptiveCorrectKeywords() -> bool {
    return testSetup.adaptiveCorrectKeywords();
}

auto Presenter::testComplete() -> bool { return model.testComplete(); }

void Presenter::switchToTestView() {
    hideTestSetup();
    showTestView();
}

void Presenter::hideTestSetup() { testSetup.hide(); }

void Presenter::showTestView() {
    experimenter.show();
    displayTrialNumber(experimenter, model);
    if (closedSet())
        subject.show();
    else
        testing.show();
    if (adaptiveCorrectKeywords())
        displayTarget(experimenter, model);
}

auto Presenter::closedSet() -> bool { return testSetup.closedSet(); }

auto Presenter::defaultAdaptive() -> bool {
    return testSetup.defaultAdaptive();
}

auto Presenter::fixedLevelClosedSet() -> bool {
    return testSetup.fixedLevelClosedSet();
}

auto Presenter::trialCompletionHandler() -> TrialCompletionHandler * {
    if (adaptiveClosedSet())
        return &adaptiveClosedSetTrialCompletionHandler;
    if (adaptivePassFail())
        return &adaptiveOpenSetTrialCompletionHandler;
    if (adaptiveCorrectKeywords())
        return &adaptiveOpenSetKeywordsTrialCompletionHandler;
    if (fixedLevelClosedSet())
        return &fixedLevelClosedSetTrialCompletionHandler;
    return &fixedLevelOpenSetTrialCompletionHandler;
}

void Presenter::showErrorMessage(std::string e) {
    view.showErrorMessage(std::move(e));
}

void Presenter::playTrial() {
    AudioSettings p;
    p.audioDevice = view.audioDevice();
    model.playTrial(p);
    experimenter.hideExitTestButton();
}

void Presenter::trialComplete() {
    trialCompletionHandler_->showResponseView();
    experimenter.showExitTestButton();
}

void Presenter::submitSubjectResponse() {
    submitSubjectResponse_();
    displayTrialNumber(experimenter, model);
    displayTarget(experimenter, model);
    if (testComplete())
        switchToSetupView();
    else
        playTrial();
}

void Presenter::submitSubjectResponse_() {
    model.submitResponse(subject.subjectResponse());
}

void Presenter::submitCorrectKeywords_() {
    model.submit(testing.correctKeywords());
}

void Presenter::submitExperimenterResponse() {
    proceedToNextTrialAfter(&Presenter::submitExperimenterResponse_);
}

void Presenter::submitExperimenterResponse_() {
    model.submitResponse(testing.openSetResponse());
}

void Presenter::submitPassedTrial() {
    proceedToNextTrialAfter(&Presenter::submitPassedTrial_);
}

void Presenter::submitPassedTrial_() { model.submitCorrectResponse(); }

void Presenter::submitFailedTrial() {
    proceedToNextTrialAfter(&Presenter::submitFailedTrial_);
}

void Presenter::submitCorrectKeywords() {
    try {
        proceedToNextTrialAfter(&Presenter::submitCorrectKeywords_);
        testing.hideCorrectKeywordsSubmission();
        testing.showNextTrialButton();
        displayTarget(experimenter, model);
    } catch (const std::runtime_error &e) {
        showErrorMessage(e.what());
    }
}

void Presenter::submitFailedTrial_() { model.submitIncorrectResponse(); }

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

void Presenter::showTestSetup() { testSetup.show(); }

void Presenter::hideTestView() {
    testing.hide();
    experimenter.hide();
    subject.hide();
}

void Presenter::playCalibration() {
    try {
        playCalibration_();
    } catch (const std::runtime_error &e) {
        showErrorMessage(e.what());
    }
}

void Presenter::playCalibration_() {
    auto p = testSetup.calibrationParameters();
    p.audioSettings.audioDevice = view.audioDevice();
    model.playCalibration(p);
}

void Presenter::browseForTargetList() {
    applyIfBrowseNotCancelled(
        view.browseForDirectory(), &TestSetup::setStimulusList);
}

void Presenter::applyIfBrowseNotCancelled(
    std::string s, void (TestSetup::*f)(std::string)) {
    if (!view.browseCancelled())
        (testSetup.*f)(std::move(s));
}

void Presenter::browseForMasker() {
    applyIfBrowseNotCancelled(
        view.browseForOpeningFile(), &TestSetup::setMasker);
}

void Presenter::browseForCalibration() {
    applyIfBrowseNotCancelled(
        view.browseForOpeningFile(), &TestSetup::setCalibrationFilePath);
}

void Presenter::browseForTrackSettingsFile() {
    applyIfBrowseNotCancelled(
        view.browseForOpeningFile(), &TestSetup::setTrackSettingsFile);
}

Presenter::TestSetup::TestSetup(View::TestSetup *view) : view{view} {
    view->populateConditionMenu({conditionName(Condition::audioVisual),
        conditionName(Condition::auditoryOnly)});
    view->populateMethodMenu({methodName(Method::defaultAdaptiveClosedSet),
        methodName(Method::adaptiveClosedSetSingleSpeaker),
        methodName(Method::adaptiveClosedSetDelayedMasker),
        methodName(Method::adaptivePassFail),
        methodName(Method::adaptiveCorrectKeywords),
        methodName(Method::defaultFixedLevelClosedSet),
        methodName(Method::fixedLevelClosedSetSilentIntervals),
        methodName(Method::defaultFixedLevelOpenSet),
        methodName(Method::fixedLevelOpenSetSilentIntervals),
        methodName(Method::fixedLevelOpenSetAllStimuli)});
    view->subscribe(this);
}

void Presenter::TestSetup::show() { view->show(); }

void Presenter::TestSetup::hide() { view->hide(); }

static auto readInteger(const std::string &x, const std::string &identifier)
    -> int {
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

auto Presenter::TestSetup::fixedLevelTest() -> FixedLevelTest {
    FixedLevelTest p;
    initialize(p);
    p.snr_dB = readInteger(view->startingSnr_dB(), "SNR");
    return p;
}

auto Presenter::TestSetup::testIdentity() -> TestIdentity {
    TestIdentity p;
    p.subjectId = view->subjectId();
    p.testerId = view->testerId();
    p.session = view->session();
    p.method = view->method();
    return p;
}

void Presenter::TestSetup::initialize(Test &p) {
    p.maskerLevel_dB_SPL = readMaskerLevel();
    p.targetListDirectory = view->targetListDirectory();
    p.maskerFilePath = view->maskerFilePath();
    p.fullScaleLevel_dB_SPL = fullScaleLevel_dB_SPL;
    p.condition = readCondition();
    p.identity = testIdentity();
}

auto Presenter::TestSetup::adaptiveTest() -> AdaptiveTest {
    AdaptiveTest p;
    initialize(p);
    p.startingSnr_dB = readInteger(view->startingSnr_dB(), "SNR");
    p.ceilingSnr_dB = ceilingSnr_dB;
    p.floorSnr_dB = floorSnr_dB;
    p.trackBumpLimit = trackBumpLimit;
    p.trackSettingsFile = view->trackSettingsFile();
    return p;
}

auto Presenter::TestSetup::readCondition() -> Condition {
    return auditoryOnly() ? Condition::auditoryOnly : Condition::audioVisual;
}

auto Presenter::TestSetup::auditoryOnly() -> bool {
    return view->condition() == conditionName(Condition::auditoryOnly);
}

void Presenter::TestSetup::playCalibration() { parent->playCalibration(); }

auto Presenter::TestSetup::calibrationParameters() -> Calibration {
    Calibration p;
    p.filePath = view->calibrationFilePath();
    p.level_dB_SPL = readCalibrationLevel();
    p.fullScaleLevel_dB_SPL = fullScaleLevel_dB_SPL;
    p.condition = readCondition();
    return p;
}

auto Presenter::TestSetup::readCalibrationLevel() -> int {
    return readInteger(view->calibrationLevel_dB_SPL(), "calibration level");
}

auto Presenter::TestSetup::readMaskerLevel() -> int {
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

auto Presenter::TestSetup::defaultAdaptive() -> bool {
    return defaultAdaptiveClosedSet() || adaptivePassFail();
}

auto Presenter::TestSetup::defaultAdaptiveClosedSet() -> bool {
    return method(Method::defaultAdaptiveClosedSet);
}

auto Presenter::TestSetup::adaptivePassFail() -> bool {
    return method(Method::adaptivePassFail);
}

auto Presenter::TestSetup::adaptiveClosedSet() -> bool {
    return defaultAdaptiveClosedSet() || adaptiveClosedSetSingleSpeaker() ||
        adaptiveClosedSetDelayedMasker();
}

auto Presenter::TestSetup::adaptiveClosedSetDelayedMasker() -> bool {
    return method(Method::adaptiveClosedSetDelayedMasker);
}

auto Presenter::TestSetup::adaptiveClosedSetSingleSpeaker() -> bool {
    return method(Method::adaptiveClosedSetSingleSpeaker);
}

auto Presenter::TestSetup::adaptiveCorrectKeywords() -> bool {
    return method(Method::adaptiveCorrectKeywords);
}

auto Presenter::TestSetup::fixedLevelClosedSet() -> bool {
    return method(Method::defaultFixedLevelClosedSet) ||
        fixedLevelClosedSetSilentIntervals();
}

auto Presenter::TestSetup::fixedLevelClosedSetSilentIntervals() -> bool {
    return method(Method::fixedLevelClosedSetSilentIntervals);
}

auto Presenter::TestSetup::fixedLevelSilentIntervals() -> bool {
    return fixedLevelClosedSetSilentIntervals() ||
        method(Method::fixedLevelOpenSetSilentIntervals);
}

auto Presenter::TestSetup::fixedLevelAllStimuli() -> bool {
    return method(Method::fixedLevelOpenSetAllStimuli);
}

auto Presenter::TestSetup::closedSet() -> bool {
    return adaptiveClosedSet() || fixedLevelClosedSet();
}

auto Presenter::TestSetup::method(Method m) -> bool {
    return view->method() == methodName(m);
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

auto Presenter::Subject::subjectResponse()
    -> coordinate_response_measure::Response {
    coordinate_response_measure::Response p{};
    p.color = colorResponse();
    p.number = std::stoi(view->numberResponse());
    return p;
}

auto Presenter::Subject::colorResponse() -> coordinate_response_measure::Color {
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

void Presenter::Testing::submitCorrectKeywords() {
    parent->submitCorrectKeywords();
}

void Presenter::Testing::hideCorrectKeywordsSubmission() {
    view->hideCorrectKeywordsSubmission();
}

void Presenter::Testing::hide() { view->hide(); }

void Presenter::Testing::showEvaluationButtons() {
    view->showEvaluationButtons();
}

void Presenter::Testing::showCorrectKeywordsSubmission() {
    view->showCorrectKeywordsSubmission();
}

void Presenter::Testing::showFreeResponseSubmission() {
    view->showFreeResponseSubmission();
}

void Presenter::Testing::submitFreeResponse() {
    parent->submitExperimenterResponse();
    view->hideFreeResponseSubmission();
    showNextTrialButton();
}

auto Presenter::Testing::openSetResponse() -> open_set::FreeResponse {
    return {view->freeResponse(), view->flagged()};
}

auto Presenter::Testing::correctKeywords() -> open_set::CorrectKeywords {
    open_set::CorrectKeywords p{};
    p.count = readInteger(view->correctKeywords(), "number");
    return p;
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

void Presenter::Experimenter::secondaryDisplay(std::string s) {
    view->secondaryDisplay(std::move(s));
}

void Presenter::Experimenter::becomeChild(Presenter *p) { parent = p; }

void Presenter::Experimenter::exitTest() { parent->exitTest(); }

void Presenter::Experimenter::hide() { view->hide(); }
}
