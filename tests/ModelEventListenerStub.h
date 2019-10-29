#ifndef ModelEventListenerStub_h
#define ModelEventListenerStub_h

#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise::tests {
class ModelEventListenerStub : public Model::EventListener {
    bool notified_{};

  public:
    void trialComplete() override { notified_ = true; }

    auto notified() const { return notified_; }
};
}

#endif
