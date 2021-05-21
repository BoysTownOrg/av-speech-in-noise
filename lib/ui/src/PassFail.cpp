#include "PassFail.hpp"

namespace av_speech_in_noise {
PassFailController::PassFailController(
    TestController &testController, Model &model, PassFailControl &view)
    : testController{testController}, model{model} {
    view.attach(this);
}

static void notifyThatUserIsDoneResponding(TestController &controller) {
    controller
        .notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion();
}

void PassFailController::notifyThatCorrectButtonHasBeenClicked() {
    model.submitCorrectResponse();
    notifyThatUserIsDoneResponding(testController);
}

void PassFailController::notifyThatIncorrectButtonHasBeenClicked() {
    model.submitIncorrectResponse();
    notifyThatUserIsDoneResponding(testController);
}

PassFailPresenter::PassFailPresenter(
    TestView &experimenterView, PassFailView &view)
    : testView{experimenterView}, view{view} {}

void PassFailPresenter::start() { testView.showNextTrialButton(); }

void PassFailPresenter::stop() { view.hideEvaluationButtons(); }

void PassFailPresenter::hideResponseSubmission() {
    view.hideEvaluationButtons();
}

void PassFailPresenter::showResponseSubmission() {
    view.showEvaluationButtons();
}
}
