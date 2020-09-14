#include "FreeResponse.hpp"

namespace av_speech_in_noise {
FreeResponseController::FreeResponseController(
    Model &model, FreeResponseControl &control)
    : model{model}, control{control} {
    control.attach(this);
}

void FreeResponseController::attach(TaskController::Observer *e) {
    observer = e;
}

void FreeResponseController::attach(TestController *e) { controller = e; }

void FreeResponseController::notifyThatSubmitButtonHasBeenClicked() {
    model.submit(FreeResponse{control.freeResponse(), control.flagged()});
    observer->notifyThatUserIsDoneResponding();
    controller->notifyThatUserIsDoneResponding();
}

FreeResponsePresenter::FreeResponsePresenter(
    TestView &testView, FreeResponseView &view)
    : testView{testView}, view{view} {}

void FreeResponsePresenter::start() { testView.showNextTrialButton(); }

void FreeResponsePresenter::stop() { view.hideFreeResponseSubmission(); }

void FreeResponsePresenter::notifyThatTaskHasStarted() {
    testView.hideNextTrialButton();
}

void FreeResponsePresenter::notifyThatUserIsDoneResponding() {
    view.hideFreeResponseSubmission();
}

void FreeResponsePresenter::showResponseSubmission() {
    view.clearFreeResponse();
    view.showFreeResponseSubmission();
}
}
