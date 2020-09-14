#include "Consonant.hpp"

namespace av_speech_in_noise {
ConsonantTaskPresenter::ConsonantTaskPresenter(ConsonantTaskView &view)
    : view{view} {}

void ConsonantTaskPresenter::start() {
    view.show();
    view.showReadyButton();
}

static void hideCursor(ConsonantTaskView &view) { view.hideCursor(); }

static void hideResponseButtons(ConsonantTaskView &view) {
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
    : model{model}, control{view} {
    view.attach(this);
}

void ConsonantTaskController::attach(TaskController::Observer *e) {
    observer = e;
}

void ConsonantTaskController::attach(TestController *p) { controller = p; }

static void notifyThatUserIsReadyForNextTrial(TestController *r) {
    r->notifyThatUserIsReadyForNextTrial();
}

void ConsonantTaskController::notifyThatReadyButtonHasBeenClicked() {
    observer->notifyThatTaskHasStarted();
    notifyThatUserIsReadyForNextTrial(controller);
}

void ConsonantTaskController::notifyThatResponseButtonHasBeenClicked() {
    model.submit(ConsonantResponse{control.consonant().front()});
    observer->notifyThatUserIsDoneResponding();
    notifyThatUserIsReadyForNextTrial(controller);
}
}
