#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TASK_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TASK_HPP_

#include "Presenter.hpp"
#include "Test.hpp"

namespace av_speech_in_noise {
class TaskController {
  public:
    class Observer {
      public:
        virtual ~Observer() = default;
        virtual void notifyThatTaskHasStarted() {}
        virtual void notifyThatUserIsDoneResponding() {}
        virtual void notifyThatTrialHasStarted() {}
    };
    virtual ~TaskController() = default;
};

class TaskPresenter : virtual public TaskController::Observer,
                      virtual public Presenter {
  public:
    virtual void showResponseSubmission() = 0;
};
}

#endif
