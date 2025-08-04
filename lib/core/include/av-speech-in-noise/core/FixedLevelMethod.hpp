#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_FIXEDLEVELMETHODHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_FIXEDLEVELMETHODHPP_

#include "Configuration.hpp"
#include "IResponseEvaluator.hpp"
#include "IFixedLevelMethod.hpp"

#include <av-speech-in-noise/core/IOutputFile.hpp>

namespace av_speech_in_noise {
class FixedLevelMethodImpl : public FixedLevelMethod, public Configurable {
  public:
    FixedLevelMethodImpl(ConfigurationRegistry &, ResponseEvaluator &);
    void initialize(const FixedLevelTest &, FiniteTargetPlaylist *) override;
    void initialize(
        const FixedLevelFixedTrialsTest &, TargetPlaylist *) override;
    void initialize(
        const FixedLevelTest &, FiniteTargetPlaylistWithRepeatables *) override;
    void submit(const coordinate_response_measure::Response &) override;
    void submit(const Flaggable &) override;
    void submit(const ThreeKeywordsResponse &) override;
    void writeLastCoordinateResponse(OutputFile &) override;
    void write(std::ostream &) override;
    void writeTestResult(OutputFile &) override {}
    auto snr() -> FloatSNR override;
    auto nextTarget() -> LocalUrl override;
    auto currentTarget() -> LocalUrl override;
    auto complete() -> bool override;
    auto keywordsTestResults() -> KeywordsTestResults override;
    void configure(const std::string &, const std::string &) override;

  private:
    coordinate_response_measure::FixedLevelTrial
        lastCoordinateResponseMeasureTrial{};
    LocalUrl targetsUrl;
    const FixedLevelTest *test_{};
    TargetPlaylist *targetList{};
    FiniteTargetPlaylist *finiteTargetPlaylist{};
    FiniteTargetPlaylistWithRepeatables *finiteTargetPlaylistWithRepeatables{};
    ResponseEvaluator &evaluator;
    SNR snr_{};
    int totalKeywordsCorrect_{};
    int totalKeywordsSubmitted_{};
    int trials_{};
    bool finiteTargetsExhausted_{};
    bool usingFiniteTargetPlaylist_{};
};
}

#endif
