#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "TestViewStub.hpp"
#include "SessionViewStub.hpp"
#include <presentation/TestImpl.hpp>
#include <av-speech-in-noise/name.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include <utility>

namespace av_speech_in_noise {
namespace {
class TestControlStub : public TestControl {
  public:
    void attach(TestControl::Observer *e) override { observer = e; }

    void declineContinuingTesting() { observer->declineContinuingTesting(); }

    void acceptContinuingTesting() { observer->acceptContinuingTesting(); }

    void exitTest() { observer->exitTest(); }

    void playTrial() { observer->playTrial(); }

  private:
    TestControl::Observer *observer{};
};

class TaskPresenterStub : public TaskPresenter {
  public:
    void showResponseSubmission() override {}
    void notifyThatTaskHasStarted() override {}
    void notifyThatUserIsDoneResponding() override {}
    void notifyThatTrialHasStarted() override {}
    void start() override {}
    void stop() override {}
};

class PresenterUseCase {
  public:
    virtual ~PresenterUseCase() = default;
    virtual void run(TestPresenter &) = 0;
};

class InitializingAdaptiveCoordinateResponseMeasureMethod
    : public PresenterUseCase {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(Method::adaptiveCoordinateResponseMeasure);
    }
};

class InitializingAdaptiveCoordinateResponseMeasureMethodWithSingleSpeaker
    : public PresenterUseCase {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(
            Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker);
    }
};

class InitializingAdaptiveCoordinateResponseMeasureMethodWithEyeTracking
    : public PresenterUseCase {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(
            Method::adaptiveCoordinateResponseMeasureWithEyeTracking);
    }
};

class InitializingAdaptiveCoordinateResponseMeasureMethodWithDelayedMasker
    : public PresenterUseCase {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(
            Method::adaptiveCoordinateResponseMeasureWithDelayedMasker);
    }
};

class InitializingAdaptivePassFailMethod : public PresenterUseCase {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(Method::adaptivePassFail);
    }
};

class InitializingAdaptivePassFailMethodWithEyeTracking
    : public PresenterUseCase {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(Method::adaptivePassFailWithEyeTracking);
    }
};

class InitializingAdaptiveCorrectKeywordsMethod : public PresenterUseCase {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(Method::adaptiveCorrectKeywords);
    }
};

class InitializingAdaptiveCorrectKeywordsMethodWithEyeTracking
    : public PresenterUseCase {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(Method::adaptiveCorrectKeywordsWithEyeTracking);
    }
};

class InitializingFixedLevelFreeResponseWithTargetReplacementMethod
    : public PresenterUseCase {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(
            Method::fixedLevelFreeResponseWithTargetReplacement);
    }
};

class InitializingFixedLevelConsonantMethod : public PresenterUseCase {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(Method::fixedLevelConsonants);
    }
};

class InitializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacement
    : public PresenterUseCase {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(
            Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
    }
};

class
    InitializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacementAndEyeTracking
    : public PresenterUseCase {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(Method::
                fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking);
    }
};

class
    InitializingFixedLevelCoordinateResponseMeasureMethodWithSilentIntervalTargets
    : public PresenterUseCase {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(Method::
                fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets);
    }
};

class InitializingFixedLevelFreeResponseWithSilentIntervalTargetsMethod
    : public PresenterUseCase {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(
            Method::fixedLevelFreeResponseWithSilentIntervalTargets);
    }
};

class InitializingFixedLevelFreeResponseMethodWithAllTargets
    : public PresenterUseCase {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(Method::fixedLevelFreeResponseWithAllTargets);
    }
};

class InitializingFixedLevelFreeResponseMethodWithAllTargetsAndEyeTracking
    : public PresenterUseCase {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(
            Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking);
    }
};

void exitTest(TestControlStub &c) { c.exitTest(); }

void declineContinuingTesting(TestControlStub &c) {
    c.declineContinuingTesting();
}

void playTrial(TestControlStub &c) { c.playTrial(); }

void acceptContinuingTesting(TestControlStub &c) {
    c.acceptContinuingTesting();
}

void notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion(
    TestController &c) {
    c.notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion();
}

void setAudioDevice(SessionViewStub &view, std::string s) {
    view.setAudioDevice(std::move(s));
}

void setTestComplete(ModelStub &model) { model.setTestComplete(); }

class SessionControllerStub : public SessionController {
  public:
    void notifyThatTestIsComplete() override {
        notifiedThatTestIsComplete_ = true;
    }

    [[nodiscard]] auto notifiedThatTestIsComplete() const -> bool {
        return notifiedThatTestIsComplete_;
    }
    void prepare(Method) override {}

  private:
    bool notifiedThatTestIsComplete_{};
};

class TestControllerListenerStub : public TestController::Observer {
  public:
    void notifyThatTrialHasStarted() override {
        notifiedThatTrialHasStarted_ = true;
    }
    [[nodiscard]] auto notifiedThatTrialHasStarted() const -> bool {
        return notifiedThatTrialHasStarted_;
    }
    void notifyThatNextTrialIsReady() override {
        notifiedThatNextTrialIsReady_ = true;
    }
    [[nodiscard]] auto notifiedThatNextTrialIsReady() const -> bool {
        return notifiedThatNextTrialIsReady_;
    }
    void setContinueTestingDialogMessage(const std::string &s) override {
        continueTestingDialogMessage_ = s;
    }
    auto continueTestingDialogMessage() -> std::string {
        return continueTestingDialogMessage_;
    }
    void showContinueTestingDialog() override {
        continueTestingDialogShown_ = true;
    }
    [[nodiscard]] auto continueTestingDialogShown() const -> bool {
        return continueTestingDialogShown_;
    }
    void display(const std::string &s) override { displayed_ = s; }
    auto displayed() -> std::string { return displayed_; }
    void secondaryDisplay(const std::string &s) override {
        displayedSecondary_ = s;
    }
    auto displayedSecondary() -> std::string { return displayedSecondary_; }
    auto messageToSubject() -> std::string { return messageToSubject_; }
    void tellSubject(const std::string &s) override { messageToSubject_ = s; }

  private:
    std::string displayed_;
    std::string displayedSecondary_;
    std::string continueTestingDialogMessage_;
    std::string messageToSubject_;
    bool notifiedThatTrialHasStarted_{};
    bool notifiedThatNextTrialIsReady_{};
    bool continueTestingDialogShown_{};
};

class ControllerUseCase {
  public:
    virtual ~ControllerUseCase() = default;
    virtual void run() = 0;
};

class AcceptingContinuingTesting : public ControllerUseCase {
  public:
    explicit AcceptingContinuingTesting(TestControlStub &control)
        : control{control} {}

    void run() override { acceptContinuingTesting(control); }

  private:
    TestControlStub &control;
};

class ExitingTest : public ControllerUseCase {
  public:
    explicit ExitingTest(TestControlStub &control) : control{control} {}

    void run() override { control.exitTest(); }

  private:
    TestControlStub &control;
};

class DecliningContinuingTesting : public ControllerUseCase {
  public:
    explicit DecliningContinuingTesting(TestControlStub &control)
        : control{control} {}

    void run() override { control.declineContinuingTesting(); }

  private:
    TestControlStub &control;
};

class NotifyingThatUserIsReadyForNextTrialForATestThatCongratulatesAtTheEnd
    : public ControllerUseCase {
  public:
    explicit NotifyingThatUserIsReadyForNextTrialForATestThatCongratulatesAtTheEnd(
        TestControllerImpl &controller)
        : controller{controller} {}

    void run() override {
        controller
            .notifyThatUserIsReadyForNextTrialForATestThatCongratulatesAtTheEnd();
    }

  private:
    TestControllerImpl &controller;
};

class NotifyingThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion
    : public ControllerUseCase {
  public:
    explicit NotifyingThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion(
        TestControllerImpl &controller)
        : controller{controller} {}

    void run() override {
        notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion(
            controller);
    }

  private:
    TestControllerImpl &controller;
};

class NotifyingThatUserIsDoneResponding : public ControllerUseCase {
  public:
    explicit NotifyingThatUserIsDoneResponding(TestControllerImpl &controller)
        : controller{controller} {}

    void run() override { controller.notifyThatUserIsDoneResponding(); }

  private:
    TestControllerImpl &controller;
};

class NotifyingThatUserIsReadyForNextTrial : public ControllerUseCase {
  public:
    explicit NotifyingThatUserIsReadyForNextTrial(
        TestControllerImpl &controller)
        : controller{controller} {}

    void run() override { controller.notifyThatUserIsReadyForNextTrial(); }

  private:
    TestControllerImpl &controller;
};

class UninitializedTaskPresenterStub : public UninitializedTaskPresenter {
  public:
    void initialize(TaskPresenter *p) override { presenter_ = p; }
    auto presenter() -> TaskPresenter * { return presenter_; }
    void showResponseSubmission() override { responseSubmissionShown_ = true; }
    [[nodiscard]] auto responseSubmissionShown() const -> bool {
        return responseSubmissionShown_;
    }
    void notifyThatTaskHasStarted() override {}
    void notifyThatUserIsDoneResponding() override {}
    void notifyThatTrialHasStarted() override {
        notifiedThatTrialHasStarted_ = true;
    }
    void start() override { started_ = true; }
    [[nodiscard]] auto started() const -> bool { return started_; }
    void stop() override { stopped_ = true; }
    [[nodiscard]] auto stopped() const -> bool { return stopped_; }
    [[nodiscard]] auto notifiedThatTrialHasStarted() const -> bool {
        return notifiedThatTrialHasStarted_;
    }

  private:
    TaskPresenter *presenter_{};
    bool stopped_{};
    bool started_{};
    bool notifiedThatTrialHasStarted_{};
    bool responseSubmissionShown_{};
};

class TestControllerTests : public ::testing::Test {
  protected:
    ModelStub model;
    SessionViewStub sessionView;
    TestControlStub control;
    TestControllerImpl controller{model, sessionView, control};
    DecliningContinuingTesting decliningContinuingTesting{control};
    AcceptingContinuingTesting acceptingContinuingTesting{control};
    ExitingTest exitingTest{control};
    NotifyingThatUserIsReadyForNextTrialForATestThatCongratulatesAtTheEnd
        notifyingThatUserIsReadyForNextTrialForATestThatCongratulatesAtTheEnd{
            controller};
    NotifyingThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion
        notifyingThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion{
            controller};
    NotifyingThatUserIsDoneResponding notifyingThatUserIsDoneResponding{
        controller};
    NotifyingThatUserIsReadyForNextTrial notifyingThatUserIsReadyForNextTrial{
        controller};
    SessionControllerStub sessionController;
    TestControllerListenerStub experimenterControllerListener;

    TestControllerTests() {
        controller.attach(&sessionController);
        controller.attach(&experimenterControllerListener);
    }
};

class TestPresenterTests : public ::testing::Test {
  protected:
    ModelStub model;
    TestViewStub view;
    TaskPresenterStub consonantPresenter;
    TaskPresenterStub coordinateResponseMeasurePresenter;
    TaskPresenterStub freeResponsePresenter;
    TaskPresenterStub correctKeywordsPresenter;
    TaskPresenterStub passFailPresenter;
    UninitializedTaskPresenterStub taskPresenter;
    TestPresenterImpl presenter{model, view, &consonantPresenter,
        &coordinateResponseMeasurePresenter, &freeResponsePresenter,
        &correctKeywordsPresenter, &passFailPresenter, &taskPresenter};
    InitializingAdaptiveCoordinateResponseMeasureMethod
        initializingAdaptiveCoordinateResponseMeasureMethod;
    InitializingAdaptiveCoordinateResponseMeasureMethodWithSingleSpeaker
        initializingAdaptiveCoordinateResponseMeasureMethodWithSingleSpeaker;
    InitializingAdaptiveCoordinateResponseMeasureMethodWithDelayedMasker
        initializingAdaptiveCoordinateResponseMeasureMethodWithDelayedMasker;
    InitializingAdaptiveCoordinateResponseMeasureMethodWithEyeTracking
        initializingAdaptiveCoordinateResponseMeasureMethodWithEyeTracking;
    InitializingAdaptivePassFailMethod initializingAdaptivePassFailMethod;
    InitializingAdaptivePassFailMethodWithEyeTracking
        initializingAdaptivePassFailMethodWithEyeTracking;
    InitializingFixedLevelFreeResponseWithTargetReplacementMethod
        initializingFixedLevelFreeResponseWithTargetReplacementMethod;
    InitializingFixedLevelConsonantMethod initializingFixedLevelConsonantMethod;
    InitializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacement
        initializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacement;
    InitializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacementAndEyeTracking
        initializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacementAndEyeTracking;
    InitializingAdaptiveCorrectKeywordsMethod
        initializingAdaptiveCorrectKeywordsMethod;
    InitializingAdaptiveCorrectKeywordsMethodWithEyeTracking
        initializingAdaptiveCorrectKeywordsMethodWithEyeTracking;
    InitializingFixedLevelCoordinateResponseMeasureMethodWithSilentIntervalTargets
        initializingFixedLevelCoordinateResponseMeasureSilentIntervalsMethod;
    InitializingFixedLevelFreeResponseWithSilentIntervalTargetsMethod
        initializingFixedLevelFreeResponseWithSilentIntervalTargetsMethod;
    InitializingFixedLevelFreeResponseMethodWithAllTargets
        initializingFixedLevelFreeResponseMethodWithAllTargets;
    InitializingFixedLevelFreeResponseMethodWithAllTargetsAndEyeTracking
        initializingFixedLevelFreeResponseMethodWithAllTargetsAndEyeTracking;
};

void run(ControllerUseCase &useCase) { useCase.run(); }

#define AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TARGET(                             \
    model, useCase, experimenterControllerListener)                            \
    model.setTargetFileName("a");                                              \
    run(useCase);                                                              \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"},                          \
        (experimenterControllerListener).displayedSecondary())

#define AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TRIAL(                              \
    model, useCase, experimenterControllerListener)                            \
    model.setTrialNumber(1);                                                   \
    run(useCase);                                                              \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(                                           \
        std::string{"Trial 1"}, (experimenterControllerListener).displayed())

#define AV_SPEECH_IN_NOISE_EXPECT_NOTIFIES_THAT_NEXT_TRIAL_IS_READY(           \
    useCase, experimenterControllerListener)                                   \
    run(useCase);                                                              \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(                                            \
        (experimenterControllerListener).notifiedThatNextTrialIsReady())

#define AV_SPEECH_IN_NOISE_EXPECT_NOTIFIED_THAT_TEST_IS_COMPLETE(a)            \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).notifiedThatTestIsComplete())

#define AV_SPEECH_IN_NOISE_EXPECT_NOTIFIED_THAT_TEST_IS_COMPLETE_WHEN_COMPLETE( \
    useCase, sessionController)                                                 \
    setTestComplete(model);                                                     \
    run(useCase);                                                               \
    AV_SPEECH_IN_NOISE_EXPECT_NOTIFIED_THAT_TEST_IS_COMPLETE(sessionController)

#define AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(                  \
    useCase, experimenterPresenter, expected)                                  \
    useCase.run(experimenterPresenter);                                        \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(                                           \
        static_cast<TaskPresenter *>(&(expected)), taskPresenter.presenter())

#define TEST_CONTROLLER_TEST(a) TEST_F(TestControllerTests, a)

#define TEST_PRESENTER_TEST(a) TEST_F(TestPresenterTests, a)

TEST_CONTROLLER_TEST(notifiesThatTestIsCompleteAfterExitTestButtonClicked) {
    exitTest(control);
    AV_SPEECH_IN_NOISE_EXPECT_NOTIFIED_THAT_TEST_IS_COMPLETE(sessionController);
}

TEST_CONTROLLER_TEST(
    notifiesThatTestIsCompleteAfterContinueTestingDialogIsDeclined) {
    declineContinuingTesting(control);
    AV_SPEECH_IN_NOISE_EXPECT_NOTIFIED_THAT_TEST_IS_COMPLETE(sessionController);
}

TEST_CONTROLLER_TEST(notifiesThatTestIsCompleteAfterUserIsDoneResponding) {
    AV_SPEECH_IN_NOISE_EXPECT_NOTIFIED_THAT_TEST_IS_COMPLETE_WHEN_COMPLETE(
        notifyingThatUserIsDoneResponding, sessionController);
}

TEST_CONTROLLER_TEST(
    notifiesThatTestIsCompleteAfterUserReadyForNextTrialForATestThatCongratulates) {
    AV_SPEECH_IN_NOISE_EXPECT_NOTIFIED_THAT_TEST_IS_COMPLETE_WHEN_COMPLETE(
        notifyingThatUserIsReadyForNextTrialForATestThatCongratulatesAtTheEnd,
        sessionController);
}

TEST_CONTROLLER_TEST(
    notifiesThatTestIsCompleteAfterNotifyingThatUserIsReadyForNextTrial) {
    AV_SPEECH_IN_NOISE_EXPECT_NOTIFIED_THAT_TEST_IS_COMPLETE_WHEN_COMPLETE(
        notifyingThatUserIsReadyForNextTrial, sessionController);
}

TEST_CONTROLLER_TEST(responderPlaysTrialAfterPlayTrialButtonClicked) {
    setAudioDevice(sessionView, "a");
    playTrial(control);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, model.trialParameters().audioDevice);
}

TEST_CONTROLLER_TEST(
    responderPlaysTrialAfterNotifyingThatUserIsReadyForNextTrial) {
    setAudioDevice(sessionView, "a");
    controller.notifyThatUserIsReadyForNextTrial();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, model.trialParameters().audioDevice);
}

TEST_CONTROLLER_TEST(
    responderPlaysTrialAfterNotifyingThatUserIsReadyForNextTrialForATestThatCongratulatesAtTheEnd) {
    setAudioDevice(sessionView, "a");
    run(notifyingThatUserIsReadyForNextTrialForATestThatCongratulatesAtTheEnd);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, model.trialParameters().audioDevice);
}

TEST_CONTROLLER_TEST(notifiesThatTrialHasStartedAfterPlayTrialButtonClicked) {
    playTrial(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterControllerListener.notifiedThatTrialHasStarted());
}

TEST_CONTROLLER_TEST(
    notifiesThatTrialHasStartedAfterNotifyingThatUserIsReadyForNextTrial) {
    controller.notifyThatUserIsReadyForNextTrial();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterControllerListener.notifiedThatTrialHasStarted());
}

TEST_CONTROLLER_TEST(
    responderRestartsAdaptiveTestWhilePreservingTargetsAfterContinueTestingDialogIsAccepted) {
    acceptContinuingTesting(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        model.adaptiveTestRestartedWhilePreservingCyclicTargets());
}

TEST_CONTROLLER_TEST(
    notifiesThatNextTrialIsReadyAfterContinueTestingDialogIsAccepted) {
    AV_SPEECH_IN_NOISE_EXPECT_NOTIFIES_THAT_NEXT_TRIAL_IS_READY(
        acceptingContinuingTesting, experimenterControllerListener);
}

TEST_CONTROLLER_TEST(
    notifiesThatNextTrialIsReadyAfterNotifyingThatUserIsDoneResponding) {
    AV_SPEECH_IN_NOISE_EXPECT_NOTIFIES_THAT_NEXT_TRIAL_IS_READY(
        notifyingThatUserIsDoneResponding, experimenterControllerListener);
}

TEST_CONTROLLER_TEST(
    notifiesThatNextTrialIsReadyAfterNotShowingContinueTestingDialogWithResults) {
    AV_SPEECH_IN_NOISE_EXPECT_NOTIFIES_THAT_NEXT_TRIAL_IS_READY(
        notifyingThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion,
        experimenterControllerListener);
}

TEST_CONTROLLER_TEST(responderDisplaysTargetAfterUserIsDoneResponding) {
    AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TARGET(model,
        notifyingThatUserIsDoneResponding, experimenterControllerListener);
}

TEST_CONTROLLER_TEST(
    displaysTargetFileNameAfterNotShowingContinueTestingDialogWithResults) {
    AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TARGET(model,
        notifyingThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion,
        experimenterControllerListener);
}

TEST_CONTROLLER_TEST(displaysTargetFileNameAfterNotShowingCongratulations) {
    AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TARGET(model,
        notifyingThatUserIsReadyForNextTrialForATestThatCongratulatesAtTheEnd,
        experimenterControllerListener);
}

TEST_CONTROLLER_TEST(
    responderDisplaysTargetFileNameAfterContinueTestingDialogIsAccepted) {
    AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TARGET(
        model, acceptingContinuingTesting, experimenterControllerListener);
}

TEST_CONTROLLER_TEST(
    responderDisplaysTargetFileNameAfterNotifyingThatUserIsReadyForNextTrial) {
    AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TARGET(model,
        notifyingThatUserIsReadyForNextTrial, experimenterControllerListener);
}

TEST_CONTROLLER_TEST(responderDisplaysTrialNumberAfterUserIsDoneResponding) {
    AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TRIAL(model,
        notifyingThatUserIsDoneResponding, experimenterControllerListener);
}

TEST_CONTROLLER_TEST(
    displaysTrialNumberAfterNotShowingContinueTestingDialogWithResults) {
    AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TRIAL(model,
        notifyingThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion,
        experimenterControllerListener);
}

TEST_CONTROLLER_TEST(displaysTrialNumberAfterNotShowingCongratulations) {
    AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TRIAL(model,
        notifyingThatUserIsReadyForNextTrialForATestThatCongratulatesAtTheEnd,
        experimenterControllerListener);
}

TEST_CONTROLLER_TEST(
    responderDisplaysTrialNumberAfterContinueTestingDialogIsAccepted) {
    AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TRIAL(
        model, acceptingContinuingTesting, experimenterControllerListener);
}

TEST_CONTROLLER_TEST(
    responderDisplaysTrialNumberAfterNotifyingThatUserIsReadyForNextTrial) {
    AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TRIAL(model,
        notifyingThatUserIsReadyForNextTrial, experimenterControllerListener);
}

TEST_CONTROLLER_TEST(showsContinueTestingDialog) {
    setTestComplete(model);
    notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion(
        controller);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterControllerListener.continueTestingDialogShown());
}

TEST_CONTROLLER_TEST(showsAdaptiveTestResults) {
    setTestComplete(model);
    model.setAdaptiveTestResults({{{"a"}, 1.}, {{"b"}, 2.}, {{"c"}, 3.}});
    notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion(
        controller);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"thresholds (targets: dB SNR)\na: 1\nb: 2\nc: 3"},
        experimenterControllerListener.continueTestingDialogMessage());
}

TEST_CONTROLLER_TEST(showsCongratulations) {
    setTestComplete(model);
    run(notifyingThatUserIsReadyForNextTrialForATestThatCongratulatesAtTheEnd);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"Condition complete, great work!"},
        experimenterControllerListener.messageToSubject());
}

TEST_PRESENTER_TEST(showsViewAfterStarting) {
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.shown());
}

TEST_PRESENTER_TEST(hidesViewAfterStopping) {
    presenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.hidden());
}

TEST_PRESENTER_TEST(hidesContinueTestingDialogAfterStopping) {
    presenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.continueTestingDialogHidden());
}

TEST_PRESENTER_TEST(stopsTaskAfterStopping) {
    presenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(taskPresenter.stopped());
}

TEST_PRESENTER_TEST(hidesExitTestButtonAfterTrialStarts) {
    presenter.notifyThatTrialHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.exitTestButtonHidden());
}

TEST_PRESENTER_TEST(hidesNextTrialButtonAfterTrialStarts) {
    presenter.notifyThatTrialHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.nextTrialButtonHidden());
}

TEST_PRESENTER_TEST(notifiesTaskPresenterThatTrialHasStarted) {
    presenter.notifyThatTrialHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(taskPresenter.notifiedThatTrialHasStarted());
}

TEST_PRESENTER_TEST(showsExitTestButtonWhenTrialCompletes) {
    model.completeTrial();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.exitTestButtonShown());
}

TEST_PRESENTER_TEST(showsTaskResponseSubmissionWhenTrialCompletes) {
    model.completeTrial();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(taskPresenter.responseSubmissionShown());
}

TEST_PRESENTER_TEST(hidesContinueTestingDialogAfterNextTrialIsReady) {
    presenter.notifyThatNextTrialIsReady();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.continueTestingDialogHidden());
}

TEST_PRESENTER_TEST(showsNextTrialButtonAfterNextTrialIsReady) {
    presenter.notifyThatNextTrialIsReady();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.nextTrialButtonShown());
}

TEST_PRESENTER_TEST(displaysMessage) {
    presenter.display("a");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, view.displayed());
}

TEST_PRESENTER_TEST(displaysSecondaryMessage) {
    presenter.secondaryDisplay("a");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, view.secondaryDisplayed());
}

TEST_PRESENTER_TEST(displaysMessageToSubject) {
    presenter.tellSubject("a");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, view.messageToSubject());
}

TEST_PRESENTER_TEST(showsContinueTestingDialog) {
    presenter.showContinueTestingDialog();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.continueTestingDialogShown());
}

TEST_PRESENTER_TEST(setsContinueTestingDialogMessage) {
    presenter.setContinueTestingDialogMessage("a");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, view.continueTestingDialogMessage());
}

TEST_PRESENTER_TEST(startsTaskPresenterWhenInitializing) {
    presenter.initialize(Method::unknown);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(taskPresenter.started());
}

TEST_PRESENTER_TEST(
    initializingAdaptiveCoordinateResponseMeasureMethodInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingAdaptiveCoordinateResponseMeasureMethod, presenter,
        coordinateResponseMeasurePresenter);
}

TEST_PRESENTER_TEST(
    initializingAdaptiveCoordinateResponseMeasureMethodWithSingleSpeakerInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingAdaptiveCoordinateResponseMeasureMethodWithSingleSpeaker,
        presenter, coordinateResponseMeasurePresenter);
}

TEST_PRESENTER_TEST(
    initializingAdaptiveCoordinateResponseMeasureMethodWithDelayedMaskerInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingAdaptiveCoordinateResponseMeasureMethodWithDelayedMasker,
        presenter, coordinateResponseMeasurePresenter);
}

TEST_PRESENTER_TEST(
    initializingAdaptiveCoordinateResponseMeasureMethodWithEyeTrackingInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingAdaptiveCoordinateResponseMeasureMethodWithEyeTracking,
        presenter, coordinateResponseMeasurePresenter);
}

TEST_PRESENTER_TEST(
    initializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacementInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacement,
        presenter, coordinateResponseMeasurePresenter);
}

TEST_PRESENTER_TEST(
    initializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacementAndEyeTrackingInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacementAndEyeTracking,
        presenter, coordinateResponseMeasurePresenter);
}

TEST_PRESENTER_TEST(
    initializingFixedLevelCoordinateResponseMeasureSilentIntervalsMethodInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingFixedLevelCoordinateResponseMeasureSilentIntervalsMethod,
        presenter, coordinateResponseMeasurePresenter);
}

TEST_PRESENTER_TEST(
    initializingFixedLevelFreeResponseMethodWithAllTargetsInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingFixedLevelFreeResponseMethodWithAllTargets, presenter,
        freeResponsePresenter);
}

TEST_PRESENTER_TEST(
    initializingFixedLevelFreeResponseMethodWithAllTargetsAndEyeTrackingInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingFixedLevelFreeResponseMethodWithAllTargetsAndEyeTracking,
        presenter, freeResponsePresenter);
}

TEST_PRESENTER_TEST(
    initializingFixedLevelFreeResponseWithSilentIntervalTargetsMethodInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingFixedLevelFreeResponseWithSilentIntervalTargetsMethod,
        presenter, freeResponsePresenter);
}

TEST_PRESENTER_TEST(
    initializingFixedLevelFreeResponseWithTargetReplacementMethodInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingFixedLevelFreeResponseWithTargetReplacementMethod,
        presenter, freeResponsePresenter);
}

TEST_PRESENTER_TEST(initializingAdaptiveCorrectKeywordsMethodInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingAdaptiveCorrectKeywordsMethod, presenter,
        correctKeywordsPresenter);
}

TEST_PRESENTER_TEST(
    initializingAdaptiveCorrectKeywordsMethodWithEyeTrackingInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingAdaptiveCorrectKeywordsMethodWithEyeTracking, presenter,
        correctKeywordsPresenter);
}

TEST_PRESENTER_TEST(initializingFixedLevelConsonantMethodInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingFixedLevelConsonantMethod, presenter, consonantPresenter);
}

TEST_PRESENTER_TEST(initializingAdaptivePassFailMethodInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingAdaptivePassFailMethod, presenter, passFailPresenter);
}

TEST_PRESENTER_TEST(
    initializingAdaptivePassFailMethodWithEyeTrackingInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingAdaptivePassFailMethodWithEyeTracking, presenter,
        passFailPresenter);
}

TEST_PRESENTER_TEST(displaysTrialNumberWhenInitializing) {
    model.setTrialNumber(1);
    presenter.initialize(Method::unknown);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"Trial 1"}, view.displayed());
}

TEST_PRESENTER_TEST(displaysTargetWhenInitializing) {
    model.setTargetFileName("a");
    presenter.initialize(Method::unknown);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, view.secondaryDisplayed());
}
}
}
