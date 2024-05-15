#include "KeyPress.hpp"

namespace av_speech_in_noise::submitting_keypress {
Presenter::Presenter(TestView &testView, TestController &testController,
    Interactor &interactor, Control &control)
    : testView{testView}, testController{testController},
      interactor{interactor}, control{control} {
    control.attach(this);
}

void Presenter::notifyThatKeyHasBeenPressed() {
    if (ready) {
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
        interactor.submit(response);
        testController.notifyThatUserIsDoneResponding();
    }
}

void Presenter::start() { testView.showNextTrialButton(); }

void Presenter::stop() { ready = false; }

void Presenter::hideResponseSubmission() { ready = false; }

void Presenter::showResponseSubmission() { ready = true; }
}
