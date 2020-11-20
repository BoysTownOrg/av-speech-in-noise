#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_OUTPUTFILEPATH_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_OUTPUTFILEPATH_HPP_

#include "OutputFile.hpp"

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
    virtual auto homeDirectory() -> std::string = 0;
    virtual void createDirectory(std::string) = 0;
};

class OutputFileName {
  public:
    OutputFileName(TimeStamp &timeStamp);
    auto generate(const TestIdentity &identity) -> std::string;

  private:
    TimeStamp &timeStamp;
};

class OutputFilePathImpl : public OutputFilePath {
  public:
    OutputFilePathImpl(TimeStamp &, FileSystemPath &);
    auto generateFileName(const TestIdentity &) -> std::string override;
    auto homeDirectory() -> std::string override;
    auto outputDirectory() -> std::string override;
    void setRelativeOutputDirectory(std::string);

  private:
    auto homeDirectory_() -> std::string;
    auto outputDirectory_() -> std::string;

    OutputFileName outputFileName;
    std::string relativePath_{};
    FileSystemPath &systemPath;
};
}

#endif
