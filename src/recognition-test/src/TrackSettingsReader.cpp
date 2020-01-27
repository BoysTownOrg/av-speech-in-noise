#include "TrackSettingsReader.hpp"

namespace av_speech_in_noise {
TrackSettingsReaderImpl::TrackSettingsReaderImpl(
    TextFileReader *reader, TrackSettingsInterpreter *interpreter)
    : reader{reader}, interpreter{interpreter} {}

auto TrackSettingsReaderImpl::read(std::string filePath)
    -> const TrackingRule * {
    return interpreter->trackingRule(reader->read(std::move(filePath)));
}
}
