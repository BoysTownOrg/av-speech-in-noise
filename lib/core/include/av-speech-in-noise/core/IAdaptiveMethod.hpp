#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_IADAPTIVEMETHODHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_IADAPTIVEMETHODHPP_

#include "TargetPlaylist.hpp"
#include "TestMethod.hpp"

#include <limits>
#include <vector>
#include <memory>

namespace av_speech_in_noise {
class TargetPlaylistReader {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TargetPlaylistReader);
    using lists_type = typename std::vector<std::shared_ptr<TargetPlaylist>>;
    virtual auto read(const LocalUrl &) -> lists_type = 0;
};

constexpr auto maximumInt{std::numeric_limits<int>::max()};
constexpr auto minimumInt{std::numeric_limits<int>::min()};

class AdaptiveTrack {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(AdaptiveTrack);
    struct Settings {
        const TrackingRule *rule{};
        int startingX{};
        int ceiling{maximumInt};
        int floor{minimumInt};
        int bumpLimit{maximumInt};
        int thresholdReversals;
        int trials;
    };
    virtual void down() = 0;
    virtual void up() = 0;
    virtual auto x() -> double = 0;
    virtual auto complete() -> bool = 0;
    virtual auto reversals() -> int = 0;
    virtual void reset() = 0;
    virtual auto result() -> std::variant<Threshold, Phi> = 0;

    class Factory {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Factory);
        virtual auto make(const Settings &)
            -> std::shared_ptr<AdaptiveTrack> = 0;
    };
};

class AdaptiveMethod : public virtual TestMethod {
  public:
    virtual void initialize(const AdaptiveTest &, TargetPlaylistReader *,
        AdaptiveTrack::Factory *) = 0;
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
