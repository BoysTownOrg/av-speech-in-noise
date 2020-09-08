#include "FreeResponse.hpp"

namespace av_speech_in_noise {
FreeResponseResponder::FreeResponseResponder(
    Model &model, FreeResponseInputView &view)
    : model{model}, view{view} {
    view.subscribe(this);
}

void FreeResponseResponder::subscribe(TaskResponder::EventListener *e) {
    listener = e;
}

void FreeResponseResponder::subscribe(ExperimenterResponder *e) {
    responder = e;
}

void FreeResponseResponder::notifyThatSubmitButtonHasBeenClicked() {
    model.submit(FreeResponse{view.freeResponse(), view.flagged()});
    listener->notifyThatUserIsDoneResponding();
    responder->readyNextTrialIfNeeded();
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
