#include "ChooseKeywords.hpp"
#include <regex>

namespace av_speech_in_noise {
ChooseKeywordsController::ChooseKeywordsController(
    TestController &testController, Model &model,
    ChooseKeywordsControl &control, ChooseKeywordsPresenter &presenter)
    : testController{testController}, model{model}, control{control},
      presenter{presenter} {
    control.attach(this);
}

void ChooseKeywordsController::notifyThatSubmitButtonHasBeenClicked() {
    ThreeKeywordsResponse threeKeywords{};
    threeKeywords.firstCorrect = control.firstKeywordCorrect();
    threeKeywords.secondCorrect = control.secondKeywordCorrect();
    threeKeywords.thirdCorrect = control.thirdKeywordCorrect();
    model.submit(threeKeywords);
    testController.notifyThatUserIsDoneResponding();
    presenter.hideResponseSubmission();
}

void ChooseKeywordsController::notifyThatAllWrongButtonHasBeenClicked() {
    presenter.markAllKeywordsIncorrect();
}

void ChooseKeywordsController::notifyThatResetButtonIsClicked() {
    presenter.markAllKeywordsCorrect();
}

void ChooseKeywordsController::notifyThatFirstKeywordButtonIsClicked() {
    presenter.markFirstKeywordIncorrect();
}

void ChooseKeywordsController::notifyThatSecondKeywordButtonIsClicked() {
    presenter.markSecondKeywordIncorrect();
}

void ChooseKeywordsController::notifyThatThirdKeywordButtonIsClicked() {
    presenter.markThirdKeywordIncorrect();
}

static void hideResponseSubmission(ChooseKeywordsView &view) {
    view.hideResponseSubmission();
}

ChooseKeywordsPresenterImpl::ChooseKeywordsPresenterImpl(
    TestView &testView, ChooseKeywordsView &view)
    : testView{testView}, view{view} {}

void ChooseKeywordsPresenterImpl::start() { testView.showNextTrialButton(); }

void ChooseKeywordsPresenterImpl::hideResponseSubmission() {
    view.hideResponseSubmission();
}

void ChooseKeywordsPresenterImpl::stop() {
    av_speech_in_noise::hideResponseSubmission(view);
}

void ChooseKeywordsPresenterImpl::showResponseSubmission() {
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

void ChooseKeywordsPresenterImpl::set(
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

void ChooseKeywordsPresenterImpl::markFirstKeywordIncorrect() {
    view.markFirstKeywordIncorrect();
}

void ChooseKeywordsPresenterImpl::markSecondKeywordIncorrect() {
    view.markSecondKeywordIncorrect();
}

void ChooseKeywordsPresenterImpl::markThirdKeywordIncorrect() {
    view.markThirdKeywordIncorrect();
}

void ChooseKeywordsPresenterImpl::markAllKeywordsIncorrect() {
    view.markFirstKeywordIncorrect();
    view.markSecondKeywordIncorrect();
    view.markThirdKeywordIncorrect();
}

void ChooseKeywordsPresenterImpl::markAllKeywordsCorrect() {
    view.markFirstKeywordCorrect();
    view.markSecondKeywordCorrect();
    view.markThirdKeywordCorrect();
}
}
