#include "ConfigurationRegistryStub.hpp"
#include "TestPresenterStub.hpp"
#include "TimerStub.hpp"
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

    void forceSubmit(const std::vector<KeyPressResponse> &) override {
        forceSubmitted = true;
    }

    void deferNextTrial() override { trialAdvancementDeferred = true; }

    void dontDeferNextTrial() override { trialAdvancementNotDeferred = true; }

    std::vector<KeyPressResponse> responses{};
    bool submitted{false};
    bool submits_{false};
    bool forceSubmitted{false};
    bool trialAdvancementDeferred{};
    bool trialAdvancementNotDeferred{};
};

class TaskPresenterStub : public TaskPresenter {
  public:
    void showResponseSubmission() override { responseShown = true; }
    void hideResponseSubmission() override { responseHidden = true; }
    void start() override {}
    void stop() override {}

    bool responseShown{};
    bool responseHidden{};
};

class KeyPressUITests : public ::testing::Test {
  protected:
    TestViewStub testView;
    InteractorStub model;
    ControlStub control;
    TestControllerStub testController;
    TimerStub timer;
    ConfigurationRegistryStub registry;
    TestPresenterStub testPresenter;
    Presenter presenter{registry, testView, testController, model, control,
        timer, testPresenter};
    TaskPresenterStub dualTask;
};

#define KEY_PRESS_UI_TEST(a) TEST_F(KeyPressUITests, a)

KEY_PRESS_UI_TEST(presenterShowsReadyButtonWhenStarted) {
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testView.nextTrialButtonShown());
}

KEY_PRESS_UI_TEST(submitsKeyPressResponseOnKeyPress) {
    presenter.notifyThatTrialHasStarted();
    control.keyPressed_ = "2";
    control.listener_->notifyThatKeyHasBeenPressed();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(KeyPressed::second, model.responses[0].key);
}

KEY_PRESS_UI_TEST(attemptsToSubmitWhenResponseSubmissionShown) {
    presenter.notifyThatTrialHasStarted();
    control.keyPressed_ = "1";
    control.listener_->notifyThatKeyHasBeenPressed();
    model.responses.clear();
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(KeyPressed::first, model.responses[0].key);
}

KEY_PRESS_UI_TEST(doesntSubmitResponseFromUnknownKey) {
    presenter.notifyThatTrialHasStarted();
    control.keyPressed_ = "?";
    control.listener_->notifyThatKeyHasBeenPressed();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(model.submitted);
}

KEY_PRESS_UI_TEST(doesntSubmitResponseBeforeTrialHasStarted) {
    control.listener_->notifyThatKeyHasBeenPressed();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(model.submitted);
}

KEY_PRESS_UI_TEST(doesntSubmitResponseAfterStopped) {
    presenter.notifyThatTrialHasStarted();
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

KEY_PRESS_UI_TEST(schedulesCallbackWhenStimulusEnds) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(timer.callbackScheduled());
}

KEY_PRESS_UI_TEST(validResponseCancelsCallback) {
    presenter.notifyThatTrialHasStarted();
    model.submits_ = true;
    control.listener_->notifyThatKeyHasBeenPressed();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(timer.callbackCancelled);
}

KEY_PRESS_UI_TEST(stoppingBeforeResponseCancelsCallback) {
    presenter.notifyThatTrialHasStarted();
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(timer.callbackCancelled);
    presenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(timer.callbackCancelled);
}

KEY_PRESS_UI_TEST(dualTaskDefersTrialAdvancement) {
    presenter.enableDualTask(&dualTask);
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.trialAdvancementDeferred);
}

KEY_PRESS_UI_TEST(singleTaskDoesNotDeferTrialAdvancement) {
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.trialAdvancementNotDeferred);
}

KEY_PRESS_UI_TEST(dualTaskShowsDualResponse) {
    presenter.enableDualTask(&dualTask);
    presenter.start();
    presenter.notifyThatTrialHasStarted();
    model.submits_ = true;
    control.listener_->notifyThatKeyHasBeenPressed();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(dualTask.responseShown);
}

KEY_PRESS_UI_TEST(dualTaskHidesDualResponse) {
    presenter.enableDualTask(&dualTask);
    presenter.start();
    presenter.hideResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(dualTask.responseHidden);
}

KEY_PRESS_UI_TEST(tbd) {
    presenter.configure(
        "method", "fixed-level button response predetermined stimuli");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(&presenter, testPresenter.taskPresenter);
}
}
}
