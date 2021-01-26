#include "TestImpl.hpp"
#include <sstream>
#include <functional>

namespace av_speech_in_noise {
static void displayTrialInformation(Model &model, TestPresenter &presenter) {
    std::stringstream stream;
    stream << "Trial " << model.trialNumber();
    presenter.display(stream.str());
    presenter.secondaryDisplay(model.targetFileName());
}

static void readyNextTrial(Model &model, TestPresenter &presenter) {
    displayTrialInformation(model, presenter);
    presenter.notifyThatNextTrialIsReady();
}

TestControllerImpl::TestControllerImpl(SessionController &sessionController,
    Model &model, SessionControl &sessionControl, TestControl &control,
    TestPresenter &presenter)
    : sessionController{sessionController}, model{model},
      sessionControl{sessionControl}, presenter{presenter} {
    control.attach(this);
}

static void notifyThatTestIsComplete(SessionController &controller) {
    controller.notifyThatTestIsComplete();
}

void TestControllerImpl::exitTest() {
    notifyThatTestIsComplete(sessionController);
}

static void playTrial(
    Model &model, SessionControl &control, TestPresenter &presenter) {
    model.playTrial(AudioSettings{control.audioDevice()});
    presenter.notifyThatTrialHasStarted();
}

void TestControllerImpl::playTrial() {
    av_speech_in_noise::playTrial(model, sessionControl, presenter);
}

void TestControllerImpl::declineContinuingTesting() {
    notifyThatTestIsComplete(sessionController);
}

void TestControllerImpl::acceptContinuingTesting() {
    model.restartAdaptiveTestWhilePreservingTargets();
    readyNextTrial(model, presenter);
}

static void ifTestCompleteElse(Model &model, const std::function<void()> &f,
    const std::function<void()> &g) {
    if (model.testComplete())
        f();
    else
        g();
}

static void readyNextTrialIfTestIncompleteElse(
    Model &model, TestPresenter &presenter, const std::function<void()> &f) {
    ifTestCompleteElse(model, f, [&]() { readyNextTrial(model, presenter); });
}

static void notifyIfTestIsCompleteElse(Model &model,
    SessionController &controller, const std::function<void()> &f) {
    ifTestCompleteElse(
        model, [&]() { notifyThatTestIsComplete(controller); }, f);
}

void TestControllerImpl::
    notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion() {
    presenter.hideResponseSubmission();
    readyNextTrialIfTestIncompleteElse(
        model, presenter, [&] { presenter.updateAdaptiveTestResults(); });
}

void TestControllerImpl::notifyThatUserIsDoneResponding() {
    presenter.hideResponseSubmission();
    notifyIfTestIsCompleteElse(
        model, sessionController, [&]() { readyNextTrial(model, presenter); });
}

void TestControllerImpl::notifyThatUserIsReadyForNextTrial() {
    notifyIfTestIsCompleteElse(model, sessionController, [&]() {
        displayTrialInformation(model, presenter);
        av_speech_in_noise::playTrial(model, sessionControl, presenter);
    });
}

void TestControllerImpl::
    notifyThatUserIsDoneRespondingAndIsReadyForNextTrial() {
    presenter.hideResponseSubmission();
    notifyIfTestIsCompleteElse(model, sessionController, [&]() {
        displayTrialInformation(model, presenter);
        av_speech_in_noise::playTrial(model, sessionControl, presenter);
    });
}

TestPresenterImpl::TestPresenterImpl(
    Model &model, TestView &view, UninitializedTaskPresenter *taskPresenter_)
    : model{model}, view{view}, taskPresenter_{taskPresenter_} {
    model.attach(this);
}

void TestPresenterImpl::start() { view.show(); }

void TestPresenterImpl::stop() {
    taskPresenter_->stop();
    view.hideContinueTestingDialog();
    view.hide();
}

void TestPresenterImpl::notifyThatTrialHasStarted() {
    view.hideExitTestButton();
    view.hideNextTrialButton();
}

void TestPresenterImpl::trialComplete() {
    view.showExitTestButton();
    taskPresenter_->showResponseSubmission();
}

void TestPresenterImpl::notifyThatNextTrialIsReady() {
    view.hideContinueTestingDialog();
    view.showNextTrialButton();
}

void TestPresenterImpl::display(const std::string &s) { view.display(s); }

void TestPresenterImpl::secondaryDisplay(const std::string &s) {
    view.secondaryDisplay(s);
}

void TestPresenterImpl::updateAdaptiveTestResults() {
    view.showContinueTestingDialog();
    std::stringstream thresholds;
    thresholds << "thresholds (targets: dB SNR)";
    for (const auto &result : model.adaptiveTestResults())
        thresholds << '\n'
                   << result.targetsUrl.path << ": " << result.threshold;
    view.setContinueTestingDialogMessage(thresholds.str());
}

void TestPresenterImpl::initialize(TaskPresenter &p) {
    displayTrialInformation(model, *this);
    taskPresenter_->initialize(&p);
    taskPresenter_->start();
}

void TestPresenterImpl::hideResponseSubmission() {
    taskPresenter_->hideResponseSubmission();
}
}
