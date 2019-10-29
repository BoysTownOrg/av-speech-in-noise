#include "AudioReaderImpl.hpp"

namespace stimulus_players {
AudioReaderImpl::AudioReaderImpl(BufferedAudioReader *reader)
    : reader{reader} {}

std::vector<std::vector<float>> AudioReaderImpl::read(std::string filePath) {
    loadFile(std::move(filePath));
    std::vector<std::vector<float>> audio{};
    auto minimumSample = reader->minimumPossibleSample();
    for (auto buffer = reader->readNextBuffer(); !buffer->empty();
         buffer = reader->readNextBuffer()) {
        audio.resize(buffer->channels());
        for (int channel = 0; channel < buffer->channels(); ++channel)
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
