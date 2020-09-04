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
    parent->showContinueTestingDialogWithResultsWhenComplete();
}

void PassFailResponder::notifyThatIncorrectButtonHasBeenClicked() {
    model.submitIncorrectResponse();
    listener->notifyThatUserIsDoneResponding();
    parent->showContinueTestingDialogWithResultsWhenComplete();
}

void PassFailResponder::becomeChild(ParentPresenter *p) { parent = p; }

PassFailPresenter::PassFailPresenter(
    ExperimenterOutputView &experimenterView, PassFailOutputView &view)
    : experimenterView{experimenterView}, view{view} {}

void PassFailPresenter::start() {
    experimenterView.show();
    experimenterView.showNextTrialButton();
}

void PassFailPresenter::stop() {
    experimenterView.hide();
    view.hideEvaluationButtons();
}

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
