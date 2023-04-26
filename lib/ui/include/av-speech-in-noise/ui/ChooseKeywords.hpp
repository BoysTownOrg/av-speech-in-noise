#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CHOOSEKEYWORDS_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CHOOSEKEYWORDS_HPP_

#include "View.hpp"
#include "Task.hpp"
#include "Test.hpp"

#include <av-speech-in-noise/core/IFixedLevelMethod.hpp>
#include <av-speech-in-noise/core/IRecognitionTestModel.hpp>
#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/Interface.hpp>

#include <string>
#include <vector>
#include <map>
#include <string_view>

namespace av_speech_in_noise::submitting_keywords {
class View {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(View);
    virtual void hideResponseSubmission() = 0;
    virtual void showResponseSubmission() = 0;
    virtual void setFirstKeywordButtonText(const std::string &s) = 0;
    virtual void setSecondKeywordButtonText(const std::string &s) = 0;
    virtual void setThirdKeywordButtonText(const std::string &s) = 0;
    virtual void setTextPrecedingFirstKeywordButton(const std::string &s) = 0;
    virtual void setTextFollowingFirstKeywordButton(const std::string &s) = 0;
    virtual void setTextFollowingSecondKeywordButton(const std::string &s) = 0;
    virtual void setTextFollowingThirdKeywordButton(const std::string &s) = 0;
    virtual void markFirstKeywordIncorrect() = 0;
    virtual void markSecondKeywordIncorrect() = 0;
    virtual void markThirdKeywordIncorrect() = 0;
    virtual void markFirstKeywordCorrect() = 0;
    virtual void markSecondKeywordCorrect() = 0;
    virtual void markThirdKeywordCorrect() = 0;
    virtual void clearFlag() = 0;
};

class Control {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatSubmitButtonHasBeenClicked() = 0;
        virtual void notifyThatAllWrongButtonHasBeenClicked() = 0;
        virtual void notifyThatResetButtonIsClicked() = 0;
        virtual void notifyThatFirstKeywordButtonIsClicked() = 0;
        virtual void notifyThatSecondKeywordButtonIsClicked() = 0;
        virtual void notifyThatThirdKeywordButtonIsClicked() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Control);
    virtual void attach(Observer *) = 0;
    virtual auto flagged() -> bool = 0;
    virtual auto firstKeywordCorrect() -> bool = 0;
    virtual auto secondKeywordCorrect() -> bool = 0;
    virtual auto thirdKeywordCorrect() -> bool = 0;
};

class Presenter {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Presenter);
    virtual void markFirstKeywordIncorrect() = 0;
    virtual void markSecondKeywordIncorrect() = 0;
    virtual void markThirdKeywordIncorrect() = 0;
    virtual void markAllKeywordsIncorrect() = 0;
    virtual void markAllKeywordsCorrect() = 0;
};

class Controller : public Control::Observer {
  public:
    Controller(TestController &, Interactor &, Control &, Presenter &);
    void notifyThatSubmitButtonHasBeenClicked() override;
    void notifyThatAllWrongButtonHasBeenClicked() override;
    void notifyThatResetButtonIsClicked() override;
    void notifyThatFirstKeywordButtonIsClicked() override;
    void notifyThatSecondKeywordButtonIsClicked() override;
    void notifyThatThirdKeywordButtonIsClicked() override;

  private:
    TestController &testController;
    Interactor &interactor;
    Control &control;
    Presenter &presenter;
};

struct SentenceWithThreeKeywords {
    std::string sentence;
    std::string firstKeyword;
    std::string secondKeyword;
    std::string thirdKeyword;
};

auto sentencesWithThreeKeywords(std::string_view s)
    -> std::vector<SentenceWithThreeKeywords>;

static auto operator==(const SentenceWithThreeKeywords &a,
    const SentenceWithThreeKeywords &b) -> bool {
    return a.sentence == b.sentence && a.firstKeyword == b.firstKeyword &&
        a.secondKeyword == b.secondKeyword && a.thirdKeyword == b.thirdKeyword;
}

class PresenterImpl : public Presenter, public TaskPresenter {
  public:
    PresenterImpl(RunningATest &, FixedLevelMethod &, TestView &, View &,
        const std::vector<SentenceWithThreeKeywords> &);
    void start() override;
    void stop() override;
    void showResponseSubmission() override;
    void hideResponseSubmission() override;
    void set(const SentenceWithThreeKeywords &);
    void markFirstKeywordIncorrect() override;
    void markSecondKeywordIncorrect() override;
    void markThirdKeywordIncorrect() override;
    void markAllKeywordsIncorrect() override;
    void markAllKeywordsCorrect() override;
    void complete() override;

  private:
    std::map<std::string, SentenceWithThreeKeywords>
        sentencesWithThreeKeywordsFromExpectedFileNameSentence;
    RunningATest &model;
    FixedLevelMethod &fixedLevelMethod;
    TestView &testView;
    View &view;
};
}

#endif
