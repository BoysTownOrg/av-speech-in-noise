#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_FREERESPONSE_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_FREERESPONSE_HPP_

#include "Task.hpp"
#include "Experimenter.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class FreeResponseInputView {
  public:
    class Observer {
      public:
        virtual ~Observer() = default;
        virtual void notifyThatSubmitButtonHasBeenClicked() = 0;
    };
    virtual ~FreeResponseInputView() = default;
    virtual void attach(Observer *) = 0;
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

class FreeResponseController : public TaskController,
                              public FreeResponseInputView::Observer {
  public:
    FreeResponseController(Model &, FreeResponseInputView &);
    void attach(TaskController::Observer *) override;
    void attach(ExperimenterController *) override;
    void notifyThatSubmitButtonHasBeenClicked() override;

  private:
    Model &model;
    FreeResponseInputView &view;
    TaskController::Observer *listener{};
    ExperimenterController *responder{};
};

class FreeResponsePresenter : public TaskPresenter {
  public:
    FreeResponsePresenter(ExperimenterOutputView &, FreeResponseOutputView &);
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
