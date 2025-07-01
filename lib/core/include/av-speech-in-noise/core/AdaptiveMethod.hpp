#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_ADAPTIVEMETHODHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_ADAPTIVEMETHODHPP_

#include "Randomizer.hpp"
#include "IResponseEvaluator.hpp"
#include "IAdaptiveMethod.hpp"
#include "IOutputFile.hpp"

#include <av-speech-in-noise/Interface.hpp>

#include <limits>
#include <memory>
#include <vector>

namespace av_speech_in_noise {
constexpr auto maximumInt{std::numeric_limits<int>::max()};
constexpr auto minimumInt{std::numeric_limits<int>::min()};

class Track {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Track);
    struct Settings {
        const TrackingRule *rule{};
        int startingX{};
        int ceiling{maximumInt};
        int floor{minimumInt};
        int bumpLimit{maximumInt};
    };
    virtual void down() = 0;
    virtual void up() = 0;
    virtual auto x() -> int = 0;
    virtual auto complete() -> bool = 0;
    virtual auto reversals() -> int = 0;
    virtual void reset() = 0;
    virtual auto threshold(int reversals) -> double = 0;

    class Factory {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Factory);
        virtual auto make(const Settings &) -> std::shared_ptr<Track> = 0;
    };
};

struct TargetPlaylistWithTrack {
    std::shared_ptr<TargetPlaylist> list;
    std::shared_ptr<Track> track;
};

class AdaptiveMethodImpl : public AdaptiveMethod {
  public:
    AdaptiveMethodImpl(Track::Factory &, ResponseEvaluator &, Randomizer &);
    void initialize(const AdaptiveTest &, TargetPlaylistReader *) override;
    void submitIncorrectResponse() override;
    void submitCorrectResponse() override;
    void submit(const CorrectKeywords &) override;
    void submit(const coordinate_response_measure::Response &) override;
    void writeLastCoordinateResponse(OutputFile &) override;
    void writeLastCorrectResponse(OutputFile &) override;
    void writeLastIncorrectResponse(OutputFile &) override;
    void writeLastCorrectKeywords(OutputFile &) override;
    void writeTestingParameters(OutputFile &) override;
    void writeTestResult(OutputFile &) override;
    auto snr() -> FloatSNR override;
    auto complete() -> bool override;
    auto nextTarget() -> LocalUrl override;
    auto currentTarget() -> LocalUrl override;
    auto testResults() -> AdaptiveTestResults override;
    void resetTracks() override;

  private:
    void selectNextList();

    std::vector<TargetPlaylistWithTrack> targetListsWithTracks{};
    coordinate_response_measure::AdaptiveTrial
        lastCoordinateResponseMeasureTrial{};
    open_set::AdaptiveTrial lastOpenSetTrial{};
    CorrectKeywordsTrial lastCorrectKeywordsTrial{};
    const AdaptiveTest *test{};
    Track::Factory &snrTrackFactory;
    ResponseEvaluator &evaluator;
    Randomizer &randomizer;
    Track *snrTrack{};
    TargetPlaylist *targetList{};
    int thresholdReversals{};
};
}

#endif
