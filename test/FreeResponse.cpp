#include "ConfigurationRegistryStub.hpp"
#include "TestPresenterStub.hpp"
#include "assert-utility.hpp"
#include "TestViewStub.hpp"
#include "TestControllerStub.hpp"
#include "PuzzleStub.hpp"
#include "TimerStub.hpp"

#include <av-speech-in-noise/ui/FreeResponse.hpp>

#include <gtest/gtest.h>

#include <utility>

namespace av_speech_in_noise::submitting_free_response {
namespace {
class ControlStub : public Control {
  public:
    void notifyThatSubmitButtonHasBeenClicked() {
        listener_->notifyThatSubmitButtonHasBeenClicked();
    }

    void attach(Observer *e) override { listener_ = e; }

    void setFreeResponse(std::string c) { freeResponse_ = std::move(c); }

    auto response() -> std::string override { return freeResponse_; }

    auto flagged() -> bool override { return flagged_; }

    void setFlagged() { flagged_ = true; }

  private:
    std::string freeResponse_{"0"};
    Observer *listener_{};
    bool flagged_{};
};

class ViewStub : public View {
  public:
    void show() override { freeResponseSubmissionShown_ = true; }

    [[nodiscard]] auto freeResponseSubmissionShown() const -> bool {
        return freeResponseSubmissionShown_;
    }

    void hide() override { freeResponseSubmissionHidden_ = true; }

    [[nodiscard]] auto freeResponseSubmissionHidden() const -> bool {
        return freeResponseSubmissionHidden_;
    }

    void clearResponse() override { freeResponseCleared_ = true; }

    void clearFlag() override { flagCleared_ = true; }

    [[nodiscard]] auto freeResponseCleared() const -> bool {
        return freeResponseCleared_;
    }

    [[nodiscard]] auto flagCleared() const -> bool { return flagCleared_; }

  private:
    bool freeResponseSubmissionShown_{};
    bool freeResponseSubmissionHidden_{};
    bool freeResponseCleared_{};
    bool flagCleared_{};
};

void notifyThatSubmitButtonHasBeenClicked(ControlStub &view) {
    view.notifyThatSubmitButtonHasBeenClicked();
}

void stop(TaskPresenter &presenter) { presenter.stop(); }

void start(TaskPresenter &presenter) { presenter.start(); }

class InteractorStub : public Interactor {
  public:
    void submit(const FreeResponse &s) override { freeResponse_ = s; }

    auto response() -> FreeResponse { return freeResponse_; }

  private:
    FreeResponse freeResponse_;
};

class FreeResponseControllerTests : public ::testing::Test {
  protected:
    InteractorStub model;
    ControlStub control;
    PuzzleStub puzzle;
    TimerStub timer;
    TestControllerStub testController;
    ConfigurationRegistryStub registry;
    Controller controller{
        registry, testController, model, control, puzzle, timer};
};

class FreeResponsePresenterTests : public ::testing::Test {
  protected:
    TestViewStub testView;
    ViewStub view;
    PuzzleStub puzzle;
    ConfigurationRegistryStub registry;
    TestPresenterStub testPresenter;
    Presenter presenter{registry, testView, view, testPresenter, puzzle};
};

#define FREE_RESPONSE_CONTROLLER_TEST(a) TEST_F(FreeResponseControllerTests, a)

#define FREE_RESPONSE_PRESENTER_TEST(a) TEST_F(FreeResponsePresenterTests, a)

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).freeResponseSubmissionHidden())

FREE_RESPONSE_PRESENTER_TEST(presenterHidesResponseSubmission) {
    presenter.hideResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

FREE_RESPONSE_PRESENTER_TEST(presenterHidesResponseButtonsWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

FREE_RESPONSE_PRESENTER_TEST(presenterShowsReadyButtonWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testView.nextTrialButtonShown());
}

FREE_RESPONSE_PRESENTER_TEST(
    presenterShowsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.freeResponseSubmissionShown());
}

FREE_RESPONSE_PRESENTER_TEST(
    presenterClearsFreeResponseWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.freeResponseCleared());
}

FREE_RESPONSE_PRESENTER_TEST(presenterClearsFlagWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.flagCleared());
}

FREE_RESPONSE_CONTROLLER_TEST(
    controllerSubmitsFreeResponseAfterResponseButtonIsClicked) {
    control.setFreeResponse("a");
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, model.response().response);
}

FREE_RESPONSE_CONTROLLER_TEST(
    controllerSubmitsFlaggedFreeResponseAfterResponseButtonIsClicked) {
    control.setFlagged();
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.response().flagged);
}

FREE_RESPONSE_CONTROLLER_TEST(
    controllerNotifiesThatUserIsReadyForNextTrialAfterResponseButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
}

FREE_RESPONSE_PRESENTER_TEST(presenterResetsPuzzleOnStart) {
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(puzzle.hasBeenReset());
}

FREE_RESPONSE_PRESENTER_TEST(tbd) {
    presenter.configure("method", "fixed-level free response with replacement");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(&presenter, testPresenter.taskPresenter);
}

FREE_RESPONSE_CONTROLLER_TEST(
    controllerNotifiesThatUserHasRespondedButTrialIsNotQuiteDoneAfterResponseButtonIsClickedWhenUsingPuzzle) {
    controller.setNTrialsPerNewPuzzlePiece(1);
    controller.configure("puzzle", "/Users/user/puzzle.png");
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserHasRespondedButTrialIsNotQuiteDone());
}

FREE_RESPONSE_CONTROLLER_TEST(
    controllerNotifiesThatUserHasRespondedButTrialIsNotQuiteDoneAfterResponseButtonIsClickedWhenUsingPuzzleEveryNTrials) {
    controller.setNTrialsPerNewPuzzlePiece(3);
    controller.configure("puzzle", "/Users/user/puzzle.png");
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

FREE_RESPONSE_CONTROLLER_TEST(
    controllerNotifiesThatUserIsDoneRespondingAfterResponseButtonIsClickedIfFlaggedAndUsingPuzzle) {
    controller.configure("puzzle", "/Users/user/puzzle.png");
    control.setFlagged();
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
}

FREE_RESPONSE_CONTROLLER_TEST(
    controllerShowsPuzzleAfterResponseButtonIsClickedWhenUsingPuzzle) {
    controller.setNTrialsPerNewPuzzlePiece(1);
    controller.configure("puzzle", "/Users/user/puzzle.png");
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(puzzle.shown());
}

FREE_RESPONSE_CONTROLLER_TEST(
    controllerSchedulesCallbackAfterResponseButtonIsClickedWhenUsingPuzzle) {
    controller.setNTrialsPerNewPuzzlePiece(1);
    controller.configure("puzzle", "/Users/user/puzzle.png");
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(timer.callbackScheduled());
}

FREE_RESPONSE_CONTROLLER_TEST(
    controllerAdvancesPuzzleAfterFirstCallbackWhenUsingPuzzle) {
    controller.setNTrialsPerNewPuzzlePiece(1);
    controller.configure("puzzle", "/Users/user/puzzle.png");
    notifyThatSubmitButtonHasBeenClicked(control);
    timer.callback();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(puzzle.advanced());
}

FREE_RESPONSE_CONTROLLER_TEST(
    controllerHidesPuzzleAfterSecondCallbackWhenUsingPuzzle) {
    controller.configure("puzzle", "/Users/user/puzzle.png");
    notifyThatSubmitButtonHasBeenClicked(control);
    timer.callback();
    timer.callback();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(puzzle.hidden());
}

FREE_RESPONSE_CONTROLLER_TEST(controllerOnlyAdvancesPuzzleOnceWhenUsingPuzzle) {
    controller.configure("puzzle", "/Users/user/puzzle.png");
    notifyThatSubmitButtonHasBeenClicked(control);
    timer.callback();
    puzzle.clearAdvanced();
    timer.callback();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(puzzle.advanced());
}

FREE_RESPONSE_CONTROLLER_TEST(
    controllerSchedulesCallbackAfterPuzzleAdvancedWhenUsingPuzzle) {
    controller.setNTrialsPerNewPuzzlePiece(1);
    controller.configure("puzzle", "/Users/user/puzzle.png");
    notifyThatSubmitButtonHasBeenClicked(control);
    timer.clearCallbackCount();
    timer.callback();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(timer.callbackScheduled());
}

FREE_RESPONSE_CONTROLLER_TEST(
    controllerNotifiesThatUserIsDoneRespondingAfterPuzzleHiddenWhenUsingPuzzle) {
    controller.configure("puzzle", "/Users/user/puzzle.png");
    notifyThatSubmitButtonHasBeenClicked(control);
    timer.callback();
    timer.callback();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
}

FREE_RESPONSE_CONTROLLER_TEST(tbd) {
    controller.configure("puzzle", "/Users/user/puzzle.png");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "/Users/user/puzzle.png", puzzle.url().path);
}
}
}
