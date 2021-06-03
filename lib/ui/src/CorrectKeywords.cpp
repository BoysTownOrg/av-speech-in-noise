#include "CorrectKeywords.hpp"
#include "Input.hpp"

namespace av_speech_in_noise::submitting_number_keywords {
Controller::Controller(TestController &testController, Model &model,
    SessionView &view, Control &keywordsView)
    : testController{testController}, model{model},
      sessionView{view}, control{keywordsView} {
    keywordsView.attach(this);
}

static void submitCorrectKeywords(
    Model &model, Control &control, TestController &controller) {
    model.submit(
        CorrectKeywords{readInteger(control.correctKeywords(), "number")});
    controller
        .notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion();
}

void Controller::notifyThatSubmitButtonHasBeenClicked() {
    try {
        submitCorrectKeywords(model, control, testController);
    } catch (const std::runtime_error &e) {
        sessionView.showErrorMessage(e.what());
    }
}

Presenter::Presenter(TestView &testView, View &view)
    : testView{testView}, view{view} {}

void Presenter::start() { testView.showNextTrialButton(); }

void Presenter::stop() { view.hide(); }

void Presenter::hideResponseSubmission() { view.hide(); }

void Presenter::showResponseSubmission() { view.show(); }
}
