#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CHOOSEKEYWORDS_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CHOOSEKEYWORDS_HPP_

#include "View.hpp"
#include "Task.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/Interface.hpp>

namespace av_speech_in_noise {
class ChooseKeywordsView {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(ChooseKeywordsView);
    virtual void hideResponseSubmission() = 0;
    virtual void showResponseSubmission() = 0;
};

class ChooseKeywordsControl {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatSubmitButtonHasBeenClicked() = 0;
        virtual void notifyThatAllWrongButtonHasBeenClicked() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(
        ChooseKeywordsControl);
    virtual void attach(Observer *) = 0;
    virtual auto firstKeywordCorrect() -> bool = 0;
    virtual auto secondKeywordCorrect() -> bool = 0;
    virtual auto thirdKeywordCorrect() -> bool = 0;
    virtual void markFirstKeywordIncorrect() = 0;
    virtual void markSecondKeywordIncorrect() = 0;
    virtual void markThirdKeywordIncorrect() = 0;
};

class ChooseKeywordsController : public TaskController,
                                 public ChooseKeywordsControl::Observer {
  public:
    ChooseKeywordsController(Model &, ChooseKeywordsControl &);
    void attach(TaskController::Observer *) override;
    void attach(TestController *) override;
    void notifyThatSubmitButtonHasBeenClicked() override;
    void notifyThatAllWrongButtonHasBeenClicked() override;

  private:
    ChooseKeywordsControl &control;
    Model &model;
};

class ChooseKeywordsPresenter : public TaskPresenter {
  public:
    ChooseKeywordsPresenter(TestView &, ChooseKeywordsView &);
    void start() override;
    void stop() override;
    void notifyThatTaskHasStarted() override;
    void notifyThatUserIsDoneResponding() override;
    void notifyThatTrialHasStarted() override;
    void showResponseSubmission() override;

  private:
    TestView &testView;
    ChooseKeywordsView &view;
};
}

#endif
