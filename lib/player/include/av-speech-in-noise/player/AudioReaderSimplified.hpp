#ifndef AV_SPEECH_IN_NOISE_LIB_PLAYER_INCLUDE_AVSPEECHINNOISE_PLAYER_AUDIOREADERSIMPLIFIEDHPP_
#define AV_SPEECH_IN_NOISE_LIB_PLAYER_INCLUDE_AVSPEECHINNOISE_PLAYER_AUDIOREADERSIMPLIFIEDHPP_

#include "AudioReader.hpp"

#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>

#include <gsl/gsl>

#include <exception>
#include <vector>
#include <memory>

namespace av_speech_in_noise {
class BufferedAudioReader {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(BufferedAudioReader);
    virtual auto channel(gsl::index) -> std::vector<float> = 0;
    virtual auto channels() -> gsl::index = 0;

    class CannotReadFile : public std::exception {};

    class Factory {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Factory);
        virtual auto make(const LocalUrl &)
            -> std::shared_ptr<BufferedAudioReader> = 0;
    };
};

class AudioReaderSimplified : public AudioReader {
  public:
    explicit AudioReaderSimplified(BufferedAudioReader::Factory &);
    auto read(std::string filePath) -> audio_type override;

  private:
    BufferedAudioReader::Factory &readerFactory;
};
}

#endif
