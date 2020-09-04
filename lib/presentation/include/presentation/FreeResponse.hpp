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
    explicit FreeResponseResponder(Model &, FreeResponseInputView &);
    void subscribe(TaskResponder::EventListener *e) override;
    void notifyThatSubmitButtonHasBeenClicked() override;
    void becomeChild(ParentPresenter *p) override;

  private:
    Model &model;
    FreeResponseInputView &view;
    TaskResponder::EventListener *listener{};
    ParentPresenter *parent{};
};

class FreeResponsePresenter : public TaskPresenter {
  public:
    explicit FreeResponsePresenter(
        ExperimenterOutputView &, FreeResponseOutputView &);
    void start() override;
    void stop() override;
    void notifyThatTaskHasStarted() override;
    void notifyThatUserIsDoneResponding() override;
    void showResponseSubmission() override;

  private:
    ExperimenterOutputView &experimenterView;
    FreeResponseOutputView &view;
};
}

#endif
