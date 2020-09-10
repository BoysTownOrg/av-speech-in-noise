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

class CoordinateResponseMeasureViewStub
    : public CoordinateResponseMeasureInputView,
      public CoordinateResponseMeasureOutputView {
  public:
    void show() override { shown_ = true; }

    [[nodiscard]] auto shown() const { return shown_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const { return hidden_; }

    auto whiteResponse() -> bool override { return grayResponse_; }

    void setGrayResponse() { grayResponse_ = true; }

    auto blueResponse() -> bool override { return blueResponse_; }

    void setBlueResponse() { blueResponse_ = true; }

    void setRedResponse() { redResponse_ = true; }

    void setGreenResponse() { greenResponse_ = true; }

    auto greenResponse() -> bool override { return greenResponse_; }

    void hideNextTrialButton() override { nextTrialButtonHidden_ = true; }

    [[nodiscard]] auto nextTrialButtonHidden() const {
        return nextTrialButtonHidden_;
    }

    void hideResponseButtons() override { responseButtonsHidden_ = true; }

    [[nodiscard]] auto responseButtonsHidden() const {
        return responseButtonsHidden_;
    }

    void showNextTrialButton() override { nextTrialButtonShown_ = true; }

    [[nodiscard]] auto nextTrialButtonShown() const {
        return nextTrialButtonShown_;
    }

    void showResponseButtons() override { responseButtonsShown_ = true; }

    [[nodiscard]] auto responseButtonsShown() const {
        return responseButtonsShown_;
    }

    void setNumberResponse(std::string s) { numberResponse_ = std::move(s); }

    auto numberResponse() -> std::string override { return numberResponse_; }

    void subscribe(EventListener *e) override { listener_ = e; }

    void submitResponse() {
        listener_->notifyThatResponseButtonHasBeenClicked();
    }

    void notifyThatReadyButtonHasBeenClicked() {
        listener_->notifyThatReadyButtonHasBeenClicked();
    }

  private:
    std::string numberResponse_{"0"};
    EventListener *listener_{};
    bool responseButtonsShown_{};
    bool responseButtonsHidden_{};
    bool shown_{};
    bool hidden_{};
    bool greenResponse_{};
    bool redResponse_{};
    bool blueResponse_{};
    bool grayResponse_{};
    bool nextTrialButtonHidden_{};
    bool nextTrialButtonShown_{};
};

void notifyThatReadyButtonHasBeenClicked(
    CoordinateResponseMeasureViewStub &view) {
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

class LevelUseCase : public virtual UseCase {
  public:
    virtual auto fullScaleLevel(ModelStub &) -> int = 0;
};

class TrialSubmission : public virtual UseCase {
  public:
    virtual auto nextTrialButtonShown() -> bool = 0;
    virtual auto responseViewShown() -> bool = 0;
    virtual auto responseViewHidden() -> bool = 0;
};

class SubmittingCoordinateResponseMeasure : public TrialSubmission {
    CoordinateResponseMeasureViewStub *view;

  public:
    explicit SubmittingCoordinateResponseMeasure(
        CoordinateResponseMeasureViewStub *view)
        : view{view} {}

    void run() override { view->submitResponse(); }

    auto nextTrialButtonShown() -> bool override {
        return view->nextTrialButtonShown();
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

class PlayingCoordinateResponseMeasureTrial : public PlayingTrial {
    CoordinateResponseMeasureViewStub *view;

  public:
    explicit PlayingCoordinateResponseMeasureTrial(
        CoordinateResponseMeasureViewStub *view)
        : view{view} {}

    void run() override { view->notifyThatReadyButtonHasBeenClicked(); }

    auto nextTrialButtonHidden() -> bool override {
        return view->nextTrialButtonHidden();
    }

    auto nextTrialButtonShown() -> bool override {
        return view->nextTrialButtonShown();
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

void notifyThatUserIsDoneResponding(TaskPresenter &presenter) {
    presenter.notifyThatUserIsDoneResponding();
}

void notifyThatTrialHasStarted(TaskPresenter &presenter) {
    presenter.notifyThatTrialHasStarted();
}

void stop(TaskPresenter &presenter) { presenter.stop(); }

void start(TaskPresenter &presenter) { presenter.start(); }

class CoordinateResponseMeasureTests : public ::testing::Test {
  protected:
    ModelStub model;
    CoordinateResponseMeasureViewStub view;
    CoordinateResponseMeasureResponder responder{model, view};
    CoordinateResponseMeasurePresenter presenter{view};
    SubmittingCoordinateResponseMeasure submittingCoordinateResponseMeasure{
        &view};
    ExperimenterResponderStub experimenterResponder;
    TaskResponderListenerStub taskResponder;

    CoordinateResponseMeasureTests() {
        responder.subscribe(&experimenterResponder);
        responder.subscribe(&taskResponder);
    }
};

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).responseButtonsHidden())

#define COORDINATE_RESPONSE_MEASURE_TEST(a)                                    \
    TEST_F(CoordinateResponseMeasureTests, a)

COORDINATE_RESPONSE_MEASURE_TEST(presenterHidesReadyButtonWhenTaskStarts) {
    presenter.notifyThatTaskHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.nextTrialButtonHidden());
}

COORDINATE_RESPONSE_MEASURE_TEST(
    presenterHidesResponseButtonsAfterUserIsDoneResponding) {
    notifyThatUserIsDoneResponding(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

COORDINATE_RESPONSE_MEASURE_TEST(presenterHidesViewWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.hidden());
}

COORDINATE_RESPONSE_MEASURE_TEST(presenterHidesResponseButtonsWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

COORDINATE_RESPONSE_MEASURE_TEST(presenterShowsViewWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.shown());
}

COORDINATE_RESPONSE_MEASURE_TEST(presenterShowsReadyButtonWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.nextTrialButtonShown());
}

COORDINATE_RESPONSE_MEASURE_TEST(
    presenterShowsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.responseButtonsShown());
}

COORDINATE_RESPONSE_MEASURE_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterReadyButtonIsClicked) {
    notifyThatReadyButtonHasBeenClicked(view);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterResponder.notifiedThatUserIsReadyForNextTrial());
}

COORDINATE_RESPONSE_MEASURE_TEST(
    responderNotifiesThatTaskHasStartedAfterReadyButtonIsClicked) {
    notifyThatReadyButtonHasBeenClicked(view);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(taskResponder.notifiedThatTaskHasStarted());
}

COORDINATE_RESPONSE_MEASURE_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterResponseButtonIsClicked) {
    run(submittingCoordinateResponseMeasure);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterResponder.notifiedThatUserIsReadyForNextTrial());
}

COORDINATE_RESPONSE_MEASURE_TEST(
    responderNotifiesThatUserIsDoneRespondingAfterResponseButtonIsClicked) {
    run(submittingCoordinateResponseMeasure);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        taskResponder.notifiedThatUserIsDoneResponding());
}

}
}
