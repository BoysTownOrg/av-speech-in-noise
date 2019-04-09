#ifndef stimulus_players_AudioReader_hpp
#define stimulus_players_AudioReader_hpp

#include <string>
#include <vector>

namespace stimulus_players {
    class AudioFileReader {
    public:
        virtual ~AudioFileReader() = default;
        virtual void loadFile(std::string) = 0;
        virtual bool failed() = 0;
        virtual std::vector<int> readNextBuffer() = 0;
        virtual int minimumPossibleSample() = 0;
    };

    class AudioReader {
        AudioFileReader *reader;
    public:
        AudioReader(AudioFileReader *);
        void loadFile(std::string filePath);
        class InvalidFile {};
        std::vector<float> read();
    };
}

#endif
