#include "PassFail.hpp"

namespace av_speech_in_noise {
PassFailResponder::PassFailResponder(Model &model, PassFailInputView &view)
    : model{model} {
    view.subscribe(this);
}

void PassFailResponder::subscribe(TaskResponder::EventListener *e) {
    listener = e;
}

void PassFailResponder::notifyThatCorrectButtonHasBeenClicked() {
    model.submitCorrectResponse();
    listener->notifyThatUserIsDoneResponding();
    responder->showContinueTestingDialogWithResultsWhenComplete();
}

void PassFailResponder::notifyThatIncorrectButtonHasBeenClicked() {
    model.submitIncorrectResponse();
    listener->notifyThatUserIsDoneResponding();
    responder->showContinueTestingDialogWithResultsWhenComplete();
}

void PassFailResponder::subscribe(ExperimenterResponder *p) { responder = p; }

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
