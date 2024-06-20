#include "Emotion.hpp"

namespace av_speech_in_noise::submitting_emotion {
Presenter::Presenter(UI &ui, TestController &testController,
    Interactor &interactor, SystemTime &systemTime)
    : ui{ui}, testController{testController}, interactor{interactor},
      systemTime{systemTime} {
    ui.attach(this);
}

void Presenter::start() {
    ui.show();
    ui.playButton().show();
}

void Presenter::notifyThatPlayButtonHasBeenClicked() {
    testController.notifyThatUserIsReadyForNextTrial();
}

void Presenter::notifyThatTrialHasStarted() {
    ui.playButton().hide();
    ui.cursor().hide();
}

void Presenter::notifyThatResponseButtonHasBeenClicked() {
    EmotionResponse response;
    response.emotion = ui.emotion();
    response.reactionTimeMilliseconds =
        (systemTime.nowSeconds() - lastResponseShownSeconds) * 1000.;
    interactor.submit(response);
    testController.notifyThatUserIsDoneResponding();
    ui.playButton().show();
}

void Presenter::hideResponseSubmission() { ui.responseButtons().hide(); }

void Presenter::showResponseSubmission() {
    ui.responseButtons().show();
    ui.cursor().show();
    lastResponseShownSeconds = systemTime.nowSeconds();
}

void Presenter::stop() {
    ui.responseButtons().hide();
    ui.hide();
}
}
