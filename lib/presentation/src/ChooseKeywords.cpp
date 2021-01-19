#include "ChooseKeywords.hpp"

namespace av_speech_in_noise {
ChooseKeywordsController::ChooseKeywordsController(
    Model &model, ChooseKeywordsControl &control)
    : control{control}, model{model} {
    control.attach(this);
}

void ChooseKeywordsController::attach(TaskController::Observer *) {}

void ChooseKeywordsController::attach(TestController *) {}

void ChooseKeywordsController::notifyThatSubmitButtonHasBeenClicked() {
    ThreeKeywords threeKeywords{};
    threeKeywords.firstCorrect = control.firstKeywordCorrect();
    threeKeywords.secondCorrect = control.secondKeywordCorrect();
    threeKeywords.thirdCorrect = control.thirdKeywordCorrect();
    model.submit(threeKeywords);
}

void ChooseKeywordsController::notifyThatAllWrongButtonHasBeenClicked() {
    control.markFirstKeywordIncorrect();
    control.markSecondKeywordIncorrect();
    control.markThirdKeywordIncorrect();
}

static void hideResponseSubmission(ChooseKeywordsView &view) {
    view.hideResponseSubmission();
}

ChooseKeywordsPresenter::ChooseKeywordsPresenter(
    TestView &testView, ChooseKeywordsView &view)
    : testView{testView}, view{view} {}

void ChooseKeywordsPresenter::start() { testView.showNextTrialButton(); }

void ChooseKeywordsPresenter::stop() { hideResponseSubmission(view); }

void ChooseKeywordsPresenter::notifyThatTaskHasStarted() {
    testView.hideNextTrialButton();
}

void ChooseKeywordsPresenter::notifyThatUserIsDoneResponding() {
    hideResponseSubmission(view);
}

void ChooseKeywordsPresenter::notifyThatTrialHasStarted() {}

void ChooseKeywordsPresenter::showResponseSubmission() {
    view.showResponseSubmission();
}
}
