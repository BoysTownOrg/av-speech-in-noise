#include "Presenter.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/name.hpp>
#include <string>
#include <sstream>
#include <functional>

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

static void displayTrialNumber(
    Presenter::Experimenter &experimenterPresenter, Model &model) {
    experimenterPresenter.display(
        "Trial " + std::to_string(model.trialNumber()));
}

static void displayTarget(
    Presenter::Experimenter &experimenterPresenter, Model &model) {
    experimenterPresenter.secondaryDisplay(model.targetFileName());
}

static void displayTrialInformation(
    Presenter::Experimenter &experimenterPresenter, Model &model) {
    displayTrialNumber(experimenterPresenter, model);
    displayTarget(experimenterPresenter, model);
}

static void readyNextTrial(
    Presenter::Experimenter &experimenterPresenter, Model &model) {
    displayTrialInformation(experimenterPresenter, model);
    experimenterPresenter.readyNextTrial();
}

static auto fixedLevelConsonant(Method m) -> bool {
    return m == Method::fixedLevelConsonants;
}

static auto coordinateResponseMeasure(Method m) -> bool {
    return m == Method::adaptiveCoordinateResponseMeasure ||
        m == Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker ||
        m == Method::adaptiveCoordinateResponseMeasureWithDelayedMasker ||
        m == Method::adaptiveCoordinateResponseMeasureWithEyeTracking ||
        m == Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement ||
        m ==
        Method::
            fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking ||
        m ==
        Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets;
}

static auto consonant(Method m) -> bool { return fixedLevelConsonant(m); }

static auto testComplete(Model &model) -> bool { return model.testComplete(); }

static void hide(Presenter::TestSetup &testSetup) { testSetup.hide(); }

static auto readInteger(const std::string &x, const std::string &identifier)
    -> int {
    try {
        return std::stoi(x);
    } catch (const std::invalid_argument &) {
        std::stringstream stream;
        stream << '"' << x << '"';
        stream << " is not a valid ";
        stream << identifier;
        stream << '.';
        throw BadInput{stream.str()};
    }
}

static void showContinueTestingDialogWithResultsWhenComplete(
    Presenter::Experimenter &experimenterPresenter, Model &model) {
    if (testComplete(model)) {
        experimenterPresenter.hideSubmissions();
        experimenterPresenter.showContinueTestingDialog();
        std::stringstream thresholds;
        thresholds << "thresholds (targets: dB SNR)";
        for (const auto &result : model.adaptiveTestResults())
            thresholds << '\n'
                       << result.targetsUrl.path << ": " << result.threshold;
        experimenterPresenter.setContinueTestingDialogMessage(thresholds.str());
    } else
        readyNextTrial(experimenterPresenter, model);
}

Presenter::Presenter(Model &model, View &view, TestSetup &testSetup,
    CoordinateResponseMeasure &coordinateResponseMeasurePresenter,
    Experimenter &experimenterPresenter,
    TestSettingsInterpreter &testSettingsInterpreter,
    TextFileReader &textFileReader, TaskResponder *consonantResponder,
    TaskPresenter *consonantPresenter,
    TaskResponder *coordinateResponseMeasureResponder,
    TaskPresenter *coordinateResponseMeasurePresenterRefactored)
    : freeResponseTrialCompletionHandler{experimenterPresenter},
      passFailTrialCompletionHandler{experimenterPresenter},
      correctKeywordsTrialCompletionHandler{experimenterPresenter},
      coordinateResponseMeasureTrialCompletionHandler{
          coordinateResponseMeasurePresenterRefactored},
      consonantTrialCompletionHandler{consonantPresenter}, model{model},
      view{view}, testSetup{testSetup},
      coordinateResponseMeasurePresenter{coordinateResponseMeasurePresenter},
      experimenterPresenter{experimenterPresenter},
      testSettingsInterpreter{testSettingsInterpreter},
      textFileReader{textFileReader},
      trialCompletionHandler_{&coordinateResponseMeasureTrialCompletionHandler},
      consonantPresenter{consonantPresenter},
      coordinateResponseMeasurePresenterRefactored{
          coordinateResponseMeasurePresenterRefactored} {
    model.subscribe(this);
    testSetup.becomeChild(this);
    coordinateResponseMeasurePresenter.becomeChild(this);
    experimenterPresenter.becomeChild(this);
    if (consonantResponder != nullptr)
        consonantResponder->becomeChild(this);
    if (coordinateResponseMeasureResponder != nullptr)
        coordinateResponseMeasureResponder->becomeChild(this);
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
    const auto testSettings{
        textFileReader.read({testSetup.testSettingsFile()})};
    testSettingsInterpreter.initialize(model, testSettings,
        testIdentity(testSetup),
        SNR{readInteger(testSetup.startingSnr(), "starting SNR")});
    if (!av_speech_in_noise::testComplete(model)) {
        const auto method{testSettingsInterpreter.method(testSettings)};
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
        coordinateResponseMeasurePresenterRefactored->start();
    else if (consonant(m))
        consonantPresenter->start();
    else
        experimenterPresenter.start();
}

auto Presenter::trialCompletionHandler(Method m) -> TrialCompletionHandler * {
    if (coordinateResponseMeasure(m))
        return &coordinateResponseMeasureTrialCompletionHandler;
    if (consonant(m))
        return &consonantTrialCompletionHandler;
    if (m == Method::adaptivePassFail ||
        m == Method::adaptivePassFailWithEyeTracking)
        return &passFailTrialCompletionHandler;
    if (m == Method::adaptiveCorrectKeywords ||
        m == Method::adaptiveCorrectKeywordsWithEyeTracking)
        return &correctKeywordsTrialCompletionHandler;
    return &freeResponseTrialCompletionHandler;
}

void Presenter::showErrorMessage(std::string e) {
    view.showErrorMessage(std::move(e));
}

static void playTrial(
    Model &model, View &view, Presenter::Experimenter &presenter) {
    AudioSettings p;
    p.audioDevice = view.audioDevice();
    model.playTrial(p);
    presenter.trialPlayed();
}

void Presenter::playTrial() {
    av_speech_in_noise::playTrial(model, view, experimenterPresenter);
}

void Presenter::trialComplete() {
    trialCompletionHandler_->showResponseSubmission();
    experimenterPresenter.trialComplete();
    view.showCursor();
}

static void show(Presenter::TestSetup &presenter) { presenter.show(); }

static void switchToTestSetupView(Presenter::TestSetup &testSetup,
    Presenter::Experimenter &experimenter,
    TaskPresenter *coordinateResponseMeasure, TaskPresenter *consonant) {
    show(testSetup);
    experimenter.stop();
    coordinateResponseMeasure->stop();
    consonant->stop();
}

static void updateTrialInformationAndPlayNext(
    Model &model, View &view, Presenter::Experimenter &experimenter) {
    displayTrialInformation(experimenter, model);
    av_speech_in_noise::playTrial(model, view, experimenter);
}

static void switchToTestSetupViewIfCompleteElse(Model &model,
    Presenter::TestSetup &testSetup, Presenter::Experimenter &experimenter,
    TaskPresenter *coordinateResponseMeasure, TaskPresenter *consonant,
    const std::function<void()> &f) {
    if (testComplete(model))
        switchToTestSetupView(
            testSetup, experimenter, coordinateResponseMeasure, consonant);
    else
        f();
}

void Presenter::playNextTrialIfNeeded() {
    switchToTestSetupViewIfCompleteElse(model, testSetup, experimenterPresenter,
        coordinateResponseMeasurePresenterRefactored, consonantPresenter,
        [&]() {
            updateTrialInformationAndPlayNext(
                model, view, experimenterPresenter);
        });
}

void Presenter::readyNextTrialIfNeeded() {
    switchToTestSetupViewIfCompleteElse(model, testSetup, experimenterPresenter,
        coordinateResponseMeasurePresenterRefactored, consonantPresenter,
        [&]() { readyNextTrial(experimenterPresenter, model); });
}

void Presenter::submitCoordinateResponse() {
    model.submit(coordinateResponseMeasurePresenter.subjectResponse());
    playNextTrialIfNeeded();
}

void Presenter::submitFreeResponse() {
    readyNextTrialAfter(&Presenter::submitFreeResponse_);
}

void Presenter::submitPassedTrial() {
    submitPassedTrial_();
    showContinueTestingDialogWithResultsWhenComplete(
        experimenterPresenter, model);
}

void Presenter::submitFailedTrial() {
    submitFailedTrial_();
    showContinueTestingDialogWithResultsWhenComplete(
        experimenterPresenter, model);
}

void Presenter::submitCorrectKeywords() {
    try {
        submitCorrectKeywords_();
        showContinueTestingDialogWithResultsWhenComplete(
            experimenterPresenter, model);
    } catch (const std::runtime_error &e) {
        showErrorMessage(e.what());
    }
}

void Presenter::submitCorrectKeywords_() {
    model.submit(experimenterPresenter.correctKeywords());
}

void Presenter::submitFreeResponse_() {
    model.submit(experimenterPresenter.freeResponse());
}

void Presenter::submitPassedTrial_() { model.submitCorrectResponse(); }

void Presenter::submitFailedTrial_() { model.submitIncorrectResponse(); }

void Presenter::declineContinuingTesting() {
    av_speech_in_noise::switchToTestSetupView(testSetup, experimenterPresenter,
        coordinateResponseMeasurePresenterRefactored, consonantPresenter);
}

void Presenter::acceptContinuingTesting() {
    model.restartAdaptiveTestWhilePreservingTargets();
    readyNextTrial(experimenterPresenter, model);
}

void Presenter::readyNextTrialAfter(void (Presenter::*f)()) {
    (this->*f)();
    readyNextTrialIfNeeded();
}

void Presenter::exitTest() {
    av_speech_in_noise::switchToTestSetupView(testSetup, experimenterPresenter,
        coordinateResponseMeasurePresenterRefactored, consonantPresenter);
}

void Presenter::playCalibration() {
    try {
        playCalibration_();
    } catch (const std::runtime_error &e) {
        showErrorMessage(e.what());
    }
}

void Presenter::playCalibration_() {
    auto p{testSettingsInterpreter.calibration(
        textFileReader.read({testSetup.testSettingsFile()}))};
    p.audioDevice = view.audioDevice();
    model.playCalibration(p);
}

void Presenter::applyIfBrowseNotCancelled(
    std::string s, void (TestSetup::*f)(std::string)) {
    if (!view.browseCancelled())
        (testSetup.*f)(std::move(s));
}

static auto browseForOpeningFile(View &view) -> std::string {
    return view.browseForOpeningFile();
}

void Presenter::browseForTestSettingsFile() {
    applyIfBrowseNotCancelled(
        browseForOpeningFile(view), &TestSetup::setTestSettingsFile);
}

auto Presenter::testComplete() -> bool {
    return av_speech_in_noise::testComplete(model);
}

Presenter::TestSetup::TestSetup(View::TestSetup *view) : view{view} {
    view->populateTransducerMenu({name(Transducer::headphone),
        name(Transducer::oneSpeaker), name(Transducer::twoSpeakers)});
    view->subscribe(this);
}

void Presenter::TestSetup::show() { view->show(); }

void Presenter::TestSetup::hide() { view->hide(); }

auto Presenter::TestSetup::testIdentity() -> TestIdentity {
    TestIdentity p;
    p.subjectId = view->subjectId();
    p.testerId = view->testerId();
    p.session = view->session();
    p.rmeSetting = view->rmeSetting();
    p.transducer = view->transducer();
    return p;
}

void Presenter::TestSetup::becomeChild(Presenter *p) { parent = p; }

void Presenter::TestSetup::notifyThatPlayCalibrationButtonHasBeenClicked() {
    parent->playCalibration();
}

void Presenter::TestSetup::notifyThatConfirmButtonHasBeenClicked() {
    parent->confirmTestSetup();
}

void Presenter::TestSetup::
    notifyThatBrowseForTestSettingsButtonHasBeenClicked() {
    parent->browseForTestSettingsFile();
}

void Presenter::TestSetup::setTestSettingsFile(std::string s) {
    view->setTestSettingsFile(std::move(s));
}

auto Presenter::TestSetup::testSettingsFile() -> std::string {
    return view->testSettingsFile();
}

auto Presenter::TestSetup::startingSnr() -> std::string {
    return view->startingSnr();
}

Presenter::CoordinateResponseMeasure::CoordinateResponseMeasure(
    View::CoordinateResponseMeasureInput *inputView,
    View::CoordinateResponseMeasureOutput *outputView)
    : inputView{inputView}, outputView{outputView} {
    inputView->subscribe(this);
}

static void hideResponseButtons(View::CoordinateResponseMeasureOutput *view) {
    view->hideResponseButtons();
}

static void showNextTrialButton(View::CoordinateResponseMeasureOutput *view) {
    view->showNextTrialButton();
}

void Presenter::CoordinateResponseMeasure::start() {
    outputView->show();
    showNextTrialButton(outputView);
}

void Presenter::CoordinateResponseMeasure::stop() {
    hideResponseButtons(outputView);
    outputView->hide();
}

void Presenter::CoordinateResponseMeasure::
    notifyThatReadyButtonHasBeenClicked() {
    parent->playTrial();
    outputView->hideNextTrialButton();
}

void Presenter::CoordinateResponseMeasure::
    notifyThatResponseButtonHasBeenClicked() {
    parent->submitCoordinateResponse();
    hideResponseButtons(outputView);
}

void Presenter::CoordinateResponseMeasure::becomeChild(Presenter *p) {
    parent = p;
}

void Presenter::CoordinateResponseMeasure::showResponseButtons() {
    outputView->showResponseButtons();
}

auto Presenter::CoordinateResponseMeasure::subjectResponse()
    -> coordinate_response_measure::Response {
    coordinate_response_measure::Response p{};
    p.color = colorResponse();
    p.number = std::stoi(inputView->numberResponse());
    return p;
}

auto Presenter::CoordinateResponseMeasure::colorResponse()
    -> coordinate_response_measure::Color {
    if (inputView->greenResponse())
        return coordinate_response_measure::Color::green;
    if (inputView->blueResponse())
        return coordinate_response_measure::Color::blue;
    if (inputView->whiteResponse())
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

static void hideSubmissions(View::Experimenter *view) {
    view->hideFreeResponseSubmission();
    view->hideEvaluationButtons();
    view->hideCorrectKeywordsSubmission();
    view->hideContinueTestingDialog();
}

void Presenter::Experimenter::hideCorrectKeywordsSubmission() {
    view->hideCorrectKeywordsSubmission();
}

void Presenter::Experimenter::stop() {
    av_speech_in_noise::hideSubmissions(view);
    view->hide();
}

void Presenter::Experimenter::trialPlayed() {
    view->hideExitTestButton();
    view->hideNextTrialButton();
}

void Presenter::Experimenter::trialComplete() { view->showExitTestButton(); }

void Presenter::Experimenter::readyNextTrial() {
    av_speech_in_noise::hideSubmissions(view);
    showNextTrialButton(view);
}

void Presenter::Experimenter::showContinueTestingDialog() {
    view->showContinueTestingDialog();
}

void Presenter::Experimenter::hideEvaluationButtons() {
    view->hideEvaluationButtons();
}

void Presenter::Experimenter::hideSubmissions() {
    av_speech_in_noise::hideSubmissions(view);
}

void Presenter::Experimenter::setContinueTestingDialogMessage(
    const std::string &s) {
    view->setContinueTestingDialogMessage(s);
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

void Presenter::Experimenter::clearFreeResponse() { view->clearFreeResponse(); }

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

void Presenter::Experimenter::declineContinuingTesting() {
    parent->declineContinuingTesting();
}

void Presenter::Experimenter::acceptContinuingTesting() {
    parent->acceptContinuingTesting();
}

void Presenter::Experimenter::playTrial() { parent->playTrial(); }

void Presenter::Experimenter::exitTest() { parent->exitTest(); }

auto Presenter::Experimenter::freeResponse() -> FreeResponse {
    return {view->freeResponse(), view->flagged()};
}

auto Presenter::Experimenter::correctKeywords() -> CorrectKeywords {
    CorrectKeywords p{};
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
