#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "TestViewStub.hpp"
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

    void clearFlag() override { flagCleared_ = true; }

    [[nodiscard]] auto freeResponseCleared() const -> bool {
        return freeResponseCleared_;
    }

    [[nodiscard]] auto flagCleared() const -> bool { return flagCleared_; }

  private:
    bool freeResponseSubmissionShown_{};
    bool freeResponseSubmissionHidden_{};
    bool freeResponseCleared_{};
    bool flagCleared_{};
};

void notifyThatSubmitButtonHasBeenClicked(FreeResponseControlStub &view) {
    view.notifyThatSubmitButtonHasBeenClicked();
}

void notifyThatUserIsDoneResponding(TaskPresenter &presenter) {
    presenter.notifyThatUserIsDoneResponding();
}

void stop(TaskPresenter &presenter) { presenter.stop(); }

void start(TaskPresenter &presenter) { presenter.start(); }

class FreeResponseControllerTests : public ::testing::Test {
  protected:
    ModelStub model;
    FreeResponseControlStub control;
    TestControllerStub testController;
    FreeResponseController controller{testController, model, control};
};

class FreeResponsePresenterTests : public ::testing::Test {
  protected:
    TestViewStub testView;
    FreeResponseViewStub view;
    FreeResponsePresenter presenter{testView, view};
};

#define FREE_RESPONSE_CONTROLLER_TEST(a) TEST_F(FreeResponseControllerTests, a)

#define FREE_RESPONSE_PRESENTER_TEST(a) TEST_F(FreeResponsePresenterTests, a)

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).freeResponseSubmissionHidden())

FREE_RESPONSE_PRESENTER_TEST(presenterHidesResponseSubmission) {
    presenter.hideResponseSubmission();
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

FREE_RESPONSE_PRESENTER_TEST(presenterClearsFlagWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.flagCleared());
}

FREE_RESPONSE_CONTROLLER_TEST(
    responderSubmitsFreeResponseAfterResponseButtonIsClicked) {
    control.setFreeResponse("a");
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, model.freeResponse().response);
}

FREE_RESPONSE_CONTROLLER_TEST(
    responderSubmitsFlaggedFreeResponseAfterResponseButtonIsClicked) {
    control.setFlagged();
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.freeResponse().flagged);
}

FREE_RESPONSE_CONTROLLER_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterResponseButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
}
}
}
