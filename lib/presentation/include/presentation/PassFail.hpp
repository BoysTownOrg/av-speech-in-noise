#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PASSFAIL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PASSFAIL_HPP_

#include "Task.hpp"
#include "Experimenter.hpp"
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
class PassFailControl {
  public:
    class Observer {
      public:
        virtual ~Observer() = default;
        virtual void notifyThatCorrectButtonHasBeenClicked() = 0;
        virtual void notifyThatIncorrectButtonHasBeenClicked() = 0;
    };
    virtual ~PassFailControl() = default;
    virtual void attach(Observer *) = 0;
};

class PassFailView {
  public:
    virtual ~PassFailView() = default;
    virtual void showEvaluationButtons() = 0;
    virtual void hideEvaluationButtons() = 0;
};

class PassFailController : public TaskController,
                          public PassFailControl::Observer {
  public:
    PassFailController(Model &, PassFailControl &);
    void attach(TaskController::Observer *) override;
    void attach(ExperimenterController *) override;
    void notifyThatCorrectButtonHasBeenClicked() override;
    void notifyThatIncorrectButtonHasBeenClicked() override;

  private:
    Model &model;
    TaskController::Observer *listener{};
    ExperimenterController *responder{};
};

class PassFailPresenter : public TaskPresenter {
  public:
    PassFailPresenter(ExperimenterView &, PassFailView &);
    void start() override;
    void stop() override;
    void notifyThatTaskHasStarted() override;
    void notifyThatUserIsDoneResponding() override;
    void showResponseSubmission() override;

  private:
    ExperimenterView &experimenterView;
    PassFailView &view;
};
}

#endif
