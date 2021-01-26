#include "Syllables.hpp"

namespace av_speech_in_noise {
SyllablesController::SyllablesController(
    SyllablesControl &control, TestController &testController, Model &model)
    : control{control}, testController{testController}, model{model} {
    control.attach(this);
}

void SyllablesController::notifyThatSubmitButtonHasBeenClicked() {
    model.submit(SyllableResponse{syllable(control.syllable())});
    testController.notifyThatUserIsDoneResponding();
}

SyllablesPresenterImpl::SyllablesPresenterImpl(
    SyllablesView &view, TestView &testView)
    : view{view}, testView{testView} {}

void SyllablesPresenterImpl::start() { testView.showNextTrialButton(); }

void SyllablesPresenterImpl::stop() { view.hide(); }

void SyllablesPresenterImpl::showResponseSubmission() { view.show(); }

void SyllablesPresenterImpl::hideResponseSubmission() { view.hide(); }
}
