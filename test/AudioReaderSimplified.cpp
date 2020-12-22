#include "assert-utility.hpp"
#include <stimulus-players/AudioReaderSimplified.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>
#include <vector>

namespace av_speech_in_noise {
namespace {
class BufferedAudioReaderSimpleStub : public BufferedAudioReader {
  public:
    void setAudio(std::vector<std::vector<float>> v) { audio = std::move(v); }

    auto channel(gsl::index n) -> std::vector<float> override {
        return audio.at(n);
    }

    auto channels() -> gsl::index override { return audio.size(); }

    void failOnLoad() { failOnLoad_ = true; }

    [[nodiscard]] auto url() const -> LocalUrl { return url_; }

    void load(const LocalUrl &s) override {
        url_ = s;
        if (failOnLoad_)
            failed_ = true;
    }

    auto failed() -> bool override { return failed_; }

  private:
    std::vector<std::vector<float>> audio;
    LocalUrl url_{};
    bool failOnLoad_{};
    bool failed_{};
};

auto read(AudioReaderSimplified &reader, std::string s = {}) -> audio_type {
    return reader.read(std::move(s));
}

class AudioReaderSimplifiedTests : public ::testing::Test {
  protected:
    BufferedAudioReaderSimpleStub bufferedReader;
    AudioReaderSimplified reader{bufferedReader};
};

#define AUDIO_READER_SIMPLIFIED_TEST(a) TEST_F(AudioReaderSimplifiedTests, a)

AUDIO_READER_SIMPLIFIED_TEST(loadsFile) {
    read(reader, "a");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, bufferedReader.url().path);
}

AUDIO_READER_SIMPLIFIED_TEST(readThrowsInvalidFileOnFailure) {
    bufferedReader.failOnLoad();
    try {
        read(reader);
        FAIL() << "Expected AudioReaderSimplified::InvalidFile";
    } catch (const AudioReaderSimplified::InvalidFile &) {
    }
}

AUDIO_READER_SIMPLIFIED_TEST(readAssemblesChannels) {
    bufferedReader.setAudio({{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}});
    assertEqual({{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}}, read(reader));
}
}
}
