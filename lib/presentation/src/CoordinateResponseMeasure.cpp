#include "CoordinateResponseMeasure.hpp"

namespace av_speech_in_noise {
using coordinate_response_measure::Color;

static auto colorResponse(CoordinateResponseMeasureInputView &inputView)
    -> Color {
    if (inputView.greenResponse())
        return Color::green;
    if (inputView.blueResponse())
        return Color::blue;
    if (inputView.whiteResponse())
        return Color::white;
    return Color::red;
}

static auto subjectResponse(CoordinateResponseMeasureInputView &inputView)
    -> coordinate_response_measure::Response {
    coordinate_response_measure::Response p{};
    p.color = colorResponse(inputView);
    p.number = std::stoi(inputView.numberResponse());
    return p;
}

CoordinateResponseMeasureController::CoordinateResponseMeasureController(
    Model &model, CoordinateResponseMeasureInputView &view)
    : model{model}, view{view} {
    view.attach(this);
}

void CoordinateResponseMeasureController::attach(
    TaskController::Observer *e) {
    listener = e;
}

static void notifyThatUserIsReadyForNextTrial(ExperimenterController *r) {
    r->notifyThatUserIsReadyForNextTrial();
}

void CoordinateResponseMeasureController::notifyThatReadyButtonHasBeenClicked() {
    listener->notifyThatTaskHasStarted();
    notifyThatUserIsReadyForNextTrial(responder);
}

void CoordinateResponseMeasureController::
    notifyThatResponseButtonHasBeenClicked() {
    model.submit(subjectResponse(view));
    listener->notifyThatUserIsDoneResponding();
    notifyThatUserIsReadyForNextTrial(responder);
}

void CoordinateResponseMeasureController::attach(ExperimenterController *e) {
    responder = e;
}

static void hideResponseButtons(CoordinateResponseMeasureOutputView &view) {
    view.hideResponseButtons();
}

CoordinateResponseMeasurePresenter::CoordinateResponseMeasurePresenter(
    CoordinateResponseMeasureOutputView &view)
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
