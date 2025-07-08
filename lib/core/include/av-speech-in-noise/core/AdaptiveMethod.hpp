#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_ADAPTIVEMETHODHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_ADAPTIVEMETHODHPP_

#include "Randomizer.hpp"
#include "IResponseEvaluator.hpp"
#include "IAdaptiveMethod.hpp"
#include "IOutputFile.hpp"

#include <av-speech-in-noise/Interface.hpp>

#include <memory>
#include <vector>

namespace av_speech_in_noise {
struct TargetPlaylistWithTrack {
    std::shared_ptr<TargetPlaylist> list;
    std::shared_ptr<Track> track;
};

class AdaptiveMethodImpl : public AdaptiveMethod {
  public:
    AdaptiveMethodImpl(ResponseEvaluator &, Randomizer &);
    void initialize(const AdaptiveTest &, TargetPlaylistReader *,
        Track::Factory *) override;
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
    auto testResults() -> std::string override;
    void resetTracks() override;

  private:
    void selectNextList();

    std::vector<TargetPlaylistWithTrack> targetListsWithTracks;
    coordinate_response_measure::AdaptiveTrial
        lastCoordinateResponseMeasureTrial{};
    open_set::AdaptiveTrial lastOpenSetTrial{};
    CorrectKeywordsTrial lastCorrectKeywordsTrial{};
    const AdaptiveTest *test{};
    ResponseEvaluator &evaluator;
    Randomizer &randomizer;
    Track *snrTrack{};
    TargetPlaylist *targetList{};
};
}

#endif
