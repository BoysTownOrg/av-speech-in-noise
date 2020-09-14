#include "ExperimenterImpl.hpp"
#include <sstream>
#include <functional>

namespace av_speech_in_noise {
static void displayTrialInformation(
    Model &model, TestController::Observer *presenter) {
    presenter->display("Trial " + std::to_string(model.trialNumber()));
    presenter->secondaryDisplay(model.targetFileName());
}

static void readyNextTrial(Model &model, TestController::Observer *presenter) {
    displayTrialInformation(model, presenter);
    presenter->notifyThatNextTrialIsReady();
}

ExperimenterControllerImpl::ExperimenterControllerImpl(Model &model,
    SessionView &mainView, ExperimenterControl &view,
    TaskController *consonantController, TaskPresenter *consonantPresenter,
    TaskController *coordinateResponseMeasureController,
    TaskPresenter *coordinateResponseMeasurePresenter,
    TaskController *freeResponseController,
    TaskPresenter *freeResponsePresenter,
    TaskController *correctKeywordsController,
    TaskPresenter *correctKeywordsPresenter, TaskController *passFailController,
    TaskPresenter *passFailPresenter)
    : model{model}, mainView{mainView} {
    view.attach(this);
    if (consonantController != nullptr) {
        consonantController->attach(this);
        consonantController->attach(consonantPresenter);
    }
    if (freeResponseController != nullptr) {
        freeResponseController->attach(this);
        freeResponseController->attach(freeResponsePresenter);
    }
    if (correctKeywordsController != nullptr) {
        correctKeywordsController->attach(this);
        correctKeywordsController->attach(correctKeywordsPresenter);
    }
    if (passFailController != nullptr) {
        passFailController->attach(this);
        passFailController->attach(passFailPresenter);
    }
    if (coordinateResponseMeasureController != nullptr) {
        coordinateResponseMeasureController->attach(this);
        coordinateResponseMeasureController->attach(
            coordinateResponseMeasurePresenter);
    }
}

void ExperimenterControllerImpl::attach(TestController::Observer *e) {
    listener = e;
}

static void notifyThatTestIsComplete(SessionController *presenter) {
    presenter->notifyThatTestIsComplete();
}

void ExperimenterControllerImpl::exitTest() {
    notifyThatTestIsComplete(responder);
}

static void playTrial(
    Model &model, SessionView &view, TestController::Observer *listener) {
    model.playTrial(AudioSettings{view.audioDevice()});
    listener->notifyThatTrialHasStarted();
}

void ExperimenterControllerImpl::playTrial() {
    av_speech_in_noise::playTrial(model, mainView, listener);
}

void ExperimenterControllerImpl::declineContinuingTesting() {
    notifyThatTestIsComplete(responder);
}

void ExperimenterControllerImpl::acceptContinuingTesting() {
    model.restartAdaptiveTestWhilePreservingTargets();
    readyNextTrial(model, listener);
}

static void ifTestCompleteElse(Model &model, const std::function<void()> &f,
    const std::function<void()> &g) {
    if (model.testComplete())
        f();
    else
        g();
}

static void readyNextTrialIfTestIncompleteElse(Model &model,
    TestController::Observer *listener, const std::function<void()> &f) {
    ifTestCompleteElse(model, f, [&]() { readyNextTrial(model, listener); });
}

static void notifyIfTestIsCompleteElse(Model &model,
    SessionController *responder, const std::function<void()> &f) {
    ifTestCompleteElse(
        model, [&]() { notifyThatTestIsComplete(responder); }, f);
}

void ExperimenterControllerImpl::
    notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion() {
    readyNextTrialIfTestIncompleteElse(model, listener, [&] {
        listener->showContinueTestingDialog();
        std::stringstream thresholds;
        thresholds << "thresholds (targets: dB SNR)";
        for (const auto &result : model.adaptiveTestResults())
            thresholds << '\n'
                       << result.targetsUrl.path << ": " << result.threshold;
        listener->setContinueTestingDialogMessage(thresholds.str());
    });
}

void ExperimenterControllerImpl::notifyThatUserIsDoneResponding() {
    notifyIfTestIsCompleteElse(
        model, responder, [&]() { readyNextTrial(model, listener); });
}

void ExperimenterControllerImpl::notifyThatUserIsReadyForNextTrial() {
    notifyIfTestIsCompleteElse(model, responder, [&]() {
        displayTrialInformation(model, listener);
        av_speech_in_noise::playTrial(model, mainView, listener);
    });
}

void ExperimenterControllerImpl::attach(SessionController *p) { responder = p; }

ExperimenterPresenterImpl::ExperimenterPresenterImpl(Model &model,
    TestView &view, TaskPresenter *consonantPresenter,
    TaskPresenter *coordinateResponseMeasurePresenter,
    TaskPresenter *freeResponsePresenter,
    TaskPresenter *correctKeywordsPresenter, TaskPresenter *passFailPresenter,
    UninitializedTaskPresenter *taskPresenter_)
    : model{model}, view{view}, consonantPresenter{consonantPresenter},
      coordinateResponseMeasurePresenter{coordinateResponseMeasurePresenter},
      freeResponsePresenter{freeResponsePresenter},
      correctKeywordsPresenter{correctKeywordsPresenter},
      passFailPresenter{passFailPresenter}, taskPresenter_{taskPresenter_} {
    model.attach(this);
}

void ExperimenterPresenterImpl::start() { view.show(); }

void ExperimenterPresenterImpl::stop() {
    taskPresenter_->stop();
    view.hideContinueTestingDialog();
    view.hide();
}

void ExperimenterPresenterImpl::notifyThatTrialHasStarted() {
    view.hideExitTestButton();
    view.hideNextTrialButton();
    taskPresenter_->notifyThatTrialHasStarted();
}

void ExperimenterPresenterImpl::trialComplete() {
    view.showExitTestButton();
    taskPresenter_->showResponseSubmission();
}

void ExperimenterPresenterImpl::notifyThatNextTrialIsReady() {
    view.hideContinueTestingDialog();
    view.showNextTrialButton();
}

void ExperimenterPresenterImpl::display(const std::string &s) {
    view.display(s);
}

void ExperimenterPresenterImpl::secondaryDisplay(const std::string &s) {
    view.secondaryDisplay(s);
}

void ExperimenterPresenterImpl::showContinueTestingDialog() {
    view.showContinueTestingDialog();
}

void ExperimenterPresenterImpl::setContinueTestingDialogMessage(
    const std::string &s) {
    view.setContinueTestingDialogMessage(s);
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

void ExperimenterPresenterImpl::initialize(Method m) {
    displayTrialInformation(model, this);
    taskPresenter_->initialize(taskPresenter(m));
    taskPresenter_->start();
}

auto ExperimenterPresenterImpl::taskPresenter(Method m) -> TaskPresenter * {
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
}
