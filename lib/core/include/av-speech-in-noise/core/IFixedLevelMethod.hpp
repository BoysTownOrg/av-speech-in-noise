#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_IFIXEDLEVELMETHODHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_IFIXEDLEVELMETHODHPP_

#include "TargetPlaylist.hpp"
#include "TestMethod.hpp"

namespace av_speech_in_noise {
struct FixedLevelFixedTrialsTest {
    int trials{30};
};

class FixedLevelMethod : public virtual TestMethod {
  public:
    virtual void attach(FiniteTargetPlaylist *) = 0;
    virtual void attach(FiniteTargetPlaylistWithRepeatables *) = 0;
    virtual void attach(
        const FixedLevelFixedTrialsTest &, TargetPlaylist *) = 0;
    virtual void submit(const Flaggable &) = 0;
    virtual void submit(const ThreeKeywordsResponse &) = 0;
    using TestMethod::submit;
    virtual auto keywordsTestResults() -> KeywordsTestResults = 0;
};
}

#endif
