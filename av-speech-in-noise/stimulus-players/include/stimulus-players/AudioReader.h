#ifndef AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_AUDIOREADER_H_
#define AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_AUDIOREADER_H_

#include <vector>
#include <string>

namespace stimulus_players {
class AudioReader {
public:
    virtual ~AudioReader() = default;
    virtual std::vector<std::vector<float>> read(std::string filePath) = 0;
    class InvalidFile {};
};
}

#endif
