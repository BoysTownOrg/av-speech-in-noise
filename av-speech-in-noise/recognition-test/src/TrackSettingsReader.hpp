#ifndef av_speech_in_noise_TrackSettingsReader_hpp
#define av_speech_in_noise_TrackSettingsReader_hpp

#include "AdaptiveMethod.hpp"

namespace av_speech_in_noise {
    class TextFileReader {
    public:
        virtual ~TextFileReader() = default;
        virtual std::string read(std::string) = 0;
    };
    
    class ITrackSettingsInterpreter {
    public:
        virtual ~ITrackSettingsInterpreter() = default;
        virtual const TrackingRule *trackingRule(std::string) = 0;
    };

    class TrackSettingsReader : public ITrackSettingsReader {
        TextFileReader *reader;
        ITrackSettingsInterpreter *interpreter;
    public:
        TrackSettingsReader(
            TextFileReader *,
            ITrackSettingsInterpreter *
        );
        const TrackingRule *read(std::string filePath) override;
    };
}

#endif
