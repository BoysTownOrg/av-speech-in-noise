#ifndef stimulus_players_AudioReader_hpp
#define stimulus_players_AudioReader_hpp

#include <string>
#include <vector>

namespace stimulus_players {
    class BufferedAudioReader {
    public:
        virtual ~BufferedAudioReader() = default;
        virtual void loadFile(std::string) = 0;
        virtual bool failed() = 0;
        virtual std::vector<int> readNextBuffer() = 0;
        virtual int minimumPossibleSample() = 0;
    };

    class AudioReader {
        BufferedAudioReader *reader;
    public:
        AudioReader(BufferedAudioReader *);
        class InvalidFile {};
        std::vector<float> read(std::string filePath);
    private:
        void loadFile(std::string filePath);
    };
}

#endif
