#include "CoordinateResponseMeasure.hpp"

namespace av_speech_in_noise {
using coordinate_response_measure::Color;

static auto colorResponse(CoordinateResponseMeasureControl &inputView)
    -> Color {
    if (inputView.greenResponse())
        return Color::green;
    if (inputView.blueResponse())
        return Color::blue;
    if (inputView.whiteResponse())
        return Color::white;
    return Color::red;
}

static auto subjectResponse(CoordinateResponseMeasureControl &inputView)
    -> coordinate_response_measure::Response {
    coordinate_response_measure::Response p{};
    p.color = colorResponse(inputView);
    p.number = std::stoi(inputView.numberResponse());
    return p;
}

CoordinateResponseMeasureController::CoordinateResponseMeasureController(
    Model &model, CoordinateResponseMeasureControl &view)
    : model{model}, view{view} {
    view.attach(this);
}

void CoordinateResponseMeasureController::attach(TaskController::Observer *e) {
    observer = e;
}

static void notifyThatUserIsReadyForNextTrial(TestController *c) {
    c->notifyThatUserIsReadyForNextTrial();
}

void CoordinateResponseMeasureController::
    notifyThatReadyButtonHasBeenClicked() {
    observer->notifyThatTaskHasStarted();
    notifyThatUserIsReadyForNextTrial(controller);
}

void CoordinateResponseMeasureController::
    notifyThatResponseButtonHasBeenClicked() {
    model.submit(subjectResponse(view));
    observer->notifyThatUserIsDoneResponding();
    notifyThatUserIsReadyForNextTrial(controller);
}

void CoordinateResponseMeasureController::attach(TestController *c) {
    controller = c;
}

static void hideResponseButtons(CoordinateResponseMeasureView &view) {
    view.hideResponseButtons();
}

CoordinateResponseMeasurePresenter::CoordinateResponseMeasurePresenter(
    CoordinateResponseMeasureView &view)
    : view{view} {}

void CoordinateResponseMeasurePresenter::start() {
    view.show();
    view.showNextTrialButton();
}

void CoordinateResponseMeasurePresenter::stop() {
    hideResponseButtons(view);
    view.hide();
}

void CoordinateResponseMeasurePresenter::notifyThatTaskHasStarted() {
    view.hideNextTrialButton();
}

void CoordinateResponseMeasurePresenter::notifyThatUserIsDoneResponding() {
    hideResponseButtons(view);
}

void CoordinateResponseMeasurePresenter::showResponseSubmission() {
    view.showResponseButtons();
}
}
