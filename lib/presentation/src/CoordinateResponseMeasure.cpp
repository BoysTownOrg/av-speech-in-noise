#include "CoordinateResponseMeasure.hpp"

namespace av_speech_in_noise {
static auto colorResponse(CoordinateResponseMeasureInputView *inputView)
    -> coordinate_response_measure::Color {
    if (inputView->greenResponse())
        return coordinate_response_measure::Color::green;
    if (inputView->blueResponse())
        return coordinate_response_measure::Color::blue;
    if (inputView->whiteResponse())
        return coordinate_response_measure::Color::white;

    return coordinate_response_measure::Color::red;
}

static auto subjectResponse(CoordinateResponseMeasureInputView *inputView)
    -> coordinate_response_measure::Response {
    coordinate_response_measure::Response p{};
    p.color = colorResponse(inputView);
    p.number = std::stoi(inputView->numberResponse());
    return p;
}

CoordinateResponseMeasureResponder::CoordinateResponseMeasureResponder(
    Model &model, CoordinateResponseMeasureInputView &view)
    : model{model}, view{view} {
    view.subscribe(this);
}

void CoordinateResponseMeasureResponder::subscribe(
    TaskResponder::EventListener *e) {
    listener = e;
}

void CoordinateResponseMeasureResponder::notifyThatReadyButtonHasBeenClicked() {
    listener->notifyThatTaskHasStarted();
    responder->nextTrial();
}

void CoordinateResponseMeasureResponder::
    notifyThatResponseButtonHasBeenClicked() {
    model.submit(subjectResponse(&view));
    listener->notifyThatUserIsDoneResponding();
    responder->nextTrial();
}

void CoordinateResponseMeasureResponder::subscribe(ExperimenterResponder *e) {
    responder = e;
}

CoordinateResponseMeasurePresenter::CoordinateResponseMeasurePresenter(
    CoordinateResponseMeasureOutputView &view)
    : view{view} {}

void CoordinateResponseMeasurePresenter::start() {
    view.show();
    view.showNextTrialButton();
}

void CoordinateResponseMeasurePresenter::stop() {
    view.hideResponseButtons();
    view.hide();
}

void CoordinateResponseMeasurePresenter::notifyThatTaskHasStarted() {
    view.hideNextTrialButton();
}

void CoordinateResponseMeasurePresenter::notifyThatUserIsDoneResponding() {
    view.hideResponseButtons();
}

void CoordinateResponseMeasurePresenter::showResponseSubmission() {
    view.showResponseButtons();
}
}
