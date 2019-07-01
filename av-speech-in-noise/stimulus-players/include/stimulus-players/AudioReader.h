#ifndef stimulus_players_AudioReader_h
#define stimulus_players_AudioReader_h

#include <vector>

namespace stimulus_players {
    class AudioReader {
    public:
        virtual ~AudioReader() = default;
        virtual std::vector<std::vector<float>> read(std::string filePath) = 0;
        class InvalidFile {};
    };
}

#endif
