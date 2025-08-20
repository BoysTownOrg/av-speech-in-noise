#include "FixedPassFail.hpp"

namespace av_speech_in_noise::submitting_fixed_pass_fail {
Presenter::Presenter(ConfigurationRegistry &registry,
    TestController &testController, TestView &testView, Interactor &interactor,
    submitting_pass_fail::UI &ui, TestPresenter &testPresenter,
    TaskPresenter &keypressPresenter)
    : testController{testController}, interactor{interactor},
      testView{testView}, ui{ui}, testPresenter{testPresenter},
      keypressPresenter{keypressPresenter} {
    registry.subscribe(*this, "method");
}

void Presenter::notifyThatCorrectButtonHasBeenClicked() {
    interactor.submitCorrectResponse();
    testController.notifyThatUserIsDoneResponding();
}

void Presenter::notifyThatIncorrectButtonHasBeenClicked() {
    interactor.submitIncorrectResponse();
    testController.notifyThatUserIsDoneResponding();
}

void Presenter::start() { testView.showNextTrialButton(); }

void Presenter::stop() { ui.hide(); }

void Presenter::hideResponseSubmission() { ui.hide(); }

void Presenter::showResponseSubmission() {
    ui.attach(this);
    ui.show();
}

void Presenter::configure(const std::string &key, const std::string &value) {
    if (key == "method") {
        if (contains(value, "fixed-level pass fail"))
            testPresenter.initialize(*this);
        else if (contains(value, "fixed-level button response pass fail"))
            keypressPresenter.enableDualTask(this);
    }
}
}
