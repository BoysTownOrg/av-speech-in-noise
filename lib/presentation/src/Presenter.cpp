#include "Presenter.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/name.hpp>
#include <string>
#include <sstream>
#include <functional>

namespace av_speech_in_noise {
static void displayTrialNumber(Model &model, ExperimenterPresenter *presenter) {
    presenter->display("Trial " + std::to_string(model.trialNumber()));
}

static void displayTarget(Model &model, ExperimenterPresenter *presenter) {
    presenter->secondaryDisplay(model.targetFileName());
}

static void displayTrialInformation(
    Model &model, ExperimenterPresenter *presenter) {
    displayTrialNumber(model, presenter);
    displayTarget(model, presenter);
}

static void readyNextTrial(Model &model, ExperimenterPresenter *presenter) {
    displayTrialInformation(model, presenter);
    presenter->notifyThatNextTrialIsReady();
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
    Model &model, ExperimenterPresenter *experimenterPresenterRefactored) {
    if (testComplete(model)) {
        experimenterPresenterRefactored->hideContinueTestingDialog();
        experimenterPresenterRefactored->showContinueTestingDialog();
        std::stringstream thresholds;
        thresholds << "thresholds (targets: dB SNR)";
        for (const auto &result : model.adaptiveTestResults())
            thresholds << '\n'
                       << result.targetsUrl.path << ": " << result.threshold;
        experimenterPresenterRefactored->setContinueTestingDialogMessage(
            thresholds.str());
    } else
        readyNextTrial(model, experimenterPresenterRefactored);
}

Presenter::Presenter(Model &model, View &view,
    TaskResponder *consonantResponder, TaskPresenter *consonantPresenter,
    TaskResponder *coordinateResponseMeasureResponder,
    TaskPresenter *coordinateResponseMeasurePresenter,
    TaskResponder *freeResponseResponder, TaskPresenter *freeResponsePresenter,
    TaskResponder *correctKeywordsResponder,
    TaskPresenter *correctKeywordsPresenter, TaskResponder *passFailResponder,
    TaskPresenter *passFailPresenter, TestSetupResponder *testSetupResponder,
    TestSetupPresenter *testSetupPresenter,
    ExperimenterResponder *experimenterResponder,
    ExperimenterPresenter *experimenterPresenterRefactored)
    : model{model}, view{view}, consonantPresenter{consonantPresenter},
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
        model, experimenterPresenterRefactored);
}

void Presenter::run() { view.eventLoop(); }

void Presenter::switchToTestView(Method m) {
    testSetupPresenter->stop();
    experimenterPresenterRefactored->start();
    displayTrialInformation(model, experimenterPresenterRefactored);
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
    ExperimenterPresenter *experimenterPresenterRefactored) {
    displayTrialInformation(model, experimenterPresenterRefactored);
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
            updateTrialInformationAndPlayNext(
                model, view, experimenterPresenterRefactored);
        });
}

void Presenter::readyNextTrialIfNeeded() {
    switchToTestSetupViewIfCompleteElse(model, taskPresenter_,
        testSetupPresenter, experimenterPresenterRefactored,
        [&]() { readyNextTrial(model, experimenterPresenterRefactored); });
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
}
