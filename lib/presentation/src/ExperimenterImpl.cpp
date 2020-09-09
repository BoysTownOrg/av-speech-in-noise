#include "ExperimenterImpl.hpp"
#include <sstream>

namespace av_speech_in_noise {
ExperimenterResponderImpl::ExperimenterResponderImpl(
    Model &model, View &mainView, ExperimenterInputView &view)
    : model{model}, mainView{mainView} {
    view.subscribe(this);
}

void ExperimenterResponderImpl::subscribe(
    ExperimenterResponder::EventListener *e) {
    listener = e;
}

void ExperimenterResponderImpl::exitTest() { parent->switchToTestSetupView(); }

void ExperimenterResponderImpl::playTrial() {
    AudioSettings p;
    p.audioDevice = mainView.audioDevice();
    model.playTrial(p);
    listener->notifyThatTrialHasStarted();
}

void ExperimenterResponderImpl::declineContinuingTesting() {
    parent->switchToTestSetupView();
}

void ExperimenterResponderImpl::acceptContinuingTesting() {
    model.restartAdaptiveTestWhilePreservingTargets();
    listener->display("Trial " + std::to_string(model.trialNumber()));
    listener->secondaryDisplay(model.targetFileName());
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
        parent->readyNextTrial();
}

void ExperimenterResponderImpl::readyNextTrialIfNeeded() {
    if (model.testComplete())
        parent->switchToTestSetupView();
    else {
        listener->display("Trial " + std::to_string(model.trialNumber()));
        listener->secondaryDisplay(model.targetFileName());
        listener->notifyThatNextTrialIsReady();
    }
}

void ExperimenterResponderImpl::playNextTrialIfNeeded() {
    if (model.testComplete())
        parent->switchToTestSetupView();
    else {
        listener->display("Trial " + std::to_string(model.trialNumber()));
        listener->secondaryDisplay(model.targetFileName());
        AudioSettings p;
        p.audioDevice = mainView.audioDevice();
        model.playTrial(p);
        listener->notifyThatTrialHasStarted();
    }
}

void ExperimenterResponderImpl::becomeChild(IPresenter *p) { parent = p; }

ExperimenterPresenterImpl::ExperimenterPresenterImpl(
    ExperimenterOutputView &view)
    : view{view} {}

void ExperimenterPresenterImpl::start() { view.show(); }

void ExperimenterPresenterImpl::stop() {
    view.hideContinueTestingDialog();
    view.hide();
}

void ExperimenterPresenterImpl::notifyThatTrialHasStarted() {
    view.hideExitTestButton();
    view.hideNextTrialButton();
}

void ExperimenterPresenterImpl::notifyThatTrialHasCompleted() {
    view.showExitTestButton();
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
}
