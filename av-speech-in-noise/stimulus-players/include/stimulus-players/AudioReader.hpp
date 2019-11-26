#ifndef AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_AUDIOREADER_HPP_
#define AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_AUDIOREADER_HPP_

#include <vector>
#include <string>

namespace stimulus_players {
using audio_type = std::vector<std::vector<float>>;
class AudioReader {
  public:
    virtual ~AudioReader() = default;
    virtual auto read(std::string filePath) -> audio_type = 0;
    class InvalidFile {};
};
}

#endif
