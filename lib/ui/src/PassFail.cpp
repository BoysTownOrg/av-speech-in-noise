#include "PassFail.hpp"

namespace av_speech_in_noise::submitting_pass_fail {
Presenter::Presenter(TestController &testController, TestView &testView,
    Interactor &interactor, UI &ui)
    : testController{testController}, interactor{interactor},
      testView{testView}, ui{ui} {
    ui.attach(this);
}

static void notifyThatUserIsDoneResponding(TestController &controller) {
    controller
        .notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion();
}

void Presenter::notifyThatCorrectButtonHasBeenClicked() {
    interactor.submitCorrectResponse();
    notifyThatUserIsDoneResponding(testController);
}

void Presenter::notifyThatIncorrectButtonHasBeenClicked() {
    interactor.submitIncorrectResponse();
    notifyThatUserIsDoneResponding(testController);
}

void Presenter::start() { testView.showNextTrialButton(); }

void Presenter::stop() { ui.hide(); }

void Presenter::hideResponseSubmission() { ui.hide(); }

void Presenter::showResponseSubmission() { ui.show(); }
}
