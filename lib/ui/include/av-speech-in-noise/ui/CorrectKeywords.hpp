#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CORRECTKEYWORDS_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CORRECTKEYWORDS_HPP_

#include "Task.hpp"
#include "Test.hpp"
#include "Session.hpp"
#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise::submitting_number_keywords {
class Control {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatSubmitButtonHasBeenClicked() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Control);
    virtual void attach(Observer *) = 0;
    virtual auto correctKeywords() -> std::string = 0;
};

class Controller : public TaskController, public Control::Observer {
  public:
    explicit Controller(TestController &, Model &, SessionView &, Control &);
    void attach(TestController *);
    void notifyThatSubmitButtonHasBeenClicked() override;

  private:
    TestController &testController;
    Model &model;
    SessionView &sessionView;
    Control &control;
};

class Presenter : public TaskPresenter {
  public:
    explicit Presenter(TestView &, View &);
    void start() override;
    void stop() override;
    void showResponseSubmission() override;
    void hideResponseSubmission() override;

  private:
    TestView &testView;
    View &view;
};
}

#endif
