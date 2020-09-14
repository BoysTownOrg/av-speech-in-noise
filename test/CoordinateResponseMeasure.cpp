#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "TaskControllerListenerStub.hpp"
#include "TestControllerStub.hpp"
#include <presentation/CoordinateResponseMeasure.hpp>
#include <gtest/gtest.h>
#include <utility>

namespace av_speech_in_noise {
namespace {
class CoordinateResponseMeasureControlStub
    : public CoordinateResponseMeasureControl {
  public:
    auto whiteResponse() -> bool override { return grayResponse_; }

    void setGrayResponse() { grayResponse_ = true; }

    auto blueResponse() -> bool override { return blueResponse_; }

    void setBlueResponse() { blueResponse_ = true; }

    void setRedResponse() { redResponse_ = true; }

    void setGreenResponse() { greenResponse_ = true; }

    auto greenResponse() -> bool override { return greenResponse_; }

    void setNumberResponse(std::string s) { numberResponse_ = std::move(s); }

    auto numberResponse() -> std::string override { return numberResponse_; }

    void attach(Observer *e) override { listener_ = e; }

    void notifyThatResponseButtonHasBeenClicked() {
        listener_->notifyThatResponseButtonHasBeenClicked();
    }

    void notifyThatReadyButtonHasBeenClicked() {
        listener_->notifyThatReadyButtonHasBeenClicked();
    }

  private:
    std::string numberResponse_{"0"};
    Observer *listener_{};
    bool greenResponse_{};
    bool redResponse_{};
    bool blueResponse_{};
    bool grayResponse_{};
};

class CoordinateResponseMeasureViewStub : public CoordinateResponseMeasureView {
  public:
    void show() override { shown_ = true; }

    [[nodiscard]] auto shown() const { return shown_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const { return hidden_; }

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

  private:
    bool responseButtonsShown_{};
    bool responseButtonsHidden_{};
    bool shown_{};
    bool hidden_{};
    bool nextTrialButtonHidden_{};
    bool nextTrialButtonShown_{};
};

void notifyThatReadyButtonHasBeenClicked(
    CoordinateResponseMeasureControlStub &view) {
    view.notifyThatReadyButtonHasBeenClicked();
}

void notifyThatUserIsDoneResponding(TaskPresenter &presenter) {
    presenter.notifyThatUserIsDoneResponding();
}

void stop(TaskPresenter &presenter) { presenter.stop(); }

void start(TaskPresenter &presenter) { presenter.start(); }

void notifyThatResponseButtonHasBeenClicked(
    CoordinateResponseMeasureControlStub &view) {
    view.notifyThatResponseButtonHasBeenClicked();
}

class CoordinateResponseMeasureTests : public ::testing::Test {
  protected:
    ModelStub model;
    CoordinateResponseMeasureControlStub inputView;
    CoordinateResponseMeasureViewStub outputView;
    CoordinateResponseMeasureController responder{model, inputView};
    CoordinateResponseMeasurePresenter presenter{outputView};
    TestControllerStub experimenterController;
    TaskControllerListenerStub taskControllerListener;

    CoordinateResponseMeasureTests() {
        responder.attach(&experimenterController);
        responder.attach(&taskControllerListener);
    }
};

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).responseButtonsHidden())

#define AV_SPEECH_IN_NOISE_EXPECT_COLOR(model, c)                              \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(c, (model).responseParameters().color)

#define COORDINATE_RESPONSE_MEASURE_TEST(a)                                    \
    TEST_F(CoordinateResponseMeasureTests, a)

COORDINATE_RESPONSE_MEASURE_TEST(presenterHidesReadyButtonWhenTaskStarts) {
    presenter.notifyThatTaskHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputView.nextTrialButtonHidden());
}

COORDINATE_RESPONSE_MEASURE_TEST(
    presenterHidesResponseButtonsAfterUserIsDoneResponding) {
    notifyThatUserIsDoneResponding(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(outputView);
}

COORDINATE_RESPONSE_MEASURE_TEST(presenterHidesViewWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputView.hidden());
}

COORDINATE_RESPONSE_MEASURE_TEST(presenterHidesResponseButtonsWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(outputView);
}

COORDINATE_RESPONSE_MEASURE_TEST(presenterShowsViewWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputView.shown());
}

COORDINATE_RESPONSE_MEASURE_TEST(presenterShowsReadyButtonWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputView.nextTrialButtonShown());
}

COORDINATE_RESPONSE_MEASURE_TEST(
    presenterShowsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputView.responseButtonsShown());
}

COORDINATE_RESPONSE_MEASURE_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterReadyButtonIsClicked) {
    notifyThatReadyButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterController.notifiedThatUserIsReadyForNextTrial());
}

COORDINATE_RESPONSE_MEASURE_TEST(
    responderNotifiesThatTaskHasStartedAfterReadyButtonIsClicked) {
    notifyThatReadyButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        taskControllerListener.notifiedThatTaskHasStarted());
}

COORDINATE_RESPONSE_MEASURE_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterResponseButtonIsClicked) {
    notifyThatResponseButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterController.notifiedThatUserIsReadyForNextTrial());
}

COORDINATE_RESPONSE_MEASURE_TEST(
    responderNotifiesThatUserIsDoneRespondingAfterResponseButtonIsClicked) {
    notifyThatResponseButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        taskControllerListener.notifiedThatUserIsDoneResponding());
}

COORDINATE_RESPONSE_MEASURE_TEST(coordinateResponsePassesNumberResponse) {
    inputView.setNumberResponse("1");
    notifyThatResponseButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, model.responseParameters().number);
}

COORDINATE_RESPONSE_MEASURE_TEST(coordinateResponsePassesGreenColor) {
    inputView.setGreenResponse();
    notifyThatResponseButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_COLOR(
        model, coordinate_response_measure::Color::green);
}

COORDINATE_RESPONSE_MEASURE_TEST(coordinateResponsePassesRedColor) {
    inputView.setRedResponse();
    notifyThatResponseButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_COLOR(
        model, coordinate_response_measure::Color::red);
}

COORDINATE_RESPONSE_MEASURE_TEST(coordinateResponsePassesBlueColor) {
    inputView.setBlueResponse();
    notifyThatResponseButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_COLOR(
        model, coordinate_response_measure::Color::blue);
}

COORDINATE_RESPONSE_MEASURE_TEST(coordinateResponsePassesWhiteColor) {
    inputView.setGrayResponse();
    notifyThatResponseButtonHasBeenClicked(inputView);
    AV_SPEECH_IN_NOISE_EXPECT_COLOR(
        model, coordinate_response_measure::Color::white);
}
}
}
