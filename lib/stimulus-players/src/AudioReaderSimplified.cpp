#include "AudioReaderSimplified.hpp"
#include <algorithm>

namespace av_speech_in_noise {
AudioReaderSimplified::AudioReaderSimplified(
    BufferedAudioReader::Factory &readerFactory)
    : readerFactory{readerFactory} {}

static auto make(BufferedAudioReader::Factory &factory, const LocalUrl &url)
    -> std::shared_ptr<BufferedAudioReader> {
    try {
        return factory.make(url);
    } catch (const BufferedAudioReader::CannotReadFile &) {
        throw AudioReader::InvalidFile{};
    }
}

auto AudioReaderSimplified::read(std::string filePath) -> audio_type {
    const auto reader{make(readerFactory, LocalUrl{filePath})};
    audio_type audio(reader->channels());
    std::generate(audio.begin(), audio.end(),
        [&, n = 0]() mutable { return reader->channel(n++); });
    return audio;
}
}
