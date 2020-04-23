#include "AudioReaderImpl.hpp"
#include <gsl/gsl>

namespace av_speech_in_noise {
static void resize(audio_type &audio, gsl::index size) { audio.resize(size); }

static auto channel(audio_type &audio, gsl::index n) -> channel_type & {
    return audio.at(n);
}

AudioReaderImpl::AudioReaderImpl(BufferedAudioReader *reader)
    : reader{reader} {}

auto AudioReaderImpl::read(std::string filePath) -> audio_type {
    loadFile(std::move(filePath));
    audio_type audio{};
    auto minimumSample = reader->minimumPossibleSample();
    for (auto buffer = reader->readNextBuffer(); !buffer->empty();
         buffer = reader->readNextBuffer()) {
        resize(audio, buffer->channels());
        for (int i = 0; i < buffer->channels(); ++i)
            for (auto x : buffer->channel(i))
                channel(audio, i).push_back(1.F * x / minimumSample);
    }
    return audio;
}

void AudioReaderImpl::loadFile(std::string filePath) {
    reader->loadFile(std::move(filePath));
    if (reader->failed())
        throw InvalidFile{};
}
}
