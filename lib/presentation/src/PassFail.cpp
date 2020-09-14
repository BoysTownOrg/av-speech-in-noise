#include "PassFail.hpp"

namespace av_speech_in_noise {
PassFailController::PassFailController(Model &model, PassFailControl &view)
    : model{model} {
    view.attach(this);
}

void PassFailController::attach(TaskController::Observer *e) {
    listener = e;
}

static void notifyThatUserIsDoneResponding(
    TaskController::Observer *listener, ExperimenterController *responder) {
    listener->notifyThatUserIsDoneResponding();
    responder
        ->notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion();
}

void PassFailController::notifyThatCorrectButtonHasBeenClicked() {
    model.submitCorrectResponse();
    notifyThatUserIsDoneResponding(listener, responder);
}

void PassFailController::notifyThatIncorrectButtonHasBeenClicked() {
    model.submitIncorrectResponse();
    notifyThatUserIsDoneResponding(listener, responder);
}

void PassFailController::attach(ExperimenterController *p) { responder = p; }

PassFailPresenter::PassFailPresenter(
    ExperimenterOutputView &experimenterView, PassFailOutputView &view)
    : experimenterView{experimenterView}, view{view} {}

void PassFailPresenter::start() { experimenterView.showNextTrialButton(); }

void PassFailPresenter::stop() { view.hideEvaluationButtons(); }

void PassFailPresenter::notifyThatTaskHasStarted() {
    experimenterView.hideNextTrialButton();
}

void PassFailPresenter::notifyThatUserIsDoneResponding() {
    view.hideEvaluationButtons();
}

void PassFailPresenter::showResponseSubmission() {
    view.showEvaluationButtons();
}
}
