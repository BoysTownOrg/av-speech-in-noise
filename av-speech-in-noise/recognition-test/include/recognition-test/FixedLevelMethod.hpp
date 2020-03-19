#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_FIXEDLEVELMETHOD_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_FIXEDLEVELMETHOD_HPP_

#include "Model.hpp"

namespace av_speech_in_noise {
class FixedLevelMethodImpl : public FixedLevelMethod {
  public:
    explicit FixedLevelMethodImpl(ResponseEvaluator *);
    void initialize(const FixedLevelTest &, TargetList *) override;
    void initialize(const FixedLevelTest &, FiniteTargetList *) override;
    void submit(const coordinate_response_measure::Response &) override;
    void submit(const FreeResponse &) override;
    void submit(const CorrectKeywords &) override {}
    void submitIncorrectResponse() override;
    void submitCorrectResponse() override;
    void writeLastCoordinateResponse(OutputFile *) override;
    void writeLastCorrectResponse(OutputFile *) override {}
    void writeLastIncorrectResponse(OutputFile *) override {}
    void writeLastCorrectKeywords(OutputFile *) override {}
    void writeTestingParameters(OutputFile *) override;
    void writeTestResult(OutputFile *) override {}
    auto snr_dB() -> int override;
    auto nextTarget() -> std::string override;
    auto complete() -> bool override;
    auto currentTarget() -> std::string override;

  private:
    coordinate_response_measure::FixedLevelTrial lastTrial{};
    const FixedLevelTest *test{};
    TargetList *targetList{};
    FiniteTargetList *finiteTargetList{};
    ResponseEvaluator *evaluator;
    int snr_dB_{};
    int trials_{};
    bool finiteTargetsExhausted_{};
    bool usingFiniteTargetList_{};
};
}

#endif
