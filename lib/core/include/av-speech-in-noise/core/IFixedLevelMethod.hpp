#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_IFIXEDLEVELMETHODHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_IFIXEDLEVELMETHODHPP_

#include "TargetPlaylist.hpp"
#include "TestMethod.hpp"

namespace av_speech_in_noise {
class FixedLevelMethod : public virtual TestMethod {
  public:
    virtual void initialize(FiniteTargetPlaylist *) = 0;
    virtual void initialize(FiniteTargetPlaylistWithRepeatables *) = 0;
    virtual void initialize(
        const FixedLevelFixedTrialsTest &, TargetPlaylist *) = 0;
    virtual void submit(const Flaggable &) = 0;
    virtual void submit(const ThreeKeywordsResponse &) = 0;
    using TestMethod::submit;
    virtual auto keywordsTestResults() -> KeywordsTestResults = 0;
};
}

#endif
