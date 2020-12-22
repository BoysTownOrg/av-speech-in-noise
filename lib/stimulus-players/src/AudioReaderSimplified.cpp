#include "AudioReaderSimplified.hpp"
#include <algorithm>

namespace av_speech_in_noise {
AudioReaderSimplified::AudioReaderSimplified(
    BufferedAudioReader &reader, BufferedAudioReader::Factory &)
    : reader{reader} {}

auto AudioReaderSimplified::read(std::string filePath) -> audio_type {
    reader.load(LocalUrl{filePath});
    if (reader.failed())
        throw InvalidFile{};
    audio_type audio(reader.channels());
    std::generate(audio.begin(), audio.end(),
        [&, n = 0]() mutable { return reader.channel(n++); });
    return audio;
}
}