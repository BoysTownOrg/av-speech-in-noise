#include "Presenter.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/name.hpp>
#include <string>
#include <sstream>
#include <functional>

namespace av_speech_in_noise {
static void displayTrialNumber(Presenter::Experimenter &experimenterPresenter,
    Model &model, ExperimenterPresenter *experimenterPresenterRefactored) {
    experimenterPresenterRefactored->display(
        "Trial " + std::to_string(model.trialNumber()));
}

static void displayTarget(
    Presenter::Experimenter &experimenterPresenter, Model &model) {
    experimenterPresenter.secondaryDisplay(model.targetFileName());
}

static void displayTrialInformation(
    Presenter::Experimenter &experimenterPresenter, Model &model,
    ExperimenterPresenter *experimenterPresenterRefactored) {
    displayTrialNumber(
        experimenterPresenter, model, experimenterPresenterRefactored);
    displayTarget(experimenterPresenter, model);
}

static void readyNextTrial(Presenter::Experimenter &experimenterPresenter,
    Model &model, ExperimenterPresenter *experimenterPresenterRefactored) {
    displayTrialInformation(
        experimenterPresenter, model, experimenterPresenterRefactored);
    experimenterPresenterRefactored->notifyThatNextTrialIsReady();
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

static void showContinueTestingDialogWithResultsWhenComplete(
    Presenter::Experimenter &experimenterPresenter, Model &model,
    ExperimenterPresenter *experimenterPresenterRefactored) {
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
        readyNextTrial(
            experimenterPresenter, model, experimenterPresenterRefactored);
}

Presenter::Presenter(Model &model, View &view,
    Experimenter &experimenterPresenter, TaskResponder *consonantResponder,
    TaskPresenter *consonantPresenter,
    TaskResponder *coordinateResponseMeasureResponder,
    TaskPresenter *coordinateResponseMeasurePresenter,
    TaskResponder *freeResponseResponder, TaskPresenter *freeResponsePresenter,
    TaskResponder *correctKeywordsResponder,
    TaskPresenter *correctKeywordsPresenter, TaskResponder *passFailResponder,
    TaskPresenter *passFailPresenter, TestSetupResponder *testSetupResponder,
    TestSetupPresenter *testSetupPresenter,
    ExperimenterResponder *experimenterResponder,
    ExperimenterPresenter *experimenterPresenterRefactored)
    : model{model}, view{view}, experimenterPresenter{experimenterPresenter},
      consonantPresenter{consonantPresenter},
      coordinateResponseMeasurePresenter{coordinateResponseMeasurePresenter},
      freeResponsePresenter{freeResponsePresenter},
      correctKeywordsPresenter{correctKeywordsPresenter},
      passFailPresenter{passFailPresenter}, taskPresenter_{passFailPresenter},
      testSetupPresenter{testSetupPresenter},
      experimenterPresenterRefactored{experimenterPresenterRefactored} {
    model.subscribe(this);
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
    if (testSetupResponder != nullptr) {
        testSetupResponder->becomeChild(this);
        testSetupResponder->subscribe(testSetupPresenter);
    }
    if (experimenterResponder != nullptr) {
        experimenterResponder->becomeChild(this);
        experimenterResponder->subscribe(experimenterPresenterRefactored);
    }
    view.populateAudioDeviceMenu(model.audioDevices());
}

void Presenter::showContinueTestingDialogWithResultsWhenComplete() {
    av_speech_in_noise::showContinueTestingDialogWithResultsWhenComplete(
        experimenterPresenter, model, experimenterPresenterRefactored);
}

void Presenter::run() { view.eventLoop(); }

void Presenter::switchToTestView(Method m) {
    testSetupPresenter->stop();
    experimenterPresenterRefactored->start();
    displayTrialInformation(
        experimenterPresenter, model, experimenterPresenterRefactored);
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

static void playTrial(
    Model &model, View &view, ExperimenterPresenter *refactored) {
    AudioSettings p;
    p.audioDevice = view.audioDevice();
    model.playTrial(p);
    refactored->notifyThatTrialHasStarted();
}

void Presenter::playTrial() {
    av_speech_in_noise::playTrial(model, view, experimenterPresenterRefactored);
}

void Presenter::trialComplete() {
    taskPresenter_->showResponseSubmission();
    experimenterPresenterRefactored->notifyThatTrialHasCompleted();
    view.showCursor();
}

static void switchToTestSetupView(TaskPresenter *taskPresenter,
    PresenterSimple *testSetupPresenter,
    PresenterSimple *experimenterPresenter) {
    testSetupPresenter->start();
    experimenterPresenter->stop();
    taskPresenter->stop();
}

static void updateTrialInformationAndPlayNext(Model &model, View &view,
    Presenter::Experimenter &experimenter,
    ExperimenterPresenter *experimenterPresenterRefactored) {
    displayTrialInformation(
        experimenter, model, experimenterPresenterRefactored);
    av_speech_in_noise::playTrial(model, view, experimenterPresenterRefactored);
}

static void switchToTestSetupViewIfCompleteElse(Model &model,
    TaskPresenter *taskPresenter, PresenterSimple *testSetupPresenter,
    PresenterSimple *experimenterPresenter, const std::function<void()> &f) {
    if (testComplete(model))
        switchToTestSetupView(
            taskPresenter, testSetupPresenter, experimenterPresenter);
    else
        f();
}

void Presenter::playNextTrialIfNeeded() {
    switchToTestSetupViewIfCompleteElse(model, taskPresenter_,
        testSetupPresenter, experimenterPresenterRefactored, [&]() {
            updateTrialInformationAndPlayNext(model, view,
                experimenterPresenter, experimenterPresenterRefactored);
        });
}

void Presenter::readyNextTrialIfNeeded() {
    switchToTestSetupViewIfCompleteElse(model, taskPresenter_,
        testSetupPresenter, experimenterPresenterRefactored, [&]() {
            readyNextTrial(
                experimenterPresenter, model, experimenterPresenterRefactored);
        });
}

void Presenter::readyNextTrialAfter(void (Presenter::*f)()) {
    (this->*f)();
    readyNextTrialIfNeeded();
}

void Presenter::switchToTestSetupView() {
    av_speech_in_noise::switchToTestSetupView(
        taskPresenter_, testSetupPresenter, experimenterPresenterRefactored);
}

Presenter::Experimenter::Experimenter(
    ExperimenterView *view, ExperimenterInputView *, ExperimenterOutputView *)
    : view{view} {}

void Presenter::Experimenter::showContinueTestingDialog() {
    view->showContinueTestingDialog();
}

void Presenter::Experimenter::hideSubmissions() {
    view->hideContinueTestingDialog();
}

void Presenter::Experimenter::setContinueTestingDialogMessage(
    const std::string &s) {
    view->setContinueTestingDialogMessage(s);
}

void Presenter::Experimenter::display(std::string s) {
    view->display(std::move(s));
}

void Presenter::Experimenter::secondaryDisplay(std::string s) {
    view->secondaryDisplay(std::move(s));
}
}
