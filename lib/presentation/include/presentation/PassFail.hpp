#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PASSFAIL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PASSFAIL_HPP_

#include "Task.hpp"
#include "Experimenter.hpp"
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
class PassFailInputView {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void notifyThatCorrectButtonHasBeenClicked() = 0;
        virtual void notifyThatIncorrectButtonHasBeenClicked() = 0;
    };
    virtual ~PassFailInputView() = default;
    virtual void attach(EventListener *) = 0;
};

class PassFailOutputView {
  public:
    virtual ~PassFailOutputView() = default;
    virtual void showEvaluationButtons() = 0;
    virtual void hideEvaluationButtons() = 0;
};

class PassFailController : public TaskController,
                          public PassFailInputView::EventListener {
  public:
    PassFailController(Model &, PassFailInputView &);
    void attach(TaskController::EventListener *) override;
    void attach(ExperimenterController *) override;
    void notifyThatCorrectButtonHasBeenClicked() override;
    void notifyThatIncorrectButtonHasBeenClicked() override;

  private:
    Model &model;
    TaskController::EventListener *listener{};
    ExperimenterController *responder{};
};

class PassFailPresenter : public TaskPresenter {
  public:
    PassFailPresenter(ExperimenterOutputView &, PassFailOutputView &);
    void start() override;
    void stop() override;
    void notifyThatTaskHasStarted() override;
    void notifyThatUserIsDoneResponding() override;
    void showResponseSubmission() override;

  private:
    ExperimenterOutputView &experimenterView;
    PassFailOutputView &view;
};
}

#endif
