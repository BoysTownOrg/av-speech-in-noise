#ifndef av_speech_in_noise_TrackSettingsReader_hpp
#define av_speech_in_noise_TrackSettingsReader_hpp

#include <av-speech-in-noise/Model.h>
#include <string>
#include <sstream>
#include <map>

namespace av_speech_in_noise {
    
    enum class TrackProperty {
        up,
        down,
        reversalsPerStepSize,
        stepSizes
    };

    constexpr const char *propertyName(TrackProperty p) {
        switch (p) {
            case TrackProperty::up:
                return "up";
            case TrackProperty::down:
                return "down";
            case TrackProperty::reversalsPerStepSize:
                return "reversals per step size";
            case TrackProperty::stepSizes:
                return "step sizes (dB)";
        }
    }
    
    class TrackSettingsReader {
        std::map<std::string, void(TrackSettingsReader::*)(TrackingSequence &, int)> propertyApplication {
            {propertyName(TrackProperty::up), &TrackSettingsReader::applyToUp},
            {propertyName(TrackProperty::down), &TrackSettingsReader::applyToDown},
            {propertyName(TrackProperty::reversalsPerStepSize), &TrackSettingsReader::applyToRunCount},
            {propertyName(TrackProperty::stepSizes), &TrackSettingsReader::applyToStepSize}
        };
        std::string contents;
    public:
        
        explicit TrackSettingsReader(std::string s) :
            contents{std::move(s)} {}
        
        TrackingRule trackingRule();
        
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
