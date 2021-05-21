#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "TaskControllerObserverStub.hpp"
#include "TestControllerStub.hpp"
#include <av-speech-in-noise/ui/CoordinateResponseMeasure.hpp>
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

    void clickResponseButton() {
        listener_->notifyThatResponseButtonHasBeenClicked();
    }

    void clickReadyButton() {
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

void clickReadyButton(CoordinateResponseMeasureControlStub &view) {
    view.clickReadyButton();
}

void stop(TaskPresenter &presenter) { presenter.stop(); }

void start(TaskPresenter &presenter) { presenter.start(); }

void clickResponseButton(CoordinateResponseMeasureControlStub &view) {
    view.clickResponseButton();
}

class CoordinateResponseMeasureControllerTests : public ::testing::Test {
  protected:
    ModelStub model;
    CoordinateResponseMeasureControlStub control;
    TestControllerStub testController;
    CoordinateResponseMeasureController controller{
        testController, model, control};
    TaskControllerObserverStub observer;

    CoordinateResponseMeasureControllerTests() { controller.attach(&observer); }
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

#define COORDINATE_RESPONSE_MEASURE_CONTROLLER_TEST(a)                         \
    TEST_F(CoordinateResponseMeasureControllerTests, a)

#define COORDINATE_RESPONSE_MEASURE_PRESENTER_TEST(a)                          \
    TEST_F(CoordinateResponseMeasurePresenterTests, a)

COORDINATE_RESPONSE_MEASURE_PRESENTER_TEST(hidesReadyButtonWhenTaskStarts) {
    presenter.notifyThatTaskHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.nextTrialButtonHidden());
}

COORDINATE_RESPONSE_MEASURE_PRESENTER_TEST(hidesResponseSubmission) {
    presenter.hideResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

COORDINATE_RESPONSE_MEASURE_PRESENTER_TEST(hidesViewWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.hidden());
}

COORDINATE_RESPONSE_MEASURE_PRESENTER_TEST(hidesResponseButtonsWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

COORDINATE_RESPONSE_MEASURE_PRESENTER_TEST(showsViewWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.shown());
}

COORDINATE_RESPONSE_MEASURE_PRESENTER_TEST(showsReadyButtonWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.nextTrialButtonShown());
}

COORDINATE_RESPONSE_MEASURE_PRESENTER_TEST(
    showsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.responseButtonsShown());
}

COORDINATE_RESPONSE_MEASURE_CONTROLLER_TEST(
    notifiesThatUserIsReadyForNextTrialAfterReadyButtonIsClicked) {
    clickReadyButton(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsReadyForNextTrial());
}

COORDINATE_RESPONSE_MEASURE_CONTROLLER_TEST(
    notifiesThatTaskHasStartedAfterReadyButtonIsClicked) {
    clickReadyButton(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(observer.notifiedThatTaskHasStarted());
}

COORDINATE_RESPONSE_MEASURE_CONTROLLER_TEST(
    notifiesThatUserIsReadyForNextTrialAfterResponseButtonIsClicked) {
    clickResponseButton(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController
            .notifiedThatUserIsDoneRespondingAndIsReadyForNextTrial());
}

COORDINATE_RESPONSE_MEASURE_CONTROLLER_TEST(
    coordinateResponsePassesNumberResponse) {
    control.setNumberResponse("1");
    clickResponseButton(control);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, model.responseParameters().number);
}

COORDINATE_RESPONSE_MEASURE_CONTROLLER_TEST(
    coordinateResponsePassesGreenColor) {
    control.setGreenResponse();
    clickResponseButton(control);
    AV_SPEECH_IN_NOISE_EXPECT_COLOR(
        model, coordinate_response_measure::Color::green);
}

COORDINATE_RESPONSE_MEASURE_CONTROLLER_TEST(coordinateResponsePassesRedColor) {
    control.setRedResponse();
    clickResponseButton(control);
    AV_SPEECH_IN_NOISE_EXPECT_COLOR(
        model, coordinate_response_measure::Color::red);
}

COORDINATE_RESPONSE_MEASURE_CONTROLLER_TEST(coordinateResponsePassesBlueColor) {
    control.setBlueResponse();
    clickResponseButton(control);
    AV_SPEECH_IN_NOISE_EXPECT_COLOR(
        model, coordinate_response_measure::Color::blue);
}

COORDINATE_RESPONSE_MEASURE_CONTROLLER_TEST(
    coordinateResponsePassesWhiteColor) {
    control.setGrayResponse();
    clickResponseButton(control);
    AV_SPEECH_IN_NOISE_EXPECT_COLOR(
        model, coordinate_response_measure::Color::white);
}
}
}
