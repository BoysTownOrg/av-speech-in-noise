#include "ExperimenterImpl.hpp"
#include <sstream>
#include <functional>

namespace av_speech_in_noise {
static void displayTrialInformation(
    Model &model, ExperimenterResponder::EventListener *presenter) {
    presenter->display("Trial " + std::to_string(model.trialNumber()));
    presenter->secondaryDisplay(model.targetFileName());
}

static void readyNextTrial(
    Model &model, ExperimenterResponder::EventListener *presenter) {
    displayTrialInformation(model, presenter);
    presenter->notifyThatNextTrialIsReady();
}

ExperimenterResponderImpl::ExperimenterResponderImpl(Model &model,
    View &mainView, ExperimenterInputView &view,
    TaskResponder *consonantResponder, TaskPresenter *consonantPresenter,
    TaskResponder *coordinateResponseMeasureResponder,
    TaskPresenter *coordinateResponseMeasurePresenter,
    TaskResponder *freeResponseResponder, TaskPresenter *freeResponsePresenter,
    TaskResponder *correctKeywordsResponder,
    TaskPresenter *correctKeywordsPresenter, TaskResponder *passFailResponder,
    TaskPresenter *passFailPresenter)
    : model{model}, mainView{mainView} {
    view.subscribe(this);
    if (consonantResponder != nullptr) {
        consonantResponder->subscribe(this);
        consonantResponder->subscribe(consonantPresenter);
    }
    if (freeResponseResponder != nullptr) {
        freeResponseResponder->subscribe(this);
        freeResponseResponder->subscribe(freeResponsePresenter);
    }
    if (correctKeywordsResponder != nullptr) {
        correctKeywordsResponder->subscribe(this);
        correctKeywordsResponder->subscribe(correctKeywordsPresenter);
    }
    if (passFailResponder != nullptr) {
        passFailResponder->subscribe(this);
        passFailResponder->subscribe(passFailPresenter);
    }
    if (coordinateResponseMeasureResponder != nullptr) {
        coordinateResponseMeasureResponder->subscribe(this);
        coordinateResponseMeasureResponder->subscribe(
            coordinateResponseMeasurePresenter);
    }
}

void ExperimenterResponderImpl::subscribe(
    ExperimenterResponder::EventListener *e) {
    listener = e;
}

static void notifyThatTestIsComplete(IPresenter *presenter) {
    presenter->notifyThatTestIsComplete();
}

void ExperimenterResponderImpl::exitTest() {
    notifyThatTestIsComplete(responder);
}

static void playTrial(
    Model &model, View &view, ExperimenterResponder::EventListener *listener) {
    model.playTrial(AudioSettings{view.audioDevice()});
    listener->notifyThatTrialHasStarted();
}

void ExperimenterResponderImpl::playTrial() {
    av_speech_in_noise::playTrial(model, mainView, listener);
}

void ExperimenterResponderImpl::declineContinuingTesting() {
    notifyThatTestIsComplete(responder);
}

void ExperimenterResponderImpl::acceptContinuingTesting() {
    model.restartAdaptiveTestWhilePreservingTargets();
    readyNextTrial(model, listener);
}

static void readyNextTrialIfTestIncompleteElse(Model &model,
    ExperimenterResponder::EventListener *listener,
    const std::function<void()> &f) {
    if (model.testComplete())
        f();
    else
        readyNextTrial(model, listener);
}

static void notifyIfTestIsCompleteElse(
    Model &model, IPresenter *responder, const std::function<void()> &f) {
    if (model.testComplete())
        notifyThatTestIsComplete(responder);
    else
        f();
}

void ExperimenterResponderImpl::
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

void ExperimenterResponderImpl::notifyThatUserIsDoneResponding() {
    notifyIfTestIsCompleteElse(
        model, responder, [&]() { readyNextTrial(model, listener); });
}

void ExperimenterResponderImpl::notifyThatUserIsReadyForNextTrial() {
    notifyIfTestIsCompleteElse(model, responder, [&]() {
        displayTrialInformation(model, listener);
        av_speech_in_noise::playTrial(model, mainView, listener);
    });
}

void ExperimenterResponderImpl::subscribe(IPresenter *p) { responder = p; }

ExperimenterPresenterImpl::ExperimenterPresenterImpl(Model &model,
    ExperimenterOutputView &view, TaskPresenter *consonantPresenter,
    TaskPresenter *coordinateResponseMeasurePresenter,
    TaskPresenter *freeResponsePresenter,
    TaskPresenter *correctKeywordsPresenter, TaskPresenter *passFailPresenter)
    : model{model}, view{view}, consonantPresenter{consonantPresenter},
      coordinateResponseMeasurePresenter{coordinateResponseMeasurePresenter},
      freeResponsePresenter{freeResponsePresenter},
      correctKeywordsPresenter{correctKeywordsPresenter},
      passFailPresenter{passFailPresenter}, taskPresenter_{passFailPresenter} {
    model.subscribe(this);
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
    taskPresenter_ = taskPresenter(m);
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
