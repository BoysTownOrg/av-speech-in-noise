#include "ChooseKeywords.hpp"
#include <regex>
#include <utility>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iterator>

namespace av_speech_in_noise::submitting_keywords {
Controller::Controller(TestController &testController, Interactor &interactor,
    Control &control, Presenter &presenter)
    : testController{testController},
      interactor{interactor}, control{control}, presenter{presenter} {
    control.attach(this);
}

void Controller::notifyThatSubmitButtonHasBeenClicked() {
    ThreeKeywordsResponse threeKeywords{};
    threeKeywords.firstCorrect = control.firstKeywordCorrect();
    threeKeywords.secondCorrect = control.secondKeywordCorrect();
    threeKeywords.thirdCorrect = control.thirdKeywordCorrect();
    threeKeywords.flagged = control.flagged();
    interactor.submit(threeKeywords);
    testController.notifyThatUserIsDoneResponding();
}

void Controller::notifyThatAllWrongButtonHasBeenClicked() {
    presenter.markAllKeywordsIncorrect();
}

void Controller::notifyThatResetButtonIsClicked() {
    presenter.markAllKeywordsCorrect();
}

void Controller::notifyThatFirstKeywordButtonIsClicked() {
    presenter.markFirstKeywordIncorrect();
}

void Controller::notifyThatSecondKeywordButtonIsClicked() {
    presenter.markSecondKeywordIncorrect();
}

void Controller::notifyThatThirdKeywordButtonIsClicked() {
    presenter.markThirdKeywordIncorrect();
}

static void hideResponseSubmission(View &view) {
    view.hideResponseSubmission();
}

static auto withoutApostrophesNorPeriods(std::string s) -> std::string {
    s.erase(std::remove_if(s.begin(), s.end(),
                [](unsigned char x) { return x == '\'' || x == '.'; }),
        s.end());
    return s;
}

static auto transformToSentencesWithThreeKeywordsFromExpectedFileNameSentence(
    const std::vector<SentenceWithThreeKeywords> &sentencesWithThreeKeywords)
    -> std::map<std::string, SentenceWithThreeKeywords> {
    std::map<std::string, SentenceWithThreeKeywords> map;
    std::transform(sentencesWithThreeKeywords.begin(),
        sentencesWithThreeKeywords.end(), std::inserter(map, map.end()),
        [](const SentenceWithThreeKeywords &p) {
            return std::make_pair(withoutApostrophesNorPeriods(p.sentence), p);
        });
    return map;
}

PresenterImpl::PresenterImpl(RunningATestFacade &model,
    FixedLevelMethod &fixedLevelMethod, TestView &testView, View &view,
    const std::vector<SentenceWithThreeKeywords> &sentencesWithThreeKeywords)
    : sentencesWithThreeKeywordsFromExpectedFileNameSentence{transformToSentencesWithThreeKeywordsFromExpectedFileNameSentence(
          sentencesWithThreeKeywords)},
      model{model},
      fixedLevelMethod{fixedLevelMethod}, testView{testView}, view{view} {}

void PresenterImpl::start() { testView.showNextTrialButton(); }

void PresenterImpl::hideResponseSubmission() {
    submitting_keywords::hideResponseSubmission(view);
}

void PresenterImpl::stop() {
    submitting_keywords::hideResponseSubmission(view);
}

static auto mlstFileNameSentence(RunningATestFacade &model) -> std::string {
    std::stringstream stream{model.targetFileName()};
    int ignore = 0;
    stream >> ignore >> std::ws;
    std::string mlstFileNameSentence;
    std::getline(stream, mlstFileNameSentence, '.');
    return withoutApostrophesNorPeriods(mlstFileNameSentence);
}

void PresenterImpl::showResponseSubmission() {
    if (sentencesWithThreeKeywordsFromExpectedFileNameSentence.count(
            mlstFileNameSentence(model)) != 0)
        set(sentencesWithThreeKeywordsFromExpectedFileNameSentence.at(
            mlstFileNameSentence(model)));
    view.markFirstKeywordCorrect();
    view.markSecondKeywordCorrect();
    view.markThirdKeywordCorrect();
    view.clearFlag();
    view.showResponseSubmission();
}

// https://stackoverflow.com/a/65440575

// we cannot return a char array from a function, therefore we need a wrapper
template <unsigned N> struct String {
    char c[N];
};

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

void PresenterImpl::set(
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

void PresenterImpl::markFirstKeywordIncorrect() {
    view.markFirstKeywordIncorrect();
}

void PresenterImpl::markSecondKeywordIncorrect() {
    view.markSecondKeywordIncorrect();
}

void PresenterImpl::markThirdKeywordIncorrect() {
    view.markThirdKeywordIncorrect();
}

void PresenterImpl::markAllKeywordsIncorrect() {
    view.markFirstKeywordIncorrect();
    view.markSecondKeywordIncorrect();
    view.markThirdKeywordIncorrect();
}

void PresenterImpl::markAllKeywordsCorrect() {
    view.markFirstKeywordCorrect();
    view.markSecondKeywordCorrect();
    view.markThirdKeywordCorrect();
}

auto sentencesWithThreeKeywords(std::string_view s)
    -> std::vector<SentenceWithThreeKeywords> {
    std::vector<SentenceWithThreeKeywords> sentencesWithThreeKeywords;
    std::stringstream stream{std::string{s}};
    while (!stream.eof()) {
        std::string maybeSentence;
        std::getline(stream, maybeSentence);
        if (maybeSentence.empty())
            continue;
        SentenceWithThreeKeywords sentenceWithThreeKeywords;
        sentenceWithThreeKeywords.sentence = maybeSentence;
        std::getline(stream, sentenceWithThreeKeywords.firstKeyword);
        std::getline(stream, sentenceWithThreeKeywords.secondKeyword);
        std::getline(stream, sentenceWithThreeKeywords.thirdKeyword);
        sentencesWithThreeKeywords.push_back(sentenceWithThreeKeywords);
    }
    return sentencesWithThreeKeywords;
}

void PresenterImpl::complete() {
    const auto results{fixedLevelMethod.keywordsTestResults()};
    std::stringstream stream;
    stream << results.totalCorrect << " (" << std::setprecision(3)
           << results.percentCorrect << "%) keywords correct";
    testView.showSheet(stream.str());
}
}
