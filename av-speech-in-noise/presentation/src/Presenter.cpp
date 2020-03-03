#include "Presenter.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <string>
#include <sstream>

namespace av_speech_in_noise {
namespace {
class BadInput : public std::runtime_error {
  public:
    explicit BadInput(const std::string &s) : std::runtime_error{s} {}
};
}

static auto testIdentity(Presenter::TestSetup &testSetup) -> TestIdentity {
    return testSetup.testIdentity();
}

static auto fixedLevelTest(Presenter::TestSetup &testSetup) -> FixedLevelTest {
    return testSetup.fixedLevelTest();
}

static auto adaptiveTest(Presenter::TestSetup &testSetup) -> AdaptiveTest {
    return testSetup.adaptiveTest();
}

static void displayTrialNumber(
    Presenter::Experimenter &experimenterPresenter, Model &model) {
    experimenterPresenter.display(
        "Trial " + std::to_string(model.trialNumber()));
}

static void displayTarget(Presenter::Experimenter &experimenter, Model &model) {
    experimenter.secondaryDisplay(model.targetFileName());
}

static void displayTrialInformation(
    Presenter::Experimenter &experimenterPresenter, Model &model) {
    displayTrialNumber(experimenterPresenter, model);
    displayTarget(experimenterPresenter, model);
}

static auto adaptiveCoordinateResponseMeasure(Presenter::TestSetup &testSetup)
    -> bool {
    return testSetup.adaptiveCoordinateResponseMeasure();
}

static auto adaptivePassFail(Presenter::TestSetup &testSetup) -> bool {
    return testSetup.adaptivePassFail();
}

static auto fixedLevelSilentIntervals(Presenter::TestSetup &testSetup) -> bool {
    return testSetup.fixedLevelSilentIntervals();
}

static auto fixedLevelAllStimuli(Presenter::TestSetup &testSetup) -> bool {
    return testSetup.fixedLevelAllStimuli();
}

static auto adaptiveCoordinateResponseMeasureWithDelayedMasker(
    Presenter::TestSetup &testSetup) -> bool {
    return testSetup.adaptiveCoordinateResponseMeasureWithDelayedMasker();
}

static auto adaptiveCoordinateResponseMeasureWithSingleSpeaker(
    Presenter::TestSetup &testSetup) -> bool {
    return testSetup.adaptiveCoordinateResponseMeasureWithSingleSpeaker();
}

static auto adaptiveCorrectKeywords(Presenter::TestSetup &testSetup) -> bool {
    return testSetup.adaptiveCorrectKeywords();
}

static auto defaultAdaptiveCoordinateResponseMeasure(Method m) -> bool {
    return m == Method::defaultAdaptiveCoordinateResponseMeasure;
}

static auto adaptiveCoordinateResponseMeasureWithSingleSpeaker(Method m)
    -> bool {
    return m == Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker;
}

static auto adaptiveCoordinateResponseMeasureWithDelayedMasker(Method m)
    -> bool {
    return m == Method::adaptiveCoordinateResponseMeasureWithDelayedMasker;
}

static auto fixedLevelCoordinateResponseMeasureWithTargetReplacement(Method m)
    -> bool {
    return m ==
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement;
}

static auto fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets(
    Method m) -> bool {
    return m ==
        Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets;
}

static auto fixedLevelFreeResponseWithAllTargets(Method m) -> bool {
    return m == Method::fixedLevelFreeResponseWithAllTargets;
}

static auto fixedLevelFreeResponseWithSilentIntervalTargets(Method m) -> bool {
    return m == Method::fixedLevelFreeResponseWithSilentIntervalTargets;
}

static auto adaptivePassFail(Method m) -> bool {
    return m == Method::adaptivePassFail;
}

static auto adaptiveCorrectKeywords(Method m) -> bool {
    return m == Method::adaptiveCorrectKeywords;
}

static auto adaptiveCoordinateResponseMeasure(Method m) -> bool {
    return defaultAdaptiveCoordinateResponseMeasure(m) ||
        adaptiveCoordinateResponseMeasureWithSingleSpeaker(m) ||
        adaptiveCoordinateResponseMeasureWithDelayedMasker(m);
}

static auto fixedLevelCoordinateResponseMeasure(Method m) -> bool {
    return fixedLevelCoordinateResponseMeasureWithTargetReplacement(m) ||
        fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets(m);
}

static auto coordinateResponseMeasure(Method m) -> bool {
    return adaptiveCoordinateResponseMeasure(m) ||
        fixedLevelCoordinateResponseMeasure(m);
}

static auto defaultAdaptive(Method m) -> bool {
    return defaultAdaptiveCoordinateResponseMeasure(m) || adaptivePassFail(m);
}

static auto fixedLevelSilentIntervals(Method m) -> bool {
    return fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets(m) ||
        fixedLevelFreeResponseWithSilentIntervalTargets(m);
}

static auto defaultAdaptive(Presenter::TestSetup &testSetup) -> bool {
    return testSetup.defaultAdaptive();
}

static auto fixedLevelCoordinateResponseMeasure(Presenter::TestSetup &testSetup)
    -> bool {
    return testSetup.fixedLevelCoordinateResponseMeasure();
}

static auto testComplete(Model &model) -> bool { return model.testComplete(); }

static void hide(Presenter::TestSetup &testSetup) { testSetup.hide(); }

static void initializeTest(Model &model, Presenter::TestSetup &testSetup,
    TestSettingsInterpreter &testSettingsInterpreter,
    const std::string &testSettings) {
    testSettingsInterpreter.apply(model, testSettings, testIdentity(testSetup));
    auto method{testSettingsInterpreter.method(testSettings)};
    if (adaptiveCoordinateResponseMeasureWithDelayedMasker(method))
        model.initializeTestWithDelayedMasker(adaptiveTest(testSetup));
    else if (defaultAdaptive(method) || adaptiveCorrectKeywords(method))
        model.initializeTest(adaptiveTest(testSetup));
    else
        model.initializeTest(fixedLevelTest(testSetup));
}

Presenter::Presenter(Model &model, View &view, TestSetup &testSetup,
    CoordinateResponseMeasure &coordinateResponseMeasurePresenter,
    Experimenter &experimenterPresenter,
    TestSettingsInterpreter &testSettingsInterpreter,
    TextFileReader &textFileReader)
    : freeResponseTrialCompletionHandler{experimenterPresenter},
      passFailTrialCompletionHandler{experimenterPresenter},
      correctKeywordsTrialCompletionHandler{experimenterPresenter},
      coordinateResponseMeasureTrialCompletionHandler{
          coordinateResponseMeasurePresenter},
      model{model}, view{view}, testSetup{testSetup},
      coordinateResponseMeasurePresenter{coordinateResponseMeasurePresenter},
      experimenterPresenter{experimenterPresenter},
      testSettingsInterpreter{testSettingsInterpreter},
      textFileReader{textFileReader},
      trialCompletionHandler_{
          &coordinateResponseMeasureTrialCompletionHandler} {
    model.subscribe(this);
    testSetup.becomeChild(this);
    coordinateResponseMeasurePresenter.becomeChild(this);
    experimenterPresenter.becomeChild(this);
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
    auto testSettings{textFileReader.read(testSetup.testSettingsFile())};
    initializeTest(model, testSetup, testSettingsInterpreter, testSettings);
    auto method{testSettingsInterpreter.method(testSettings)};
    if (!testComplete(model)) {
        switchToTestView(method);
        trialCompletionHandler_ = trialCompletionHandler(method);
    }
}

void Presenter::switchToTestView(Method m) {
    hide(testSetup);
    showTest(m);
}

void Presenter::showTest(Method m) {
    experimenterPresenter.show();
    displayTrialInformation(experimenterPresenter, model);
    if (coordinateResponseMeasure(m))
        coordinateResponseMeasurePresenter.start();
    else
        experimenterPresenter.start();
}

auto Presenter::trialCompletionHandler(Method m) -> TrialCompletionHandler * {
    if (coordinateResponseMeasure(m))
        return &coordinateResponseMeasureTrialCompletionHandler;
    if (adaptivePassFail(m))
        return &passFailTrialCompletionHandler;
    if (adaptiveCorrectKeywords(m))
        return &correctKeywordsTrialCompletionHandler;
    return &freeResponseTrialCompletionHandler;
}

void Presenter::showErrorMessage(std::string e) {
    view.showErrorMessage(std::move(e));
}

void Presenter::playTrial() {
    AudioSettings p;
    p.audioDevice = view.audioDevice();
    model.playTrial(p);
    experimenterPresenter.trialPlayed();
}

void Presenter::trialComplete() {
    trialCompletionHandler_->showResponseSubmission();
    experimenterPresenter.trialComplete();
}

void Presenter::submitSubjectResponse() {
    model.submitResponse(coordinateResponseMeasurePresenter.subjectResponse());
    if (testComplete(model))
        switchToTestSetupView();
    else {
        displayTrialInformation(experimenterPresenter, model);
        playTrial();
    }
}

void Presenter::submitFreeResponse() {
    proceedToNextTrialAfter(&Presenter::submitFreeResponse_);
}

void Presenter::submitPassedTrial() {
    proceedToNextTrialAfter(&Presenter::submitPassedTrial_);
}

void Presenter::submitFailedTrial() {
    proceedToNextTrialAfter(&Presenter::submitFailedTrial_);
}

void Presenter::submitCorrectKeywords() {
    try {
        proceedToNextTrialAfter(&Presenter::submitCorrectKeywords_);
    } catch (const std::runtime_error &e) {
        showErrorMessage(e.what());
    }
}

void Presenter::submitCorrectKeywords_() {
    model.submit(experimenterPresenter.correctKeywords());
}

void Presenter::submitFreeResponse_() {
    model.submitResponse(experimenterPresenter.freeResponse());
}

void Presenter::submitPassedTrial_() { model.submitCorrectResponse(); }

void Presenter::submitFailedTrial_() { model.submitIncorrectResponse(); }

void Presenter::proceedToNextTrialAfter(void (Presenter::*f)()) {
    (this->*f)();
    readyNextTrialIfNeeded();
}

void Presenter::readyNextTrialIfNeeded() {
    if (testComplete(model))
        switchToTestSetupView();
    else {
        displayTrialInformation(experimenterPresenter, model);
        experimenterPresenter.readyNextTrial();
    }
}

void Presenter::exitTest() { switchToTestSetupView(); }

void Presenter::switchToTestSetupView() {
    showTestSetup();
    hideTest();
}

void Presenter::showTestSetup() { testSetup.show(); }

void Presenter::hideTest() {
    experimenterPresenter.stop();
    coordinateResponseMeasurePresenter.stop();
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

static auto browseForOpeningFile(View &view) -> std::string {
    return view.browseForOpeningFile();
}

void Presenter::browseForMasker() {
    applyIfBrowseNotCancelled(
        browseForOpeningFile(view), &TestSetup::setMasker);
}

void Presenter::browseForCalibration() {
    applyIfBrowseNotCancelled(
        browseForOpeningFile(view), &TestSetup::setCalibrationFilePath);
}

void Presenter::browseForTrackSettingsFile() {
    applyIfBrowseNotCancelled(
        browseForOpeningFile(view), &TestSetup::setTrackSettingsFile);
}

void Presenter::browseForTestSettingsFile() {
    applyIfBrowseNotCancelled(
        browseForOpeningFile(view), &TestSetup::setTestSettingsFile);
}

Presenter::TestSetup::TestSetup(View::TestSetup *view) : view{view} {
    view->populateConditionMenu({conditionName(Condition::audioVisual),
        conditionName(Condition::auditoryOnly)});
    view->populateMethodMenu({methodName(Method::
                                      defaultAdaptiveCoordinateResponseMeasure),
        methodName(Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker),
        methodName(Method::adaptiveCoordinateResponseMeasureWithDelayedMasker),
        methodName(Method::adaptivePassFail),
        methodName(Method::adaptiveCorrectKeywords),
        methodName(
            Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement),
        methodName(Method::
                fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets),
        methodName(Method::fixedLevelFreeResponseWithTargetReplacement),
        methodName(Method::fixedLevelFreeResponseWithSilentIntervalTargets),
        methodName(Method::fixedLevelFreeResponseWithAllTargets)});
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

void Presenter::TestSetup::browseForTestSettingsFile() {
    parent->browseForTestSettingsFile();
}

void Presenter::TestSetup::setCalibrationFilePath(std::string s) {
    view->setCalibrationFilePath(std::move(s));
}

void Presenter::TestSetup::setTrackSettingsFile(std::string s) {
    view->setTrackSettingsFile(std::move(s));
}

void Presenter::TestSetup::setTestSettingsFile(std::string s) {
    view->setTestSettingsFile(std::move(s));
}

auto Presenter::TestSetup::testSettingsFile() -> std::string {
    return view->testSettingsFile();
}

Presenter::CoordinateResponseMeasure::CoordinateResponseMeasure(
    View::CoordinateResponseMeasure *view)
    : view{view} {
    view->subscribe(this);
}

static void hideResponseButtons(View::CoordinateResponseMeasure *view) {
    view->hideResponseButtons();
}

static void showNextTrialButton(View::CoordinateResponseMeasure *view) {
    view->showNextTrialButton();
}

void Presenter::CoordinateResponseMeasure::start() {
    view->show();
    showNextTrialButton(view);
}

void Presenter::CoordinateResponseMeasure::stop() {
    hideResponseButtons(view);
    view->hide();
}

void Presenter::CoordinateResponseMeasure::playTrial() {
    parent->playTrial();
    view->hideNextTrialButton();
}

void Presenter::CoordinateResponseMeasure::submitResponse() {
    parent->submitSubjectResponse();
    hideResponseButtons(view);
}

void Presenter::CoordinateResponseMeasure::becomeChild(Presenter *p) {
    parent = p;
}

void Presenter::CoordinateResponseMeasure::showResponseButtons() {
    view->showResponseButtons();
}

auto Presenter::CoordinateResponseMeasure::subjectResponse()
    -> coordinate_response_measure::Response {
    coordinate_response_measure::Response p{};
    p.color = colorResponse();
    p.number = std::stoi(view->numberResponse());
    return p;
}

auto Presenter::CoordinateResponseMeasure::colorResponse()
    -> coordinate_response_measure::Color {
    if (view->greenResponse())
        return coordinate_response_measure::Color::green;
    if (view->blueResponse())
        return coordinate_response_measure::Color::blue;
    if (view->whiteResponse())
        return coordinate_response_measure::Color::white;

    return coordinate_response_measure::Color::red;
}

Presenter::Experimenter::Experimenter(View::Experimenter *view) : view{view} {
    view->subscribe(this);
}

void Presenter::Experimenter::becomeChild(Presenter *p) { parent = p; }

static void showNextTrialButton(View::Experimenter *view) {
    view->showNextTrialButton();
}

void Presenter::Experimenter::show() { view->show(); }

void Presenter::Experimenter::start() {
    view->show();
    showNextTrialButton(view);
}

void Presenter::Experimenter::stop() {
    view->hideFreeResponseSubmission();
    view->hideEvaluationButtons();
    view->hideCorrectKeywordsSubmission();
    view->hide();
}

void Presenter::Experimenter::trialPlayed() {
    view->hideExitTestButton();
    view->hideNextTrialButton();
}

void Presenter::Experimenter::trialComplete() { view->showExitTestButton(); }

void Presenter::Experimenter::readyNextTrial() {
    view->hideFreeResponseSubmission();
    view->hideEvaluationButtons();
    view->hideCorrectKeywordsSubmission();
    showNextTrialButton(view);
}

void Presenter::Experimenter::showPassFailSubmission() {
    view->showEvaluationButtons();
}

void Presenter::Experimenter::showCorrectKeywordsSubmission() {
    view->showCorrectKeywordsSubmission();
}

void Presenter::Experimenter::showFreeResponseSubmission() {
    view->showFreeResponseSubmission();
}

void Presenter::Experimenter::submitPassedTrial() {
    parent->submitPassedTrial();
}

void Presenter::Experimenter::submitFailedTrial() {
    parent->submitFailedTrial();
}

void Presenter::Experimenter::submitCorrectKeywords() {
    parent->submitCorrectKeywords();
}

void Presenter::Experimenter::submitFreeResponse() {
    parent->submitFreeResponse();
}

void Presenter::Experimenter::playTrial() { parent->playTrial(); }

void Presenter::Experimenter::exitTest() { parent->exitTest(); }

auto Presenter::Experimenter::freeResponse() -> open_set::FreeResponse {
    return {view->freeResponse(), view->flagged()};
}

auto Presenter::Experimenter::correctKeywords() -> open_set::CorrectKeywords {
    open_set::CorrectKeywords p{};
    p.count = readInteger(view->correctKeywords(), "number");
    return p;
}

void Presenter::Experimenter::display(std::string s) {
    view->display(std::move(s));
}

void Presenter::Experimenter::secondaryDisplay(std::string s) {
    view->secondaryDisplay(std::move(s));
}
}
