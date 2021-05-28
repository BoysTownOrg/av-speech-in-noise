#include "PassFail.hpp"

namespace av_speech_in_noise::submitting_pass_fail {
Controller::Controller(
    TestController &testController, Model &model, Control &view)
    : testController{testController}, model{model} {
    view.attach(this);
}

static void notifyThatUserIsDoneResponding(TestController &controller) {
    controller
        .notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion();
}

void Controller::notifyThatCorrectButtonHasBeenClicked() {
    model.submitCorrectResponse();
    notifyThatUserIsDoneResponding(testController);
}

void Controller::notifyThatIncorrectButtonHasBeenClicked() {
    model.submitIncorrectResponse();
    notifyThatUserIsDoneResponding(testController);
}

Presenter::Presenter(TestView &experimenterView, View &view)
    : testView{experimenterView}, view{view} {}

void Presenter::start() { testView.showNextTrialButton(); }

void Presenter::stop() { view.hide(); }

void Presenter::hideResponseSubmission() { view.hide(); }

void Presenter::showResponseSubmission() { view.show(); }
}
