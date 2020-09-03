#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_FREERESPONSE_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_FREERESPONSE_HPP_

#include "Task.hpp"
#include "Experimenter.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class FreeResponseInputView {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void notifyThatSubmitButtonHasBeenClicked() = 0;
    };
    virtual ~FreeResponseInputView() = default;
    virtual void subscribe(EventListener *) = 0;
    virtual auto flagged() -> bool = 0;
    virtual auto freeResponse() -> std::string = 0;
};

class FreeResponseOutputView {
  public:
    virtual ~FreeResponseOutputView() = default;
    virtual void showFreeResponseSubmission() = 0;
    virtual void hideFreeResponseSubmission() = 0;
    virtual void clearFreeResponse() = 0;
};

class FreeResponseResponder : public TaskResponder,
                              public FreeResponseInputView::EventListener {
  public:
    explicit FreeResponseResponder(Model &model, FreeResponseInputView &view)
        : model{model}, view{view} {
        view.subscribe(this);
    }
    void subscribe(TaskResponder::EventListener *e) override { listener = e; }
    void notifyThatSubmitButtonHasBeenClicked() override {
        model.submit(FreeResponse{view.freeResponse(), view.flagged()});
        listener->notifyThatUserIsDoneResponding();
        parent->readyNextTrialIfNeeded();
    }
    void becomeChild(ParentPresenter *p) override { parent = p; }

  private:
    Model &model;
    FreeResponseInputView &view;
    TaskResponder::EventListener *listener{};
    ParentPresenter *parent{};
};

class FreeResponsePresenter : public TaskPresenter {
  public:
    explicit FreeResponsePresenter(
        ExperimenterView &experimenterView, FreeResponseOutputView &view)
        : experimenterView{experimenterView}, view{view} {}
    void start() override {
        experimenterView.show();
        experimenterView.showNextTrialButton();
    }
    void stop() override {
        experimenterView.hide();
        view.hideFreeResponseSubmission();
    }
    void notifyThatTaskHasStarted() override {
        experimenterView.hideNextTrialButton();
    }
    void notifyThatUserIsDoneResponding() override {
        view.hideFreeResponseSubmission();
    }
    void showResponseSubmission() override {
        view.clearFreeResponse();
        view.showFreeResponseSubmission();
    }

  private:
    ExperimenterView &experimenterView;
    FreeResponseOutputView &view;
};
}

#endif
