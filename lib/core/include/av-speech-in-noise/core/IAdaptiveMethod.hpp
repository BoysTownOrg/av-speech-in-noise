#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_IADAPTIVEMETHODHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_IADAPTIVEMETHODHPP_

#include "TargetPlaylist.hpp"
#include "TestMethod.hpp"
#include <vector>
#include <memory>

namespace av_speech_in_noise {
class TargetPlaylistReader {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TargetPlaylistReader);
    using lists_type = typename std::vector<std::shared_ptr<TargetPlaylist>>;
    virtual auto read(const LocalUrl &) -> lists_type = 0;
};

class AdaptiveMethod : public virtual TestMethod {
  public:
    virtual void initialize(const AdaptiveTest &, TargetPlaylistReader *) = 0;
    virtual void resetTracks() = 0;
    virtual auto testResults() -> AdaptiveTestResults = 0;
    using TestMethod::submit;
    virtual void submit(const CorrectKeywords &) = 0;
    virtual void writeLastCorrectKeywords(OutputFile &) = 0;
    virtual void submitCorrectResponse() = 0;
    virtual void submitIncorrectResponse() = 0;
    virtual void writeLastCorrectResponse(OutputFile &) = 0;
    virtual void writeLastIncorrectResponse(OutputFile &) = 0;
};
}

#endif
