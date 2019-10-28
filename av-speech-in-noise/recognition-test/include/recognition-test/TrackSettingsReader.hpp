#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_TRACKSETTINGSREADER_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_TRACKSETTINGSREADER_HPP_

#include "AdaptiveMethod.hpp"
#include <string>

namespace av_speech_in_noise {
class TextFileReader {
  public:
    virtual ~TextFileReader() = default;
    virtual std::string read(std::string) = 0;
};

class TrackSettingsInterpreter {
  public:
    virtual ~TrackSettingsInterpreter() = default;
    virtual const TrackingRule *trackingRule(std::string) = 0;
};

class TrackSettingsReaderImpl : public TrackSettingsReader {
    TextFileReader *reader;
    TrackSettingsInterpreter *interpreter;

  public:
    TrackSettingsReaderImpl(TextFileReader *, TrackSettingsInterpreter *);
    const TrackingRule *read(std::string filePath) override;
};
}

#endif
