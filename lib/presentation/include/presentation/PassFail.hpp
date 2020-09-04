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
    virtual void subscribe(EventListener *) = 0;
};

class PassFailOutputView {
  public:
    virtual ~PassFailOutputView() = default;
    virtual void showEvaluationButtons() = 0;
    virtual void hideEvaluationButtons() = 0;
};

class PassFailResponder : public TaskResponder,
                          public PassFailInputView::EventListener {
  public:
    explicit PassFailResponder(Model &, PassFailInputView &);
    void subscribe(TaskResponder::EventListener *e) override;
    void notifyThatCorrectButtonHasBeenClicked() override;
    void notifyThatIncorrectButtonHasBeenClicked() override;
    void becomeChild(ParentPresenter *p) override;

  private:
    Model &model;
    TaskResponder::EventListener *listener{};
    ParentPresenter *parent{};
};

class PassFailPresenter : public TaskPresenter {
  public:
    explicit PassFailPresenter(ExperimenterOutputView &, PassFailOutputView &);
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
