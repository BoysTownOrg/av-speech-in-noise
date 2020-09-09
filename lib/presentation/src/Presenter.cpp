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
        consonantResponder->subscribe(consonantPresenter);
        consonantResponder->subscribe(experimenterResponder);
    }
    if (freeResponseResponder != nullptr) {
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
        coordinateResponseMeasureResponder->subscribe(experimenterResponder);
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

void Presenter::prepare(Method m) {
    switchToTestView(m);
    taskPresenter_ = taskPresenter(m);
}

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

void Presenter::readyNextTrial() {
    av_speech_in_noise::readyNextTrial(model, experimenterPresenter);
}

void Presenter::switchToTestSetupView() {
    av_speech_in_noise::switchToTestSetupView(
        taskPresenter_, testSetupPresenter, experimenterPresenter);
}
}
