#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CONSONANTTASK_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CONSONANTTASK_HPP_

#include "Task.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class ConsonantTaskInputView {
  public:
    class Observer {
      public:
        virtual ~Observer() = default;
        virtual void notifyThatReadyButtonHasBeenClicked() = 0;
        virtual void notifyThatResponseButtonHasBeenClicked() = 0;
    };
    virtual ~ConsonantTaskInputView() = default;
    virtual void attach(Observer *) = 0;
    virtual auto consonant() -> std::string = 0;
};

class ConsonantTaskOutputView {
  public:
    virtual ~ConsonantTaskOutputView() = default;
    virtual void showCursor() = 0;
    virtual void hideCursor() = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void showReadyButton() = 0;
    virtual void hideReadyButton() = 0;
    virtual void hideResponseButtons() = 0;
    virtual void showResponseButtons() = 0;
};

class ConsonantTaskPresenter : public TaskPresenter {
  public:
    explicit ConsonantTaskPresenter(ConsonantTaskOutputView &);
    void start() override;
    void stop() override;
    void notifyThatTaskHasStarted() override;
    void notifyThatUserIsDoneResponding() override;
    void notifyThatTrialHasStarted() override;
    void showResponseSubmission() override;

  private:
    ConsonantTaskOutputView &view;
};

class ConsonantTaskController : public TaskController,
                                public ConsonantTaskInputView::Observer {
  public:
    explicit ConsonantTaskController(Model &, ConsonantTaskInputView &);
    void attach(TaskController::Observer *) override;
    void attach(ExperimenterController *) override;
    void notifyThatReadyButtonHasBeenClicked() override;
    void notifyThatResponseButtonHasBeenClicked() override;

  private:
    Model &model;
    ConsonantTaskInputView &view;
    TaskController::Observer *listener{};
    ExperimenterController *responder{};
};
}

#endif
