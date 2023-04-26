#include "TestImpl.hpp"

#include <sstream>
#include <functional>

namespace av_speech_in_noise {
static void readyNextTrial(TestPresenter &presenter) {
    presenter.updateTrialInformation();
    presenter.notifyThatNextTrialIsReady();
}

TestControllerImpl::TestControllerImpl(SessionController &sessionController,
    RunningATestFacade &model, RunningATest &runningATest,
    SessionControl &sessionControl, TestControl &control,
    TestPresenter &presenter)
    : sessionController{sessionController}, model{model},
      runningATest{runningATest},
      sessionControl{sessionControl}, presenter{presenter} {
    control.attach(this);
}

static void notifyThatTestIsComplete(SessionController &controller) {
    controller.notifyThatTestIsComplete();
}

void TestControllerImpl::exitTest() {
    notifyThatTestIsComplete(sessionController);
}

static void playTrial(RunningATestFacade &model, SessionControl &control,
    TestPresenter &presenter) {
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
    readyNextTrial(presenter);
}

static void ifTestCompleteElse(RunningATest &model,
    const std::function<void()> &f, const std::function<void()> &g) {
    if (model.testComplete())
        f();
    else
        g();
}

static void readyNextTrialIfTestIncompleteElse(RunningATest &model,
    TestPresenter &presenter, const std::function<void()> &f) {
    presenter.hideResponseSubmission();
    ifTestCompleteElse(model, f, [&]() { readyNextTrial(presenter); });
}

static void notifyIfTestIsCompleteElse(RunningATest &model,
    SessionController &controller, const std::function<void()> &f) {
    ifTestCompleteElse(
        model, [&]() { notifyThatTestIsComplete(controller); }, f);
}

void TestControllerImpl::notifyThatUserIsDoneResponding() {
    readyNextTrialIfTestIncompleteElse(runningATest, presenter, [&]() {
        presenter.completeTask();
        notifyThatTestIsComplete(sessionController);
    });
}

void TestControllerImpl::
    notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion() {
    readyNextTrialIfTestIncompleteElse(runningATest, presenter,
        [&] { presenter.updateAdaptiveTestResults(); });
}

void TestControllerImpl::
    notifyThatUserIsDoneRespondingAndIsReadyForNextTrial() {
    presenter.hideResponseSubmission();
    notifyIfTestIsCompleteElse(runningATest, sessionController, [&]() {
        presenter.updateTrialInformation();
        av_speech_in_noise::playTrial(model, sessionControl, presenter);
    });
}

void TestControllerImpl::notifyThatUserIsReadyForNextTrial() {
    notifyIfTestIsCompleteElse(runningATest, sessionController, [&]() {
        presenter.updateTrialInformation();
        av_speech_in_noise::playTrial(model, sessionControl, presenter);
    });
}

TestPresenterImpl::TestPresenterImpl(RunningATestFacade &model,
    RunningATest &runningATest, AdaptiveMethod &adaptiveMethod, TestView &view,
    UninitializedTaskPresenter *taskPresenter)
    : model{model}, runningATest{runningATest},
      adaptiveMethod{adaptiveMethod}, view{view}, taskPresenter{taskPresenter} {
    runningATest.attach(this);
}

void TestPresenterImpl::start() { view.show(); }

void TestPresenterImpl::stop() {
    taskPresenter->stop();
    view.hideContinueTestingDialog();
    view.hide();
}

void TestPresenterImpl::notifyThatTrialHasStarted() {
    view.hideExitTestButton();
    view.hideNextTrialButton();
    taskPresenter->notifyThatTrialHasStarted();
}

void TestPresenterImpl::trialComplete() {
    view.showExitTestButton();
    taskPresenter->showResponseSubmission();
}

void TestPresenterImpl::notifyThatNextTrialIsReady() {
    view.hideContinueTestingDialog();
    view.showNextTrialButton();
}

void TestPresenterImpl::updateTrialInformation() {
    std::stringstream stream;
    stream << "Trial " << runningATest.trialNumber();
    view.display(stream.str());
    view.secondaryDisplay(runningATest.targetFileName());
}

void TestPresenterImpl::updateAdaptiveTestResults() {
    view.showContinueTestingDialog();
    std::stringstream thresholds;
    thresholds << "thresholds (targets: dB SNR)";
    for (const auto &result : adaptiveMethod.testResults())
        thresholds << '\n'
                   << result.targetsUrl.path << ": " << result.threshold;
    view.setContinueTestingDialogMessage(thresholds.str());
}

void TestPresenterImpl::initialize(TaskPresenter &p) {
    updateTrialInformation();
    taskPresenter->initialize(&p);
    taskPresenter->start();
}

void TestPresenterImpl::hideResponseSubmission() {
    taskPresenter->hideResponseSubmission();
}

void TestPresenterImpl::completeTask() { taskPresenter->complete(); }
}
