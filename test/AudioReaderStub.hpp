#ifndef TESTS_AUDIOREADERSTUB_HPP_
#define TESTS_AUDIOREADERSTUB_HPP_

#include <stimulus-players/AudioReader.hpp>
#include <utility>

namespace av_speech_in_noise {
class AudioReaderStub : public AudioReader {
    std::vector<std::vector<float>> toRead_{};
    std::string filePath_{};
    bool throwOnRead_{};

  public:
    void set(std::vector<std::vector<float>> x) { toRead_ = std::move(x); }

    auto read(std::string filePath)
        -> std::vector<std::vector<float>> override {
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
