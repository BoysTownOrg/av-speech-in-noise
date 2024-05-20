#include "KeyPress.hpp"

namespace av_speech_in_noise::submitting_keypress {
Presenter::Presenter(TestView &testView, TestController &testController,
    Interactor &interactor, Control &control)
    : testView{testView}, testController{testController},
      interactor{interactor}, control{control} {
    control.attach(this);
}

void Presenter::notifyThatKeyHasBeenPressed() {
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
    if (readyForResponse) {
        interactor.submit(keyPressResponses);
        testController.notifyThatUserIsDoneResponding();
        readyForResponse = false;
    }
}

void Presenter::start() { testView.showNextTrialButton(); }

void Presenter::stop() { readyForResponse = false; }

void Presenter::hideResponseSubmission() { readyForResponse = false; }

void Presenter::notifyThatTrialHasStarted() {
    keyPressResponses.clear();
    control.giveKeyFocus();
}

void Presenter::showResponseSubmission() {
    readyForResponse = true;
    if (!keyPressResponses.empty()) {
        interactor.submit(keyPressResponses);
        testController.notifyThatUserIsDoneResponding();
        readyForResponse = false;
    }
}
}
