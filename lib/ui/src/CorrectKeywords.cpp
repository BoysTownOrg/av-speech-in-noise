#include "CorrectKeywords.hpp"
#include "Input.hpp"

namespace av_speech_in_noise::submitting_number_keywords {
Controller::Controller(TestController &testController, Interactor &interactor,
    SessionView &view, Control &keywordsView)
    : testController{testController}, interactor{interactor}, sessionView{view},
      control{keywordsView} {
    keywordsView.attach(this);
}

static void submitCorrectKeywords(
    Interactor &interactor, Control &control, TestController &controller) {
    interactor.submit(
        CorrectKeywords{readInteger(control.correctKeywords(), "number")});
    controller
        .notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion();
}

void Controller::notifyThatSubmitButtonHasBeenClicked() {
    try {
        submitCorrectKeywords(interactor, control, testController);
    } catch (const std::runtime_error &e) {
        sessionView.showErrorMessage(e.what());
    }
}

Presenter::Presenter(ConfigurationRegistry &registry, TestView &testView,
    View &view, TestPresenter &testPresenter)
    : testView{testView}, view{view}, testPresenter{testPresenter} {
    registry.subscribe(*this, "method");
}

void Presenter::configure(const std::string &key, const std::string &value) {
    if (key == "method")
        if (contains(value, "number keywords"))
            testPresenter.apply(*this);
}

void Presenter::start() { testView.showNextTrialButton(); }

void Presenter::stop() { view.hide(); }

void Presenter::hideResponseSubmission() { view.hide(); }

void Presenter::showResponseSubmission() { view.show(); }
}
