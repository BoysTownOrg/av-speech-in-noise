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
}

void ConsonantPresenter::notifyThatTrialHasStarted() {
    if (!model.testComplete())
        view.hideCursor();
}

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
    responder->playTrial();
}

void ConsonantResponder::notifyThatResponseButtonHasBeenClicked() {
    model.submit(ConsonantResponse{view.consonant().front()});
    listener->notifyThatUserIsDoneResponding();
    responder->playNextTrialIfNeeded();
}
}
