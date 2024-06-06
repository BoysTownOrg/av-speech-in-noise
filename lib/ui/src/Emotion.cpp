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
    testController.notifyThatUserIsReadyForNextTrial();
}

void Presenter::notifyThatTrialHasStarted() { ui.playButton().hide(); }

void Presenter::notifyThatResponseButtonHasBeenClicked() {
    EmotionResponse response;
    response.emotion = ui.emotion();
    interactor.submit(response);
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
