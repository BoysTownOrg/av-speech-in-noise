#include "KeyPress.hpp"

namespace av_speech_in_noise::submitting_keypress {
Presenter::Presenter(TestView &testView, TestController &testController,
    Interactor &interactor, Control &control)
    : testView{testView}, testController{testController},
      interactor{interactor}, control{control} {
    control.attach(this);
}

void Presenter::notifyThatKeyHasBeenPressed() {
    if (!acceptingKeyPresses)
        return;
    auto response{KeyPressResponse{}};
    const auto keyPressed{control.keyPressed()};
    if (keyPressed == "1")
        response.key = KeyPressed::first;
    else if (keyPressed == "2")
        response.key = KeyPressed::second;
    else
        return;
    const auto seconds{control.keyPressedSeconds()};
    response.seconds = seconds;
    keyPressResponses.push_back(response);
    if (interactor.submits(keyPressResponses)) {
        testController.notifyThatUserIsDoneResponding();
        acceptingKeyPresses = false;
    }
}

void Presenter::start() { testView.showNextTrialButton(); }

void Presenter::stop() { acceptingKeyPresses = false; }

void Presenter::hideResponseSubmission() {}

void Presenter::notifyThatTrialHasStarted() {
    keyPressResponses.clear();
    acceptingKeyPresses = true;
    control.giveKeyFocus();
}

void Presenter::showResponseSubmission() {}
}
