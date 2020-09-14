#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "TaskControllerListenerStub.hpp"
#include "ExperimenterControllerStub.hpp"
#include <presentation/FreeResponse.hpp>
#include <gtest/gtest.h>
#include <utility>

namespace av_speech_in_noise {
namespace {
class FreeResponseControlStub : public FreeResponseControl {
  public:
    void notifyThatSubmitButtonHasBeenClicked() {
        listener_->notifyThatSubmitButtonHasBeenClicked();
    }

    void attach(Observer *e) override { listener_ = e; }

    void setFreeResponse(std::string c) { freeResponse_ = std::move(c); }

    auto freeResponse() -> std::string override { return freeResponse_; }

    auto flagged() -> bool override { return flagged_; }

    void setFlagged() { flagged_ = true; }

  private:
    std::string freeResponse_{"0"};
    Observer *listener_{};
    bool flagged_{};
};

class FreeResponseViewStub : public FreeResponseView {
  public:
    void showFreeResponseSubmission() override {
        freeResponseSubmissionShown_ = true;
    }

    [[nodiscard]] auto freeResponseSubmissionShown() const -> bool {
        return freeResponseSubmissionShown_;
    }

    void hideFreeResponseSubmission() override {
        freeResponseSubmissionHidden_ = true;
    }

    [[nodiscard]] auto freeResponseSubmissionHidden() const -> bool {
        return freeResponseSubmissionHidden_;
    }

    void clearFreeResponse() override { freeResponseCleared_ = true; }

    [[nodiscard]] auto freeResponseCleared() const -> bool {
        return freeResponseCleared_;
    }

  private:
    bool freeResponseSubmissionShown_{};
    bool freeResponseSubmissionHidden_{};
    bool freeResponseCleared_{};
};

void notifyThatSubmitButtonHasBeenClicked(FreeResponseControlStub &view) {
    view.notifyThatSubmitButtonHasBeenClicked();
}

void notifyThatUserIsDoneResponding(TaskPresenter &presenter) {
    presenter.notifyThatUserIsDoneResponding();
}

void stop(TaskPresenter &presenter) { presenter.stop(); }

void start(TaskPresenter &presenter) { presenter.start(); }

class ExperimenterViewStub : public TestView {
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

class FreeResponseTests : public ::testing::Test {
  protected:
    ModelStub model;
    ExperimenterViewStub experimenterView;
    FreeResponseControlStub inputView;
    FreeResponseViewStub outputView;
    FreeResponseController responder{model, inputView};
    FreeResponsePresenter presenter{experimenterView, outputView};
    ExperimenterControllerStub experimenterController;
    TaskControllerListenerStub taskController;

    FreeResponseTests() {
        responder.attach(&experimenterController);
        responder.attach(&taskController);
    }
};

#define FREE_RESPONSE_TEST(a) TEST_F(FreeResponseTests, a)

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).freeResponseSubmissionHidden())

FREE_RESPONSE_TEST(presenterHidesReadyButtonWhenTaskStarts) {
    presenter.notifyThatTaskHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(experimenterView.nextTrialButtonHidden());
}

FREE_RESPONSE_TEST(presenterHidesResponseButtonsAfterUserIsDoneResponding) {
    notifyThatUserIsDoneResponding(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(outputView);
}

FREE_RESPONSE_TEST(presenterHidesResponseButtonsWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(outputView);
}

FREE_RESPONSE_TEST(presenterShowsReadyButtonWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(experimenterView.nextTrialButtonShown());
}

FREE_RESPONSE_TEST(presenterShowsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputView.freeResponseSubmissionShown());
}

FREE_RESPONSE_TEST(presenterClearsFreeResponseWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputView.freeResponseCleared());
}

FREE_RESPONSE_TEST(responderSubmitsFreeResponseAfterResponseButtonIsClicked) {
    inputView.setFreeResponse("a");
    notifyThatSubmitButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, model.freeResponse().response);
}

FREE_RESPONSE_TEST(
    responderSubmitsFlaggedFreeResponseAfterResponseButtonIsClicked) {
    inputView.setFlagged();
    notifyThatSubmitButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.freeResponse().flagged);
}

FREE_RESPONSE_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterResponseButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterController.notifiedThatUserIsDoneResponding());
}

FREE_RESPONSE_TEST(
    responderNotifiesThatUserIsDoneRespondingAfterResponseButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        taskController.notifiedThatUserIsDoneResponding());
}
}
}
