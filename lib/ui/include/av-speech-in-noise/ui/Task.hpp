#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TASK_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TASK_HPP_

#include "Presenter.hpp"

#include <av-speech-in-noise/Interface.hpp>

namespace av_speech_in_noise {
class TaskController {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatTaskHasStarted() {}
        virtual void notifyThatTrialHasStarted() {}
    };
};

class TaskPresenter : public Presenter {
  public:
    virtual void showResponseSubmission() = 0;
    virtual void hideResponseSubmission() = 0;
    virtual void complete() {}
    virtual void notifyThatTrialHasStarted() {}
};
}

#endif
