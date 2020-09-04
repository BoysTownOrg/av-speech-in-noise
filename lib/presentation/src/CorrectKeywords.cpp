#include "CorrectKeywords.hpp"
#include "Input.hpp"

namespace av_speech_in_noise {
CorrectKeywordsResponder::CorrectKeywordsResponder(
    Model &model, View &view, CorrectKeywordsInputView &keywordsView)
    : model{model}, view{view}, keywordsView{keywordsView} {
    keywordsView.subscribe(this);
}

void CorrectKeywordsResponder::subscribe(TaskResponder::EventListener *e) {
    listener = e;
}

void CorrectKeywordsResponder::subscribe(ExperimenterResponder *r) {
    responder = r;
}

void CorrectKeywordsResponder::notifyThatSubmitButtonHasBeenClicked() {
    try {
        CorrectKeywords p{};
        p.count = readInteger(keywordsView.correctKeywords(), "number");
        model.submit(p);
        listener->notifyThatUserIsDoneResponding();
        responder->showContinueTestingDialogWithResultsWhenComplete();
    } catch (const std::runtime_error &e) {
        view.showErrorMessage(e.what());
    }
}

CorrectKeywordsPresenter::CorrectKeywordsPresenter(
    ExperimenterOutputView &experimenterView, CorrectKeywordsOutputView &view)
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
