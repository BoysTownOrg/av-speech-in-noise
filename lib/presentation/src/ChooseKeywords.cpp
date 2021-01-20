#include "ChooseKeywords.hpp"
#include <regex>

namespace av_speech_in_noise {
ChooseKeywordsController::ChooseKeywordsController(
    Model &model, ChooseKeywordsControl &control)
    : control{control}, model{model} {
    control.attach(this);
}

void ChooseKeywordsController::attach(TaskController::Observer *t) {
    taskControllerObserver = t;
}

void ChooseKeywordsController::attach(TestController *t) { testController = t; }

void ChooseKeywordsController::notifyThatSubmitButtonHasBeenClicked() {
    ThreeKeywordsResponse threeKeywords{};
    threeKeywords.firstCorrect = control.firstKeywordCorrect();
    threeKeywords.secondCorrect = control.secondKeywordCorrect();
    threeKeywords.thirdCorrect = control.thirdKeywordCorrect();
    model.submit(threeKeywords);
    testController->notifyThatUserIsDoneResponding();
    taskControllerObserver->notifyThatUserIsDoneResponding();
}

void ChooseKeywordsController::notifyThatAllWrongButtonHasBeenClicked() {
    control.markFirstKeywordIncorrect();
    control.markSecondKeywordIncorrect();
    control.markThirdKeywordIncorrect();
}

void ChooseKeywordsController::notifyThatResetButtonIsClicked() {
    control.markFirstKeywordCorrect();
    control.markSecondKeywordCorrect();
    control.markThirdKeywordCorrect();
}

void ChooseKeywordsController::notifyThatFirstKeywordButtonIsClicked() {
    control.markFirstKeywordIncorrect();
}

void ChooseKeywordsController::notifyThatSecondKeywordButtonIsClicked() {
    control.markSecondKeywordIncorrect();
}

void ChooseKeywordsController::notifyThatThirdKeywordButtonIsClicked() {
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

void ChooseKeywordsPresenter::showResponseSubmission() {
    view.showResponseSubmission();
}

void ChooseKeywordsPresenter::set(
    const SentenceWithThreeKeywords &sentenceWithThreeKeywords) {
    std::regex pattern{"(.*?) ?" + sentenceWithThreeKeywords.firstKeyword +
        " ?(.*?) ?" + sentenceWithThreeKeywords.secondKeyword + " ?(.*?) ?" +
        sentenceWithThreeKeywords.thirdKeyword + " ?(.*)"};
    std::smatch match;
    std::regex_search(sentenceWithThreeKeywords.sentence, match, pattern);
    view.setTextPrecedingFirstKeywordButton(match[1]);
    view.setFirstKeywordButtonText(sentenceWithThreeKeywords.firstKeyword);
    view.setTextFollowingFirstKeywordButton(match[2]);
    view.setSecondKeywordButtonText(sentenceWithThreeKeywords.secondKeyword);
    view.setTextFollowingSecondKeywordButton(match[3]);
    view.setThirdKeywordButtonText(sentenceWithThreeKeywords.thirdKeyword);
    view.setTextFollowingThirdKeywordButton(match[4]);
}
}
