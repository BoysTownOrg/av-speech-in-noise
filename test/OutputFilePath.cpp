#include "LogString.hpp"
#include "assert-utility.hpp"
#include <gtest/gtest.h>
#include <recognition-test/OutputFilePath.hpp>

namespace av_speech_in_noise {
namespace {
class TimeStampStub : public TimeStamp {
  public:
    void capture() override { insert(log_, "capture "); }

    auto log() const -> const std::stringstream & { return log_; }

    void setYear(int y) { year_ = y; }

    void setMonth(int y) { month_ = y; }

    void setDayOfMonth(int y) { dayOfMonth_ = y; }

    void setHour(int y) { hour_ = y; }

    void setMinute(int y) { minute_ = y; }

    void setSecond(int y) { second_ = y; }

    auto year() -> int override {
        insert(log_, "year ");
        return year_;
    }

    auto month() -> int override {
        insert(log_, "month ");
        return month_;
    }

    auto dayOfMonth() -> int override {
        insert(log_, "dayOfMonth ");
        return dayOfMonth_;
    }

    auto hour() -> int override {
        insert(log_, "hour ");
        return hour_;
    }

    auto minute() -> int override {
        insert(log_, "minute ");
        return minute_;
    }

    auto second() -> int override {
        insert(log_, "second ");
        return second_;
    }

  private:
    std::stringstream log_{};
    int year_{};
    int month_{};
    int dayOfMonth_{};
    int hour_{};
    int minute_{};
    int second_{};
};

class FileSystemPathStub : public FileSystemPath {
    std::string homeDirectory_{};
    std::string directoryCreated_{};

  public:
    void setHomeDirectory(std::string s) { homeDirectory_ = std::move(s); }

    auto homeDirectory() -> std::string override { return homeDirectory_; }

    [[nodiscard]] auto directoryCreated() const { return directoryCreated_; }

    void createDirectory(std::string s) override {
        directoryCreated_ = std::move(s);
    }
};

auto generate(OutputFileName &fileName, const TestIdentity &identity)
    -> std::string {
    return fileName.generate(identity);
}

void setHomeDirectory(FileSystemPathStub &systemPath, std::string s) {
    systemPath.setHomeDirectory(std::move(s));
}

void setRelativeOutputDirectory(OutputFilePathImpl &path, std::string s) {
    path.setRelativeOutputDirectory(std::move(s));
}

class OutputFilePathTests : public ::testing::Test {
  protected:
    TimeStampStub timeStamp;
    FileSystemPathStub systemPath;
    OutputFilePathImpl path{timeStamp, systemPath};
};

class OutputFileNameTests : public ::testing::Test {
  protected:
    TimeStampStub timeStamp;
    OutputFileName fileName{timeStamp};
    TestIdentity identity{};
};

TEST_F(OutputFileNameTests, generateFileNameFormatsTestInformationAndTime) {
    identity.subjectId = "a";
    identity.session = "b";
    identity.testerId = "c";
    timeStamp.setYear(1);
    timeStamp.setMonth(2);
    timeStamp.setDayOfMonth(3);
    timeStamp.setHour(4);
    timeStamp.setMinute(5);
    timeStamp.setSecond(6);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"Subject_a_Session_b_Experimenter_c_1-2-3-4-5-6"},
        generate(fileName, identity));
}

TEST_F(OutputFileNameTests, generateFileNameCapturesTimePriorToQueries) {
    generate(fileName, identity);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(beginsWith(timeStamp.log(), "capture"));
}

TEST_F(OutputFilePathTests, outputDirectoryReturnsFullPath) {
    setHomeDirectory(systemPath, "a");
    setRelativeOutputDirectory(path, "b");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a/b"}, path.outputDirectory());
}

TEST_F(OutputFilePathTests, setRelativeOutputDirectoryCreatesDirectory) {
    setHomeDirectory(systemPath, "a");
    setRelativeOutputDirectory(path, "b");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a/b"}, systemPath.directoryCreated());
}
}
}
