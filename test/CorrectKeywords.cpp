#include "ConfigurationRegistryStub.hpp"
#include "TestPresenterStub.hpp"
#include "assert-utility.hpp"
#include "SessionViewStub.hpp"
#include "TestViewStub.hpp"
#include "TestControllerStub.hpp"

#include <av-speech-in-noise/ui/CorrectKeywords.hpp>

#include <gtest/gtest.h>

#include <utility>

namespace av_speech_in_noise::submitting_number_keywords {
namespace {
class InteractorStub : public Interactor {
  public:
    void submit(const CorrectKeywords &s) override { correctKeywords_ = s; }

    auto correctKeywords() -> CorrectKeywords { return correctKeywords_; }

  private:
    CorrectKeywords correctKeywords_;
};

class ControlStub : public Control {
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

class ViewStub : public View {
  public:
    void show() override { correctKeywordsSubmissionShown_ = true; }

    [[nodiscard]] auto correctKeywordsSubmissionShown() const -> bool {
        return correctKeywordsSubmissionShown_;
    }

    void hide() override { correctKeywordsSubmissionHidden_ = true; }

    [[nodiscard]] auto correctKeywordsSubmissionHidden() const -> bool {
        return correctKeywordsSubmissionHidden_;
    }

  private:
    bool correctKeywordsSubmissionShown_{};
    bool correctKeywordsSubmissionHidden_{};
};

void notifyThatSubmitButtonHasBeenClicked(ControlStub &view) {
    view.notifyThatSubmitButtonHasBeenClicked();
}

void stop(TaskPresenter &presenter) { presenter.stop(); }

void start(TaskPresenter &presenter) { presenter.start(); }

class CorrectKeywordsControllerTests : public ::testing::Test {
  protected:
    InteractorStub model;
    SessionViewStub sessionView;
    ControlStub control;
    TestControllerStub testController;
    Controller controller{testController, model, sessionView, control};
};

class CorrectKeywordsPresenterTests : public ::testing::Test {
  protected:
    TestViewStub testView;
    ViewStub view;
    ConfigurationRegistryStub registry;
    TestPresenterStub testPresenter;
    Presenter presenter{registry, testView, view, testPresenter};
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

CORRECT_KEYWORDS_PRESENTER_TEST(tbd) {
    presenter.configure("method", "adaptive number keywords");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(&presenter, testPresenter.taskPresenter);
}

CORRECT_KEYWORDS_CONTROLLER_TEST(
    responderSubmitsConsonantAfterResponseButtonIsClicked) {
    control.setCorrectKeywords("1");
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, model.correctKeywords().count);
}

CORRECT_KEYWORDS_CONTROLLER_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterResponseButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testController
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
