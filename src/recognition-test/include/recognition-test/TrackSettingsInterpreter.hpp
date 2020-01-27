#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_TRACKSETTINGSINTERPRETER_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_TRACKSETTINGSINTERPRETER_HPP_

#include "TrackSettingsReader.hpp"
#include <string>

namespace av_speech_in_noise {
class TrackSettingsInterpreterImpl : public TrackSettingsInterpreter {
  public:
    enum class Property { up, down, reversalsPerStepSize, stepSizes };

    static constexpr auto propertyName(Property p) -> const char * {
        switch (p) {
        case Property::up:
            return "up";
        case Property::down:
            return "down";
        case Property::reversalsPerStepSize:
            return "reversals per step size";
        case Property::stepSizes:
            return "step sizes (dB)";
        }
    }

    auto trackingRule(std::string) -> const TrackingRule * override;

  private:
    TrackingRule rule_{};
    std::string contents;
};
}

#endif
