#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_IFIXEDLEVELMETHOD_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_IFIXEDLEVELMETHOD_HPP_

#include "TargetPlaylist.hpp"
#include "TestMethod.hpp"

namespace av_speech_in_noise {
class FixedLevelMethod : public virtual TestMethod {
  public:
    virtual void initialize(const FixedLevelTest &, FiniteTargetPlaylist *) = 0;
    virtual void initialize(
        const FixedLevelTest &, FiniteTargetPlaylistWithRepeatables *) = 0;
    virtual void initialize(
        const FixedLevelFixedTrialsTest &, TargetPlaylist *) = 0;
    virtual void submit(const ConsonantResponse &) = 0;
    virtual void writeLastConsonant(OutputFile &) = 0;
    virtual auto keywordsTestResults() -> KeywordsTestResults = 0;
};
}

#endif
