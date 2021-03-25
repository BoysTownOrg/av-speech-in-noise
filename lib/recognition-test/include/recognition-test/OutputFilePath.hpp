#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_OUTPUTFILEPATH_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_OUTPUTFILEPATH_HPP_

#include "OutputFile.hpp"
#include <filesystem>

namespace av_speech_in_noise {
class TimeStamp {
  public:
    virtual ~TimeStamp() = default;
    virtual auto year() -> int = 0;
    virtual auto month() -> int = 0;
    virtual auto dayOfMonth() -> int = 0;
    virtual auto hour() -> int = 0;
    virtual auto minute() -> int = 0;
    virtual auto second() -> int = 0;
    virtual void capture() = 0;
};

class FileSystemPath {
  public:
    virtual ~FileSystemPath() = default;
    virtual auto homeDirectory() -> std::filesystem::path = 0;
    virtual void createDirectory(const std::filesystem::path &) = 0;
};

class OutputFileName {
  public:
    virtual ~OutputFileName() = default;
    virtual auto generate(const TestIdentity &) -> std::string = 0;
};

class DefaultOutputFileName : public OutputFileName {
  public:
    explicit DefaultOutputFileName(TimeStamp &timeStamp);
    auto generate(const TestIdentity &identity) -> std::string override;

  private:
    TimeStamp &timeStamp;
};

class MetaConditionOutputFileName : public OutputFileName {
  public:
    explicit MetaConditionOutputFileName(TimeStamp &timeStamp);
    auto generate(const TestIdentity &identity) -> std::string override;

  private:
    TimeStamp &timeStamp;
};

class OutputFilePathImpl : public OutputFilePath {
  public:
    OutputFilePathImpl(OutputFileName &, FileSystemPath &);
    auto generateFileName(const TestIdentity &) -> std::string override;
    auto outputDirectory() -> std::string override;
    void setRelativeOutputDirectory(std::string);

  private:
    auto outputDirectory_() -> std::string;

    std::string relativePath_{};
    OutputFileName &outputFileName;
    FileSystemPath &systemPath;
};
}

#endif
