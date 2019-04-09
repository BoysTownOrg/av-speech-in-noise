#include "AudioReader.hpp"

namespace stimulus_players {
    AudioReader::AudioReader(AudioFileReader *reader)  : reader{reader} {}
    
    void AudioReader::loadFile(std::string filePath) {
        reader->loadFile(std::move(filePath));
        if (reader->failed())
            throw InvalidFile{};
    }
    
    std::vector<float> AudioReader::read() {
        std::vector<float> audio{};
        for (
            auto buffer = reader->readNextBuffer();
            !buffer.empty();
            buffer = reader->readNextBuffer()
        ) {
            float minimumSample = reader->minimumPossibleSample();
            for (auto x : buffer) {
                audio.push_back(x/minimumSample);
            }
        }
        return audio;
    }
}
