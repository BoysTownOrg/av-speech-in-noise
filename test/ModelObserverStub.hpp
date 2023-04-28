#ifndef TESTS_MODELEVENTLISTENERSTUB_HPP_
#define TESTS_MODELEVENTLISTENERSTUB_HPP_

#include "av-speech-in-noise/core/IRecognitionTestModel.hpp"

namespace av_speech_in_noise {
class ModelObserverStub : public RunningATest::Observer {
    bool notified_{};

  public:
    void trialComplete() override { notified_ = true; }

    [[nodiscard]] auto notified() const { return notified_; }
};
}

#endif
