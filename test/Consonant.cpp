#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include <presentation/Consonant.hpp>
#include <av-speech-in-noise/name.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include <utility>

namespace av_speech_in_noise {
namespace {
class ConsonantInputViewStub : public ConsonantInputView {
  public:
    void notifyThatReadyButtonHasBeenClicked() {
        listener_->notifyThatReadyButtonHasBeenClicked();
    }

    void subscribe(EventListener *e) override { listener_ = e; }

    void notifyThatResponseButtonHasBeenClicked() {
        listener_->notifyThatResponseButtonHasBeenClicked();
    }

    void setConsonant(std::string c) { consonant_ = std::move(c); }

    auto consonant() -> std::string override { return consonant_; }

  private:
    std::string consonant_;
    EventListener *listener_{};
};

class ConsonantOutputViewStub : public ConsonantOutputView {
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

void notifyThatReadyButtonHasBeenClicked(ConsonantInputViewStub &view) {
    view.notifyThatReadyButtonHasBeenClicked();
}

class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run() = 0;
};

void run(UseCase &useCase) { useCase.run(); }

class ConditionUseCase : public virtual UseCase {
  public:
    virtual auto condition(ModelStub &) -> Condition = 0;
};

class TrialSubmission : public virtual UseCase {
  public:
    virtual auto nextTrialButtonShown() -> bool = 0;
    virtual auto responseViewShown() -> bool = 0;
    virtual auto responseViewHidden() -> bool = 0;
};

class SubmittingConsonant : public TrialSubmission {
    ConsonantInputViewStub &view;
    ConsonantOutputViewStub &outputView;

  public:
    explicit SubmittingConsonant(
        ConsonantInputViewStub &view, ConsonantOutputViewStub &outputView)
        : view{view}, outputView{outputView} {}

    void run() override { view.notifyThatResponseButtonHasBeenClicked(); }

    auto nextTrialButtonShown() -> bool override {
        return outputView.readyButtonShown();
    }

    auto responseViewShown() -> bool override {
        return outputView.responseButtonsShown();
    }

    auto responseViewHidden() -> bool override {
        return outputView.responseButtonsHidden();
    }
};

class ExperimenterResponderStub : public ExperimenterResponder {
  public:
    void subscribe(EventListener *) override {}
    void subscribe(IPresenter *) override {}
    void showContinueTestingDialogWithResultsWhenComplete() override {}
    void readyNextTrialIfNeeded() override {}
    void notifyThatUserIsReadyForNextTrial() override {
        notifiedThatUserIsReadyForNextTrial_ = true;
    }
    [[nodiscard]] auto notifiedThatUserIsReadyForNextTrial() const -> bool {
        return notifiedThatUserIsReadyForNextTrial_;
    }

  private:
    bool notifiedThatUserIsReadyForNextTrial_{};
};

class TaskResponderListenerStub : public TaskResponder::EventListener {
  public:
    void notifyThatTaskHasStarted() override {
        notifiedThatTaskHasStarted_ = true;
    }
    void notifyThatUserIsDoneResponding() override {
        notifiedThatUserIsDoneResponding_ = true;
    }
    [[nodiscard]] auto notifiedThatUserIsDoneResponding() const -> bool {
        return notifiedThatUserIsDoneResponding_;
    }
    [[nodiscard]] auto notifiedThatTaskHasStarted() const -> bool {
        return notifiedThatTaskHasStarted_;
    }

  private:
    bool notifiedThatUserIsDoneResponding_{};
    bool notifiedThatTaskHasStarted_{};
};

auto cursorHidden(ConsonantOutputViewStub &view) -> bool {
    return view.cursorHidden();
}

void notifyThatUserIsDoneResponding(TaskPresenter &presenter) {
    presenter.notifyThatUserIsDoneResponding();
}

void notifyThatTrialHasStarted(TaskPresenter &presenter) {
    presenter.notifyThatTrialHasStarted();
}

void stop(TaskPresenter &presenter) { presenter.stop(); }

void start(TaskPresenter &presenter) { presenter.start(); }

class ConsonantTests : public ::testing::Test {
  protected:
    ModelStub model;
    ConsonantInputViewStub inputView;
    ConsonantOutputViewStub outputView;
    ConsonantResponder responder{model, inputView};
    ConsonantPresenter presenter{outputView};
    SubmittingConsonant submittingConsonant{inputView, outputView};
    ExperimenterResponderStub experimenterResponder;
    TaskResponderListenerStub taskResponder;

    ConsonantTests() {
        responder.subscribe(&experimenterResponder);
        responder.subscribe(&taskResponder);
    }
};

#define CONSONANT_TEST(a) TEST_F(ConsonantTests, a)

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).responseButtonsHidden())

#define AV_SPEECH_IN_NOISE_EXPECT_CURSOR_HIDDEN(a)                             \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(cursorHidden(a))

CONSONANT_TEST(presenterHidesReadyButtonWhenTaskStarts) {
    presenter.notifyThatTaskHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputView.readyButtonHidden());
}

CONSONANT_TEST(presenterHidesCursorAfterTrialHasStarted) {
    notifyThatTrialHasStarted(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_CURSOR_HIDDEN(outputView);
}

CONSONANT_TEST(presenterHidesResponseButtonsAfterUserIsDoneResponding) {
    notifyThatUserIsDoneResponding(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(outputView);
}

CONSONANT_TEST(presenterHidesViewWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputView.hidden());
}

CONSONANT_TEST(presenterHidesResponseButtonsWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(outputView);
}

CONSONANT_TEST(presenterShowsViewWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputView.shown());
}

CONSONANT_TEST(presenterShowsReadyButtonWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputView.readyButtonShown());
}

CONSONANT_TEST(presenterShowsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputView.responseButtonsShown());
}

CONSONANT_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterReadyButtonIsClicked) {
    notifyThatReadyButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterResponder.notifiedThatUserIsReadyForNextTrial());
}

CONSONANT_TEST(responderNotifiesThatTaskHasStartedAfterReadyButtonIsClicked) {
    notifyThatReadyButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(taskResponder.notifiedThatTaskHasStarted());
}

CONSONANT_TEST(responderSubmitsConsonantAfterResponseButtonIsClicked) {
    inputView.setConsonant("b");
    run(submittingConsonant);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL('b', model.consonantResponse().consonant);
}

CONSONANT_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterResponseButtonIsClicked) {
    run(submittingConsonant);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterResponder.notifiedThatUserIsReadyForNextTrial());
}

CONSONANT_TEST(
    responderNotifiesThatUserIsDoneRespondingAfterResponseButtonIsClicked) {
    run(submittingConsonant);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        taskResponder.notifiedThatUserIsDoneResponding());
}
}
}
