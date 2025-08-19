#include "RunningATestStub.hpp"
#include "assert-utility.hpp"
#include "TestViewStub.hpp"
#include "TestControllerStub.hpp"

#include <av-speech-in-noise/ui/PassFail.hpp>

#include <gtest/gtest.h>
#include <utility>

namespace av_speech_in_noise::submitting_pass_fail {
namespace {
class UIStub : public UI {
  public:
    void notifyThatCorrectButtonHasBeenClicked() {
        listener_->notifyThatCorrectButtonHasBeenClicked();
    }

    void notifyThatIncorrectButtonHasBeenClicked() {
        listener_->notifyThatIncorrectButtonHasBeenClicked();
    }

    void attach(Observer *e) override { listener_ = e; }

    void show() override { shown_ = true; }

    [[nodiscard]] auto shown() const -> bool { return shown_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const -> bool { return hidden_; }

    void display(std::string s) override { displayed = std::move(s); }

    void clearDisplay() override { displayCleared = true; }

    std::string displayed;
    Observer *listener_{};
    bool displayCleared{};

  private:
    bool shown_{};
    bool hidden_{};
};

class ViewStub : public View {};

class InteractorStub : public Interactor {
  public:
    void submitCorrectResponse() override { correctResponseSubmitted_ = true; }

    void submitIncorrectResponse() override {
        incorrectResponseSubmitted_ = true;
    }

    [[nodiscard]] auto incorrectResponseSubmitted() const -> bool {
        return incorrectResponseSubmitted_;
    }

    [[nodiscard]] auto correctResponseSubmitted() const -> bool {
        return correctResponseSubmitted_;
    }

  private:
    bool correctResponseSubmitted_{};
    bool incorrectResponseSubmitted_{};
};

void notifyThatIncorrectButtonHasBeenClicked(UIStub &view) {
    view.notifyThatIncorrectButtonHasBeenClicked();
}

void notifyThatCorrectButtonHasBeenClicked(UIStub &view) {
    view.notifyThatCorrectButtonHasBeenClicked();
}

void stop(TaskPresenter &presenter) { presenter.stop(); }

void start(TaskPresenter &presenter) { presenter.start(); }

class SubmittingPassFailPresenterTests : public ::testing::Test {
  protected:
    InteractorStub model;
    UIStub ui;
    TestControllerStub testController;
    TestViewStub testView;
    RunningATestStub runningATest;
    Presenter presenter{runningATest, testController, testView, model, ui};
};

#define PASS_FAIL_PRESENTER_TEST(a) TEST_F(SubmittingPassFailPresenterTests, a)

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).hidden())

PASS_FAIL_PRESENTER_TEST(presenterHidesResponseSubmission) {
    presenter.hideResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(ui);
}

PASS_FAIL_PRESENTER_TEST(presenterHidesResponseButtonsWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(ui);
}

PASS_FAIL_PRESENTER_TEST(presenterShowsReadyButtonWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testView.nextTrialButtonShown());
}

PASS_FAIL_PRESENTER_TEST(
    presenterShowsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(ui.shown());
}

PASS_FAIL_PRESENTER_TEST(
    responderSubmitsCorrectResponseAfterCorrectButtonIsClicked) {
    start(presenter);
    notifyThatCorrectButtonHasBeenClicked(ui);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.correctResponseSubmitted());
}

PASS_FAIL_PRESENTER_TEST(
    responderSubmitsIncorrectResponseAfterIncorrectButtonIsClicked) {
    start(presenter);
    notifyThatIncorrectButtonHasBeenClicked(ui);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.incorrectResponseSubmitted());
}

PASS_FAIL_PRESENTER_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterCorrectButtonIsClicked) {
    start(presenter);
    notifyThatCorrectButtonHasBeenClicked(ui);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testController
            .notifiedThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion());
}

PASS_FAIL_PRESENTER_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterIncorrectButtonIsClicked) {
    start(presenter);
    notifyThatIncorrectButtonHasBeenClicked(ui);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testController
            .notifiedThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion());
}

PASS_FAIL_PRESENTER_TEST(showsTargetFilename) {
    start(presenter);
    runningATest.targetFileName_ = "hi";
    ui.listener_->notifyThatShowAnswerButtonHasBeenClicked();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("hi", ui.displayed);
}

PASS_FAIL_PRESENTER_TEST(clearsDisplay) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(ui.displayCleared);
    notifyThatCorrectButtonHasBeenClicked(ui);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(ui.displayCleared);

    ui.displayCleared = false;
    notifyThatIncorrectButtonHasBeenClicked(ui);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(ui.displayCleared);
}
}
}
