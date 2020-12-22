#include "assert-utility.hpp"
#include <stimulus-players/AudioReaderSimplified.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>
#include <vector>

namespace av_speech_in_noise {
namespace {
class BufferedAudioReaderStub : public BufferedAudioReader {
  public:
    void setAudio(std::vector<std::vector<float>> v) { audio = std::move(v); }

    auto channel(gsl::index n) -> std::vector<float> override {
        return audio.at(n);
    }

    auto channels() -> gsl::index override { return audio.size(); }

  private:
    std::vector<std::vector<float>> audio;
};

class BufferedAudioReaderStubFactory : public BufferedAudioReader::Factory {
  public:
    BufferedAudioReaderStubFactory(
        std::shared_ptr<BufferedAudioReaderStub> reader)
        : reader{std::move(reader)} {}

    auto make(const LocalUrl &url)
        -> std::shared_ptr<BufferedAudioReader> override {
        url_ = url;
        if (throwOnMake_)
            throw BufferedAudioReader::CannotReadFile{};
        return reader;
    }

    void throwOnMake() { throwOnMake_ = true; }

    auto url() -> LocalUrl { return url_; }

  private:
    LocalUrl url_;
    std::shared_ptr<BufferedAudioReaderStub> reader;
    bool throwOnMake_{};
};

auto read(AudioReaderSimplified &reader, std::string s = {}) -> audio_type {
    return reader.read(std::move(s));
}

class AudioReaderSimplifiedTests : public ::testing::Test {
  protected:
    std::shared_ptr<BufferedAudioReaderStub> bufferedReaderPtr =
        std::make_shared<BufferedAudioReaderStub>();
    BufferedAudioReaderStubFactory readerFactory{bufferedReaderPtr};
    AudioReaderSimplified reader{readerFactory};
};

#define AUDIO_READER_SIMPLIFIED_TEST(a) TEST_F(AudioReaderSimplifiedTests, a)

AUDIO_READER_SIMPLIFIED_TEST(loadsFile) {
    read(reader, "a");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, readerFactory.url().path);
}

AUDIO_READER_SIMPLIFIED_TEST(readThrowsInvalidFileOnFailure) {
    readerFactory.throwOnMake();
    try {
        read(reader);
        FAIL() << "Expected AudioReaderSimplified::InvalidFile";
    } catch (const AudioReaderSimplified::InvalidFile &) {
    }
}

AUDIO_READER_SIMPLIFIED_TEST(readAssemblesChannels) {
    bufferedReaderPtr->setAudio({{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}});
    assertEqual({{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}}, read(reader));
}
}
}
