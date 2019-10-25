#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_TRACKSETTINGSINTERPRETER_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_TRACKSETTINGSINTERPRETER_HPP_

#include "TrackSettingsReader.hpp"
#include <string>

namespace av_speech_in_noise {
class TrackSettingsInterpreter : public ITrackSettingsInterpreter {
    TrackingRule rule_{};
    std::string contents;

  public:
    enum class Property { up, down, reversalsPerStepSize, stepSizes };

    static constexpr const char *propertyName(Property p) {
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

    const TrackingRule *trackingRule(std::string) override;
};
}

#endif
