#include "Presenter.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/name.hpp>
#include <string>
#include <sstream>
#include <functional>

namespace av_speech_in_noise {
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

static auto freeResponse(Method m) -> bool {
    return m == Method::fixedLevelFreeResponseWithAllTargets ||
        m == Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking ||
        m == Method::fixedLevelFreeResponseWithSilentIntervalTargets ||
        m == Method::fixedLevelFreeResponseWithTargetReplacement;
}

static auto correctKeywords(Method m) -> bool {
    return m == Method::adaptiveCorrectKeywords ||
        m == Method::adaptiveCorrectKeywordsWithEyeTracking;
}

static auto consonant(Method m) -> bool {
    return m == Method::fixedLevelConsonants;
}

static auto testComplete(Model &model) -> bool { return model.testComplete(); }

static void hide(Presenter::TestSetup &testSetup) { testSetup.hide(); }

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
    Experimenter &experimenterPresenter,
    TestSettingsInterpreter &testSettingsInterpreter,
    TextFileReader &textFileReader, TaskResponder *consonantResponder,
    TaskPresenter *consonantPresenter,
    TaskResponder *coordinateResponseMeasureResponder,
    TaskPresenter *coordinateResponseMeasurePresenter,
    TaskResponder *freeResponseResponder, TaskPresenter *freeResponsePresenter,
    TaskResponder *correctKeywordsResponder,
    TaskPresenter *correctKeywordsPresenter, TaskResponder *passFailResponder,
    TaskPresenter *passFailPresenter)
    : model{model}, view{view}, testSetup{testSetup},
      experimenterPresenter{experimenterPresenter},
      testSettingsInterpreter{testSettingsInterpreter},
      textFileReader{textFileReader}, consonantPresenter{consonantPresenter},
      coordinateResponseMeasurePresenter{coordinateResponseMeasurePresenter},
      freeResponsePresenter{freeResponsePresenter},
      correctKeywordsPresenter{correctKeywordsPresenter},
      passFailPresenter{passFailPresenter}, taskPresenter_{passFailPresenter} {
    model.subscribe(this);
    testSetup.becomeChild(this);
    experimenterPresenter.becomeChild(this);
    if (consonantResponder != nullptr) {
        consonantResponder->becomeChild(this);
        consonantResponder->subscribe(consonantPresenter);
    }
    if (freeResponseResponder != nullptr) {
        freeResponseResponder->becomeChild(this);
        freeResponseResponder->subscribe(freeResponsePresenter);
    }
    if (correctKeywordsResponder != nullptr) {
        correctKeywordsResponder->becomeChild(this);
        correctKeywordsResponder->subscribe(correctKeywordsPresenter);
    }
    if (passFailResponder != nullptr) {
        passFailResponder->becomeChild(this);
        passFailResponder->subscribe(passFailPresenter);
    }
    if (coordinateResponseMeasureResponder != nullptr) {
        coordinateResponseMeasureResponder->becomeChild(this);
        coordinateResponseMeasureResponder->subscribe(
            coordinateResponseMeasurePresenter);
    }
    view.populateAudioDeviceMenu(model.audioDevices());
}

void Presenter::showContinueTestingDialogWithResultsWhenComplete() {
    av_speech_in_noise::showContinueTestingDialogWithResultsWhenComplete(
        experimenterPresenter, model);
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
        taskPresenter_ = taskPresenter(method);
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
        coordinateResponseMeasurePresenter->start();
    else if (consonant(m))
        consonantPresenter->start();
    else if (freeResponse(m))
        freeResponsePresenter->start();
    else if (correctKeywords(m))
        correctKeywordsPresenter->start();
    else
        passFailPresenter->start();
}

auto Presenter::taskPresenter(Method m) -> TaskPresenter * {
    if (coordinateResponseMeasure(m))
        return coordinateResponseMeasurePresenter;
    if (consonant(m))
        return consonantPresenter;
    if (freeResponse(m))
        return freeResponsePresenter;
    if (correctKeywords(m))
        return correctKeywordsPresenter;
    return passFailPresenter;
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
    taskPresenter_->showResponseSubmission();
    experimenterPresenter.trialComplete();
    view.showCursor();
}

static void show(Presenter::TestSetup &presenter) { presenter.show(); }

static void switchToTestSetupView(Presenter::TestSetup &testSetup,
    Presenter::Experimenter &experimenter,
    TaskPresenter *coordinateResponseMeasure, TaskPresenter *consonant,
    TaskPresenter *freeResponse, TaskPresenter *correctKeywords,
    TaskPresenter *passFail, TaskPresenter *taskPresenter) {
    show(testSetup);
    experimenter.stop();
    taskPresenter->stop();
}

static void updateTrialInformationAndPlayNext(
    Model &model, View &view, Presenter::Experimenter &experimenter) {
    displayTrialInformation(experimenter, model);
    av_speech_in_noise::playTrial(model, view, experimenter);
}

static void switchToTestSetupViewIfCompleteElse(Model &model,
    Presenter::TestSetup &testSetup, Presenter::Experimenter &experimenter,
    TaskPresenter *coordinateResponseMeasure, TaskPresenter *consonant,
    TaskPresenter *freeResponse, TaskPresenter *correctKeywords,
    TaskPresenter *passFail, TaskPresenter *taskPresenter,
    const std::function<void()> &f) {
    if (testComplete(model))
        switchToTestSetupView(testSetup, experimenter,
            coordinateResponseMeasure, consonant, freeResponse, correctKeywords,
            passFail, taskPresenter);
    else
        f();
}

void Presenter::playNextTrialIfNeeded() {
    switchToTestSetupViewIfCompleteElse(model, testSetup, experimenterPresenter,
        coordinateResponseMeasurePresenter, consonantPresenter,
        freeResponsePresenter, correctKeywordsPresenter, passFailPresenter,
        taskPresenter_, [&]() {
            updateTrialInformationAndPlayNext(
                model, view, experimenterPresenter);
        });
}

void Presenter::readyNextTrialIfNeeded() {
    switchToTestSetupViewIfCompleteElse(model, testSetup, experimenterPresenter,
        coordinateResponseMeasurePresenter, consonantPresenter,
        freeResponsePresenter, correctKeywordsPresenter, passFailPresenter,
        taskPresenter_,
        [&]() { readyNextTrial(experimenterPresenter, model); });
}

void Presenter::declineContinuingTesting() {
    av_speech_in_noise::switchToTestSetupView(testSetup, experimenterPresenter,
        coordinateResponseMeasurePresenter, consonantPresenter,
        freeResponsePresenter, correctKeywordsPresenter, passFailPresenter,
        taskPresenter_);
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
        coordinateResponseMeasurePresenter, consonantPresenter,
        freeResponsePresenter, correctKeywordsPresenter, passFailPresenter,
        taskPresenter_);
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
    view->hideContinueTestingDialog();
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

void Presenter::Experimenter::hideSubmissions() {
    av_speech_in_noise::hideSubmissions(view);
}

void Presenter::Experimenter::setContinueTestingDialogMessage(
    const std::string &s) {
    view->setContinueTestingDialogMessage(s);
}

void Presenter::Experimenter::declineContinuingTesting() {
    parent->declineContinuingTesting();
}

void Presenter::Experimenter::acceptContinuingTesting() {
    parent->acceptContinuingTesting();
}

void Presenter::Experimenter::playTrial() { parent->playTrial(); }

void Presenter::Experimenter::exitTest() { parent->exitTest(); }

void Presenter::Experimenter::display(std::string s) {
    view->display(std::move(s));
}

void Presenter::Experimenter::secondaryDisplay(std::string s) {
    view->secondaryDisplay(std::move(s));
}
}
