#ifndef AudioReaderStub_h
#define AudioReaderStub_h

#include <stimulus-players/AudioReader.h>
#include <string>

class AudioReaderStub : public stimulus_players::AudioReader {
    std::vector<std::vector<float>> toRead_{};
    std::string filePath_{};
public:
    void set(std::vector<std::vector<float>> x) {
        toRead_ = std::move(x);
    }
    std::vector<std::vector<float> > read(std::string filePath) override {
        filePath_ = std::move(filePath);
        return toRead_;
    }
    auto filePath() const {
        return filePath_;
    }
};

#endif
