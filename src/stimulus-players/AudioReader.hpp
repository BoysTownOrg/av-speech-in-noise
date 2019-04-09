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
        AudioReader(AudioFileReader *reader) : reader{reader} {}
        
        void loadFile(std::string filePath) {
            reader->loadFile(std::move(filePath));
        }
    };
}

#endif
