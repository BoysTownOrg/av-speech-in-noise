#include "assert-utility.hpp"
#include "ModelStub.hpp"
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

class ViewStub : public SessionView {
  public:
    void showErrorMessage(std::string s) override {
        errorMessage_ = std::move(s);
    }

    auto errorMessage() -> std::string { return errorMessage_; }

    auto audioDevice() -> std::string override { return {}; }
    void eventLoop() override {}
    auto browseForDirectory() -> std::string override { return {}; }
    auto browseCancelled() -> bool override { return {}; }
    auto browseForOpeningFile() -> std::string override { return {}; }
    void populateAudioDeviceMenu(std::vector<std::string>) override {}

  private:
    std::string errorMessage_;
};

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

class CorrectKeywordsTests : public ::testing::Test {
  protected:
    ModelStub model;
    ViewStub mainView;
    ExperimenterViewStub experimenterView;
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
