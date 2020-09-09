#include "Consonant.hpp"

namespace av_speech_in_noise {
ConsonantPresenter::ConsonantPresenter(ConsonantOutputView &view)
    : view{view} {}

void ConsonantPresenter::start() {
    view.show();
    view.showReadyButton();
}

static void hideCursor(ConsonantOutputView &view) { view.hideCursor(); }

static void hideResponseButtons(ConsonantOutputView &view) {
    view.hideResponseButtons();
}

void ConsonantPresenter::stop() {
    hideResponseButtons(view);
    view.hide();
}

void ConsonantPresenter::notifyThatTaskHasStarted() {
    view.hideReadyButton();
    hideCursor(view);
}

void ConsonantPresenter::notifyThatUserIsDoneResponding() {
    hideResponseButtons(view);
}

void ConsonantPresenter::notifyThatTrialHasStarted() { hideCursor(view); }

void ConsonantPresenter::showResponseSubmission() {
    view.showResponseButtons();
    view.showCursor();
}

ConsonantResponder::ConsonantResponder(Model &model, ConsonantInputView &view)
    : model{model}, view{view} {
    view.subscribe(this);
}

void ConsonantResponder::subscribe(TaskResponder::EventListener *e) {
    listener = e;
}

void ConsonantResponder::subscribe(ExperimenterResponder *p) { responder = p; }

void ConsonantResponder::notifyThatReadyButtonHasBeenClicked() {
    listener->notifyThatTaskHasStarted();
    responder->notifyThatUserIsReadyForNextTrial();
}

void ConsonantResponder::notifyThatResponseButtonHasBeenClicked() {
    model.submit(ConsonantResponse{view.consonant().front()});
    listener->notifyThatUserIsDoneResponding();
    responder->notifyThatUserIsReadyForNextTrial();
}
}
