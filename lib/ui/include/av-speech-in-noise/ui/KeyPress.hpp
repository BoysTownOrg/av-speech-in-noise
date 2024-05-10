#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_KEYPRESS_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_KEYPRESS_HPP_

#include "Task.hpp"
#include "Test.hpp"

#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise::submitting_keypress {
class Control {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatKeyHasBeenPressed() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Control);
    virtual void attach(Observer *) = 0;
};

class Controller : public Control::Observer {
  public:
    Controller(TestController &, Interactor &, Control &);
    void notifyThatKeyHasBeenPressed() override;

  private:
    TestController &testController;
    Interactor &interactor;
    Control &control;
};

class Presenter : public TaskPresenter {
  public:
    explicit Presenter(TestView &);
    void start() override;
    void stop() override;
    void showResponseSubmission() override;
    void hideResponseSubmission() override;
    void notifyThatTrialHasStarted() override;

  private:
    TestView &testView;
};
}

#endif
