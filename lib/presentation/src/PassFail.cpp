#include "PassFail.hpp"

namespace av_speech_in_noise {
PassFailController::PassFailController(Model &model, PassFailControl &view)
    : model{model} {
    view.attach(this);
}

void PassFailController::attach(TaskController::Observer *e) { observer = e; }

static void notifyThatUserIsDoneResponding(
    TaskController::Observer *listener, TestController *controller) {
    listener->notifyThatUserIsDoneResponding();
    controller
        ->notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion();
}

void PassFailController::notifyThatCorrectButtonHasBeenClicked() {
    model.submitCorrectResponse();
    notifyThatUserIsDoneResponding(observer, controller);
}

void PassFailController::notifyThatIncorrectButtonHasBeenClicked() {
    model.submitIncorrectResponse();
    notifyThatUserIsDoneResponding(observer, controller);
}

void PassFailController::attach(TestController *p) { controller = p; }

PassFailPresenter::PassFailPresenter(
    TestView &experimenterView, PassFailView &view)
    : testView{experimenterView}, view{view} {}

void PassFailPresenter::start() { testView.showNextTrialButton(); }

void PassFailPresenter::stop() { view.hideEvaluationButtons(); }

void PassFailPresenter::notifyThatUserIsDoneResponding() {
    view.hideEvaluationButtons();
}

void PassFailPresenter::showResponseSubmission() {
    view.showEvaluationButtons();
}
}
