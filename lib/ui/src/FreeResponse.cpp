#include "FreeResponse.hpp"

namespace av_speech_in_noise::submitting_free_response {
Controller::Controller(TestController &testController, Interactor &interactor,
    Control &control, Puzzle &puzzle, Timer &timer)
    : testController{testController},
      interactor{interactor}, control{control}, puzzle{puzzle}, timer{timer} {
    control.attach(this);
    timer.attach(this);
}

void Controller::initialize(bool usingPuzzle) {
    this->usingPuzzle = usingPuzzle;
    trialsTowardNewPuzzlePiece = 0;
}

void Controller::setNTrialsPerNewPuzzlePiece(int n) {
    trialsPerNewPuzzlePiece = n;
}

void Controller::notifyThatSubmitButtonHasBeenClicked() {
    FreeResponse freeResponse;
    freeResponse.flagged = control.flagged();
    freeResponse.response = control.response();
    interactor.submit(freeResponse);
    if (usingPuzzle && !freeResponse.flagged &&
        ++trialsTowardNewPuzzlePiece == trialsPerNewPuzzlePiece) {
        testController.notifyThatUserHasRespondedButTrialIsNotQuiteDone();
        puzzle.show();
        readyToAdvancePuzzle_ = true;
        timer.scheduleCallbackAfterSeconds(1.5);
    } else {
        testController.notifyThatUserIsDoneResponding();
    }
}

void Controller::callback() {
    if (readyToAdvancePuzzle_) {
        puzzle.advance();
        readyToAdvancePuzzle_ = false;
        timer.scheduleCallbackAfterSeconds(1.5);
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
