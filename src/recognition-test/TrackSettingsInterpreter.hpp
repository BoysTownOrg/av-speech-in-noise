#ifndef av_speech_in_noise_TrackSettingsInterpreter_hpp
#define av_speech_in_noise_TrackSettingsInterpreter_hpp

#include <av-speech-in-noise/Model.h>
#include <string>

namespace av_speech_in_noise {
    class TrackSettingsInterpreter {
        std::string contents;
    public:
        enum class Property {
            up,
            down,
            reversalsPerStepSize,
            stepSizes
        };

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
        
        explicit TrackSettingsInterpreter(std::string);
        TrackingRule trackingRule();
    };
}

#endif
