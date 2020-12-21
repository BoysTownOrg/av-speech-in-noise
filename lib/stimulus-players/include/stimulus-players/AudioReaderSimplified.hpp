#ifndef AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_AUDIOREADERSIMPLIFIED_HPP_
#define AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_AUDIOREADERSIMPLIFIED_HPP_

#include "AudioReader.hpp"
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>
#include <gsl/gsl>
#include <string>
#include <vector>
#include <algorithm>

namespace av_speech_in_noise {
class BufferedAudioReaderSimple {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(
        BufferedAudioReaderSimple);
    virtual void load(const LocalUrl &) = 0;
    virtual auto failed() -> bool = 0;
    virtual auto channel(gsl::index) -> std::vector<float> = 0;
    virtual auto channels() -> gsl::index = 0;
};

class AudioReaderSimplified : public AudioReader {
  public:
    AudioReaderSimplified(BufferedAudioReaderSimple &reader) : reader{reader} {}

    auto read(std::string filePath) -> audio_type override {
        reader.load(LocalUrl{filePath});
        if (reader.failed())
            throw InvalidFile{};
        audio_type audio(reader.channels());
        std::generate(audio.begin(), audio.end(),
            [&, n = 0]() mutable { return reader.channel(n++); });
        return audio;
    }

  private:
    BufferedAudioReaderSimple &reader;
};
}

#endif
