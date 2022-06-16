#include "assert-utility.hpp"
#include "av-speech-in-noise/Model.hpp"
#include "av-speech-in-noise/core/TextFileReader.hpp"
#include <av-speech-in-noise/core/TargetPlaylist.hpp>

#include <gtest/gtest.h>

#include <filesystem>
#include <sstream>
#include <string>

namespace av_speech_in_noise {
class PredeterminedTargetPlaylist : public FiniteTargetPlaylistWithRepeatables {
  public:
    explicit PredeterminedTargetPlaylist(TextFileReader &fileReader)
        : fileReader{fileReader} {}
    void load(const LocalUrl &url) override {
        targets.clear();
        index = -1;
        std::stringstream stream{fileReader.read(url)};
        for (std::string line; std::getline(stream, line);)
            targets.push_back(LocalUrl{line});
    }
    auto next() -> LocalUrl override { return targets.at(++index); }
    auto current() -> LocalUrl override { return targets.at(index); }
    auto directory() -> LocalUrl override { return {}; }
    auto empty() -> bool override { return index == targets.size() - 1; }
    void reinsertCurrent() override {}

  private:
    TextFileReader &fileReader;
    std::vector<LocalUrl> targets;
    int index{};
};

class TextFileReaderStub : public TextFileReader {
  public:
    auto read(const LocalUrl &url) -> std::string override {
        url_ = url;
        return contents;
    }

    void setContents(std::string s) { contents = std::move(s); }

    auto url() -> LocalUrl { return url_; }

  private:
    LocalUrl url_;
    std::string contents;
};

class PredeterminedTargetPlaylistTests : public ::testing::Test {
  protected:
    TextFileReaderStub fileReader;
    PredeterminedTargetPlaylist playlist{fileReader};
};

TEST_F(PredeterminedTargetPlaylistTests, passesPlaylistToTextFileReader) {
    playlist.load(LocalUrl{"/Users/user/playlist.txt"});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "/Users/user/playlist.txt", fileReader.url().path);
}

TEST_F(PredeterminedTargetPlaylistTests, returnsTargetsReadFromPlaylist) {
    fileReader.setContents(R"(/Users/user/a.wav
/Users/user/b.wav
/Users/user/c.wav
)");
    playlist.load({});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("/Users/user/a.wav", playlist.next().path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("/Users/user/b.wav", playlist.next().path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("/Users/user/c.wav", playlist.next().path);
}

TEST_F(PredeterminedTargetPlaylistTests, returnsCurrentTarget) {
    fileReader.setContents(R"(/Users/user/a.wav
/Users/user/b.wav
/Users/user/c.wav
)");
    playlist.load({});
    playlist.next();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "/Users/user/a.wav", playlist.current().path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "/Users/user/a.wav", playlist.current().path);
}

TEST_F(PredeterminedTargetPlaylistTests, emptyWhenEmpty) {
    fileReader.setContents(R"(/Users/user/a.wav
/Users/user/b.wav
/Users/user/c.wav
)");
    playlist.load({});
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(playlist.empty());
    playlist.next();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(playlist.empty());
    playlist.next();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(playlist.empty());
    playlist.next();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(playlist.empty());
}
}
