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

class CorrectKeywordsControllerTests : public ::testing::Test {
  protected:
    ModelStub model;
    SessionViewStub sessionView;
    CorrectKeywordsControlStub control;
    CorrectKeywordsController controller{model, sessionView, control};
    TestControllerStub testController;
    TaskControllerObserverStub taskController;

    CorrectKeywordsControllerTests() {
        controller.attach(&testController);
        controller.attach(&taskController);
    }
};

class CorrectKeywordsPresenterTests : public ::testing::Test {
  protected:
    TestViewStub testView;
    CorrectKeywordsViewStub view;
    CorrectKeywordsPresenter presenter{testView, view};
};

#define CORRECT_KEYWORDS_CONTROLLER_TEST(a)                                    \
    TEST_F(CorrectKeywordsControllerTests, a)

#define CORRECT_KEYWORDS_PRESENTER_TEST(a)                                     \
    TEST_F(CorrectKeywordsPresenterTests, a)

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).correctKeywordsSubmissionHidden())

CORRECT_KEYWORDS_PRESENTER_TEST(presenterHidesResponseButtons) {
    presenter.hideResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

CORRECT_KEYWORDS_PRESENTER_TEST(presenterHidesResponseButtonsWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

CORRECT_KEYWORDS_PRESENTER_TEST(presenterShowsReadyButtonWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testView.nextTrialButtonShown());
}

CORRECT_KEYWORDS_PRESENTER_TEST(
    presenterShowsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.correctKeywordsSubmissionShown());
}

CORRECT_KEYWORDS_CONTROLLER_TEST(
    responderSubmitsConsonantAfterResponseButtonIsClicked) {
    control.setCorrectKeywords("1");
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, model.correctKeywords());
}

CORRECT_KEYWORDS_CONTROLLER_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterResponseButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController
            .notifiedThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion());
}

CORRECT_KEYWORDS_CONTROLLER_TEST(
    responderShowsErrorMessageWhenInvalidCorrectKeywords) {
    control.setCorrectKeywords("a");
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "\"a\" is not a valid number.", sessionView.errorMessage());
}
}
}
