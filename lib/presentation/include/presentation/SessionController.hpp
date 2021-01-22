#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_METHOD_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_METHOD_HPP_

#include "Task.hpp"
#include <av-speech-in-noise/Interface.hpp>

namespace av_speech_in_noise {
class SessionController {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatTestIsComplete() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(SessionController);
    virtual void notifyThatTestIsComplete() = 0;
    virtual void prepare(TaskPresenter &) = 0;
    virtual void attach(Observer *) {}
};
}

#endif
