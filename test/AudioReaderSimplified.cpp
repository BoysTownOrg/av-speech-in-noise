#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>
#include <stimulus-players/AudioReader.hpp>
#include <string>

namespace av_speech_in_noise {
class BufferedAudioReaderSimple {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(
        BufferedAudioReaderSimple);
    virtual void load(const LocalUrl &) = 0;
    virtual auto failed() -> bool = 0;
};

class AudioReaderSimplified : public AudioReader {
  public:
    AudioReaderSimplified(BufferedAudioReaderSimple &reader) : reader{reader} {}

    auto read(std::string filePath) -> audio_type override {
        reader.load(LocalUrl{filePath});
        if (reader.failed())
            throw InvalidFile{};
        return {};
    }

  private:
    BufferedAudioReaderSimple &reader;
};
}

#include "assert-utility.hpp"
#include <gtest/gtest.h>
#include <gsl/gsl>

namespace av_speech_in_noise {
namespace {
class BufferedAudioReaderSimpleStub : public BufferedAudioReaderSimple {
  public:
    void failOnLoad() { failOnLoad_ = true; }

    [[nodiscard]] auto url() const -> LocalUrl { return url_; }

    void load(const LocalUrl &s) override {
        url_ = s;
        if (failOnLoad_)
            failed_ = true;
    }

    auto failed() -> bool override { return failed_; }

  private:
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

// TEST_F(AudioReaderSimplifiedTests, readConcatenatesNormalizedBuffers) {
//     bufferedReader.setBuffers({{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}},
//         {{10, 11, 12}, {13, 14, 15}, {16, 17, 18}},
//         {{19, 20, 21}, {22, 23, 24}, {25, 26, 27}}});
//     bufferedReader.setMinimumPossibleSample(-3);
//     assertEqual({dividedBy({1, 2, 3, 10, 11, 12, 19, 20, 21}, -3.F),
//                     dividedBy({4, 5, 6, 13, 14, 15, 22, 23, 24}, -3.F),
//                     dividedBy({7, 8, 9, 16, 17, 18, 25, 26, 27}, -3.F)},
//         read());
// }
}
}
