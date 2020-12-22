#ifndef AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_AUDIOREADERSIMPLIFIED_HPP_
#define AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_AUDIOREADERSIMPLIFIED_HPP_

#include "AudioReader.hpp"
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>
#include <gsl/gsl>
#include <vector>
#include <memory>

namespace av_speech_in_noise {
class BufferedAudioReader {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(BufferedAudioReader);
    virtual void load(const LocalUrl &) = 0;
    virtual auto failed() -> bool = 0;
    virtual auto channel(gsl::index) -> std::vector<float> = 0;
    virtual auto channels() -> gsl::index = 0;

    class CannotReadFile {};

    class Factory {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Factory);
        virtual auto make(const LocalUrl &)
            -> std::shared_ptr<BufferedAudioReader> = 0;
    };
};

class AudioReaderSimplified : public AudioReader {
  public:
    AudioReaderSimplified(
        BufferedAudioReader &, BufferedAudioReader::Factory &);
    auto read(std::string filePath) -> audio_type override;

  private:
    BufferedAudioReader &reader;
    BufferedAudioReader::Factory &readerFactory;
};
}

#endif
