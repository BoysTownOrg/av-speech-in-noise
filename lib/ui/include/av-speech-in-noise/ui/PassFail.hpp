#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PASSFAIL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PASSFAIL_HPP_

#include "Task.hpp"
#include "Test.hpp"
#include "View.hpp"
#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise::submitting_pass_fail {
class Control {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatCorrectButtonHasBeenClicked() = 0;
        virtual void notifyThatIncorrectButtonHasBeenClicked() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Control);
    virtual void attach(Observer *) = 0;
};

class Controller : public TaskController, public Control::Observer {
  public:
    Controller(TestController &, Model &, Control &);
    void notifyThatCorrectButtonHasBeenClicked() override;
    void notifyThatIncorrectButtonHasBeenClicked() override;

  private:
    TestController &testController;
    Model &model;
};

class Presenter : public TaskPresenter {
  public:
    Presenter(TestView &, View &);
    void start() override;
    void stop() override;
    void hideResponseSubmission() override;
    void showResponseSubmission() override;

  private:
    TestView &testView;
    View &view;
};
}

#endif
