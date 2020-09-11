#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TASK_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TASK_HPP_

#include "PresenterSimple.hpp"
#include "Experimenter.hpp"

namespace av_speech_in_noise {
class TaskController {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void notifyThatTaskHasStarted() = 0;
        virtual void notifyThatUserIsDoneResponding() = 0;
        virtual void notifyThatTrialHasStarted() {}
    };
    virtual ~TaskController() = default;
    virtual void attach(EventListener *) = 0;
    virtual void attach(ExperimenterController *) = 0;
};

class TaskPresenter : virtual public TaskController::EventListener,
                      virtual public PresenterSimple {
  public:
    virtual void showResponseSubmission() = 0;
};
}

#endif
