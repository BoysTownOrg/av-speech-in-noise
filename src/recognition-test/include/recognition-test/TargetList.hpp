#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_TARGET_LIST_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_TARGET_LIST_HPP_

#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class TargetList {
  public:
    virtual ~TargetList() = default;
    virtual void loadFromDirectory(const LocalUrl &directory) = 0;
    virtual auto next() -> LocalUrl = 0;
    virtual auto current() -> LocalUrl = 0;
    virtual auto directory() -> LocalUrl = 0;
};

class FiniteTargetList : public virtual TargetList {
  public:
    virtual auto empty() -> bool = 0;
    virtual void reinsertCurrent() = 0;
};
}

#endif
