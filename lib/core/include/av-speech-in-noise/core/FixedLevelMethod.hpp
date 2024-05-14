#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_FIXEDLEVELMETHODHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_FIXEDLEVELMETHODHPP_

#include "IResponseEvaluator.hpp"
#include "IFixedLevelMethod.hpp"
#include "OutputFile.hpp"

namespace av_speech_in_noise {
class FixedLevelMethodImpl : public FixedLevelMethod {
  public:
    explicit FixedLevelMethodImpl(ResponseEvaluator &);
    void initialize(const FixedLevelTest &, FiniteTargetPlaylist *) override;
    void initialize(
        const FixedLevelFixedTrialsTest &, TargetPlaylist *) override;
    void initialize(
        const FixedLevelTest &, FiniteTargetPlaylistWithRepeatables *) override;
    void submit(const coordinate_response_measure::Response &) override;
    void submit(const FreeResponse &) override;
    void submit(const KeyPressResponse &) override { /* TODO */
    }
    void submit(const ConsonantResponse &) override;
    void submit(const ThreeKeywordsResponse &) override;
    void submit(const SyllableResponse &) override;
    void writeLastCoordinateResponse(OutputFile &) override;
    void writeTestingParameters(OutputFile &) override;
    void writeTestResult(OutputFile &) override {}
    void writeLastConsonant(OutputFile &) override;
    auto snr() -> SNR override;
    auto nextTarget() -> LocalUrl override;
    auto currentTarget() -> LocalUrl override;
    auto complete() -> bool override;
    auto keywordsTestResults() -> KeywordsTestResults override;

  private:
    coordinate_response_measure::FixedLevelTrial
        lastCoordinateResponseMeasureTrial{};
    ConsonantTrial lastConsonantTrial{};
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
