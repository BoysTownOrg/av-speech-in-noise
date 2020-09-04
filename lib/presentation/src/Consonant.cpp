#include "Consonant.hpp"

namespace av_speech_in_noise {
ConsonantPresenter::ConsonantPresenter(Model &model, ConsonantOutputView &view)
    : model{model}, view{view} {}

void ConsonantPresenter::start() {
    view.show();
    view.showReadyButton();
}

void ConsonantPresenter::stop() {
    view.hideResponseButtons();
    view.hide();
}

void ConsonantPresenter::notifyThatTaskHasStarted() {
    view.hideReadyButton();
    view.hideCursor();
}

void ConsonantPresenter::notifyThatUserIsDoneResponding() {
    view.hideResponseButtons();
    if (!model.testComplete())
        view.hideCursor();
}

void ConsonantPresenter::showResponseSubmission() {
    view.showResponseButtons();
}

ConsonantResponder::ConsonantResponder(Model &model, ConsonantInputView &view)
    : model{model}, view{view} {
    view.subscribe(this);
}

void ConsonantResponder::subscribe(TaskResponder::EventListener *e) {
    listener = e;
}

void ConsonantResponder::notifyThatReadyButtonHasBeenClicked() {
    parent->playTrial();
    listener->notifyThatTaskHasStarted();
}

void ConsonantResponder::notifyThatResponseButtonHasBeenClicked() {
    model.submit(ConsonantResponse{view.consonant().front()});
    listener->notifyThatUserIsDoneResponding();
    parent->playNextTrialIfNeeded();
}

void ConsonantResponder::becomeChild(ParentPresenter *p) { parent = p; }
}
