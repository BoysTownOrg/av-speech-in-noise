#include "CorrectKeywords.hpp"
#include "Input.hpp"

namespace av_speech_in_noise {
CorrectKeywordsController::CorrectKeywordsController(
    Model &model, SessionView &view, CorrectKeywordsControl &keywordsView)
    : model{model}, sessionView{view}, control{keywordsView} {
    keywordsView.attach(this);
}

void CorrectKeywordsController::attach(TaskController::Observer *e) {
    observer = e;
}

void CorrectKeywordsController::attach(TestController *r) { controller = r; }

void CorrectKeywordsController::notifyThatSubmitButtonHasBeenClicked() {
    try {
        model.submit(
            CorrectKeywords{readInteger(control.correctKeywords(), "number")});
        observer->notifyThatUserIsDoneResponding();
        controller
            ->notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion();
    } catch (const std::runtime_error &e) {
        sessionView.showErrorMessage(e.what());
    }
}

CorrectKeywordsPresenter::CorrectKeywordsPresenter(
    TestView &experimenterView, CorrectKeywordsView &view)
    : experimenterView{experimenterView}, view{view} {}

void CorrectKeywordsPresenter::start() {
    experimenterView.showNextTrialButton();
}

void CorrectKeywordsPresenter::stop() { view.hideCorrectKeywordsSubmission(); }

void CorrectKeywordsPresenter::notifyThatTaskHasStarted() {
    experimenterView.hideNextTrialButton();
}

void CorrectKeywordsPresenter::notifyThatUserIsDoneResponding() {
    view.hideCorrectKeywordsSubmission();
}

void CorrectKeywordsPresenter::showResponseSubmission() {
    view.showCorrectKeywordsSubmission();
}
}
