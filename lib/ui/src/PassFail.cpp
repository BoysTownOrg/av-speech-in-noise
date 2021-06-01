#include "PassFail.hpp"

namespace av_speech_in_noise::submitting_pass_fail {
Controller::Controller(
    TestController &testController, Interactor &interactor, Control &view)
    : testController{testController}, interactor{interactor} {
    view.attach(this);
}

static void notifyThatUserIsDoneResponding(TestController &controller) {
    controller
        .notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion();
}

void Controller::notifyThatCorrectButtonHasBeenClicked() {
    interactor.submitCorrectResponse();
    notifyThatUserIsDoneResponding(testController);
}

void Controller::notifyThatIncorrectButtonHasBeenClicked() {
    interactor.submitIncorrectResponse();
    notifyThatUserIsDoneResponding(testController);
}

Presenter::Presenter(TestView &experimenterView, View &view)
    : testView{experimenterView}, view{view} {}

void Presenter::start() { testView.showNextTrialButton(); }

void Presenter::stop() { view.hide(); }

void Presenter::hideResponseSubmission() { view.hide(); }

void Presenter::showResponseSubmission() { view.show(); }
}
