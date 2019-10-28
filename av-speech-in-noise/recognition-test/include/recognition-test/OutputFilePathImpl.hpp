#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_OUTPUTFILEPATHIMPL_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_OUTPUTFILEPATHIMPL_HPP_

#include "OutputFile.hpp"
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
class TimeStamp {
  public:
    virtual ~TimeStamp() = default;
    virtual int year() = 0;
    virtual int month() = 0;
    virtual int dayOfMonth() = 0;
    virtual int hour() = 0;
    virtual int minute() = 0;
    virtual int second() = 0;
    virtual void capture() = 0;
};

class FileSystemPath {
  public:
    virtual ~FileSystemPath() = default;
    virtual std::string homeDirectory() = 0;
    virtual void createDirectory(std::string) = 0;
};

class OutputFilePathImpl : public OutputFilePath {
    std::string relativePath_{};
    TimeStamp *timeStamp;
    FileSystemPath *systemPath;

  public:
    OutputFilePathImpl(TimeStamp *, FileSystemPath *);
    std::string generateFileName(const TestIdentity &) override;
    std::string homeDirectory() override;
    std::string outputDirectory() override;
    void setRelativeOutputDirectory(std::string);

  private:
    std::string homeDirectory_();
    std::string outputDirectory_();
    std::string formatTestIdentity(const TestIdentity &);
    std::string formatTimeStamp();
};
}

#endif
