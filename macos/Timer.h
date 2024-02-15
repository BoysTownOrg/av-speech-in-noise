#ifndef AV_SPEECH_IN_NOISE_MACOS_TIMER_H_
#define AV_SPEECH_IN_NOISE_MACOS_TIMER_H_

#include <av-speech-in-noise/core/ITimer.hpp>

@class CallbackScheduler;

namespace av_speech_in_noise {
class TimerImpl : public Timer {
  public:
    TimerImpl();
    void attach(Observer *e) override;
    void scheduleCallbackAfterSeconds(double x) override;
    void timerCallback();

  private:
    Observer *listener{};
    CallbackScheduler *scheduler;
};
}

#endif
