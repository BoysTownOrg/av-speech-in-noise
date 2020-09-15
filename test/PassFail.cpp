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

class PassFailTests : public ::testing::Test {
  protected:
    ModelStub model;
    TestViewStub experimenterView;
    PassFailControlStub inputView;
    PassFailViewStub outputView;
    PassFailController responder{model, inputView};
    PassFailPresenter presenter{experimenterView, outputView};
    TestControllerStub experimenterController;
    TaskControllerObserverStub taskController;

    PassFailTests() {
        responder.attach(&experimenterController);
        responder.attach(&taskController);
    }
};

#define PASS_FAIL_TEST(a) TEST_F(PassFailTests, a)

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).evaluationButtonsHidden())

PASS_FAIL_TEST(presenterHidesReadyButtonWhenTaskStarts) {
    presenter.notifyThatTaskHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(experimenterView.nextTrialButtonHidden());
}

PASS_FAIL_TEST(presenterHidesResponseButtonsAfterUserIsDoneResponding) {
    notifyThatUserIsDoneResponding(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(outputView);
}

PASS_FAIL_TEST(presenterHidesResponseButtonsWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(outputView);
}

PASS_FAIL_TEST(presenterShowsReadyButtonWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(experimenterView.nextTrialButtonShown());
}

PASS_FAIL_TEST(presenterShowsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputView.evaluationButtonsShown());
}

PASS_FAIL_TEST(responderSubmitsCorrectResponseAfterCorrectButtonIsClicked) {
    notifyThatCorrectButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.correctResponseSubmitted());
}

PASS_FAIL_TEST(responderSubmitsIncorrectResponseAfterIncorrectButtonIsClicked) {
    notifyThatIncorrectButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.incorrectResponseSubmitted());
}

PASS_FAIL_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterCorrectButtonIsClicked) {
    notifyThatCorrectButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterController
            .notifiedThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion());
}

PASS_FAIL_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterIncorrectButtonIsClicked) {
    notifyThatIncorrectButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterController
            .notifiedThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion());
}

PASS_FAIL_TEST(
    responderNotifiesThatUserIsDoneRespondingAfterCorrectButtonIsClicked) {
    notifyThatCorrectButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        taskController.notifiedThatUserIsDoneResponding());
}

PASS_FAIL_TEST(
    responderNotifiesThatUserIsDoneRespondingAfterIncorrectButtonIsClicked) {
    notifyThatIncorrectButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        taskController.notifiedThatUserIsDoneResponding());
}
}
}
