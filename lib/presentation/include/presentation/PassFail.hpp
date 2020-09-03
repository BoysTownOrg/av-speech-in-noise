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
    explicit PassFailResponder(Model &model, PassFailInputView &view)
        : model{model} {
        view.subscribe(this);
    }
    void subscribe(TaskResponder::EventListener *e) override { listener = e; }
    void notifyThatCorrectButtonHasBeenClicked() override {
        model.submitCorrectResponse();
        listener->notifyThatUserIsDoneResponding();
        parent->showContinueTestingDialogWithResultsWhenComplete();
    }
    void notifyThatIncorrectButtonHasBeenClicked() override {
        model.submitIncorrectResponse();
        listener->notifyThatUserIsDoneResponding();
        parent->showContinueTestingDialogWithResultsWhenComplete();
    }
    void becomeChild(ParentPresenter *p) override { parent = p; }

  private:
    Model &model;
    TaskResponder::EventListener *listener{};
    ParentPresenter *parent{};
};

class PassFailPresenter : public TaskPresenter {
  public:
    explicit PassFailPresenter(
        ExperimenterView &experimenterView, PassFailOutputView &view)
        : experimenterView{experimenterView}, view{view} {}
    void start() override {
        experimenterView.show();
        experimenterView.showNextTrialButton();
    }
    void stop() override {
        experimenterView.hide();
        view.hideEvaluationButtons();
    }
    void notifyThatTaskHasStarted() override {
        experimenterView.hideNextTrialButton();
    }
    void notifyThatUserIsDoneResponding() override {
        view.hideEvaluationButtons();
    }
    void showResponseSubmission() override { view.showEvaluationButtons(); }

  private:
    ExperimenterView &experimenterView;
    PassFailOutputView &view;
};
}

#endif
