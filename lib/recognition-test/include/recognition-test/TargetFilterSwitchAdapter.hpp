#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_TARGETFILTERSWITCHADAPTER_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_TARGETFILTERSWITCHADAPTER_HPP_

#include "Model.hpp"
#include <av-speech-in-noise/Interface.hpp>

namespace av_speech_in_noise {
class SignalProcessorSwitch {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(
        SignalProcessorSwitch);
    virtual void initializeProcessor(const LocalUrl &) = 0;
    virtual void clearProcessor() = 0;
};

class TargetFilterSwitchAdapter : public TargetFilterSwitch {
  public:
    explicit TargetFilterSwitchAdapter(SignalProcessorSwitch &processorSwitch)
        : processorSwitch{processorSwitch} {}

    void turnOn(const LocalUrl &url) override {
        processorSwitch.initializeProcessor(url);
    }

    void turnOff() override { processorSwitch.clearProcessor(); }

  private:
    SignalProcessorSwitch &processorSwitch;
};
}

#endif
