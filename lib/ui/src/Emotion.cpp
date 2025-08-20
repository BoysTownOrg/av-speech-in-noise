#include "Emotion.hpp"

namespace av_speech_in_noise::submitting_emotion {
Presenter::Presenter(ConfigurationRegistry &registry, UI &ui,
    TestController &testController, Interactor &interactor,
    SystemTime &systemTime, TestPresenter &testPresenter)
    : ui{ui}, testController{testController}, interactor{interactor},
      systemTime{systemTime}, testPresenter{testPresenter} {
    ui.attach(this);
    registry.subscribe(*this, "method");
}

void Presenter::configure(const std::string &key, const std::string &value) {
    if (key == "method") {
        if (contains(value, "emotions")) {
            testPresenter.initialize(*this);
            if (contains(value, "child emotions")) {
                ui.populateResponseButtons({{Emotion::angry},
                    {Emotion::happy, Emotion::neutral, Emotion::sad},
                    {Emotion::scared}});
            } else {
                ui.populateResponseButtons(
                    {{Emotion::angry, Emotion::disgusted},
                        {Emotion::happy, Emotion::neutral, Emotion::sad},
                        {Emotion::scared, Emotion::surprised}});
            }
        }
    }
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
