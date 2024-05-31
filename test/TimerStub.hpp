#ifndef AV_SPEECH_IN_NOISE_TESTS_TIMERSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TESTS_TIMERSTUB_HPP_

#include <av-speech-in-noise/core/ITimer.hpp>

namespace av_speech_in_noise {
class TimerStub : public Timer {
  public:
    void scheduleCallbackAfterSeconds(double) override {
        callbackScheduled_ = true;
    }

    [[nodiscard]] auto callbackScheduled() const { return callbackScheduled_; }

    void clearCallbackCount() { callbackScheduled_ = false; }

    void callback() { observer->callback(); }

    void attach(Observer *a) override { observer = a; }

    void cancelLastCallback() override { callbackCancelled = true; }

    bool callbackCancelled{};

  private:
    Observer *observer{};
    bool callbackScheduled_{};
};
}

#endif
