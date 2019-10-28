#include "TrackSettingsReader.hpp"

namespace av_speech_in_noise {
TrackSettingsReaderImpl::TrackSettingsReaderImpl(
    TextFileReader *reader, ITrackSettingsInterpreter *interpreter)
    : reader{reader}, interpreter{interpreter} {}

const TrackingRule *TrackSettingsReaderImpl::read(std::string filePath) {
    return interpreter->trackingRule(reader->read(std::move(filePath)));
}
}
