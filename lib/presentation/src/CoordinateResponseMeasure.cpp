#include "CoordinateResponseMeasure.hpp"

namespace av_speech_in_noise {
using coordinate_response_measure::Color;

static auto colorResponse(CoordinateResponseMeasureControl &control) -> Color {
    if (control.greenResponse())
        return Color::green;
    if (control.blueResponse())
        return Color::blue;
    if (control.whiteResponse())
        return Color::white;
    return Color::red;
}

static auto subjectResponse(CoordinateResponseMeasureControl &control)
    -> coordinate_response_measure::Response {
    coordinate_response_measure::Response p{};
    p.color = colorResponse(control);
    p.number = std::stoi(control.numberResponse());
    return p;
}

CoordinateResponseMeasureController::CoordinateResponseMeasureController(
    Model &model, CoordinateResponseMeasureControl &control)
    : model{model}, control{control} {
    control.attach(this);
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
    model.submit(subjectResponse(control));
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
