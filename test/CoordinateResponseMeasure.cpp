#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "TaskControllerObserverStub.hpp"
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
    CoordinateResponseMeasureControlStub control;
    CoordinateResponseMeasureViewStub view;
    CoordinateResponseMeasureController controller{model, control};
    CoordinateResponseMeasurePresenter presenter{view};
    TestControllerStub testController;
    TaskControllerObserverStub observer;

    CoordinateResponseMeasureTests() {
        controller.attach(&testController);
        controller.attach(&observer);
    }
};

class CoordinateResponseMeasurePresenterTests : public ::testing::Test {
  protected:
    CoordinateResponseMeasureViewStub view;
    CoordinateResponseMeasurePresenter presenter{view};
};

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).responseButtonsHidden())

#define AV_SPEECH_IN_NOISE_EXPECT_COLOR(model, c)                              \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(c, (model).responseParameters().color)

#define COORDINATE_RESPONSE_MEASURE_TEST(a)                                    \
    TEST_F(CoordinateResponseMeasureTests, a)

#define COORDINATE_RESPONSE_MEASURE_PRESENTER_TEST(a)                          \
    TEST_F(CoordinateResponseMeasurePresenterTests, a)

COORDINATE_RESPONSE_MEASURE_PRESENTER_TEST(
    presenterHidesReadyButtonWhenTaskStarts) {
    presenter.notifyThatTaskHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.nextTrialButtonHidden());
}

COORDINATE_RESPONSE_MEASURE_PRESENTER_TEST(
    presenterHidesResponseButtonsAfterUserIsDoneResponding) {
    notifyThatUserIsDoneResponding(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

COORDINATE_RESPONSE_MEASURE_PRESENTER_TEST(presenterHidesViewWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.hidden());
}

COORDINATE_RESPONSE_MEASURE_PRESENTER_TEST(
    presenterHidesResponseButtonsWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

COORDINATE_RESPONSE_MEASURE_PRESENTER_TEST(presenterShowsViewWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.shown());
}

COORDINATE_RESPONSE_MEASURE_PRESENTER_TEST(
    presenterShowsReadyButtonWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.nextTrialButtonShown());
}

COORDINATE_RESPONSE_MEASURE_PRESENTER_TEST(
    presenterShowsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.responseButtonsShown());
}

COORDINATE_RESPONSE_MEASURE_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterReadyButtonIsClicked) {
    notifyThatReadyButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsReadyForNextTrial());
}

COORDINATE_RESPONSE_MEASURE_TEST(
    responderNotifiesThatTaskHasStartedAfterReadyButtonIsClicked) {
    notifyThatReadyButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(observer.notifiedThatTaskHasStarted());
}

COORDINATE_RESPONSE_MEASURE_TEST(
    responderNotifiesThatUserIsReadyForNextTrialAfterResponseButtonIsClicked) {
    notifyThatResponseButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsReadyForNextTrial());
}

COORDINATE_RESPONSE_MEASURE_TEST(
    responderNotifiesThatUserIsDoneRespondingAfterResponseButtonIsClicked) {
    notifyThatResponseButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(observer.notifiedThatUserIsDoneResponding());
}

COORDINATE_RESPONSE_MEASURE_TEST(coordinateResponsePassesNumberResponse) {
    control.setNumberResponse("1");
    notifyThatResponseButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, model.responseParameters().number);
}

COORDINATE_RESPONSE_MEASURE_TEST(coordinateResponsePassesGreenColor) {
    control.setGreenResponse();
    notifyThatResponseButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_COLOR(
        model, coordinate_response_measure::Color::green);
}

COORDINATE_RESPONSE_MEASURE_TEST(coordinateResponsePassesRedColor) {
    control.setRedResponse();
    notifyThatResponseButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_COLOR(
        model, coordinate_response_measure::Color::red);
}

COORDINATE_RESPONSE_MEASURE_TEST(coordinateResponsePassesBlueColor) {
    control.setBlueResponse();
    notifyThatResponseButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_COLOR(
        model, coordinate_response_measure::Color::blue);
}

COORDINATE_RESPONSE_MEASURE_TEST(coordinateResponsePassesWhiteColor) {
    control.setGrayResponse();
    notifyThatResponseButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_COLOR(
        model, coordinate_response_measure::Color::white);
}
}
}
