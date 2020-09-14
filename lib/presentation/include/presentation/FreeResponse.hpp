#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_FREERESPONSE_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_FREERESPONSE_HPP_

#include "Task.hpp"
#include "Experimenter.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class FreeResponseControl {
  public:
    class Observer {
      public:
        virtual ~Observer() = default;
        virtual void notifyThatSubmitButtonHasBeenClicked() = 0;
    };
    virtual ~FreeResponseControl() = default;
    virtual void attach(Observer *) = 0;
    virtual auto flagged() -> bool = 0;
    virtual auto freeResponse() -> std::string = 0;
};

class FreeResponseView {
  public:
    virtual ~FreeResponseView() = default;
    virtual void showFreeResponseSubmission() = 0;
    virtual void hideFreeResponseSubmission() = 0;
    virtual void clearFreeResponse() = 0;
};

class FreeResponseController : public TaskController,
                               public FreeResponseControl::Observer {
  public:
    FreeResponseController(Model &, FreeResponseControl &);
    void attach(TaskController::Observer *) override;
    void attach(TestController *) override;
    void notifyThatSubmitButtonHasBeenClicked() override;

  private:
    Model &model;
    FreeResponseControl &view;
    TaskController::Observer *listener{};
    TestController *responder{};
};

class FreeResponsePresenter : public TaskPresenter {
  public:
    FreeResponsePresenter(ExperimenterView &, FreeResponseView &);
    void start() override;
    void stop() override;
    void notifyThatTaskHasStarted() override;
    void notifyThatUserIsDoneResponding() override;
    void showResponseSubmission() override;

  private:
    ExperimenterView &experimenterView;
    FreeResponseView &view;
};
}

#endif
