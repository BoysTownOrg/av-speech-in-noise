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
};

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

class PlayingTrial : public virtual UseCase {
  public:
    virtual auto nextTrialButtonHidden() -> bool = 0;
    virtual auto nextTrialButtonShown() -> bool = 0;
};

void setTestComplete(ModelStub &model) { model.setTestComplete(); }

auto trialPlayed(ModelStub &model) -> bool { return model.trialPlayed(); }

class ExperimenterResponderStub : public ExperimenterResponder {
  public:
    void subscribe(EventListener *) override {}
    void becomeChild(Presenter *) override {}
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

class ConsonantTests : public ::testing::Test {
  protected:
    ModelStub model;
    ConsonantViewStub consonantView;
    ConsonantResponder consonantScreenResponder{model, consonantView};
    ConsonantPresenter consonantPresenterRefactored{model, consonantView};
    SubmittingConsonant submittingConsonant{&consonantView};
    ExperimenterResponderStub experimenterResponder;
    TaskResponderListenerStub taskResponder;

    ConsonantTests() {
        consonantScreenResponder.subscribe(&experimenterResponder);
        consonantScreenResponder.subscribe(&taskResponder);
    }

    void assertCompleteTestDoesNotPlayTrial(UseCase &useCase) {
        setTestComplete(model);
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(trialPlayed(model));
    }
};

#define CONSONANT_TEST(a) TEST_F(ConsonantTests, a)

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(a.responseButtonsHidden())

#define AV_SPEECH_IN_NOISE_EXPECT_CURSOR_HIDDEN(a)                             \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(a.cursorHidden())

CONSONANT_TEST(
    presenterDoesNotHideCursorAfterUserIsDoneRespondingWhenTestIsComplete) {
    setTestComplete(model);
    run(submittingConsonant);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(consonantView.cursorHidden());
}

CONSONANT_TEST(presenterHidesCursorWhenTaskStarts) {
    consonantPresenterRefactored.notifyThatTaskHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_CURSOR_HIDDEN(consonantView);
}

CONSONANT_TEST(submittingConsonantTrialHidesCursor) {
    consonantPresenterRefactored.notifyThatUserIsDoneResponding();
    AV_SPEECH_IN_NOISE_EXPECT_CURSOR_HIDDEN(consonantView);
}

CONSONANT_TEST(clickingReadyPlaysNextTrial) {
    consonantScreenResponder.notifyThatReadyButtonHasBeenClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(experimenterResponder.nextTrialPlayed());
}

CONSONANT_TEST(consonantResponsePassesConsonant) {
    consonantView.setConsonant("b");
    run(submittingConsonant);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL('b', model.consonantResponse().consonant);
}

CONSONANT_TEST(submittingConsonantDoesNotShowSetupViewWhenTestIncomplete) {
    consonantScreenResponder.notifyThatResponseButtonHasBeenClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterResponder.nextTrialPlayedIfNeeded());
}

CONSONANT_TEST(submittingConsonantDoesNotPlayTrialWhenTestComplete) {
    assertCompleteTestDoesNotPlayTrial(submittingConsonant);
}

CONSONANT_TEST(submittingConsonantHidesResponseButtons) {
    TaskResponderListenerStub taskResponder;
    consonantScreenResponder.subscribe(&taskResponder);
    consonantScreenResponder.notifyThatResponseButtonHasBeenClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        taskResponder.notifiedThatUserIsDoneResponding());
}

CONSONANT_TEST(
    responderNotifiesThatTaskHasStartedWhenReadyButtonHasBeenClicked) {
    TaskResponderListenerStub taskResponder;
    consonantScreenResponder.subscribe(&taskResponder);
    consonantScreenResponder.notifyThatReadyButtonHasBeenClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(taskResponder.notifiedThatTaskHasStarted());
}

CONSONANT_TEST(presenterHidesResponseButtonsWhenUserIsDoneResponding) {
    consonantPresenterRefactored.notifyThatUserIsDoneResponding();
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(consonantView);
}

CONSONANT_TEST(submittingConsonantHidesConsonantViewWhenTestComplete) {
    consonantPresenterRefactored.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(consonantView.hidden());
}

CONSONANT_TEST(exitTestHidesConsonantResponseButtons) {
    consonantPresenterRefactored.stop();
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(consonantView);
}
}
}
