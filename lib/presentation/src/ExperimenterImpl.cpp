#include "ExperimenterImpl.hpp"
#include <sstream>

namespace av_speech_in_noise {
static void displayTrialNumber(
    Model &model, ExperimenterResponder::EventListener *presenter) {
    presenter->display("Trial " + std::to_string(model.trialNumber()));
}

static void displayTarget(
    Model &model, ExperimenterResponder::EventListener *presenter) {
    presenter->secondaryDisplay(model.targetFileName());
}

static void displayTrialInformation(
    Model &model, ExperimenterResponder::EventListener *presenter) {
    displayTrialNumber(model, presenter);
    displayTarget(model, presenter);
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

void ExperimenterResponderImpl::exitTest() { parent->switchToTestSetupView(); }

static void playTrial(
    Model &model, View &view, ExperimenterResponder::EventListener *listener) {
    model.playTrial(AudioSettings{view.audioDevice()});
    listener->notifyThatTrialHasStarted();
}

void ExperimenterResponderImpl::playTrial() {
    av_speech_in_noise::playTrial(model, mainView, listener);
}

void ExperimenterResponderImpl::declineContinuingTesting() {
    parent->switchToTestSetupView();
}

void ExperimenterResponderImpl::acceptContinuingTesting() {
    model.restartAdaptiveTestWhilePreservingTargets();
    displayTrialInformation(model, listener);
    listener->notifyThatNextTrialIsReady();
}

void ExperimenterResponderImpl::
    showContinueTestingDialogWithResultsWhenComplete() {
    if (model.testComplete()) {
        listener->showContinueTestingDialog();
        std::stringstream thresholds;
        thresholds << "thresholds (targets: dB SNR)";
        for (const auto &result : model.adaptiveTestResults())
            thresholds << '\n'
                       << result.targetsUrl.path << ": " << result.threshold;
        listener->setContinueTestingDialogMessage(thresholds.str());
    } else
        readyNextTrial(model, listener);
}

void ExperimenterResponderImpl::readyNextTrialIfNeeded() {
    if (model.testComplete())
        parent->switchToTestSetupView();
    else {
        displayTrialInformation(model, listener);
        listener->notifyThatNextTrialIsReady();
    }
}

void ExperimenterResponderImpl::playNextTrialIfNeeded() {
    if (model.testComplete())
        parent->switchToTestSetupView();
    else {
        displayTrialInformation(model, listener);
        av_speech_in_noise::playTrial(model, mainView, listener);
    }
}

void ExperimenterResponderImpl::subscribe(IPresenter *p) { parent = p; }

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
    taskPresenter_ = taskPresenter(m);
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
