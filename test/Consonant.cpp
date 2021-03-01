#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "TaskControllerObserverStub.hpp"
#include "TestControllerStub.hpp"
#include <presentation/Consonant.hpp>
#include <gtest/gtest.h>
#include <utility>

namespace av_speech_in_noise {
namespace {
class ConsonantControlStub : public ConsonantTaskControl {
  public:
    void notifyThatReadyButtonHasBeenClicked() {
        listener_->notifyThatReadyButtonHasBeenClicked();
    }

    void attach(Observer *e) override { listener_ = e; }

    void notifyThatResponseButtonHasBeenClicked() {
        listener_->notifyThatResponseButtonHasBeenClicked();
    }

    void setConsonant(std::string c) { consonant_ = std::move(c); }

    auto consonant() -> std::string override { return consonant_; }

  private:
    std::string consonant_;
    Observer *listener_{};
};

class ConsonantViewStub : public ConsonantTaskView {
  public:
    void show() override { shown_ = true; }

    [[nodiscard]] auto shown() const -> bool { return shown_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const -> bool { return hidden_; }

    void showReadyButton() override { readyButtonShown_ = true; }

    [[nodiscard]] auto readyButtonShown() const -> bool {
        return readyButtonShown_;
    }

    void hideReadyButton() override { readyButtonHidden_ = true; }

    [[nodiscard]] auto readyButtonHidden() const -> bool {
        return readyButtonHidden_;
    }

    void hideResponseButtons() override { responseButtonsHidden_ = true; }

    [[nodiscard]] auto responseButtonsHidden() const -> bool {
        return responseButtonsHidden_;
    }

    void showResponseButtons() override { responseButtonsShown_ = true; }

    [[nodiscard]] auto responseButtonsShown() const -> bool {
        return responseButtonsShown_;
    }

    [[nodiscard]] auto cursorHidden() const -> bool { return cursorHidden_; }

    void hideCursor() override { cursorHidden_ = true; }

    void showCursor() override { cursorShown_ = true; }

    [[nodiscard]] auto cursorShown() const -> bool { return cursorShown_; }

  private:
    bool shown_{};
    bool hidden_{};
    bool responseButtonsShown_{};
    bool responseButtonsHidden_{};
    bool readyButtonShown_{};
    bool readyButtonHidden_{};
    bool cursorHidden_{};
    bool cursorShown_{};
};

class ConsonantTaskPresenterStub : public ConsonantTaskPresenter {
  public:
    void hideCursor() override { cursorHidden_ = true; }

    void hideReadyButton() override { readyButtonHidden_ = true; }

    [[nodiscard]] auto cursorHidden() const -> bool { return cursorHidden_; }

    [[nodiscard]] auto readyButtonHidden() const -> bool {
        return readyButtonHidden_;
    }

  private:
    bool cursorHidden_{};
    bool readyButtonHidden_{};
};

void notifyThatReadyButtonHasBeenClicked(ConsonantControlStub &view) {
    view.notifyThatReadyButtonHasBeenClicked();
}

auto cursorHidden(ConsonantViewStub &view) -> bool {
    return view.cursorHidden();
}

void stop(TaskPresenter &presenter) { presenter.stop(); }

void start(TaskPresenter &presenter) { presenter.start(); }

void notifyThatResponseButtonHasBeenClicked(ConsonantControlStub &view) {
    view.notifyThatResponseButtonHasBeenClicked();
}

class ConsonantTaskControllerTests : public ::testing::Test {
  protected:
    ModelStub model;
    ConsonantControlStub control;
    TestControllerStub testController;
    ConsonantTaskPresenterStub presenter;
    ConsonantTaskController controller{
        testController, model, control, presenter};
    TaskControllerObserverStub observer;
};

class ConsonantTaskPresenterTests : public ::testing::Test {
  protected:
    ConsonantViewStub view;
    ConsonantTaskPresenterImpl presenter{view};
};

#define CONSONANT_TASK_CONTROLLER_TEST(a)                                      \
    TEST_F(ConsonantTaskControllerTests, a)

#define CONSONANT_TASK_PRESENTER_TEST(a) TEST_F(ConsonantTaskPresenterTests, a)

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).responseButtonsHidden())

#define AV_SPEECH_IN_NOISE_EXPECT_CURSOR_HIDDEN(a)                             \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(cursorHidden(a))

CONSONANT_TASK_PRESENTER_TEST(hidesReadyButton) {
    presenter.hideReadyButton();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.readyButtonHidden());
}

CONSONANT_TASK_PRESENTER_TEST(hidesCursor) {
    presenter.hideCursor();
    AV_SPEECH_IN_NOISE_EXPECT_CURSOR_HIDDEN(view);
}

CONSONANT_TASK_PRESENTER_TEST(hidesCursorAfterTrialStarts) {
    presenter.notifyThatTrialHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_CURSOR_HIDDEN(view);
}

CONSONANT_TASK_PRESENTER_TEST(hidesResponseSubmission) {
    presenter.hideResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

CONSONANT_TASK_PRESENTER_TEST(hidesViewWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.hidden());
}

CONSONANT_TASK_PRESENTER_TEST(hidesResponseButtonsWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

CONSONANT_TASK_PRESENTER_TEST(showsViewWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.shown());
}

CONSONANT_TASK_PRESENTER_TEST(showsReadyButtonWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.readyButtonShown());
}

CONSONANT_TASK_PRESENTER_TEST(
    showsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.responseButtonsShown());
}

CONSONANT_TASK_PRESENTER_TEST(showsCursorWhenTaskCompletes) {
    presenter.complete();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.cursorShown());
}

CONSONANT_TASK_CONTROLLER_TEST(
    notifiesThatUserIsReadyForNextTrialAfterReadyButtonIsClicked) {
    notifyThatReadyButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsReadyForNextTrial());
}

CONSONANT_TASK_CONTROLLER_TEST(hidesReadyButtonAfterClicked) {
    notifyThatReadyButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(presenter.readyButtonHidden());
}

CONSONANT_TASK_CONTROLLER_TEST(submitsConsonantAfterResponseButtonIsClicked) {
    control.setConsonant("b");
    notifyThatResponseButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL('b', model.consonantResponse().consonant);
}

CONSONANT_TASK_CONTROLLER_TEST(
    notifiesThatUserIsDoneRespondingAndIsReadyForNextTrialAfterResponseButtonIsClicked) {
    notifyThatResponseButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController
            .notifiedThatUserIsDoneRespondingAndIsReadyForNextTrial());
}
}
}
