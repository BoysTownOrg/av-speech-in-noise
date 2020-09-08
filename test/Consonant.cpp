#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include <presentation/Consonant.hpp>
#include <presentation/CoordinateResponseMeasure.hpp>
#include <presentation/FreeResponse.hpp>
#include <presentation/PassFail.hpp>
#include <presentation/CorrectKeywords.hpp>
#include <presentation/TestSetupImpl.hpp>
#include <presentation/ExperimenterImpl.hpp>
#include <presentation/Presenter.hpp>
#include <av-speech-in-noise/name.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include <utility>

namespace av_speech_in_noise {
namespace {
template <typename T> class Collection {
  public:
    explicit Collection(std::vector<T> items = {}) : items{std::move(items)} {}

    [[nodiscard]] auto contains(const T &item) const -> bool {
        return std::find(items.begin(), items.end(), item) != items.end();
    }

  private:
    std::vector<T> items{};
};

class ConsonantViewStub : public ConsonantOutputView,
                          public ConsonantInputView {
  public:
    void show() override { shown_ = true; }

    [[nodiscard]] auto shown() const -> bool { return shown_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const -> bool { return hidden_; }

    void playTrial() { listener_->notifyThatReadyButtonHasBeenClicked(); }

    void subscribe(EventListener *e) override { listener_ = e; }

    void showReadyButton() override { readyButtonShown_ = true; }

    [[nodiscard]] auto readyButtonShown() const -> bool {
        return readyButtonShown_;
    }

    void hideReadyButton() override { readyButtonHidden_ = true; }

    [[nodiscard]] auto readyButtonHidden() const -> bool {
        return readyButtonHidden_;
    }

    void submitResponse() {
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

    void run() override { view->submitResponse(); }

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

class PlayingConsonantTrial : public PlayingTrial {
    ConsonantViewStub *view;

  public:
    explicit PlayingConsonantTrial(ConsonantViewStub *view) : view{view} {}

    void run() override { view->playTrial(); }

    auto nextTrialButtonHidden() -> bool override {
        return view->readyButtonHidden();
    }

    auto nextTrialButtonShown() -> bool override {
        return view->readyButtonShown();
    }
};

void setTestComplete(ModelStub &model) { model.setTestComplete(); }

auto trialPlayed(ModelStub &model) -> bool { return model.trialPlayed(); }

class ConsonantTests : public ::testing::Test {
  protected:
    ModelStub model;
    ConsonantViewStub consonantView;
    ConsonantResponder consonantScreenResponder{model, consonantView};
    ConsonantPresenter consonantPresenterRefactored{model, consonantView};
    PlayingConsonantTrial playingConsonantTrial{&consonantView};
    SubmittingConsonant submittingConsonant{&consonantView};

    void assertPlaysTrial(UseCase &useCase) {
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(trialPlayed(model));
    }

    void assertCompleteTestDoesNotPlayTrial(UseCase &useCase) {
        setTestComplete(model);
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(trialPlayed(model));
    }

    void assertCompleteTestDoesNotHideCursor(TrialSubmission &useCase) {
        setTestComplete(model);
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(consonantView.cursorHidden());
    }
};

#define CONSONANT_TEST(a) TEST_F(ConsonantTests, a)

CONSONANT_TEST(submittingConsonantDoesNotHideCursorWhenTestComplete) {
    assertCompleteTestDoesNotHideCursor(submittingConsonant);
}

CONSONANT_TEST(playingConsonantTrialHidesCursor) {
    run(playingConsonantTrial);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(consonantView.cursorHidden());
}

CONSONANT_TEST(submittingConsonantTrialHidesCursor) {
    run(submittingConsonant);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(consonantView.cursorHidden());
}

CONSONANT_TEST(submittingConsonantPlaysTrial) {
    assertPlaysTrial(submittingConsonant);
}

CONSONANT_TEST(playingConsonantTrialPlaysTrial) {
    assertPlaysTrial(playingConsonantTrial);
}

CONSONANT_TEST(consonantResponsePassesConsonant) {
    consonantView.setConsonant("b");
    run(submittingConsonant);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL('b', model.consonantResponse().consonant);
}

class ExperimenterResponderStub : public ExperimenterResponder {
  public:
    void subscribe(EventListener *) override {}
    void becomeChild(Presenter *) override {}
    void showContinueTestingDialogWithResultsWhenComplete() override {}
    void readyNextTrialIfNeeded() override {}
    void playNextTrialIfNeeded() override { nextTrialPlayedIfNeeded_ = true; }
    void playTrial() override {}
    [[nodiscard]] auto nextTrialPlayedIfNeeded() const -> bool {
        return nextTrialPlayedIfNeeded_;
    }

  private:
    bool nextTrialPlayedIfNeeded_{};
};

CONSONANT_TEST(submittingConsonantDoesNotShowSetupViewWhenTestIncomplete) {
    ExperimenterResponderStub experimenterResponder;
    consonantScreenResponder.subscribe(&experimenterResponder);
    consonantScreenResponder.notifyThatResponseButtonHasBeenClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterResponder.nextTrialPlayedIfNeeded());
}

CONSONANT_TEST(submittingConsonantDoesNotPlayTrialWhenTestComplete) {
    assertCompleteTestDoesNotPlayTrial(submittingConsonant);
}

class TaskResponderListenerStub : public TaskResponder::EventListener {
  public:
    void notifyThatTaskHasStarted() override {}
    void notifyThatUserIsDoneResponding() override {
        notifiedThatUserIsDoneResponding_ = true;
    }
    [[nodiscard]] auto notifiedThatUserIsDoneResponding() const -> bool {
        return notifiedThatUserIsDoneResponding_;
    }

  private:
    bool notifiedThatUserIsDoneResponding_{};
};

CONSONANT_TEST(submittingConsonantHidesResponseButtons) {
    TaskResponderListenerStub taskResponder;
    consonantScreenResponder.subscribe(&taskResponder);
    consonantScreenResponder.notifyThatResponseButtonHasBeenClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        taskResponder.notifiedThatUserIsDoneResponding());
}

CONSONANT_TEST(presenterHidesResponseButtonsWhenUserIsDoneResponding) {
    consonantPresenterRefactored.notifyThatUserIsDoneResponding();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(consonantView.responseButtonsHidden());
}

CONSONANT_TEST(submittingConsonantHidesConsonantViewWhenTestComplete) {
    consonantPresenterRefactored.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(consonantView.hidden());
}

CONSONANT_TEST(exitTestHidesConsonantResponseButtons) {
    consonantPresenterRefactored.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(consonantView.responseButtonsHidden());
}
}
}
