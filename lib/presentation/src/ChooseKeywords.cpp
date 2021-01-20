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

// https://stackoverflow.com/a/65440575

// we cannot return a char array from a function, therefore we need a wrapper
template <unsigned N> struct String { char c[N]; };

template <unsigned... Len>
constexpr auto concatenate(const char (&...strings)[Len]) {
    constexpr auto N{(... + Len) - sizeof...(Len)};
    String<N + 1> result = {};
    result.c[N] = '\0';

    auto *dst{result.c};
    for (const auto *src : {strings...})
        for (; *src != '\0'; src++, dst++)
            *dst = *src;
    return result;
}

constexpr const char spaceIfPresent[]{" ?"};
constexpr const char captureAnythingLazily[]{"(.*?)"};
constexpr auto captureAnythingExceptTrailingSpace{
    concatenate(captureAnythingLazily, spaceIfPresent)};
constexpr auto captureAnythingExceptLeadingOrTrailingSpace{
    concatenate(spaceIfPresent, captureAnythingExceptTrailingSpace.c)};

void ChooseKeywordsPresenter::set(
    const SentenceWithThreeKeywords &sentenceWithThreeKeywords) {
    std::smatch match;
    std::regex_search(sentenceWithThreeKeywords.sentence, match,
        std::regex{captureAnythingExceptTrailingSpace.c +
            sentenceWithThreeKeywords.firstKeyword +
            captureAnythingExceptLeadingOrTrailingSpace.c +
            sentenceWithThreeKeywords.secondKeyword +
            captureAnythingExceptLeadingOrTrailingSpace.c +
            sentenceWithThreeKeywords.thirdKeyword +
            concatenate(spaceIfPresent, "(.*)").c});
    view.setTextPrecedingFirstKeywordButton(match[1]);
    view.setFirstKeywordButtonText(sentenceWithThreeKeywords.firstKeyword);
    view.setTextFollowingFirstKeywordButton(match[2]);
    view.setSecondKeywordButtonText(sentenceWithThreeKeywords.secondKeyword);
    view.setTextFollowingSecondKeywordButton(match[3]);
    view.setThirdKeywordButtonText(sentenceWithThreeKeywords.thirdKeyword);
    view.setTextFollowingThirdKeywordButton(match[4]);
}
}
