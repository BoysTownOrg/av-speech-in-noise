#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CONSONANTTASK_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CONSONANTTASK_HPP_

#include <av-speech-in-noise/Interface.hpp>
#include "View.hpp"
#include "Task.hpp"
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

class ConsonantTaskPresenter : public TaskPresenter {
  public:
    explicit ConsonantTaskPresenter(ConsonantTaskView &);
    void start() override;
    void stop() override;
    void notifyThatTaskHasStarted() override;
    void notifyThatUserIsDoneResponding() override;
    void notifyThatTrialHasStarted() override;
    void showResponseSubmission() override;

  private:
    ConsonantTaskView &view;
};

class ConsonantTaskController : public TaskController,
                                public ConsonantTaskControl::Observer {
  public:
    ConsonantTaskController(Model &, ConsonantTaskControl &);
    void attach(TaskController::Observer *) override;
    void attach(TestController *) override;
    void notifyThatReadyButtonHasBeenClicked() override;
    void notifyThatResponseButtonHasBeenClicked() override;

  private:
    Model &model;
    ConsonantTaskControl &control;
    TaskController::Observer *observer{};
    TestController *controller{};
};
}

#endif
