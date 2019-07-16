#ifndef av_speech_in_noise_TrackSettingsReader_hpp
#define av_speech_in_noise_TrackSettingsReader_hpp

#include <av-speech-in-noise/Model.h>
#include <string>
#include <map>

namespace av_speech_in_noise {
    class TrackSettingsReader {
        std::map<
            std::string,
            void(TrackSettingsReader::*)(TrackingSequence &, int)
        > propertyApplication;
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
        
        explicit TrackSettingsReader(std::string);
        TrackingRule trackingRule();
    private:
        void applyToUp(TrackingSequence &sequence, int x) {
            sequence.up = x;
        }
        
        void applyToDown(TrackingSequence &sequence, int x) {
            sequence.down = x;
        }
        
        void applyToRunCount(TrackingSequence &sequence, int x) {
            sequence.runCount = x;
        }
        
        void applyToStepSize(TrackingSequence &sequence, int x) {
            sequence.stepSize = x;
        }
    };
}

#endif
