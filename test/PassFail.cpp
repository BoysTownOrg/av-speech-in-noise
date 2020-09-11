#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "TaskControllerListenerStub.hpp"
#include "ExperimenterControllerStub.hpp"
#include <presentation/PassFail.hpp>
#include <gtest/gtest.h>
#include <utility>

namespace av_speech_in_noise {
namespace {
class PassFailInputViewStub : public PassFailInputView {
  public:
    void notifyThatCorrectButtonHasBeenClicked() {
        listener_->notifyThatCorrectButtonHasBeenClicked();
    }

    void notifyThatIncorrectButtonHasBeenClicked() {
        listener_->notifyThatIncorrectButtonHasBeenClicked();
    }

    void subscribe(EventListener *e) override { listener_ = e; }

  private:
    EventListener *listener_{};
};

class PassFailOutputViewStub : public PassFailOutputView {
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

void notifyThatIncorrectButtonHasBeenClicked(PassFailInputViewStub &view) {
    view.notifyThatIncorrectButtonHasBeenClicked();
}

void notifyThatCorrectButtonHasBeenClicked(PassFailInputViewStub &view) {
    view.notifyThatCorrectButtonHasBeenClicked();
}

void notifyThatUserIsDoneResponding(TaskPresenter &presenter) {
    presenter.notifyThatUserIsDoneResponding();
}

void stop(TaskPresenter &presenter) { presenter.stop(); }

void start(TaskPresenter &presenter) { presenter.start(); }

class ExperimenterOutputViewStub : public ExperimenterOutputView {
  public:
    void showNextTrialButton() override { nextTrialButtonShown_ = true; }

    [[nodiscard]] auto nextTrialButtonShown() const {
        return nextTrialButtonShown_;
    }

    void hideNextTrialButton() override { nextTrialButtonHidden_ = true; }

    [[nodiscard]] auto nextTrialButtonHidden() const {
        return nextTrialButtonHidden_;
    }

    void setContinueTestingDialogMessage(const std::string &) override {}
    void showContinueTestingDialog() override {}
    void hideContinueTestingDialog() override {}
    void show() override {}
    void hide() override {}
    void display(std::string) override {}
    void secondaryDisplay(std::string) override {}
    void showExitTestButton() override {}
    void hideExitTestButton() override {}

  private:
    bool nextTrialButtonShown_{};
    bool nextTrialButtonHidden_{};
};

class PassFailTests : public ::testing::Test {
  protected:
    ModelStub model;
    ExperimenterOutputViewStub experimenterOutputView;
    PassFailInputViewStub inputView;
    PassFailOutputViewStub outputView;
    PassFailController responder{model, inputView};
    PassFailPresenter presenter{experimenterOutputView, outputView};
    ExperimenterControllerStub experimenterController;
    TaskControllerListenerStub taskController;

    PassFailTests() {
        responder.subscribe(&experimenterController);
        responder.subscribe(&taskController);
    }
};

#define PASS_FAIL_TEST(a) TEST_F(PassFailTests, a)

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).evaluationButtonsHidden())

PASS_FAIL_TEST(presenterHidesReadyButtonWhenTaskStarts) {
    presenter.notifyThatTaskHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterOutputView.nextTrialButtonHidden());
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
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterOutputView.nextTrialButtonShown());
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
