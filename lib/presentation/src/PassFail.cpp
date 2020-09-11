#include "PassFail.hpp"

namespace av_speech_in_noise {
PassFailController::PassFailController(Model &model, PassFailInputView &view)
    : model{model} {
    view.subscribe(this);
}

void PassFailController::subscribe(TaskController::EventListener *e) {
    listener = e;
}

static void notifyThatUserIsDoneResponding(
    TaskController::EventListener *listener, ExperimenterController *responder) {
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

void PassFailController::subscribe(ExperimenterController *p) { responder = p; }

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
