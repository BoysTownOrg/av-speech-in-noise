#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_ADAPTIVEMETHODHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_ADAPTIVEMETHODHPP_

#include "Configuration.hpp"
#include "Randomizer.hpp"
#include "IResponseEvaluator.hpp"
#include "IAdaptiveMethod.hpp"
#include "IOutputFile.hpp"
#include "av-speech-in-noise/Model.hpp"

#include <av-speech-in-noise/Interface.hpp>

#include <memory>
#include <vector>

namespace av_speech_in_noise {
struct TargetPlaylistWithTrack {
    std::shared_ptr<TargetPlaylist> list;
    std::shared_ptr<AdaptiveTrack> track;
};

class AdaptiveMethodImpl : public AdaptiveMethod, public Configurable {
  public:
    AdaptiveMethodImpl(
        ConfigurationRegistry &, ResponseEvaluator &, Randomizer &);
    void initialize(const AdaptiveTest &, TargetPlaylistReader *,
        AdaptiveTrack::Factory *) override;
    void submitIncorrectResponse() override;
    void submitCorrectResponse() override;
    void submit(const CorrectKeywords &) override;
    void submit(const coordinate_response_measure::Response &) override;
    void writeLastCoordinateResponse(OutputFile &) override;
    void writeLastCorrectResponse(OutputFile &) override;
    void writeLastIncorrectResponse(OutputFile &) override;
    void writeLastCorrectKeywords(OutputFile &) override;
    void writeTestResult(OutputFile &) override;
    void write(std::ostream &) override;
    auto snr() -> FloatSNR override;
    auto complete() -> bool override;
    auto nextTarget() -> LocalUrl override;
    auto currentTarget() -> LocalUrl override;
    auto testResults() -> AdaptiveTestResults override;
    void resetTracks() override;
    void configure(const std::string &, const std::string &) override;

  private:
    void selectNextList();

    std::vector<TargetPlaylistWithTrack> targetListsWithTracks;
    coordinate_response_measure::AdaptiveTrial
        lastCoordinateResponseMeasureTrial{};
    open_set::AdaptiveTrial lastOpenSetTrial{};
    CorrectKeywordsTrial lastCorrectKeywordsTrial{};
    LocalUrl targetsUrl;
    const AdaptiveTest *test{};
    FloatSNR startingSNR;
    ResponseEvaluator &evaluator;
    Randomizer &randomizer;
    AdaptiveTrack *snrTrack{};
    TargetPlaylist *targetList{};
    AdaptiveTrack::Factory *adaptiveTrackFactory{};
};
}

#endif
