#ifndef AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_AUDIOREADER_HPP_
#define AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_AUDIOREADER_HPP_

#include <vector>
#include <string>

namespace stimulus_players {
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
