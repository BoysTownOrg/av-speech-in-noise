#ifndef stimulus_players_AudioReaderImpl_hpp
#define stimulus_players_AudioReaderImpl_hpp

#include <string>
#include <vector>

namespace stimulus_players {
    class AudioBuffer {
    public:
        virtual ~AudioBuffer() = default;
        virtual int channels() = 0;
        virtual std::vector<int> channel(int) = 0;
        virtual bool empty() = 0;
    };
    
    class BufferedAudioReader {
    public:
        virtual ~BufferedAudioReader() = default;
        virtual void loadFile(std::string) = 0;
        virtual bool failed() = 0;
        virtual AudioBuffer *readNextBuffer() = 0;
        virtual int minimumPossibleSample() = 0;
    };

    class AudioReaderImpl {
        BufferedAudioReader *reader;
    public:
        AudioReaderImpl(BufferedAudioReader *);
        class InvalidFile {};
        std::vector<std::vector<float>> read(std::string filePath);
    private:
        void loadFile(std::string filePath);
    };
}

#endif
