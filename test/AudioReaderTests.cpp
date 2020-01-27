#include "assert-utility.h"
#include <gsl/gsl>
#include <gtest/gtest.h>
#include <stimulus-players/AudioReaderImpl.hpp>

namespace {
class AudioBufferStub : public stimulus_players::AudioBuffer {
    std::vector<std::vector<int>> audio_{};

  public:
    void setAudio(std::vector<std::vector<int>> x) { audio_ = std::move(x); }

    auto channels() -> int override { return gsl::narrow<int>(audio_.size()); }

    auto channel(int i) -> std::vector<int> override { return audio_.at(i); }

    auto empty() -> bool override { return audio_.empty(); }
};

class BufferedAudioReaderStub : public stimulus_players::BufferedAudioReader {
    std::vector<std::vector<std::vector<int>>> buffers_{};
    std::string file_{};
    std::shared_ptr<AudioBufferStub> buffer =
        std::make_shared<AudioBufferStub>();
    int minimumPossibleSample_{};
    bool failOnLoad_{};
    bool failed_{};

  public:
    void failOnLoad() { failOnLoad_ = true; }

    [[nodiscard]] auto file() const { return file_; }

    auto readNextBuffer()
        -> std::shared_ptr<stimulus_players::AudioBuffer> override {
        if (!buffers_.empty()) {
            buffer->setAudio(buffers_.front());
            buffers_.erase(buffers_.begin());
        } else
            buffer->setAudio({});
        return buffer;
    }

    void loadFile(std::string s) override {
        file_ = std::move(s);
        if (failOnLoad_)
            failed_ = true;
    }

    auto failed() -> bool override { return failed_; }

    auto minimumPossibleSample() -> int override {
        return minimumPossibleSample_;
    }

    void setMinimumPossibleSample(int x) { minimumPossibleSample_ = x; }

    void setBuffers(std::vector<std::vector<std::vector<int>>> x) {
        buffers_ = std::move(x);
    }
};

class AudioReaderTests : public ::testing::Test {
  protected:
    BufferedAudioReaderStub bufferedReader{};
    stimulus_players::AudioReaderImpl reader{&bufferedReader};

    template <typename T>
    auto dividedBy(std::vector<T> x, T c) -> std::vector<T> {
        std::for_each(x.begin(), x.end(), [&](T &x_) { x_ /= c; });
        return x;
    }

    auto read(std::string s = {}) { return reader.read(std::move(s)); }
};

TEST_F(AudioReaderTests, loadsFile) {
    read("a");
    assertEqual("a", bufferedReader.file());
}

TEST_F(AudioReaderTests, readThrowsInvalidFileOnFailure) {
    bufferedReader.failOnLoad();
    try {
        read();
        FAIL() << "Expected stimulus_players::AudioReader::InvalidFile";
    } catch (const stimulus_players::AudioReaderImpl::InvalidFile &) {
    }
}

TEST_F(AudioReaderTests, readConcatenatesNormalizedBuffers) {
    bufferedReader.setBuffers({{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}},
        {{10, 11, 12}, {13, 14, 15}, {16, 17, 18}},
        {{19, 20, 21}, {22, 23, 24}, {25, 26, 27}}});
    bufferedReader.setMinimumPossibleSample(-3);
    assertEqual({dividedBy({1, 2, 3, 10, 11, 12, 19, 20, 21}, -3.F),
                    dividedBy({4, 5, 6, 13, 14, 15, 22, 23, 24}, -3.F),
                    dividedBy({7, 8, 9, 16, 17, 18, 25, 26, 27}, -3.F)},
        read());
}
}
