#include "FixedPassFail.hpp"

namespace av_speech_in_noise::submitting_fixed_pass_fail {
Presenter::Presenter(TestController &testController, TestView &testView,
    Interactor &interactor, submitting_pass_fail::UI &ui)
    : testController{testController}, interactor{interactor},
      testView{testView}, ui{ui} {}

void Presenter::notifyThatCorrectButtonHasBeenClicked() {
    interactor.submitCorrectResponse();
    testController.notifyThatUserIsDoneResponding();
}

void Presenter::notifyThatIncorrectButtonHasBeenClicked() {
    interactor.submitIncorrectResponse();
    testController.notifyThatUserIsDoneResponding();
}

void Presenter::start() {
    ui.attach(this);
    testView.showNextTrialButton();
}

void Presenter::stop() { ui.hide(); }

void Presenter::hideResponseSubmission() { ui.hide(); }

void Presenter::showResponseSubmission() { ui.show(); }
}
