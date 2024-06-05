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
    EmotionResponse response;
    response.emotion = ui.emotion();
    interactor.submit(response);
    // TODO: This shows the play button on the tester window, but that is not wanted
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
