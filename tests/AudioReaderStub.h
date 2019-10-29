#ifndef TESTS_AUDIOREADERSTUB_H_
#define TESTS_AUDIOREADERSTUB_H_

#include <stimulus-players/AudioReader.hpp>

namespace stimulus_players::tests {
class AudioReaderStub : public AudioReader {
    std::vector<std::vector<float>> toRead_{};
    std::string filePath_{};
    bool throwOnRead_{};

  public:
    void set(std::vector<std::vector<float>> x) { toRead_ = std::move(x); }

    std::vector<std::vector<float>> read(std::string filePath) override {
        filePath_ = std::move(filePath);
        if (throwOnRead_)
            throw InvalidFile{};
        return toRead_;
    }

    [[nodiscard]] auto filePath() const { return filePath_; }

    void throwOnRead() { throwOnRead_ = true; }
};
}

#endif
