#include "KeyPress.hpp"

namespace av_speech_in_noise::submitting_keypress {
Presenter::Presenter(TestView &testView, TestController &testController,
    Interactor &interactor, Control &control, MaskerPlayer &maskerPlayer)
    : testView{testView}, testController{testController},
      interactor{interactor}, control{control}, maskerPlayer{maskerPlayer} {
    control.attach(this);
}

void Presenter::notifyThatTargetWillPlayAt(const PlayerTimeWithDelay &t) {
    targetStartTimeMilliseconds =
        static_cast<double>(
            (maskerPlayer.nanoseconds(t.playerTime) + 500000) / 1000000) +
        t.delay.seconds * 1000;
}

void Presenter::notifyThatKeyHasBeenPressed() {
    if (ready) {
        auto response{KeyPressResponse{}};
        const auto seconds{control.keyPressedSeconds()};
        response.rt.milliseconds = seconds * 1000 - targetStartTimeMilliseconds;
        const auto keyPressed{control.keyPressed()};
        if (keyPressed == "1")
            response.key = KeyPressed::first;
        else if (keyPressed == "2")
            response.key = KeyPressed::second;
        else
            response.key = KeyPressed::unknown;
        interactor.submit(response);
        testController.notifyThatUserIsDoneResponding();
    }
}

void Presenter::start() { testView.showNextTrialButton(); }

void Presenter::stop() { ready = false; }

void Presenter::hideResponseSubmission() { ready = false; }

void Presenter::showResponseSubmission() { ready = true; }
}
