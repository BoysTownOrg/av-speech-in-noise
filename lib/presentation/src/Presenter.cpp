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
    ExperimenterPresenter *experimenterPresenter)
    : model{model}, view{view}, consonantPresenter{consonantPresenter},
      coordinateResponseMeasurePresenter{coordinateResponseMeasurePresenter},
      freeResponsePresenter{freeResponsePresenter},
      correctKeywordsPresenter{correctKeywordsPresenter},
      passFailPresenter{passFailPresenter}, taskPresenter_{passFailPresenter},
      testSetupPresenter{testSetupPresenter}, experimenterPresenter{
                                                  experimenterPresenter} {
    model.subscribe(this);
    if (consonantResponder != nullptr) {
        consonantResponder->becomeChild(this);
        consonantResponder->subscribe(consonantPresenter);
    }
    if (freeResponseResponder != nullptr) {
        freeResponseResponder->becomeChild(this);
        freeResponseResponder->subscribe(experimenterResponder);
        freeResponseResponder->subscribe(freeResponsePresenter);
    }
    if (correctKeywordsResponder != nullptr) {
        correctKeywordsResponder->subscribe(correctKeywordsPresenter);
        correctKeywordsResponder->subscribe(experimenterResponder);
    }
    if (passFailResponder != nullptr) {
        passFailResponder->subscribe(passFailPresenter);
        passFailResponder->subscribe(experimenterResponder);
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
        experimenterResponder->subscribe(experimenterPresenter);
    }
    view.populateAudioDeviceMenu(model.audioDevices());
}

void Presenter::run() { view.eventLoop(); }

void Presenter::switchToTestView(Method m) {
    testSetupPresenter->stop();
    experimenterPresenter->start();
    displayTrialInformation(model, experimenterPresenter);
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
    av_speech_in_noise::playTrial(model, view, experimenterPresenter);
}

void Presenter::trialComplete() {
    taskPresenter_->showResponseSubmission();
    experimenterPresenter->notifyThatTrialHasCompleted();
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
        testSetupPresenter, experimenterPresenter, [&]() {
            updateTrialInformationAndPlayNext(
                model, view, experimenterPresenter);
        });
}

void Presenter::readyNextTrial() {
    av_speech_in_noise::readyNextTrial(model, experimenterPresenter);
}

void Presenter::readyNextTrialIfNeeded() {}

void Presenter::readyNextTrialAfter(void (Presenter::*f)()) {
    (this->*f)();
    readyNextTrialIfNeeded();
}

void Presenter::switchToTestSetupView() {
    av_speech_in_noise::switchToTestSetupView(
        taskPresenter_, testSetupPresenter, experimenterPresenter);
}
}
