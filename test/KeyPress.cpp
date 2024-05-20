#include "assert-utility.hpp"
#include "TestViewStub.hpp"
#include "TestControllerStub.hpp"

#include <av-speech-in-noise/ui/KeyPress.hpp>

#include <gtest/gtest.h>

namespace av_speech_in_noise::submitting_keypress {
namespace {
class ControlStub : public Control {
  public:
    void attach(Observer *e) override { listener_ = e; }

    auto keyPressed() -> std::string override { return keyPressed_; }

    auto keyPressedSeconds() -> double override { return keyPressedSeconds_; }

    void giveKeyFocus() override { keyFocusGiven = true; }

    Observer *listener_{};
    std::string keyPressed_{"1"};
    double keyPressedSeconds_{};
    bool keyFocusGiven{};
};

class InteractorStub : public Interactor {
  public:
    auto submits(const std::vector<KeyPressResponse> &s) -> bool override {
        submitted = true;
        responses = s;
        return submits_;
    }

    std::vector<KeyPressResponse> responses{};
    bool submitted{false};
    bool submits_{false};
};

class KeyPressUITests : public ::testing::Test {
  protected:
    TestViewStub testView;
    InteractorStub model;
    ControlStub control;
    TestControllerStub testController;
    Presenter presenter{testView, testController, model, control};
};

#define KEY_PRESS_UI_TEST(a) TEST_F(KeyPressUITests, a)

KEY_PRESS_UI_TEST(presenterShowsReadyButtonWhenStarted) {
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testView.nextTrialButtonShown());
}

KEY_PRESS_UI_TEST(submitsKeyPressResponseOnKeyPress) {
    control.keyPressed_ = "2";
    presenter.notifyThatTrialHasStarted();
    control.listener_->notifyThatKeyHasBeenPressed();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(KeyPressed::second, model.responses[0].key);
}

KEY_PRESS_UI_TEST(waitsUntilResponseSubmissionShownToSubmitKeyPress) {
    control.keyPressed_ = "1";
    control.listener_->notifyThatKeyHasBeenPressed();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(model.submitted);
    presenter.notifyThatTrialHasStarted();
    control.listener_->notifyThatKeyHasBeenPressed();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(KeyPressed::first, model.responses[0].key);
}

KEY_PRESS_UI_TEST(doesntSubmitResponseFromUnknownKey) {
    presenter.showResponseSubmission();
    control.keyPressed_ = "?";
    control.listener_->notifyThatKeyHasBeenPressed();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(model.submitted);
}

KEY_PRESS_UI_TEST(doesntSubmitResponseAfterSubmissionHidden) {
    presenter.showResponseSubmission();
    presenter.hideResponseSubmission();
    control.listener_->notifyThatKeyHasBeenPressed();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(model.submitted);
}

KEY_PRESS_UI_TEST(doesntSubmitResponseAfterStopped) {
    presenter.showResponseSubmission();
    presenter.stop();
    control.listener_->notifyThatKeyHasBeenPressed();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(model.submitted);
}

KEY_PRESS_UI_TEST(
    notifiesThatUserIsReadyForNextTrialAfterResponseButtonIsClicked) {
    control.listener_->notifyThatKeyHasBeenPressed();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(
        testController.notifiedThatUserIsDoneResponding());
    presenter.notifyThatTrialHasStarted();
    model.submits_ = true;
    control.listener_->notifyThatKeyHasBeenPressed();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
}

KEY_PRESS_UI_TEST(passesKeyPressedSeconds) {
    presenter.notifyThatTrialHasStarted();
    control.keyPressedSeconds_ = .9;
    control.listener_->notifyThatKeyHasBeenPressed();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(0.9, model.responses[0].seconds);
}

KEY_PRESS_UI_TEST(givesKeyFocusAfterTrialStarts) {
    presenter.notifyThatTrialHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(control.keyFocusGiven);
}
}
}
