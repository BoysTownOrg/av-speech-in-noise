#include "AudioReaderImpl.hpp"

namespace stimulus_players {
AudioReaderImpl::AudioReaderImpl(BufferedAudioReader *reader)
    : reader{reader} {}

auto AudioReaderImpl::read(std::string filePath) -> audio_type {
    loadFile(std::move(filePath));
    audio_type audio{};
    auto minimumSample = reader->minimumPossibleSample();
    for (auto buffer = reader->readNextBuffer(); !buffer->empty();
         buffer = reader->readNextBuffer()) {
        audio.resize(buffer->channels());
        for (channel_index_type channel{0}; channel < buffer->channels();
             ++channel)
            for (auto x : buffer->channel(channel))
                audio.at(channel).push_back(1.F * x / minimumSample);
    }
    return audio;
}

void AudioReaderImpl::loadFile(std::string filePath) {
    reader->loadFile(std::move(filePath));
    if (reader->failed())
        throw InvalidFile{};
}
}
