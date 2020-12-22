#include "AudioReaderSimplified.hpp"
#include <algorithm>

namespace av_speech_in_noise {
AudioReaderSimplified::AudioReaderSimplified(
    BufferedAudioReader &reader, BufferedAudioReader::Factory &readerFactory)
    : reader{reader}, readerFactory{readerFactory} {}

auto AudioReaderSimplified::read(std::string filePath) -> audio_type {
    try {
        readerFactory.make(LocalUrl{filePath});
    } catch (const BufferedAudioReader::CannotReadFile &) {
        throw InvalidFile{};
    }
    reader.load(LocalUrl{filePath});
    if (reader.failed())
        throw InvalidFile{};
    audio_type audio(reader.channels());
    std::generate(audio.begin(), audio.end(),
        [&, n = 0]() mutable { return reader.channel(n++); });
    return audio;
}
}