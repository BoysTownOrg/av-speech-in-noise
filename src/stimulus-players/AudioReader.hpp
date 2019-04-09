#ifndef stimulus_players_AudioReader_hpp
#define stimulus_players_AudioReader_hpp

#include <string>

namespace stimulus_players {
    class AudioFileReader {
    public:
        virtual ~AudioFileReader() = default;
        virtual void loadFile(std::string) = 0;
    };

    class AudioReader {
        AudioFileReader *reader;
    public:
        AudioReader(AudioFileReader *);
        void loadFile(std::string filePath);
    };
}

#endif
