#include "assert-utility.hpp"
#include "TaskControllerObserverStub.hpp"
#include "TestControllerStub.hpp"

#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/ui/Consonant.hpp>

#include <gtest/gtest.h>

namespace av_speech_in_noise::submitting_consonant {
namespace {
class ConsonantControlStub : public Control {
  public:
    void notifyThatReadyButtonHasBeenClicked() {
        listener_->notifyThatReadyButtonHasBeenClicked();
    }

    void attach(Observer *e) override { listener_ = e; }

    void notifyThatResponseButtonHasBeenClicked() {
        listener_->notifyThatResponseButtonHasBeenClicked();
    }

    void setConsonant(Consonant c) { consonant_ = c; }

    auto consonant() -> Consonant override { return consonant_; }

  private:
    Consonant consonant_{Consonant::unknown};
    Observer *listener_{};
};

class ConsonantViewStub : public View {
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

class ConsonantTaskPresenterStub : public Presenter {
  public:
    void hideReadyButton() override { readyButtonHidden_ = true; }

    [[nodiscard]] auto readyButtonHidden() const -> bool {
        return readyButtonHidden_;
    }

  private:
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

class InteractorStub : public Interactor {
  public:
    void submit(const ConsonantResponse &s) override { freeResponse_ = s; }

    auto response() -> ConsonantResponse { return freeResponse_; }

  private:
    ConsonantResponse freeResponse_;
};

class ConsonantTaskControllerTests : public ::testing::Test {
  protected:
    InteractorStub model;
    ConsonantControlStub control;
    TestControllerStub testController;
    ConsonantTaskPresenterStub presenter;
    Controller controller{testController, model, control, presenter};
    TaskControllerObserverStub observer;
};

class ConsonantTaskPresenterTests : public ::testing::Test {
  protected:
    ConsonantViewStub view;
    PresenterImpl presenter{view};
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

CONSONANT_TASK_PRESENTER_TEST(showsCursorWithResponseSubmission) {
    presenter.showResponseSubmission();
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
    control.setConsonant(Consonant::bi);
    notifyThatResponseButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(Consonant::bi, model.response().consonant);
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
