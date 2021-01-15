#include "ChooseKeywords.hpp"

namespace av_speech_in_noise {
static void hideResponseSubmission(ChooseKeywordsView &view) {
    view.hideResponseSubmission();
}

ChooseKeywordsPresenter::ChooseKeywordsPresenter(
    TestView &testView, ChooseKeywordsView &view)
    : testView{testView}, view{view} {}

void ChooseKeywordsPresenter::start() {}

void ChooseKeywordsPresenter::stop() { hideResponseSubmission(view); }

void ChooseKeywordsPresenter::notifyThatTaskHasStarted() {
    testView.hideNextTrialButton();
}

void ChooseKeywordsPresenter::notifyThatUserIsDoneResponding() {
    hideResponseSubmission(view);
}

void ChooseKeywordsPresenter::notifyThatTrialHasStarted() {}

void ChooseKeywordsPresenter::showResponseSubmission() {}
}
