#include "ChooseKeywords.hpp"

namespace av_speech_in_noise {
ChooseKeywordsPresenter::ChooseKeywordsPresenter(
    TestView &testView, ChooseKeywordsView &view)
    : testView{testView}, view{view} {}

void ChooseKeywordsPresenter::start() {}

void ChooseKeywordsPresenter::stop() {}

void ChooseKeywordsPresenter::notifyThatTaskHasStarted() {
    testView.hideNextTrialButton();
}

void ChooseKeywordsPresenter::notifyThatUserIsDoneResponding() {}

void ChooseKeywordsPresenter::notifyThatTrialHasStarted() {}

void ChooseKeywordsPresenter::showResponseSubmission() {}
}
