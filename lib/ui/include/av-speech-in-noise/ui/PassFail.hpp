#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PASSFAIL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PASSFAIL_HPP_

#include "Task.hpp"
#include "Test.hpp"
#include "View.hpp"

#include <av-speech-in-noise/core/IRunningATest.hpp>
#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>

#include <string>

namespace av_speech_in_noise::submitting_pass_fail {
class UI : public View {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatCorrectButtonHasBeenClicked() = 0;
        virtual void notifyThatIncorrectButtonHasBeenClicked() = 0;
        virtual void notifyThatShowAnswerButtonHasBeenClicked() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(UI);
    virtual void attach(Observer *) = 0;
    virtual void display(std::string) = 0;
    virtual void clearDisplay() = 0;
};

class Presenter : public TaskPresenter, public UI::Observer {
  public:
    Presenter(RunningATest &, TestController &, TestView &, Interactor &, UI &);
    void start() override;
    void stop() override;
    void hideResponseSubmission() override;
    void showResponseSubmission() override;
    void notifyThatCorrectButtonHasBeenClicked() override;
    void notifyThatIncorrectButtonHasBeenClicked() override;
    void notifyThatShowAnswerButtonHasBeenClicked() override;

  private:
    RunningATest &model;
    TestController &testController;
    Interactor &interactor;
    TestView &testView;
    UI &ui;
};
}

#endif
