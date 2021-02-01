#include "ChooseKeywords.hpp"
#include <regex>
#include <utility>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iterator>

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
    threeKeywords.flagged = control.flagged();
    model.submit(threeKeywords);
    testController.notifyThatUserIsDoneResponding();
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

ChooseKeywordsPresenterImpl::ChooseKeywordsPresenterImpl(Model &model,
    TestView &testView, ChooseKeywordsView &view,
    const std::vector<SentenceWithThreeKeywords> &sentencesWithThreeKeywords)
    : sentencesWithThreeKeywordsFromExpectedFileNameSentence{transformToSentencesWithThreeKeywordsFromExpectedFileNameSentence(
          sentencesWithThreeKeywords)},
      model{model}, testView{testView}, view{view} {}

void ChooseKeywordsPresenterImpl::start() { testView.showNextTrialButton(); }

void ChooseKeywordsPresenterImpl::hideResponseSubmission() {
    av_speech_in_noise::hideResponseSubmission(view);
}

void ChooseKeywordsPresenterImpl::stop() {
    av_speech_in_noise::hideResponseSubmission(view);
}

static auto mlstFileNameSentence(Model &model) -> std::string {
    std::stringstream stream{model.targetFileName()};
    int ignore = 0;
    stream >> ignore >> std::ws;
    std::string mlstFileNameSentence;
    std::getline(stream, mlstFileNameSentence, '.');
    return mlstFileNameSentence;
}

void ChooseKeywordsPresenterImpl::showResponseSubmission() {
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

void ChooseKeywordsPresenterImpl::complete() {
    const auto results{model.keywordsTestResults()};
    std::stringstream stream;
    stream << results.totalCorrect << " (" << std::setprecision(3)
           << results.percentCorrect << "%) keywords correct";
    testView.showSheet(stream.str());
}
}
