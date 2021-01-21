#include "TestImpl.hpp"
#include <sstream>
#include <functional>
#include <utility>

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

TestControllerImpl::TestControllerImpl(
    Model &model, SessionView &sessionView, TestControl &control)
    : model{model}, sessionView{sessionView} {
    control.attach(this);
}

void TestControllerImpl::attach(TestController::Observer *e) { observer = e; }

static void notifyThatTestIsComplete(SessionController *presenter) {
    presenter->notifyThatTestIsComplete();
}

void TestControllerImpl::exitTest() { notifyThatTestIsComplete(controller); }

static void playTrial(
    Model &model, SessionView &view, TestController::Observer *observer) {
    model.playTrial(AudioSettings{view.audioDevice()});
    observer->notifyThatTrialHasStarted();
}

void TestControllerImpl::playTrial() {
    av_speech_in_noise::playTrial(model, sessionView, observer);
}

void TestControllerImpl::declineContinuingTesting() {
    notifyThatTestIsComplete(controller);
}

void TestControllerImpl::acceptContinuingTesting() {
    model.restartAdaptiveTestWhilePreservingTargets();
    readyNextTrial(model, observer);
}

static void ifTestCompleteElse(Model &model, const std::function<void()> &f,
    const std::function<void()> &g) {
    if (model.testComplete())
        f();
    else
        g();
}

static void readyNextTrialIfTestIncompleteElse(Model &model,
    TestController::Observer *observer, const std::function<void()> &f) {
    ifTestCompleteElse(model, f, [&]() { readyNextTrial(model, observer); });
}

static void notifyIfTestIsCompleteElse(Model &model,
    SessionController *controller, const std::function<void()> &f) {
    ifTestCompleteElse(
        model, [&]() { notifyThatTestIsComplete(controller); }, f);
}

void TestControllerImpl::
    notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion() {
    observer->hideResponseSubmission();
    readyNextTrialIfTestIncompleteElse(model, observer, [&] {
        observer->showContinueTestingDialog();
        std::stringstream thresholds;
        thresholds << "thresholds (targets: dB SNR)";
        for (const auto &result : model.adaptiveTestResults())
            thresholds << '\n'
                       << result.targetsUrl.path << ": " << result.threshold;
        observer->setContinueTestingDialogMessage(thresholds.str());
    });
}

void TestControllerImpl::notifyThatUserIsDoneResponding() {
    observer->hideResponseSubmission();
    notifyIfTestIsCompleteElse(
        model, controller, [&]() { readyNextTrial(model, observer); });
}

void TestControllerImpl::notifyThatUserIsReadyForNextTrial() {
    notifyIfTestIsCompleteElse(model, controller, [&]() {
        displayTrialInformation(model, observer);
        av_speech_in_noise::playTrial(model, sessionView, observer);
    });
}

void TestControllerImpl::attach(SessionController *p) { controller = p; }

TestPresenterImpl::TestPresenterImpl(Model &model, TestView &view,
    std::map<Method, TaskPresenter &> taskPresenters,
    UninitializedTaskPresenter *taskPresenter_)
    : taskPresenters{std::move(taskPresenters)}, model{model}, view{view},
      taskPresenter_{taskPresenter_} {
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
    taskPresenter_->notifyThatTrialHasStarted();
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

void TestPresenterImpl::showContinueTestingDialog() {
    view.showContinueTestingDialog();
}

void TestPresenterImpl::setContinueTestingDialogMessage(const std::string &s) {
    view.setContinueTestingDialogMessage(s);
}

void TestPresenterImpl::initialize(Method m) {
    displayTrialInformation(model, this);
    taskPresenter_->initialize(&taskPresenters.at(m));
    taskPresenter_->start();
}

void TestPresenterImpl::hideResponseSubmission() {
    taskPresenter_->hideResponseSubmission();
}
}
