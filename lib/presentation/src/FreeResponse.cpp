#include "FreeResponse.hpp"

namespace av_speech_in_noise {
FreeResponseController::FreeResponseController(
    Model &model, FreeResponseInputView &view)
    : model{model}, view{view} {
    view.subscribe(this);
}

void FreeResponseController::subscribe(TaskController::EventListener *e) {
    listener = e;
}

void FreeResponseController::subscribe(ExperimenterController *e) {
    responder = e;
}

void FreeResponseController::notifyThatSubmitButtonHasBeenClicked() {
    model.submit(FreeResponse{view.freeResponse(), view.flagged()});
    listener->notifyThatUserIsDoneResponding();
    responder->notifyThatUserIsDoneResponding();
}

FreeResponsePresenter::FreeResponsePresenter(
    ExperimenterOutputView &experimenterView, FreeResponseOutputView &view)
    : experimenterView{experimenterView}, view{view} {}

void FreeResponsePresenter::start() { experimenterView.showNextTrialButton(); }

void FreeResponsePresenter::stop() { view.hideFreeResponseSubmission(); }

void FreeResponsePresenter::notifyThatTaskHasStarted() {
    experimenterView.hideNextTrialButton();
}

void FreeResponsePresenter::notifyThatUserIsDoneResponding() {
    view.hideFreeResponseSubmission();
}

void FreeResponsePresenter::showResponseSubmission() {
    view.clearFreeResponse();
    view.showFreeResponseSubmission();
}
}
