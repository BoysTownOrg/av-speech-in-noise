#include "FreeResponse.hpp"

namespace av_speech_in_noise {
FreeResponseController::FreeResponseController(
    TestController &testController, Model &model, FreeResponseControl &control)
    : testController{testController}, model{model}, control{control} {
    control.attach(this);
}

void FreeResponseController::notifyThatSubmitButtonHasBeenClicked() {
    model.submit(FreeResponse{control.freeResponse(), control.flagged()});
    testController.notifyThatUserIsDoneResponding();
}

FreeResponsePresenter::FreeResponsePresenter(
    TestView &testView, FreeResponseView &view)
    : testView{testView}, view{view} {}

void FreeResponsePresenter::start() { testView.showNextTrialButton(); }

void FreeResponsePresenter::stop() { view.hideFreeResponseSubmission(); }

void FreeResponsePresenter::hideResponseSubmission() {
    view.hideFreeResponseSubmission();
}

void FreeResponsePresenter::showResponseSubmission() {
    view.clearFreeResponse();
    view.clearFlag();
    view.showFreeResponseSubmission();
}
}
