#include "FreeResponse.hpp"

namespace av_speech_in_noise {
FreeResponseController::FreeResponseController(
    Model &model, FreeResponseControl &view)
    : model{model}, view{view} {
    view.attach(this);
}

void FreeResponseController::attach(TaskController::Observer *e) {
    listener = e;
}

void FreeResponseController::attach(TestController *e) { responder = e; }

void FreeResponseController::notifyThatSubmitButtonHasBeenClicked() {
    model.submit(FreeResponse{view.freeResponse(), view.flagged()});
    listener->notifyThatUserIsDoneResponding();
    responder->notifyThatUserIsDoneResponding();
}

FreeResponsePresenter::FreeResponsePresenter(
    ExperimenterView &experimenterView, FreeResponseView &view)
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
