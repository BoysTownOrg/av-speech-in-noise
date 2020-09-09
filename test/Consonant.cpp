#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include <presentation/Consonant.hpp>
#include <av-speech-in-noise/name.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include <utility>

namespace av_speech_in_noise {
namespace {
class ConsonantViewStub : public ConsonantOutputView,
                          public ConsonantInputView {
  public:
    void show() override { shown_ = true; }

    [[nodiscard]] auto shown() const -> bool { return shown_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const -> bool { return hidden_; }

    void notifyThatReadyButtonHasBeenClicked() {
        listener_->notifyThatReadyButtonHasBeenClicked();
    }

    void subscribe(EventListener *e) override { listener_ = e; }

    void showReadyButton() override { readyButtonShown_ = true; }

    [[nodiscard]] auto readyButtonShown() const -> bool {
        return readyButtonShown_;
    }

    void hideReadyButton() override { readyButtonHidden_ = true; }

    [[nodiscard]] auto readyButtonHidden() const -> bool {
        return readyButtonHidden_;
    }

    void notifyThatResponseButtonHasBeenClicked() {
        listener_->notifyThatResponseButtonHasBeenClicked();
    }

    void hideResponseButtons() override { responseButtonsHidden_ = true; }

    [[nodiscard]] auto responseButtonsHidden() const -> bool {
        return responseButtonsHidden_;
    }

    void showResponseButtons() override { responseButtonsShown_ = true; }

    [[nodiscard]] auto responseButtonsShown() const -> bool {
        return responseButtonsShown_;
    }

    void setConsonant(std::string c) { consonant_ = std::move(c); }

    auto consonant() -> std::string override { return consonant_; }

    [[nodiscard]] auto cursorHidden() const -> bool { return cursorHidden_; }

    void hideCursor() override { cursorHidden_ = true; }

    void showCursor() override { cursorShown_ = true; }

  private:
    std::string consonant_;
    EventListener *listener_{};
    bool shown_{};
    bool hidden_{};
    bool responseButtonsShown_{};
    bool responseButtonsHidden_{};
    bool readyButtonShown_{};
    bool readyButtonHidden_{};
    bool cursorHidden_{};
    bool cursorShown_{};
};

void notifyThatReadyButtonHasBeenClicked(ConsonantViewStub &view) {
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
    ConsonantViewStub *view;

  public:
    explicit SubmittingConsonant(ConsonantViewStub *view) : view{view} {}

    void run() override { view->notifyThatResponseButtonHasBeenClicked(); }

    auto nextTrialButtonShown() -> bool override {
        return view->readyButtonShown();
    }

    auto responseViewShown() -> bool override {
        return view->responseButtonsShown();
    }

    auto responseViewHidden() -> bool override {
        return view->responseButtonsHidden();
    }
};

void setTestComplete(ModelStub &model) { model.setTestComplete(); }

class ExperimenterResponderStub : public ExperimenterResponder {
  public:
    void subscribe(EventListener *) override {}
    void subscribe(IPresenter *) override {}
    void showContinueTestingDialogWithResultsWhenComplete() override {}
    void readyNextTrialIfNeeded() override {}
    void playNextTrialIfNeeded() override { nextTrialPlayedIfNeeded_ = true; }
    void playTrial() override { nextTrialPlayed_ = true; }
    [[nodiscard]] auto nextTrialPlayedIfNeeded() const -> bool {
        return nextTrialPlayedIfNeeded_;
    }
    [[nodiscard]] auto nextTrialPlayed() const -> bool {
        return nextTrialPlayed_;
    }

  private:
    bool nextTrialPlayedIfNeeded_{};
    bool nextTrialPlayed_{};
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

auto cursorHidden(ConsonantViewStub &view) -> bool {
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
    ConsonantViewStub view;
    ConsonantResponder responder{model, view};
    ConsonantPresenter presenter{model, view};
    SubmittingConsonant submittingConsonant{&view};
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

CONSONANT_TEST(presenterHidesCursorWhenTaskStarts) {
    presenter.notifyThatTaskHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_CURSOR_HIDDEN(view);
}

CONSONANT_TEST(presenterHidesReadyButtonWhenTaskStarts) {
    presenter.notifyThatTaskHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.readyButtonHidden());
}

CONSONANT_TEST(presenterHidesCursorAfterTrialHasStarted) {
    notifyThatTrialHasStarted(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_CURSOR_HIDDEN(view);
}

CONSONANT_TEST(
    presenterDoesNotHideCursorAfterTrialHasStartedWhenTestIsComplete) {
    setTestComplete(model);
    notifyThatTrialHasStarted(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(cursorHidden(view));
}

CONSONANT_TEST(presenterHidesResponseButtonsAfterUserIsDoneResponding) {
    notifyThatUserIsDoneResponding(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

CONSONANT_TEST(presenterHidesViewWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.hidden());
}

CONSONANT_TEST(presenterHidesResponseButtonsWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

CONSONANT_TEST(presenterShowsViewWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.shown());
}

CONSONANT_TEST(presenterShowsReadyButtonWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.readyButtonShown());
}

CONSONANT_TEST(presenterShowsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.responseButtonsShown());
}

CONSONANT_TEST(responderPlaysTrialAfterReadyButtonIsClicked) {
    notifyThatReadyButtonHasBeenClicked(view);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(experimenterResponder.nextTrialPlayed());
}

CONSONANT_TEST(responderNotifiesThatTaskHasStartedAfterReadyButtonIsClicked) {
    notifyThatReadyButtonHasBeenClicked(view);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(taskResponder.notifiedThatTaskHasStarted());
}

CONSONANT_TEST(responderSubmitsConsonantAfterResponseButtonIsClicked) {
    view.setConsonant("b");
    run(submittingConsonant);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL('b', model.consonantResponse().consonant);
}

CONSONANT_TEST(responderPlaysNextTrialIfNeededAfterResponseButtonIsClicked) {
    run(submittingConsonant);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterResponder.nextTrialPlayedIfNeeded());
}

CONSONANT_TEST(
    responderNotifiesThatUserIsDoneRespondingAfterResponseButtonIsClicked) {
    run(submittingConsonant);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        taskResponder.notifiedThatUserIsDoneResponding());
}
}
}
