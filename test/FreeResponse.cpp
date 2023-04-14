#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "TestViewStub.hpp"
#include "TestControllerStub.hpp"
#include <av-speech-in-noise/ui/FreeResponse.hpp>
#include <gtest/gtest.h>
#include <utility>

namespace av_speech_in_noise::submitting_free_response {
namespace {
class ControlStub : public Control {
  public:
    void notifyThatSubmitButtonHasBeenClicked() {
        listener_->notifyThatSubmitButtonHasBeenClicked();
    }

    void attach(Observer *e) override { listener_ = e; }

    void setFreeResponse(std::string c) { freeResponse_ = std::move(c); }

    auto response() -> std::string override { return freeResponse_; }

    auto flagged() -> bool override { return flagged_; }

    void setFlagged() { flagged_ = true; }

  private:
    std::string freeResponse_{"0"};
    Observer *listener_{};
    bool flagged_{};
};

class ViewStub : public View {
  public:
    void show() override { freeResponseSubmissionShown_ = true; }

    [[nodiscard]] auto freeResponseSubmissionShown() const -> bool {
        return freeResponseSubmissionShown_;
    }

    void hide() override { freeResponseSubmissionHidden_ = true; }

    [[nodiscard]] auto freeResponseSubmissionHidden() const -> bool {
        return freeResponseSubmissionHidden_;
    }

    void clearResponse() override { freeResponseCleared_ = true; }

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

void notifyThatSubmitButtonHasBeenClicked(ControlStub &view) {
    view.notifyThatSubmitButtonHasBeenClicked();
}

void stop(TaskPresenter &presenter) { presenter.stop(); }

void start(TaskPresenter &presenter) { presenter.start(); }

class InteractorStub : public Interactor {
  public:
    void submit(const FreeResponse &s) override { freeResponse_ = s; }

    auto response() -> FreeResponse { return freeResponse_; }

  private:
    FreeResponse freeResponse_;
};

class ControllerTests : public ::testing::Test {
  protected:
    InteractorStub model;
    ControlStub control;
    TestControllerStub testController;
    Controller controller{testController, model, control};
};

class PresenterTests : public ::testing::Test {
  protected:
    TestViewStub testView;
    ViewStub view;
    Presenter presenter{testView, view};
};

#define FREE_RESPONSE_CONTROLLER_TEST(a) TEST_F(ControllerTests, a)

#define FREE_RESPONSE_PRESENTER_TEST(a) TEST_F(PresenterTests, a)

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
        std::string{"a"}, model.response().response);
}

FREE_RESPONSE_CONTROLLER_TEST(
    responderSubmitsFlaggedFreeResponseAfterResponseButtonIsClicked) {
    control.setFlagged();
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.response().flagged);
}

FREE_RESPONSE_CONTROLLER_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterResponseButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
}
}

namespace with_puzzle {
namespace {
class ControllerWithPuzzleTests : public ::testing::Test {
  protected:
    InteractorStub model;
    ControlStub control;
    TestControllerStub testController;
    Controller controller{testController, model, control};
};

class PresenterWithPuzzleTests : public ::testing::Test {
  protected:
    TestViewStub testView;
    ViewStub view;
    Presenter presenter{testView, view};
};

#define FREE_RESPONSE_CONTROLLER_TEST(a) TEST_F(ControllerWithPuzzleTests, a)

#define FREE_RESPONSE_PRESENTER_TEST(a) TEST_F(PresenterWithPuzzleTests, a)

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
        std::string{"a"}, model.response().response);
}

FREE_RESPONSE_CONTROLLER_TEST(
    responderSubmitsFlaggedFreeResponseAfterResponseButtonIsClicked) {
    control.setFlagged();
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.response().flagged);
}

FREE_RESPONSE_CONTROLLER_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterResponseButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
}
}
}
}
