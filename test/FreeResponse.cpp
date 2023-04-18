#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "TestViewStub.hpp"
#include "TestControllerStub.hpp"
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
    TestControllerStub testController;
    Controller controller{testController, model, control};
};

class FreeResponsePresenterTests : public ::testing::Test {
  protected:
    TestViewStub testView;
    ViewStub view;
    Presenter presenter{testView, view};
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
}

namespace with_puzzle {
namespace {
class PuzzleStub : public Puzzle {
  public:
    [[nodiscard]] auto advanced() const -> bool { return advanced_; }

    void advance() override { advanced_ = true; }

    void reset() override {}

    [[nodiscard]] auto shown() const -> bool { return shown_; }

    void show() override { shown_ = true; }

    [[nodiscard]] auto hidden() const -> bool { return hidden_; }

    void hide() override { hidden_ = true; }

    void clearAdvanced() { advanced_ = false; }

  private:
    bool advanced_{};
    bool shown_{};
    bool hidden_{};
};

class TimerStub : public Timer {
  public:
    void scheduleCallbackAfterSeconds(double) override {
        callbackScheduled_ = true;
    }

    [[nodiscard]] auto callbackScheduled() const { return callbackScheduled_; }

    void clearCallbackCount() { callbackScheduled_ = false; }

    void callback() { observer->callback(); }

    void attach(Observer *a) override { observer = a; }

  private:
    Observer *observer{};
    bool callbackScheduled_{};
};

class FreeResponseControllerWithPuzzleTests : public ::testing::Test {
  protected:
    InteractorStub model;
    ControlStub control;
    PuzzleStub puzzle;
    TimerStub timer;
    TestControllerStub testController;
    Controller controller{testController, model, control, puzzle, timer};
};

class FreeResponsePresenterWithPuzzleTests : public ::testing::Test {
  protected:
    TestViewStub testView;
    ViewStub view;
    Presenter presenter{testView, view};
};

#define FREE_RESPONSE_CONTROLLER_WITH_PUZZLE_TEST(a)                           \
    TEST_F(FreeResponseControllerWithPuzzleTests, a)

#define FREE_RESPONSE_PRESENTER_WITH_PUZZLE_TEST(a)                            \
    TEST_F(FreeResponsePresenterWithPuzzleTests, a)

FREE_RESPONSE_PRESENTER_WITH_PUZZLE_TEST(presenterHidesResponseSubmission) {
    presenter.hideResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

FREE_RESPONSE_PRESENTER_WITH_PUZZLE_TEST(
    presenterHidesResponseButtonsWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

FREE_RESPONSE_PRESENTER_WITH_PUZZLE_TEST(presenterShowsReadyButtonWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testView.nextTrialButtonShown());
}

FREE_RESPONSE_PRESENTER_WITH_PUZZLE_TEST(
    presenterShowsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.freeResponseSubmissionShown());
}

FREE_RESPONSE_PRESENTER_WITH_PUZZLE_TEST(
    presenterClearsFreeResponseWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.freeResponseCleared());
}

FREE_RESPONSE_PRESENTER_WITH_PUZZLE_TEST(
    presenterClearsFlagWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.flagCleared());
}

FREE_RESPONSE_CONTROLLER_WITH_PUZZLE_TEST(
    controllerSubmitsFreeResponseAfterResponseButtonIsClicked) {
    control.setFreeResponse("a");
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, model.response().response);
}

FREE_RESPONSE_CONTROLLER_WITH_PUZZLE_TEST(
    controllerSubmitsFlaggedFreeResponseAfterResponseButtonIsClicked) {
    control.setFlagged();
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.response().flagged);
}

FREE_RESPONSE_CONTROLLER_WITH_PUZZLE_TEST(
    controllerNotifiesThatUserHasRespondedButTrialIsNotQuiteDoneAfterResponseButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserHasRespondedButTrialIsNotQuiteDone());
}

FREE_RESPONSE_CONTROLLER_WITH_PUZZLE_TEST(
    controllerNotifiesThatUserIsDoneRespondingAfterResponseButtonIsClickedIfFlagged) {
    control.setFlagged();
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
}

FREE_RESPONSE_CONTROLLER_WITH_PUZZLE_TEST(
    controllerShowsPuzzleAfterResponseButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(puzzle.shown());
}

FREE_RESPONSE_CONTROLLER_WITH_PUZZLE_TEST(
    controllerSchedulesCallbackAfterResponseButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(timer.callbackScheduled());
}

FREE_RESPONSE_CONTROLLER_WITH_PUZZLE_TEST(
    controllerAdvancesPuzzleAfterFirstCallback) {
    notifyThatSubmitButtonHasBeenClicked(control);
    timer.callback();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(puzzle.advanced());
}

FREE_RESPONSE_CONTROLLER_WITH_PUZZLE_TEST(
    controllerHidesPuzzleAfterSecondCallback) {
    notifyThatSubmitButtonHasBeenClicked(control);
    timer.callback();
    timer.callback();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(puzzle.hidden());
}

FREE_RESPONSE_CONTROLLER_WITH_PUZZLE_TEST(controllerOnlyAdvancesPuzzleOnce) {
    notifyThatSubmitButtonHasBeenClicked(control);
    timer.callback();
    puzzle.clearAdvanced();
    timer.callback();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(puzzle.advanced());
}

FREE_RESPONSE_CONTROLLER_WITH_PUZZLE_TEST(
    controllerSchedulesCallbackAfterPuzzleAdvanced) {
    notifyThatSubmitButtonHasBeenClicked(control);
    timer.clearCallbackCount();
    timer.callback();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(timer.callbackScheduled());
}

FREE_RESPONSE_CONTROLLER_WITH_PUZZLE_TEST(
    controllerNotifiesThatUserIsDoneRespondingAfterPuzzleHidden) {
    notifyThatSubmitButtonHasBeenClicked(control);
    timer.callback();
    timer.callback();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
}
}
}
}
