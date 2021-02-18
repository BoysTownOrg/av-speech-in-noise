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

void ConsonantTaskPresenter::hideResponseSubmission() {
    hideResponseButtons(view);
}

void ConsonantTaskPresenter::notifyThatTrialHasStarted() { hideCursor(view); }

void ConsonantTaskPresenter::showResponseSubmission() {
    view.showResponseButtons();
    view.showCursor();
}

ConsonantTaskController::ConsonantTaskController(
    TestController &testController, Model &model, ConsonantTaskControl &view)
    : testController{testController}, model{model}, control{view} {
    view.attach(this);
}

void ConsonantTaskController::attach(TaskController::Observer *e) {
    observer = e;
}

void ConsonantTaskController::notifyThatReadyButtonHasBeenClicked() {
    observer->notifyThatTaskHasStarted();
    testController.notifyThatUserIsReadyForNextTrial();
    observer->notifyThatTrialHasStarted();
}

void ConsonantTaskController::notifyThatResponseButtonHasBeenClicked() {
    model.submit(ConsonantResponse{control.consonant().front()});
    testController.notifyThatUserIsDoneRespondingAndIsReadyForNextTrial();
    observer->notifyThatTrialHasStarted();
}
}
