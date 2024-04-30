#include "Consonant.hpp"

namespace av_speech_in_noise::submitting_consonant {
Controller::Controller(TestController &testController, Interactor &interactor,
    Control &control, Presenter &presenter)
    : testController{testController}, interactor{interactor}, control{control},
      presenter{presenter} {
    control.attach(this);
}

void Controller::notifyThatReadyButtonHasBeenClicked() {
    presenter.hideReadyButton();
    testController.notifyThatUserIsReadyForNextTrial();
}

void Controller::notifyThatResponseButtonHasBeenClicked() {
    interactor.submit(ConsonantResponse{control.consonant()});
    testController.notifyThatUserIsDoneRespondingAndIsReadyForNextTrial();
}

PresenterImpl::PresenterImpl(View &view) : view{view} {}

void PresenterImpl::start() {
    view.show();
    view.showReadyButton();
}

static void hideResponseButtons(View &view) { view.hideResponseButtons(); }

void PresenterImpl::stop() {
    hideResponseButtons(view);
    view.hide();
}

void PresenterImpl::hideReadyButton() { view.hideReadyButton(); }

void PresenterImpl::hideResponseSubmission() { hideResponseButtons(view); }

void PresenterImpl::showResponseSubmission() {
    view.showResponseButtons();
    view.showCursor();
}

void PresenterImpl::notifyThatTrialHasStarted() { view.hideCursor(); }
}
