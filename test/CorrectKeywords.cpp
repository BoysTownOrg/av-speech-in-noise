#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "TaskResponderListenerStub.hpp"
#include "ExperimenterResponderStub.hpp"
#include <presentation/CorrectKeywords.hpp>
#include <gtest/gtest.h>
#include <utility>

namespace av_speech_in_noise {
namespace {
class CorrectKeywordsInputViewStub : public CorrectKeywordsInputView {
  public:
    void notifyThatSubmitButtonHasBeenClicked() {
        listener_->notifyThatSubmitButtonHasBeenClicked();
    }

    void subscribe(EventListener *e) override { listener_ = e; }

    void setCorrectKeywords(std::string c) { correctKeywords_ = std::move(c); }

    auto correctKeywords() -> std::string override { return correctKeywords_; }

  private:
    std::string correctKeywords_{"0"};
    EventListener *listener_{};
};

class CorrectKeywordsOutputViewStub : public CorrectKeywordsOutputView {
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

void notifyThatSubmitButtonHasBeenClicked(CorrectKeywordsInputViewStub &view) {
    view.notifyThatSubmitButtonHasBeenClicked();
}

void notifyThatUserIsDoneResponding(TaskPresenter &presenter) {
    presenter.notifyThatUserIsDoneResponding();
}

void notifyThatTrialHasStarted(TaskPresenter &presenter) {
    presenter.notifyThatTrialHasStarted();
}

void stop(TaskPresenter &presenter) { presenter.stop(); }

void start(TaskPresenter &presenter) { presenter.start(); }

class ViewStub : public View {
  public:
    void setAudioDevice(std::string s) { audioDevice_ = std::move(s); }

    auto audioDevice() -> std::string override { return audioDevice_; }

    void showErrorMessage(std::string s) override {
        errorMessage_ = std::move(s);
    }

    auto errorMessage() { return errorMessage_; }

    void eventLoop() override { eventLoopCalled_ = true; }

    [[nodiscard]] auto eventLoopCalled() const { return eventLoopCalled_; }

    auto browseForDirectory() -> std::string override {
        return browseForDirectoryResult_;
    }

    auto browseCancelled() -> bool override { return browseCancelled_; }

    auto browseForOpeningFile() -> std::string override {
        return browseForOpeningFileResult_;
    }

    void setBrowseForOpeningFileResult(std::string s) {
        browseForOpeningFileResult_ = std::move(s);
    }

    void setBrowseCancelled() { browseCancelled_ = true; }

    void populateAudioDeviceMenu(std::vector<std::string> v) override {
        audioDevices_ = std::move(v);
    }

    [[nodiscard]] auto audioDevices() const { return audioDevices_; }

  private:
    std::vector<std::string> audioDevices_;
    std::string errorMessage_;
    std::string browseForDirectoryResult_;
    std::string browseForOpeningFileResult_;
    std::string audioDevice_;
    bool eventLoopCalled_{};
    bool browseCancelled_{};
};

class ExperimenterOutputViewStub : public ExperimenterOutputView {
  public:
    [[nodiscard]] auto continueTestingDialogMessage() const -> std::string {
        return continueTestingDialogMessage_;
    }

    void setContinueTestingDialogMessage(const std::string &s) override {
        continueTestingDialogMessage_ = s;
    }

    void showContinueTestingDialog() override {
        continueTestingDialogShown_ = true;
    }

    [[nodiscard]] auto continueTestingDialogShown() const -> bool {
        return continueTestingDialogShown_;
    }

    void hideContinueTestingDialog() override {
        continueTestingDialogHidden_ = true;
    }

    [[nodiscard]] auto continueTestingDialogHidden() const -> bool {
        return continueTestingDialogHidden_;
    }

    void show() override { shown_ = true; }

    [[nodiscard]] auto shown() const { return shown_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const { return hidden_; }

    void display(std::string s) override { displayed_ = std::move(s); }

    [[nodiscard]] auto displayed() const { return displayed_; }

    void secondaryDisplay(std::string s) override {
        secondaryDisplayed_ = std::move(s);
    }

    [[nodiscard]] auto secondaryDisplayed() const {
        return secondaryDisplayed_;
    }

    void showNextTrialButton() override { nextTrialButtonShown_ = true; }

    [[nodiscard]] auto nextTrialButtonShown() const {
        return nextTrialButtonShown_;
    }

    void hideNextTrialButton() override { nextTrialButtonHidden_ = true; }

    [[nodiscard]] auto nextTrialButtonHidden() const {
        return nextTrialButtonHidden_;
    }

    void showExitTestButton() override { exitTestButtonShown_ = true; }

    [[nodiscard]] auto exitTestButtonShown() const {
        return exitTestButtonShown_;
    }

    void hideExitTestButton() override { exitTestButtonHidden_ = true; }

    [[nodiscard]] auto exitTestButtonHidden() const {
        return exitTestButtonHidden_;
    }

    [[nodiscard]] auto freeResponseCleared() const -> bool {
        return freeResponseCleared_;
    }

  private:
    std::string displayed_;
    std::string secondaryDisplayed_;
    std::string continueTestingDialogMessage_;
    std::string response_;
    std::string correctKeywords_{"0"};
    bool freeResponseCleared_{};
    bool exitTestButtonHidden_{};
    bool exitTestButtonShown_{};
    bool nextTrialButtonShown_{};
    bool nextTrialButtonHidden_{};
    bool continueTestingDialogShown_{};
    bool continueTestingDialogHidden_{};
    bool shown_{};
    bool hidden_{};
};

class CorrectKeywordsTests : public ::testing::Test {
  protected:
    ModelStub model;
    ViewStub mainView;
    ExperimenterOutputViewStub experimenterOutputView;
    CorrectKeywordsInputViewStub inputView;
    CorrectKeywordsOutputViewStub outputView;
    CorrectKeywordsResponder responder{model, mainView, inputView};
    CorrectKeywordsPresenter presenter{experimenterOutputView, outputView};
    ExperimenterResponderStub experimenterResponder;
    TaskResponderListenerStub taskResponder;

    CorrectKeywordsTests() {
        responder.subscribe(&experimenterResponder);
        responder.subscribe(&taskResponder);
    }
};

#define CORRECT_KEYWORDS_TEST(a) TEST_F(CorrectKeywordsTests, a)

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).correctKeywordsSubmissionHidden())

#define AV_SPEECH_IN_NOISE_EXPECT_CURSOR_HIDDEN(a)                             \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(cursorHidden(a))

CORRECT_KEYWORDS_TEST(presenterHidesReadyButtonWhenTaskStarts) {
    presenter.notifyThatTaskHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterOutputView.nextTrialButtonHidden());
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
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterOutputView.nextTrialButtonShown());
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
        experimenterResponder
            .continueTestingDialogShownWithResultsWhenComplete());
}

CORRECT_KEYWORDS_TEST(
    responderNotifiesThatUserIsDoneRespondingAfterResponseButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        taskResponder.notifiedThatUserIsDoneResponding());
}

CORRECT_KEYWORDS_TEST(responderShowsErrorMessageWhenInvalidCorrectKeywords) {
    inputView.setCorrectKeywords("a");
    notifyThatSubmitButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "\"a\" is not a valid number.", mainView.errorMessage());
}
}
}
