#include "KeyPress.hpp"

namespace av_speech_in_noise::submitting_keypress {
Controller::Controller(
    TestController &testController, Interactor &interactor, Control &control)
    : testController{testController}, interactor{interactor}, control{control} {
    control.attach(this);
}

void Controller::notifyThatKeyHasBeenPressed() {
    // TODO
    // interactor.submit(KeyPressResponse{});
    // testController.notifyThatUserIsDoneResponding();
}

Presenter::Presenter(TestView &testView) : testView{testView} {}

void Presenter::start() { testView.showNextTrialButton(); }

void Presenter::stop() {}

void Presenter::hideResponseSubmission() {}

void Presenter::showResponseSubmission() {}

void Presenter::notifyThatTrialHasStarted() {}
}
