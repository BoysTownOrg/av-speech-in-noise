#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CHOOSEKEYWORDS_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CHOOSEKEYWORDS_HPP_

#include "View.hpp"
#include "Task.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <string>

namespace av_speech_in_noise {
class ChooseKeywordsView {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(ChooseKeywordsView);
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
};

class ChooseKeywordsControl {
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
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(
        ChooseKeywordsControl);
    virtual void attach(Observer *) = 0;
    virtual auto firstKeywordCorrect() -> bool = 0;
    virtual auto secondKeywordCorrect() -> bool = 0;
    virtual auto thirdKeywordCorrect() -> bool = 0;
};

class ChooseKeywordsPresenter {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(
        ChooseKeywordsPresenter);
    virtual void markFirstKeywordIncorrect() = 0;
    virtual void markSecondKeywordIncorrect() = 0;
    virtual void markThirdKeywordIncorrect() = 0;
    virtual void markFirstKeywordCorrect() = 0;
    virtual void markSecondKeywordCorrect() = 0;
    virtual void markThirdKeywordCorrect() = 0;
    virtual void hideResponseSubmission() = 0;
};

class ChooseKeywordsController : public TaskController,
                                 public ChooseKeywordsControl::Observer {
  public:
    ChooseKeywordsController(
        Model &, ChooseKeywordsControl &, ChooseKeywordsPresenter &);
    void attach(TestController *) override;
    void notifyThatSubmitButtonHasBeenClicked() override;
    void notifyThatAllWrongButtonHasBeenClicked() override;
    void notifyThatResetButtonIsClicked() override;
    void notifyThatFirstKeywordButtonIsClicked() override;
    void notifyThatSecondKeywordButtonIsClicked() override;
    void notifyThatThirdKeywordButtonIsClicked() override;

  private:
    ChooseKeywordsControl &control;
    Model &model;
    ChooseKeywordsPresenter &presenter;
    TestController *testController{};
};

struct SentenceWithThreeKeywords {
    std::string sentence;
    std::string firstKeyword;
    std::string secondKeyword;
    std::string thirdKeyword;
};

class ChooseKeywordsPresenterImpl : public ChooseKeywordsPresenter,
                                    public TaskPresenter {
  public:
    ChooseKeywordsPresenterImpl(TestView &, ChooseKeywordsView &);
    void start() override;
    void stop() override;
    void notifyThatTaskHasStarted() override;
    void notifyThatUserIsDoneResponding() override;
    void showResponseSubmission() override;
    void hideResponseSubmission() override;
    void set(const SentenceWithThreeKeywords &);
    void markFirstKeywordIncorrect() override;
    void markSecondKeywordIncorrect() override;
    void markThirdKeywordIncorrect() override;
    void markFirstKeywordCorrect() override;
    void markSecondKeywordCorrect() override;
    void markThirdKeywordCorrect() override;

  private:
    TestView &testView;
    ChooseKeywordsView &view;
};
}

#endif
