#include "KeyPress.hpp"

namespace av_speech_in_noise::submitting_keypress {
Presenter::Presenter(ConfigurationRegistry &registry, TestView &testView,
    TestController &testController, Interactor &interactor, Control &control,
    Timer &timer, TestPresenter &testPresenter)
    : testView{testView}, testController{testController},
      interactor{interactor}, control{control}, timer{timer},
      testPresenter{testPresenter} {
    control.attach(this);
    timer.attach(this);
    registry.subscribe(*this, "method");
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

void Presenter::start() {
    switch (state) {
    case State::aboutToDualTask:
        state = State::dualTasking;
        interactor.deferNextTrial();
        break;
    case State::dualTasking:
    case State::singleTask:
        state = State::singleTask;
        interactor.dontDeferNextTrial();
        break;
    }

    testView.showNextTrialButton();
}

void Presenter::stop() {
    if (acceptingKeyPresses)
        timer.cancelLastCallback();
    acceptingKeyPresses = false;
    hideResponseSubmission();
}

void Presenter::hideResponseSubmission() {
    if (state == State::dualTasking)
        if (dualTask != nullptr)
            dualTask->hideResponseSubmission();
}

void Presenter::notifyThatTrialHasStarted() {
    keyPressResponses.clear();
    acceptingKeyPresses = true;
    control.giveKeyFocus();
}

void Presenter::showResponseSubmission() {
    timer.scheduleCallbackAfterSeconds(5);
    attemptToSubmitResponse();
}

void Presenter::attemptToSubmitResponse() {
    if (interactor.submits(keyPressResponses)) {
        timer.cancelLastCallback();
        acceptingKeyPresses = false;
        moveAlong();
    }
}

void Presenter::callback() {
    interactor.forceSubmit(keyPressResponses);
    acceptingKeyPresses = false;
    moveAlong();
}

void Presenter::moveAlong() {
    if (state == State::dualTasking) {
        if (dualTask != nullptr)
            dualTask->showResponseSubmission();
    } else
        testController.notifyThatUserIsDoneResponding();
}

void Presenter::enableDualTask(TaskPresenter *other) {
    dualTask = other;
    state = State::aboutToDualTask;
}

void Presenter::configure(const std::string &key, const std::string &value) {
    if (key == "method")
        if (contains(value, "button response"))
            testPresenter.apply(*this);
}
}
