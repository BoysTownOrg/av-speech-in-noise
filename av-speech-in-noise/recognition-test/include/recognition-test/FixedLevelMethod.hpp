#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_FIXEDLEVELMETHOD_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_FIXEDLEVELMETHOD_HPP_

#include "RecognitionTestModel.hpp"

namespace av_speech_in_noise {
class EmptyTargetListTestConcluder : public TestConcluder {
  public:
    void initialize(const FixedLevelTest &) override {}

    void submitResponse() override {}

    bool complete(TargetList *t) override { return t->empty(); }
};

class FixedTrialTestConcluder : public TestConcluder {
    int trials_{};

  public:
    void initialize(const FixedLevelTest &p) override { trials_ = p.trials; }

    void submitResponse() override { --trials_; }

    bool complete(TargetList *) override { return trials_ == 0; }
};

class FixedLevelMethod : public IFixedLevelMethod {
    coordinate_response_measure::FixedLevelTrial lastTrial{};
    const FixedLevelTest *test{};
    TargetList *targetList;
    ResponseEvaluator *evaluator;
    TestConcluder *concluder;
    int snr_dB_{};

  public:
    explicit FixedLevelMethod(ResponseEvaluator *);
    void initialize(
        const FixedLevelTest &, TargetList *, TestConcluder *) override;
    int snr_dB() override;
    std::string next() override;
    bool complete() override;
    std::string current() override;
    void submitIncorrectResponse() override;
    void submitCorrectResponse() override;
    void writeLastCoordinateResponse(OutputFile *) override;
    void writeLastCorrectResponse(OutputFile *) override {}
    void writeLastIncorrectResponse(OutputFile *) override {}
    void writeTestingParameters(OutputFile *) override;
    void submitResponse(
        const coordinate_response_measure::Response &) override;
    void submitResponse(const FreeResponse &) override;
};
}
#endif
