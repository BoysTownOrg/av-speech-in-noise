#include "PassFail.hpp"

namespace av_speech_in_noise::submitting_pass_fail {
Presenter::Presenter(RunningATest &model, TestController &testController,
    TestView &testView, Interactor &interactor, UI &ui)
    : model{model}, testController{testController}, interactor{interactor},
      testView{testView}, ui{ui} {}

static void notifyThatUserIsDoneResponding(TestController &controller) {
    controller
        .notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion();
}

void Presenter::notifyThatCorrectButtonHasBeenClicked() {
    interactor.submitCorrectResponse();
    ui.clearDisplay();
    notifyThatUserIsDoneResponding(testController);
}

void Presenter::notifyThatIncorrectButtonHasBeenClicked() {
    interactor.submitIncorrectResponse();
    ui.clearDisplay();
    notifyThatUserIsDoneResponding(testController);
}

void Presenter::notifyThatShowAnswerButtonHasBeenClicked() {
    ui.display(model.targetFileName());
}

void Presenter::start() {
    ui.attach(this);
    testView.showNextTrialButton();
}

void Presenter::stop() { ui.hide(); }

void Presenter::hideResponseSubmission() { ui.hide(); }

void Presenter::showResponseSubmission() { ui.show(); }
}
