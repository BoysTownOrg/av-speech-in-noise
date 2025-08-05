#include "TestImpl.hpp"

#include <av-speech-in-noise/core/Configuration.hpp>

#include <sstream>
#include <functional>
#include <variant>

namespace av_speech_in_noise {
static void readyNextTrial(TestPresenter &presenter) {
    presenter.updateTrialInformation();
    presenter.notifyThatNextTrialIsReady();
}

TestControllerImpl::TestControllerImpl(SessionController &sessionController,
    RunningATest &runningATest, AdaptiveMethod &adaptiveMethod,
    SessionControl &sessionControl, TestControl &control,
    TestPresenter &presenter)
    : sessionController{sessionController}, runningATest{runningATest},
      adaptiveMethod{adaptiveMethod}, sessionControl{sessionControl},
      presenter{presenter} {
    control.attach(this);
}

static void notifyThatTestIsComplete(SessionController &controller) {
    controller.notifyThatTestIsComplete();
}

void TestControllerImpl::exitTest() {
    notifyThatTestIsComplete(sessionController);
}

static void playTrial(
    RunningATest &model, SessionControl &control, TestPresenter &presenter) {
    model.playTrial(AudioSettings{control.audioDevice()});
    presenter.notifyThatTrialHasStarted();
}

void TestControllerImpl::playTrial() {
    av_speech_in_noise::playTrial(runningATest, sessionControl, presenter);
}

void TestControllerImpl::declineContinuingTesting() {
    notifyThatTestIsComplete(sessionController);
}

void TestControllerImpl::acceptContinuingTesting() {
    adaptiveMethod.resetTracks();
    runningATest.prepareNextTrialIfNeeded();
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
        av_speech_in_noise::playTrial(runningATest, sessionControl, presenter);
    });
}

void TestControllerImpl::notifyThatUserIsReadyForNextTrial() {
    notifyIfTestIsCompleteElse(runningATest, sessionController, [&]() {
        presenter.updateTrialInformation();
        av_speech_in_noise::playTrial(runningATest, sessionControl, presenter);
    });
}

void TestControllerImpl::notifyThatUserHasRespondedButTrialIsNotQuiteDone() {
    presenter.hideResponseSubmission();
    presenter.hideExitTestButton();
}

TestPresenterImpl::TestPresenterImpl(
    RunningATest &runningATest, AdaptiveMethod &adaptiveMethod, TestView &view)
    : runningATest{runningATest}, adaptiveMethod{adaptiveMethod}, view{view},
      taskPresenter{} {
    runningATest.attach(this);
}

void TestPresenterImpl::subscribe(ConfigurationRegistry &registry) {
    registry.subscribe(*this, "show target filename");
}

void TestPresenterImpl::configure(
    const std::string &key, const std::string &value) {
    if (key == "show target filename")
        showTargetFilename = boolean(value);
}

void TestPresenterImpl::start() {
    view.show();
    updateTrialInformation();
    if (taskPresenter != nullptr)
        taskPresenter->start();
}

void TestPresenterImpl::stop() {
    if (taskPresenter != nullptr)
        taskPresenter->stop();
    view.hideContinueTestingDialog();
    view.hide();
}

void TestPresenterImpl::notifyThatTrialHasStarted() {
    view.hideExitTestButton();
    view.hideNextTrialButton();
    if (taskPresenter != nullptr)
        taskPresenter->notifyThatTrialHasStarted();
}

void TestPresenterImpl::notifyThatPlayTrialHasCompleted() {
    view.showExitTestButton();
    if (taskPresenter != nullptr)
        taskPresenter->showResponseSubmission();
}

void TestPresenterImpl::notifyThatNextTrialIsReady() {
    view.hideContinueTestingDialog();
    view.showNextTrialButton();
    view.showExitTestButton();
}

void TestPresenterImpl::updateTrialInformation() {
    std::stringstream stream;
    stream << "Trial " << runningATest.trialNumber();
    view.display(stream.str());
    if (showTargetFilename)
        view.secondaryDisplay(runningATest.targetFileName());
}

void TestPresenterImpl::updateAdaptiveTestResults() {
    view.showContinueTestingDialog();
    std::stringstream thresholds;
    thresholds << "thresholds (targets: dB SNR)";
    for (const auto &result : adaptiveMethod.testResults())
        std::visit(
            [&thresholds, &result](auto arg) {
                thresholds << '\n' << result.targetsUrl.path << ": " << arg;
            },
            result.result);
    view.setContinueTestingDialogMessage(thresholds.str());
}

void TestPresenterImpl::initialize(TaskPresenter &p) { taskPresenter = &p; }

void TestPresenterImpl::hideExitTestButton() { view.hideExitTestButton(); }

void TestPresenterImpl::hideResponseSubmission() {
    if (taskPresenter != nullptr)
        taskPresenter->hideResponseSubmission();
}

void TestPresenterImpl::completeTask() {
    if (taskPresenter != nullptr)
        taskPresenter->complete();
}
}
