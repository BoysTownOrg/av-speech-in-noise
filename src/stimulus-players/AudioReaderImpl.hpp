#ifndef stimulus_players_AudioReaderImpl_hpp
#define stimulus_players_AudioReaderImpl_hpp

#include "AudioReader.h"
#include <string>
#include <vector>
#include <memory>

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
        virtual std::shared_ptr<AudioBuffer> readNextBuffer() = 0;
        virtual int minimumPossibleSample() = 0;
    };

    class AudioReaderImpl : public AudioReader {
        BufferedAudioReader *reader;
    public:
        explicit AudioReaderImpl(BufferedAudioReader *);
        std::vector<std::vector<float>> read(std::string filePath) override;
    private:
        void loadFile(std::string filePath);
    };
}

#endif
