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

static void submitCorrectKeywords(
    Model &model, CorrectKeywordsControl &control, TestController *controller) {
    model.submit(
        CorrectKeywords{readInteger(control.correctKeywords(), "number")});
    controller
        ->notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion();
}

void CorrectKeywordsController::notifyThatSubmitButtonHasBeenClicked() {
    try {
        submitCorrectKeywords(model, control, controller);
    } catch (const std::runtime_error &e) {
        sessionView.showErrorMessage(e.what());
    }
}

CorrectKeywordsPresenter::CorrectKeywordsPresenter(
    TestView &testView, CorrectKeywordsView &view)
    : testView{testView}, view{view} {}

void CorrectKeywordsPresenter::start() { testView.showNextTrialButton(); }

void CorrectKeywordsPresenter::stop() { view.hideCorrectKeywordsSubmission(); }

void CorrectKeywordsPresenter::hideResponseSubmission() {
    view.hideCorrectKeywordsSubmission();
}

void CorrectKeywordsPresenter::showResponseSubmission() {
    view.showCorrectKeywordsSubmission();
}
}
