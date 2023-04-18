#include "FreeResponse.hpp"

namespace av_speech_in_noise::submitting_free_response {
Controller::Controller(
    TestController &testController, Interactor &interactor, Control &control)
    : testController{testController}, interactor{interactor}, control{control} {
    control.attach(this);
}

void Controller::notifyThatSubmitButtonHasBeenClicked() {
    FreeResponse freeResponse;
    freeResponse.flagged = control.flagged();
    freeResponse.response = control.response();
    interactor.submit(freeResponse);
    testController.notifyThatUserIsDoneResponding();
}

Presenter::Presenter(TestView &testView, View &view)
    : testView{testView}, view{view} {}

void Presenter::start() { testView.showNextTrialButton(); }

void Presenter::stop() { view.hide(); }

void Presenter::hideResponseSubmission() { view.hide(); }

void Presenter::showResponseSubmission() {
    view.clearResponse();
    view.clearFlag();
    view.show();
}

namespace with_puzzle {
Controller::Controller(TestController &testController, Interactor &interactor,
    Control &control, Puzzle &puzzle, Timer &timer)
    : testController{testController},
      interactor{interactor}, control{control}, puzzle{puzzle}, timer{timer} {
    control.attach(this);
    timer.attach(this);
}

void Controller::notifyThatSubmitButtonHasBeenClicked() {
    FreeResponse freeResponse;
    freeResponse.flagged = control.flagged();
    freeResponse.response = control.response();
    interactor.submit(freeResponse);
    if (freeResponse.flagged)
        testController.notifyThatUserIsDoneResponding();
    else {
        testController.notifyThatUserHasRespondedButTrialIsNotQuiteDone();
        puzzle.show();
        readyToAdvancePuzzle_ = true;
        timer.scheduleCallbackAfterSeconds(1);
    }
}

void Controller::callback() {
    if (readyToAdvancePuzzle_) {
        puzzle.advance();
        readyToAdvancePuzzle_ = false;
        timer.scheduleCallbackAfterSeconds(1);
    } else {
        puzzle.hide();
        testController.notifyThatUserIsDoneResponding();
    }
}

Presenter::Presenter(TestView &testView, View &view, Puzzle &puzzle)
    : testView{testView}, view{view}, puzzle{puzzle} {}

void Presenter::start() {
    puzzle.reset();
    testView.showNextTrialButton();
}

void Presenter::stop() { view.hide(); }

void Presenter::hideResponseSubmission() { view.hide(); }

void Presenter::showResponseSubmission() {
    view.clearResponse();
    view.clearFlag();
    view.show();
}
}
}
