#include "PassFail.hpp"

namespace av_speech_in_noise::submitting_pass_fail {
Presenter::Presenter(ConfigurationRegistry &registry,
    TestController &testController, TestView &testView, Interactor &interactor,
    UI &ui, TestPresenter &testPresenter)
    : testController{testController}, interactor{interactor},
      testView{testView}, ui{ui}, testPresenter{testPresenter} {
    registry.subscribe(*this, "method");
}

static void notifyThatUserIsDoneResponding(TestController &controller) {
    controller
        .notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion();
}

void Presenter::notifyThatCorrectButtonHasBeenClicked() {
    interactor.submitCorrectResponse();
    notifyThatUserIsDoneResponding(testController);
}

void Presenter::notifyThatIncorrectButtonHasBeenClicked() {
    interactor.submitIncorrectResponse();
    notifyThatUserIsDoneResponding(testController);
}

void Presenter::start() {
    ui.attach(this);
    testView.showNextTrialButton();
}

void Presenter::stop() { ui.hide(); }

void Presenter::hideResponseSubmission() { ui.hide(); }

void Presenter::showResponseSubmission() { ui.show(); };

void Presenter::configure(const std::string &key, const std::string &value) {
    if (key == "method")
        if (contains(value, "adaptive pass fail"))
            testPresenter.initialize(*this);
}
}
