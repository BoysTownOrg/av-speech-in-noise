#include "AudioReader.hpp"

namespace stimulus_players {
    AudioReader::AudioReader(AudioFileReader *reader)  : reader{reader} {}
    
    void AudioReader::loadFile(std::string filePath) {
        reader->loadFile(std::move(filePath));
        if (reader->failed())
            throw InvalidFile{};
    }
}
