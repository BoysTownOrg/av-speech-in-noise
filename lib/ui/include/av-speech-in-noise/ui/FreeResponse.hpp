#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_FREERESPONSE_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_FREERESPONSE_HPP_

#include "Task.hpp"
#include "Test.hpp"
#include "View.hpp"
#include <av-speech-in-noise/core/ITimer.hpp>
#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise::submitting_free_response {
class Control {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatSubmitButtonHasBeenClicked() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Control);
    virtual void attach(Observer *) = 0;
    virtual auto flagged() -> bool = 0;
    virtual auto response() -> std::string = 0;
};

class View : public av_speech_in_noise::View {
  public:
    virtual void clearResponse() = 0;
    virtual void clearFlag() = 0;
};

class Controller : public TaskController, public Control::Observer {
  public:
    Controller(TestController &, Interactor &, Control &);
    void notifyThatSubmitButtonHasBeenClicked() override;

  private:
    TestController &testController;
    Interactor &interactor;
    Control &control;
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

namespace with_puzzle {
class Puzzle {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Puzzle);
    virtual void reset() = 0;
    virtual void advance() = 0;
    virtual void show() {}
    virtual void hide() {}
};

class Controller : public TaskController,
                   public Control::Observer,
                   public Timer::Observer {
  public:
    Controller(TestController &, Interactor &, Control &, Puzzle &, Timer &);
    void notifyThatSubmitButtonHasBeenClicked() override;
    void callback() override;

  private:
    TestController &testController;
    Interactor &interactor;
    Control &control;
    Puzzle &puzzle;
    Timer &timer;
    bool readyToAdvancePuzzle_{};
};

class Presenter : public TaskPresenter {
  public:
    Presenter(TestView &, View &, Puzzle &);
    void start() override;
    void stop() override;
    void hideResponseSubmission() override;
    void showResponseSubmission() override;

  private:
    TestView &testView;
    View &view;
    Puzzle &puzzle;
};
}
}

#endif
