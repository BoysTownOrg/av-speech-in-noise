#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_FILESYSTEMPATHHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_FILESYSTEMPATHHPP_

#include <av-speech-in-noise/Interface.hpp>

#include <filesystem>

namespace av_speech_in_noise {
class TimeStamp {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TimeStamp);
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
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(FileSystemPath);
    virtual auto homeDirectory() -> std::filesystem::path = 0;
    virtual void createDirectory(const std::filesystem::path &) = 0;
};
}

#endif
