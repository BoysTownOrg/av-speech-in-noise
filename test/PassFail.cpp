#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "TestViewStub.hpp"
#include "TestControllerStub.hpp"
#include <av-speech-in-noise/ui/PassFail.hpp>
#include <gtest/gtest.h>
#include <utility>

namespace av_speech_in_noise::submitting_pass_fail {
namespace {
class ControlStub : public Control {
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

class ViewStub : public View {
  public:
    void show() override { shown_ = true; }

    [[nodiscard]] auto shown() const -> bool { return shown_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const -> bool { return hidden_; }

  private:
    bool shown_{};
    bool hidden_{};
};

void notifyThatIncorrectButtonHasBeenClicked(ControlStub &view) {
    view.notifyThatIncorrectButtonHasBeenClicked();
}

void notifyThatCorrectButtonHasBeenClicked(ControlStub &view) {
    view.notifyThatCorrectButtonHasBeenClicked();
}

void stop(TaskPresenter &presenter) { presenter.stop(); }

void start(TaskPresenter &presenter) { presenter.start(); }

class SubmittingPassFailControllerTests : public ::testing::Test {
  protected:
    ModelStub model;
    ControlStub control;
    TestControllerStub testController;
    Controller controller{testController, model, control};
};

class SubmittingPassFailPresenterTests : public ::testing::Test {
  protected:
    TestViewStub testView;
    ViewStub view;
    Presenter presenter{testView, view};
};

#define PASS_FAIL_CONTROLLER_TEST(a)                                           \
    TEST_F(SubmittingPassFailControllerTests, a)

#define PASS_FAIL_PRESENTER_TEST(a) TEST_F(SubmittingPassFailPresenterTests, a)

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).hidden())

PASS_FAIL_PRESENTER_TEST(presenterHidesResponseSubmission) {
    presenter.hideResponseSubmission();
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
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.shown());
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
}
}
