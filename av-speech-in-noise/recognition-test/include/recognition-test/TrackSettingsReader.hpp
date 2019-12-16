#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_TRACKSETTINGSREADER_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_TRACKSETTINGSREADER_HPP_

#include "AdaptiveMethod.hpp"
#include <string>

namespace av_speech_in_noise {
class TextFileReader {
  public:
    virtual ~TextFileReader() = default;
    virtual auto read(std::string) -> std::string = 0;
};

class TrackSettingsInterpreter {
  public:
    virtual ~TrackSettingsInterpreter() = default;
    virtual auto trackingRule(std::string) -> const TrackingRule * = 0;
};

class TrackSettingsReaderImpl : public TrackSettingsReader {
  public:
    TrackSettingsReaderImpl(TextFileReader *, TrackSettingsInterpreter *);
    auto read(std::string filePath) -> const TrackingRule * override;

  private:
    TextFileReader *reader;
    TrackSettingsInterpreter *interpreter;
};
}

#endif
