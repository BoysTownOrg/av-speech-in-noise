#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_OUTPUTFILEPATHHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_OUTPUTFILEPATHHPP_

#include "OutputFile.hpp"
#include "FileSystemPath.hpp"
#include <filesystem>

namespace av_speech_in_noise {
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
    void setRelativeOutputDirectory(std::filesystem::path);

  private:
    auto outputDirectory_() -> std::string;

    std::filesystem::path relativeOutputDirectory{};
    OutputFileName &outputFileName;
    FileSystemPath &systemPath;
};
}

#endif
