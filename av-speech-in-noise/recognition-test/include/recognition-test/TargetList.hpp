#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_TARGET_LIST_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_TARGET_LIST_HPP_

#include <string>

namespace av_speech_in_noise {
class TargetList {
  public:
    virtual ~TargetList() = default;
    virtual void loadFromDirectory(std::string directory) = 0;
    virtual auto next() -> std::string = 0;
    virtual auto current() -> std::string = 0;
    virtual auto empty() -> bool = 0;
    virtual void reinsertCurrent() = 0;
};
}

#endif
