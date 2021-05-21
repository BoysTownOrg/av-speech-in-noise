#ifndef AV_SPEECH_IN_NOISE_TEST_TESTVIEWSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TEST_TESTVIEWSTUB_HPP_

#include <av-speech-in-noise/ui/Test.hpp>

namespace av_speech_in_noise {
class TestViewStub : public TestView {
  public:
    void declineContinuingTesting() { listener_->declineContinuingTesting(); }

    [[nodiscard]] auto continueTestingDialogMessage() const -> std::string {
        return continueTestingDialogMessage_;
    }

    auto sheetMessage() -> std::string { return sheetMessage_; }

    void showSheet(std::string_view s) override { sheetMessage_ = s; }

    void setContinueTestingDialogMessage(const std::string &s) override {
        continueTestingDialogMessage_ = s;
    }

    void acceptContinuingTesting() { listener_->acceptContinuingTesting(); }

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

    void exitTest() { listener_->exitTest(); }

    void notifyThatPlayTrialButtonHasBeenClicked() { listener_->playTrial(); }

    void display(std::string s) override { displayed_ = std::move(s); }

    [[nodiscard]] auto displayed() const { return displayed_; }

    void secondaryDisplay(std::string s) override {
        secondaryDisplayed_ = std::move(s);
    }

    [[nodiscard]] auto secondaryDisplayed() const {
        return secondaryDisplayed_;
    }

    void informUser(const std::string &s) { messageToSubject_ = s; }

    auto messageToSubject() -> std::string { return messageToSubject_; }

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

  private:
    std::string displayed_;
    std::string secondaryDisplayed_;
    std::string continueTestingDialogMessage_;
    std::string response_;
    std::string messageToSubject_;
    std::string correctKeywords_{"0"};
    std::string sheetMessage_;
    TestControl::Observer *listener_{};
    bool exitTestButtonHidden_{};
    bool exitTestButtonShown_{};
    bool nextTrialButtonShown_{};
    bool nextTrialButtonHidden_{};
    bool continueTestingDialogShown_{};
    bool continueTestingDialogHidden_{};
    bool shown_{};
    bool hidden_{};
};
}

#endif
