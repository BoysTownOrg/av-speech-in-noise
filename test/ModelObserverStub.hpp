#ifndef AV_SPEECH_IN_NOISE_TEST_MODELEVENTLISTENERSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TEST_MODELEVENTLISTENERSTUB_HPP_

#include <av-speech-in-noise/core/IRunningATest.hpp>

namespace av_speech_in_noise {
class ModelObserverStub : public RunningATest::RequestObserver {
    bool notified_{};

  public:
    void notifyThatPlayTrialHasCompleted() override { notified_ = true; }

    [[nodiscard]] auto notified() const { return notified_; }
};
}

#endif
