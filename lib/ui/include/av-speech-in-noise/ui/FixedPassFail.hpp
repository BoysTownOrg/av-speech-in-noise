#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_FIXEDPASSFAIL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_FIXEDPASSFAIL_HPP_

#include "Task.hpp"
#include "Test.hpp"
#include "View.hpp"

#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise::submitting_fixed_pass_fail {
class UI : public View {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatCorrectButtonHasBeenClicked() = 0;
        virtual void notifyThatIncorrectButtonHasBeenClicked() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(UI);
    virtual void attach(Observer *) = 0;
};

class Presenter : public TaskPresenter, public UI::Observer {
  public:
    Presenter(TestController &, TestView &, Interactor &, UI &);
    void notifyThatCorrectButtonHasBeenClicked() override;
    void notifyThatIncorrectButtonHasBeenClicked() override;
    void start() override;
    void stop() override;
    void hideResponseSubmission() override;
    void showResponseSubmission() override;

  private:
    TestController &testController;
    Interactor &interactor;
    TestView &testView;
    UI &ui;
};
}

#endif
