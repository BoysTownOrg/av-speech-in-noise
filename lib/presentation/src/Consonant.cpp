#include "Consonant.hpp"

namespace av_speech_in_noise {
ConsonantTaskController::ConsonantTaskController(TestController &testController,
    Model &model, ConsonantTaskControl &view, ConsonantTaskPresenter &presenter)
    : testController{testController}, model{model}, control{view},
      presenter{presenter} {
    view.attach(this);
}

void ConsonantTaskController::notifyThatReadyButtonHasBeenClicked() {
    presenter.hideReadyButton();
    testController.notifyThatUserIsReadyForNextTrial();
}

void ConsonantTaskController::notifyThatResponseButtonHasBeenClicked() {
    model.submit(ConsonantResponse{control.consonant().front()});
    testController.notifyThatUserIsDoneRespondingAndIsReadyForNextTrial();
}

ConsonantTaskPresenterImpl::ConsonantTaskPresenterImpl(ConsonantTaskView &view)
    : view{view} {}

void ConsonantTaskPresenterImpl::start() {
    view.show();
    view.showReadyButton();
}

static void hideResponseButtons(ConsonantTaskView &view) {
    view.hideResponseButtons();
}

void ConsonantTaskPresenterImpl::stop() {
    hideResponseButtons(view);
    view.hide();
}

void ConsonantTaskPresenterImpl::hideReadyButton() { view.hideReadyButton(); }

void ConsonantTaskPresenterImpl::hideResponseSubmission() {
    hideResponseButtons(view);
}

void ConsonantTaskPresenterImpl::complete() { view.showCursor(); }

void ConsonantTaskPresenterImpl::showResponseSubmission() {
    view.showResponseButtons();
    view.showCursor();
}

void ConsonantTaskPresenterImpl::notifyThatTrialHasStarted() {
    view.hideCursor();
}
}
