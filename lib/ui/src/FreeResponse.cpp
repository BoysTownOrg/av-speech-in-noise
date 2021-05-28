#include "FreeResponse.hpp"

namespace av_speech_in_noise::submitting_free_response {
Controller::Controller(
    TestController &testController, Interactor &model, Control &control)
    : testController{testController}, model{model}, control{control} {
    control.attach(this);
}

void Controller::notifyThatSubmitButtonHasBeenClicked() {
    FreeResponse freeResponse;
    freeResponse.flagged = control.flagged();
    freeResponse.response = control.response();
    model.submit(freeResponse);
    testController.notifyThatUserIsDoneResponding();
}

Presenter::Presenter(TestView &testView, View &view)
    : testView{testView}, view{view} {}

void Presenter::start() { testView.showNextTrialButton(); }

void Presenter::stop() { view.hide(); }

void Presenter::hideResponseSubmission() { view.hide(); }

void Presenter::showResponseSubmission() {
    view.clearResponse();
    view.clearFlag();
    view.show();
}
}
