#include "FreeResponse.hpp"
#include "av-speech-in-noise/core/Configuration.hpp"

namespace av_speech_in_noise::submitting_free_response {
Controller::Controller(ConfigurationRegistry &registry,
    TestController &testController, Interactor &interactor, Control &control,
    Puzzle &puzzle, Timer &timer)
    : testController{testController}, interactor{interactor}, control{control},
      puzzle{puzzle}, timer{timer} {
    registry.subscribe(*this, "puzzle");
    control.attach(this);
    timer.attach(this);
}

void Controller::configure(const std::string &key, const std::string &value) {
    if (key == "puzzle") {
        if (value.empty()) {
            usingPuzzle = false;
        } else {
            LocalUrl url;
            url.path = value;
            puzzle.initialize(url);
            usingPuzzle = true;
        }
        trialsTowardNewPuzzlePiece = 0;
    }
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
        trialsTowardNewPuzzlePiece = 0;
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

Presenter::Presenter(ConfigurationRegistry &registry, TestView &testView,
    View &view, TestPresenter &testPresenter, Puzzle &puzzle)
    : testView{testView}, view{view}, testPresenter{testPresenter},
      puzzle{puzzle} {
    registry.subscribe(*this, "method");
}

void Presenter::configure(const std::string &key, const std::string &value) {
    if (key == "method")
        if (contains(value, "free response"))
            testPresenter.initialize(*this);
}

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
