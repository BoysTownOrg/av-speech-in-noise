#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CONSONANTTASK_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CONSONANTTASK_HPP_

#include "View.hpp"
#include "Task.hpp"
#include "Test.hpp"
#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class ConsonantTaskControl {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatReadyButtonHasBeenClicked() = 0;
        virtual void notifyThatResponseButtonHasBeenClicked() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(ConsonantTaskControl);
    virtual void attach(Observer *) = 0;
    virtual auto consonant() -> std::string = 0;
};

class ConsonantTaskView : public virtual View {
  public:
    virtual void showCursor() = 0;
    virtual void hideCursor() = 0;
    virtual void showReadyButton() = 0;
    virtual void hideReadyButton() = 0;
    virtual void hideResponseButtons() = 0;
    virtual void showResponseButtons() = 0;
};

class ConsonantTaskPresenter {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(
        ConsonantTaskPresenter);
    virtual void hideReadyButton() = 0;
};

class ConsonantTaskController : public ConsonantTaskControl::Observer {
  public:
    ConsonantTaskController(TestController &, Model &, ConsonantTaskControl &,
        ConsonantTaskPresenter &);
    void notifyThatReadyButtonHasBeenClicked() override;
    void notifyThatResponseButtonHasBeenClicked() override;

  private:
    TestController &testController;
    Model &model;
    ConsonantTaskControl &control;
    ConsonantTaskPresenter &presenter;
};

class ConsonantTaskPresenterImpl : public ConsonantTaskPresenter,
                                   public TaskPresenter {
  public:
    explicit ConsonantTaskPresenterImpl(ConsonantTaskView &);
    void start() override;
    void stop() override;
    void showResponseSubmission() override;
    void hideResponseSubmission() override;
    void hideReadyButton() override;
    void notifyThatTrialHasStarted() override;

  private:
    ConsonantTaskView &view;
};
}

#endif