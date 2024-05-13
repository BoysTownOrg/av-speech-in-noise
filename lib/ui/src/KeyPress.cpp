#include "KeyPress.hpp"

namespace av_speech_in_noise::submitting_keypress {
Presenter::Presenter(TestView &testView, TestController &testController,
    Interactor &interactor, Control &control)
    : testView{testView}, testController{testController},
      interactor{interactor}, control{control} {
    control.attach(this);
}

void Presenter::notifyThatKeyHasBeenPressed() {
    // TODO
    // if (ready) {
        // interactor.submit(KeyPressResponse{});
        // testController.notifyThatUserIsDoneResponding();
    // }
}

void Presenter::start() {
    testView.showNextTrialButton();
}

void Presenter::stop() {}

void Presenter::hideResponseSubmission() {
    // ready = false;
}

void Presenter::showResponseSubmission() {
    // ready = true;
}
}
