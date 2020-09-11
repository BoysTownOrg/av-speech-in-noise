#ifndef TESTS_MODELEVENTLISTENERSTUB_HPP_
#define TESTS_MODELEVENTLISTENERSTUB_HPP_

#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
class ModelObserverStub : public Model::Observer {
    bool notified_{};

  public:
    void trialComplete() override { notified_ = true; }

    [[nodiscard]] auto notified() const { return notified_; }
};
}

#endif
