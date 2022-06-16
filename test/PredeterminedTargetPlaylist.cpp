#include "assert-utility.hpp"
#include "av-speech-in-noise/Model.hpp"
#include "av-speech-in-noise/core/TextFileReader.hpp"
#include <av-speech-in-noise/core/TargetPlaylist.hpp>

#include <gtest/gtest.h>

#include <filesystem>

namespace av_speech_in_noise {
class PredeterminedTargetPlaylist : public TargetPlaylist {
  public:
    explicit PredeterminedTargetPlaylist(TextFileReader &fileReader)
        : fileReader{fileReader} {}
    void load(const LocalUrl &url) override { fileReader.read(url); }
    auto next() -> LocalUrl override { return {}; }
    auto current() -> LocalUrl override { return {}; }
    auto directory() -> LocalUrl override { return {}; }

  private:
    TextFileReader &fileReader;
};

class TextFileReaderStub : public TextFileReader {
  public:
    auto read(const LocalUrl &url) -> std::string override {
        url_ = url;
        return {};
    }

    auto url() -> LocalUrl { return url_; }

  private:
    LocalUrl url_;
};

class PredeterminedTargetPlaylistTests : public ::testing::Test {};

TEST_F(PredeterminedTargetPlaylistTests, passesPlaylistToTextFileReader) {
    TextFileReaderStub fileReader;
    PredeterminedTargetPlaylist playlist{fileReader};
    playlist.load(LocalUrl{"/Users/user/playlist.txt"});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "/Users/user/playlist.txt", fileReader.url().path);
}
}
