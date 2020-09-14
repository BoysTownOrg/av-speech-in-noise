#include "Consonant.hpp"

namespace av_speech_in_noise {
ConsonantTaskPresenter::ConsonantTaskPresenter(ConsonantTaskOutputView &view)
    : view{view} {}

void ConsonantTaskPresenter::start() {
    view.show();
    view.showReadyButton();
}

static void hideCursor(ConsonantTaskOutputView &view) { view.hideCursor(); }

static void hideResponseButtons(ConsonantTaskOutputView &view) {
    view.hideResponseButtons();
}

void ConsonantTaskPresenter::stop() {
    hideResponseButtons(view);
    view.hide();
}

void ConsonantTaskPresenter::notifyThatTaskHasStarted() {
    view.hideReadyButton();
}

void ConsonantTaskPresenter::notifyThatUserIsDoneResponding() {
    hideResponseButtons(view);
}

void ConsonantTaskPresenter::notifyThatTrialHasStarted() { hideCursor(view); }

void ConsonantTaskPresenter::showResponseSubmission() {
    view.showResponseButtons();
    view.showCursor();
}

ConsonantTaskController::ConsonantTaskController(
    Model &model, ConsonantTaskControl &view)
    : model{model}, view{view} {
    view.attach(this);
}

void ConsonantTaskController::attach(TaskController::Observer *e) {
    listener = e;
}

void ConsonantTaskController::attach(ExperimenterController *p) {
    responder = p;
}

static void notifyThatUserIsReadyForNextTrial(ExperimenterController *r) {
    r->notifyThatUserIsReadyForNextTrial();
}

void ConsonantTaskController::notifyThatReadyButtonHasBeenClicked() {
    listener->notifyThatTaskHasStarted();
    notifyThatUserIsReadyForNextTrial(responder);
}

void ConsonantTaskController::notifyThatResponseButtonHasBeenClicked() {
    model.submit(ConsonantResponse{view.consonant().front()});
    listener->notifyThatUserIsDoneResponding();
    notifyThatUserIsReadyForNextTrial(responder);
}
}
