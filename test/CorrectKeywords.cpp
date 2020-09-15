#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "SessionViewStub.hpp"
#include "TestViewStub.hpp"
#include "TaskControllerObserverStub.hpp"
#include "TestControllerStub.hpp"
#include <presentation/CorrectKeywords.hpp>
#include <gtest/gtest.h>
#include <utility>

namespace av_speech_in_noise {
namespace {
class CorrectKeywordsControlStub : public CorrectKeywordsControl {
  public:
    void notifyThatSubmitButtonHasBeenClicked() {
        listener_->notifyThatSubmitButtonHasBeenClicked();
    }

    void attach(Observer *e) override { listener_ = e; }

    void setCorrectKeywords(std::string c) { correctKeywords_ = std::move(c); }

    auto correctKeywords() -> std::string override { return correctKeywords_; }

  private:
    std::string correctKeywords_{"0"};
    Observer *listener_{};
};

class CorrectKeywordsViewStub : public CorrectKeywordsView {
  public:
    void showCorrectKeywordsSubmission() override {
        correctKeywordsSubmissionShown_ = true;
    }

    [[nodiscard]] auto correctKeywordsSubmissionShown() const -> bool {
        return correctKeywordsSubmissionShown_;
    }

    void hideCorrectKeywordsSubmission() override {
        correctKeywordsSubmissionHidden_ = true;
    }

    [[nodiscard]] auto correctKeywordsSubmissionHidden() const -> bool {
        return correctKeywordsSubmissionHidden_;
    }

  private:
    bool correctKeywordsSubmissionShown_{};
    bool correctKeywordsSubmissionHidden_{};
};

void notifyThatSubmitButtonHasBeenClicked(CorrectKeywordsControlStub &view) {
    view.notifyThatSubmitButtonHasBeenClicked();
}

void notifyThatUserIsDoneResponding(TaskPresenter &presenter) {
    presenter.notifyThatUserIsDoneResponding();
}

void stop(TaskPresenter &presenter) { presenter.stop(); }

void start(TaskPresenter &presenter) { presenter.start(); }

class CorrectKeywordsTests : public ::testing::Test {
  protected:
    ModelStub model;
    SessionViewStub mainView;
    TestViewStub experimenterView;
    CorrectKeywordsControlStub inputView;
    CorrectKeywordsViewStub outputView;
    CorrectKeywordsController responder{model, mainView, inputView};
    CorrectKeywordsPresenter presenter{experimenterView, outputView};
    TestControllerStub experimenterController;
    TaskControllerObserverStub taskController;

    CorrectKeywordsTests() {
        responder.attach(&experimenterController);
        responder.attach(&taskController);
    }
};

#define CORRECT_KEYWORDS_TEST(a) TEST_F(CorrectKeywordsTests, a)

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).correctKeywordsSubmissionHidden())

CORRECT_KEYWORDS_TEST(presenterHidesReadyButtonWhenTaskStarts) {
    presenter.notifyThatTaskHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(experimenterView.nextTrialButtonHidden());
}

CORRECT_KEYWORDS_TEST(presenterHidesResponseButtonsAfterUserIsDoneResponding) {
    notifyThatUserIsDoneResponding(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(outputView);
}

CORRECT_KEYWORDS_TEST(presenterHidesResponseButtonsWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(outputView);
}

CORRECT_KEYWORDS_TEST(presenterShowsReadyButtonWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(experimenterView.nextTrialButtonShown());
}

CORRECT_KEYWORDS_TEST(
    presenterShowsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputView.correctKeywordsSubmissionShown());
}

CORRECT_KEYWORDS_TEST(responderSubmitsConsonantAfterResponseButtonIsClicked) {
    inputView.setCorrectKeywords("1");
    notifyThatSubmitButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, model.correctKeywords());
}

CORRECT_KEYWORDS_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterResponseButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterController
            .notifiedThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion());
}

CORRECT_KEYWORDS_TEST(
    responderNotifiesThatUserIsDoneRespondingAfterResponseButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        taskController.notifiedThatUserIsDoneResponding());
}

CORRECT_KEYWORDS_TEST(responderShowsErrorMessageWhenInvalidCorrectKeywords) {
    inputView.setCorrectKeywords("a");
    notifyThatSubmitButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "\"a\" is not a valid number.", mainView.errorMessage());
}
}
}
