#ifndef AV_SPEECH_IN_NOISE_TEST_TESTVIEWSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TEST_TESTVIEWSTUB_HPP_

#include <presentation/Test.hpp>

namespace av_speech_in_noise {
class TestViewStub : public TestView {
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
}

#endif
