#include "assert-utility.hpp"

#include <av-speech-in-noise/playlist/PredeterminedTargetPlaylist.hpp>

#include <gtest/gtest.h>

#include <set>
#include <string>

namespace av_speech_in_noise {
namespace {
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

class FailingFileReader : public TextFileReader {
  public:
    auto read(const LocalUrl &) -> std::string override {
        throw FileDoesNotExist{};
    }
};

class FileValidatorStub : public TargetValidator {
  public:
    void failOn(std::string s) { failingFiles.insert(s); }

    auto isValid(const LocalUrl &url) -> bool override {
        return failingFiles.count(url.path) == 0;
    }

  private:
    std::set<std::string> failingFiles;
};
}

class PredeterminedTargetPlaylistTests : public ::testing::Test {
  protected:
    TextFileReaderStub fileReader;
    FileValidatorStub fileValidator;
    PredeterminedTargetPlaylist playlist{fileReader, fileValidator};
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
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(playlist.empty());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("/Users/user/c.wav", playlist.next().path);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(playlist.empty());
}

TEST_F(PredeterminedTargetPlaylistTests, doesNotRequireNewlineAtEOF) {
    fileReader.setContents(R"(/Users/user/a.wav
/Users/user/b.wav
/Users/user/c.wav)");
    playlist.load({});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("/Users/user/a.wav", playlist.next().path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("/Users/user/b.wav", playlist.next().path);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(playlist.empty());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("/Users/user/c.wav", playlist.next().path);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(playlist.empty());
}

TEST_F(PredeterminedTargetPlaylistTests, removesTrailingAndLeadingWhitespace) {
    fileReader.setContents(R"(/Users/user/a.wav 
 /Users/user/b.wav
 /Users/user/c.wav 
)");
    playlist.load({});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("/Users/user/a.wav", playlist.next().path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("/Users/user/b.wav", playlist.next().path);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(playlist.empty());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("/Users/user/c.wav", playlist.next().path);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(playlist.empty());
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

TEST_F(PredeterminedTargetPlaylistTests, reinsertCurrentRevisitsAtTheEnd) {
    fileReader.setContents(R"(/Users/user/a.wav
/Users/user/b.wav
/Users/user/c.wav
)");
    playlist.load({});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("/Users/user/a.wav", playlist.next().path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("/Users/user/b.wav", playlist.next().path);
    playlist.reinsertCurrent();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("/Users/user/c.wav", playlist.next().path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("/Users/user/b.wav", playlist.next().path);
}

TEST_F(PredeterminedTargetPlaylistTests, directoryIsThatOfCurrentTarget) {
    fileReader.setContents(R"(/Users/user/1/a.wav
/Users/user/2/b.wav
/Users/user/3/c.wav
)");
    playlist.load({});
    playlist.next();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("/Users/user/1", playlist.directory().path);
    playlist.next();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("/Users/user/2", playlist.directory().path);
}

TEST_F(PredeterminedTargetPlaylistTests,
    throwsLoadFailureIfAnyTargetsFailToBeFound) {
    fileReader.setContents(R"(/Users/user/a.wav
/Users/user/b.wav
/Users/user/c.wav
)");
    fileValidator.failOn("/Users/user/b.wav");
    try {
        playlist.load({});
        FAIL() << "Expected TargetPlaylist::LoadFailure";
    } catch (const TargetPlaylist::LoadFailure &) {
    }
}

TEST_F(PredeterminedTargetPlaylistTests, ignoresEmptyLines) {
    fileReader.setContents(R"(/Users/user/a.wav
/Users/user/b.wav

/Users/user/c.wav

    
)"); // The last line has 4 spaces
    fileValidator.failOn("");
    fileValidator.failOn("    ");
    playlist.load({});
}

TEST(PredeterminedTargetPlaylistWithFailingFileReaderTests,
    throwsLoadFailureIfFileNotFound) {
    FailingFileReader fileReader;
    FileValidatorStub fileValidator;
    PredeterminedTargetPlaylist playlist{fileReader, fileValidator};
    try {
        playlist.load({});
        FAIL() << "Expected TargetPlaylist::LoadFailure";
    } catch (const TargetPlaylist::LoadFailure &) {
    }
}
}
