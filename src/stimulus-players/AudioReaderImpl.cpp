#include "AudioReaderImpl.hpp"

namespace stimulus_players {
    AudioReaderImpl::AudioReaderImpl(BufferedAudioReader *reader) :
        reader{reader} {}
    
    std::vector<float> AudioReaderImpl::read(std::string filePath) {
        loadFile(std::move(filePath));
        std::vector<float> audio{};
        float minimumSample = reader->minimumPossibleSample();
        for (
            auto buffer = reader->readNextBuffer();
            !buffer.empty();
            buffer = reader->readNextBuffer()
        )
            for (auto x : buffer)
                audio.push_back(x/minimumSample);
        return audio;
    }
    
    void AudioReaderImpl::loadFile(std::string filePath) {
        reader->loadFile(std::move(filePath));
        if (reader->failed())
            throw InvalidFile{};
    }
}
