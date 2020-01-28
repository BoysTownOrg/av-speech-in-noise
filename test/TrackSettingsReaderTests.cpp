#include "assert-utility.hpp"
#include <recognition-test/TrackSettingsReader.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise {
namespace {
class TextFileReaderStub : public TextFileReader {
  public:
    [[nodiscard]] auto filePath() const { return filePath_; }

    auto read(std::string s) -> std::string override {
        filePath_ = std::move(s);
        return contents_;
    }

    void setContents(std::string s) { contents_ = std::move(s); }

  private:
    std::string filePath_{};
    std::string contents_{};
};

class TrackSettingsInterpreterStub : public TrackSettingsInterpreter {
  public:
    auto trackingRule(std::string s) -> const TrackingRule * override {
        content_ = std::move(s);
        return trackingRule_;
    }

    [[nodiscard]] auto content() const { return content_; }

    void setTrackingRule(const TrackingRule *r) { trackingRule_ = r; }

  private:
    std::string content_{};
    const TrackingRule *trackingRule_{};
};

class TrackSettingsReaderTests : public ::testing::Test {
  protected:
    TextFileReaderStub fileReader;
    TrackSettingsInterpreterStub interpreter;
    TrackSettingsReaderImpl reader{&fileReader, &interpreter};

    auto read(std::string s = {}) { return reader.read(std::move(s)); }
};

TEST_F(TrackSettingsReaderTests, readPassesFilePathToFileReader) {
    read("a");
    assertEqual("a", fileReader.filePath());
}

TEST_F(TrackSettingsReaderTests, readPassesContentsToInterpreter) {
    fileReader.setContents("a");
    read();
    assertEqual("a", interpreter.content());
}

TEST_F(TrackSettingsReaderTests, readReturnsInterpretedResult) {
    TrackingRule trackingRule;
    interpreter.setTrackingRule(&trackingRule);
    assertEqual(&std::as_const(trackingRule), read());
}
}
}
