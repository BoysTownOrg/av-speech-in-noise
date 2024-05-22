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
    KeyPressResponse response;
    const auto keyPressed{control.keyPressed()};
    if (keyPressed == "1")
        response.key = KeyPressed::first;
    else if (keyPressed == "2")
        response.key = KeyPressed::second;
    else
        return;
    response.seconds = control.keyPressedSeconds();
    keyPressResponses.push_back(response);
    attemptToSubmitResponse();
}

void Presenter::start() { testView.showNextTrialButton(); }

void Presenter::stop() { acceptingKeyPresses = false; }

void Presenter::hideResponseSubmission() {}

void Presenter::notifyThatTrialHasStarted() {
    keyPressResponses.clear();
    acceptingKeyPresses = true;
    control.giveKeyFocus();
}

void Presenter::showResponseSubmission() { attemptToSubmitResponse(); }

void Presenter::attemptToSubmitResponse() {
    if (interactor.submits(keyPressResponses)) {
        testController.notifyThatUserIsDoneResponding();
        acceptingKeyPresses = false;
    }
}
}
