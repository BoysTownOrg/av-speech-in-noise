#ifndef AV_SPEECH_IN_NOISE_TEST_ADAPTIVEMETHODSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TEST_ADAPTIVEMETHODSTUB_HPP_

#include <av-speech-in-noise/core/IAdaptiveMethod.hpp>

namespace av_speech_in_noise {
class AdaptiveMethodStub : public AdaptiveMethod {
  public:
    void initialize(
        const AdaptiveTest &t, TargetPlaylistReader *reader) override {
        test_ = t;
        targetListReader_ = reader;
    }
    auto testResults() -> AdaptiveTestResults override { return testResults_; }
    void resetTracks() override { tracksResetted_ = true; }
    auto complete() -> bool override { return {}; }
    auto nextTarget() -> LocalUrl override { return {}; }
    auto currentTarget() -> LocalUrl override { return {}; }
    auto snr() -> SNR override { return SNR{}; }
    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
    void submit(const CorrectKeywords &) override {}
    void writeTestingParameters(OutputFile &) override {}
    void writeLastCoordinateResponse(OutputFile &) override {}
    void writeLastCorrectResponse(OutputFile &) override {}
    void writeLastIncorrectResponse(OutputFile &) override {}
    void writeLastCorrectKeywords(OutputFile &) override {}
    void writeTestResult(OutputFile &) override {}
    void submit(const coordinate_response_measure::Response &) override {}

    AdaptiveTestResults testResults_;
    AdaptiveTest test_{};
    TargetPlaylistReader *targetListReader_{};
    bool tracksResetted_{};
};
}

#endif
