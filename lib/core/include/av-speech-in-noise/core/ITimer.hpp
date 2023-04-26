
#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_ITIMER_HPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_ITIMER_HPP_

#include <av-speech-in-noise/Interface.hpp>

namespace av_speech_in_noise {
class Timer {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void callback() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Timer);
    virtual void attach(Observer *) = 0;
    virtual void scheduleCallbackAfterSeconds(double) = 0;
};
}

#endif
