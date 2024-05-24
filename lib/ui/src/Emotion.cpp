#include "Emotion.hpp"

namespace av_speech_in_noise::submitting_emotion {
Presenter::Presenter(
    UI &ui, TestController &testController, Interactor &interactor)
    : ui{ui}, testController{testController}, interactor{interactor} {
    ui.attach(this);
}

void Presenter::start() {
    ui.show();
    ui.playButton().show();
}

void Presenter::notifyThatPlayButtonHasBeenClicked() {
    ui.playButton().hide();
    testController.notifyThatUserIsReadyForNextTrial();
}

void Presenter::notifyThatResponseButtonHasBeenClicked() {
    interactor.submit(EmotionResponse{ui.emotion()});
    testController.notifyThatUserIsDoneResponding();
    ui.playButton().show();
}

void Presenter::hideResponseSubmission() { ui.responseButtons().hide(); }

void Presenter::showResponseSubmission() { ui.responseButtons().show(); }

void Presenter::stop() {
    ui.responseButtons().hide();
    ui.hide();
}
}
