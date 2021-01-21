#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "TestViewStub.hpp"
#include "TaskControllerObserverStub.hpp"
#include "TestControllerStub.hpp"
#include <presentation/PassFail.hpp>
#include <gtest/gtest.h>
#include <utility>

namespace av_speech_in_noise {
namespace {
class PassFailControlStub : public PassFailControl {
  public:
    void notifyThatCorrectButtonHasBeenClicked() {
        listener_->notifyThatCorrectButtonHasBeenClicked();
    }

    void notifyThatIncorrectButtonHasBeenClicked() {
        listener_->notifyThatIncorrectButtonHasBeenClicked();
    }

    void attach(Observer *e) override { listener_ = e; }

  private:
    Observer *listener_{};
};

class PassFailViewStub : public PassFailView {
  public:
    void showEvaluationButtons() override { evaluationButtonsShown_ = true; }

    [[nodiscard]] auto evaluationButtonsShown() const -> bool {
        return evaluationButtonsShown_;
    }

    void hideEvaluationButtons() override { evaluationButtonsHidden_ = true; }

    [[nodiscard]] auto evaluationButtonsHidden() const -> bool {
        return evaluationButtonsHidden_;
    }

  private:
    bool evaluationButtonsShown_{};
    bool evaluationButtonsHidden_{};
};

void notifyThatIncorrectButtonHasBeenClicked(PassFailControlStub &view) {
    view.notifyThatIncorrectButtonHasBeenClicked();
}

void notifyThatCorrectButtonHasBeenClicked(PassFailControlStub &view) {
    view.notifyThatCorrectButtonHasBeenClicked();
}

void notifyThatUserIsDoneResponding(TaskPresenter &presenter) {
    presenter.notifyThatUserIsDoneResponding();
}

void stop(TaskPresenter &presenter) { presenter.stop(); }

void start(TaskPresenter &presenter) { presenter.start(); }

class PassFailControllerTests : public ::testing::Test {
  protected:
    ModelStub model;
    PassFailControlStub control;
    PassFailController controller{model, control};
    TestControllerStub testController;
    TaskControllerObserverStub taskController;

    PassFailControllerTests() {
        controller.attach(&testController);
        controller.attach(&taskController);
    }
};

class PassFailPresenterTests : public ::testing::Test {
  protected:
    TestViewStub testView;
    PassFailViewStub view;
    PassFailPresenter presenter{testView, view};
};

#define PASS_FAIL_CONTROLLER_TEST(a) TEST_F(PassFailControllerTests, a)

#define PASS_FAIL_PRESENTER_TEST(a) TEST_F(PassFailPresenterTests, a)

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).evaluationButtonsHidden())

PASS_FAIL_PRESENTER_TEST(
    presenterHidesResponseButtonsAfterUserIsDoneResponding) {
    notifyThatUserIsDoneResponding(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

PASS_FAIL_PRESENTER_TEST(presenterHidesResponseButtonsWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

PASS_FAIL_PRESENTER_TEST(presenterShowsReadyButtonWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testView.nextTrialButtonShown());
}

PASS_FAIL_PRESENTER_TEST(
    presenterShowsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.evaluationButtonsShown());
}

PASS_FAIL_CONTROLLER_TEST(
    responderSubmitsCorrectResponseAfterCorrectButtonIsClicked) {
    notifyThatCorrectButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.correctResponseSubmitted());
}

PASS_FAIL_CONTROLLER_TEST(
    responderSubmitsIncorrectResponseAfterIncorrectButtonIsClicked) {
    notifyThatIncorrectButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.incorrectResponseSubmitted());
}

PASS_FAIL_CONTROLLER_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterCorrectButtonIsClicked) {
    notifyThatCorrectButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController
            .notifiedThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion());
}

PASS_FAIL_CONTROLLER_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterIncorrectButtonIsClicked) {
    notifyThatIncorrectButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController
            .notifiedThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion());
}

PASS_FAIL_CONTROLLER_TEST(
    responderNotifiesThatUserIsDoneRespondingAfterCorrectButtonIsClicked) {
    notifyThatCorrectButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        taskController.notifiedThatUserIsDoneResponding());
}

PASS_FAIL_CONTROLLER_TEST(
    responderNotifiesThatUserIsDoneRespondingAfterIncorrectButtonIsClicked) {
    notifyThatIncorrectButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        taskController.notifiedThatUserIsDoneResponding());
}
}
}
