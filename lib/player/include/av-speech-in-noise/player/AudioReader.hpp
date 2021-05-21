#ifndef AV_SPEECH_IN_NOISE_LIB_PLAYER_INCLUDE_AVSPEECHINNOISE_PLAYER_AUDIOREADERHPP_
#define AV_SPEECH_IN_NOISE_LIB_PLAYER_INCLUDE_AVSPEECHINNOISE_PLAYER_AUDIOREADERHPP_

#include <vector>
#include <string>

namespace av_speech_in_noise {
using sample_type = float;

using channel_type = std::vector<sample_type>;

using audio_type = std::vector<channel_type>;

class AudioReader {
  public:
    virtual ~AudioReader() = default;
    virtual auto read(std::string filePath) -> audio_type = 0;
    class InvalidFile {};
};
}

#endif
