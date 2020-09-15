#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "TestViewStub.hpp"
#include "TaskControllerObserverStub.hpp"
#include "TestControllerStub.hpp"
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

class FreeResponseTests : public ::testing::Test {
  protected:
    ModelStub model;
    TestViewStub testView;
    FreeResponseControlStub control;
    FreeResponseViewStub view;
    FreeResponseController controller{model, control};
    FreeResponsePresenter presenter{testView, view};
    TestControllerStub testController;
    TaskControllerObserverStub taskController;

    FreeResponseTests() {
        controller.attach(&testController);
        controller.attach(&taskController);
    }
};

class FreeResponsePresenterTests : public ::testing::Test {
  protected:
    TestViewStub testView;
    FreeResponseViewStub view;
    FreeResponsePresenter presenter{testView, view};
};

#define FREE_RESPONSE_TEST(a) TEST_F(FreeResponseTests, a)

#define FREE_RESPONSE_PRESENTER_TEST(a) TEST_F(FreeResponsePresenterTests, a)

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).freeResponseSubmissionHidden())

FREE_RESPONSE_PRESENTER_TEST(presenterHidesReadyButtonWhenTaskStarts) {
    presenter.notifyThatTaskHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testView.nextTrialButtonHidden());
}

FREE_RESPONSE_PRESENTER_TEST(
    presenterHidesResponseButtonsAfterUserIsDoneResponding) {
    notifyThatUserIsDoneResponding(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

FREE_RESPONSE_PRESENTER_TEST(presenterHidesResponseButtonsWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

FREE_RESPONSE_PRESENTER_TEST(presenterShowsReadyButtonWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testView.nextTrialButtonShown());
}

FREE_RESPONSE_PRESENTER_TEST(
    presenterShowsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.freeResponseSubmissionShown());
}

FREE_RESPONSE_PRESENTER_TEST(
    presenterClearsFreeResponseWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.freeResponseCleared());
}

FREE_RESPONSE_TEST(responderSubmitsFreeResponseAfterResponseButtonIsClicked) {
    control.setFreeResponse("a");
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, model.freeResponse().response);
}

FREE_RESPONSE_TEST(
    responderSubmitsFlaggedFreeResponseAfterResponseButtonIsClicked) {
    control.setFlagged();
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.freeResponse().flagged);
}

FREE_RESPONSE_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterResponseButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
}

FREE_RESPONSE_TEST(
    responderNotifiesThatUserIsDoneRespondingAfterResponseButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        taskController.notifiedThatUserIsDoneResponding());
}
}
}
