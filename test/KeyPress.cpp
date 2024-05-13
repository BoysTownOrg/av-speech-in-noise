#include "assert-utility.hpp"
#include "TestViewStub.hpp"
#include "TestControllerStub.hpp"

#include <av-speech-in-noise/ui/KeyPress.hpp>

#include <gtest/gtest.h>

#include <utility>

namespace av_speech_in_noise::submitting_keypress {
namespace {
class ControlStub : public Control {
  public:
    void attach(Observer *e) override { listener_ = e; }

    auto keyPressed() -> std::string override { return keyPressed_; }

    Observer *listener_{};
    std::string keyPressed_{};
};

class InteractorStub : public Interactor {
  public:
    void submit(const KeyPressResponse &s) override {
        submitted = true;
        response = s;
    }

    KeyPressResponse response{};
    bool submitted{false};
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
    presenter.showResponseSubmission();
    control.listener_->notifyThatKeyHasBeenPressed();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(KeyPressed::second, model.response.key);
}

KEY_PRESS_UI_TEST(waitsUntilResponseSubmissionShownToSubmitKeyPress) {
    control.keyPressed_ = "1";
    control.listener_->notifyThatKeyHasBeenPressed();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(model.submitted);
    presenter.showResponseSubmission();
    control.listener_->notifyThatKeyHasBeenPressed();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(KeyPressed::first, model.response.key);
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
    presenter.showResponseSubmission();
    control.listener_->notifyThatKeyHasBeenPressed();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
}
/*

KEY_PRESS_UI_TEST(presenterResetsPuzzleOnStart) {
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(puzzle.hasBeenReset());
}

KEY_PRESS_UI_TEST(
    controllerNotifiesThatUserHasRespondedButTrialIsNotQuiteDoneAfterResponseButtonIsClickedWhenUsingPuzzle)
{ controller.setNTrialsPerNewPuzzlePiece(1); controller.initialize(true);
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserHasRespondedButTrialIsNotQuiteDone());
}

KEY_PRESS_UI_TEST(
    controllerNotifiesThatUserHasRespondedButTrialIsNotQuiteDoneAfterResponseButtonIsClickedWhenUsingPuzzleEveryNTrials)
{ controller.setNTrialsPerNewPuzzlePiece(3); controller.initialize(true);
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(
        testController.notifiedThatUserHasRespondedButTrialIsNotQuiteDone());
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(
        testController.notifiedThatUserHasRespondedButTrialIsNotQuiteDone());
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserHasRespondedButTrialIsNotQuiteDone());
    testController.clearNotifications();
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(
        testController.notifiedThatUserHasRespondedButTrialIsNotQuiteDone());
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(
        testController.notifiedThatUserHasRespondedButTrialIsNotQuiteDone());
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserHasRespondedButTrialIsNotQuiteDone());
}

KEY_PRESS_UI_TEST(
    controllerNotifiesThatUserIsDoneRespondingAfterResponseButtonIsClickedIfFlaggedAndUsingPuzzle)
{ controller.initialize(true); control.setFlagged();
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
}

KEY_PRESS_UI_TEST(
    controllerShowsPuzzleAfterResponseButtonIsClickedWhenUsingPuzzle) {
    controller.setNTrialsPerNewPuzzlePiece(1);
    controller.initialize(true);
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(puzzle.shown());
}

KEY_PRESS_UI_TEST(
    controllerSchedulesCallbackAfterResponseButtonIsClickedWhenUsingPuzzle) {
    controller.setNTrialsPerNewPuzzlePiece(1);
    controller.initialize(true);
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(timer.callbackScheduled());
}

KEY_PRESS_UI_TEST(
    controllerAdvancesPuzzleAfterFirstCallbackWhenUsingPuzzle) {
    controller.setNTrialsPerNewPuzzlePiece(1);
    controller.initialize(true);
    notifyThatSubmitButtonHasBeenClicked(control);
    timer.callback();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(puzzle.advanced());
}

KEY_PRESS_UI_TEST(
    controllerHidesPuzzleAfterSecondCallbackWhenUsingPuzzle) {
    controller.initialize(true);
    notifyThatSubmitButtonHasBeenClicked(control);
    timer.callback();
    timer.callback();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(puzzle.hidden());
}

KEY_PRESS_UI_TEST(controllerOnlyAdvancesPuzzleOnceWhenUsingPuzzle) {
    controller.initialize(true);
    notifyThatSubmitButtonHasBeenClicked(control);
    timer.callback();
    puzzle.clearAdvanced();
    timer.callback();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(puzzle.advanced());
}

KEY_PRESS_UI_TEST(
    controllerSchedulesCallbackAfterPuzzleAdvancedWhenUsingPuzzle) {
    controller.setNTrialsPerNewPuzzlePiece(1);
    controller.initialize(true);
    notifyThatSubmitButtonHasBeenClicked(control);
    timer.clearCallbackCount();
    timer.callback();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(timer.callbackScheduled());
}

KEY_PRESS_UI_TEST(
    controllerNotifiesThatUserIsDoneRespondingAfterPuzzleHiddenWhenUsingPuzzle)
{ controller.initialize(true); notifyThatSubmitButtonHasBeenClicked(control);
    timer.callback();
    timer.callback();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
}
*/
}
}
