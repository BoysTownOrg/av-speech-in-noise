#include "TrackSettingsReader.hpp"

namespace av_speech_in_noise {
TrackSettingsReader::TrackSettingsReader(
    TextFileReader *reader, ITrackSettingsInterpreter *interpreter)
    : reader{reader}, interpreter{interpreter} {}

const TrackingRule *TrackSettingsReader::read(std::string filePath) {
    return interpreter->trackingRule(reader->read(std::move(filePath)));
}
}
