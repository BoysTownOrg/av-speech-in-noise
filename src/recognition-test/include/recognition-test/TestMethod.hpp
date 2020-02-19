#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_TEST_METHOD_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_TEST_METHOD_HPP_

#include "Model.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class OutputFile {
  public:
    virtual ~OutputFile() = default;
    virtual void openNewFile(const TestIdentity &) = 0;
    class OpenFailure {};
    virtual void write(
        const coordinate_response_measure::AdaptiveTrial &) = 0;
    virtual void write(
        const coordinate_response_measure::FixedLevelTrial &) = 0;
    virtual void write(const open_set::FreeResponseTrial &) = 0;
    virtual void write(const open_set::CorrectKeywordsTrial &) = 0;
    virtual void write(const open_set::AdaptiveTrial &) = 0;
    virtual void writeTest(const AdaptiveTest &) = 0;
    virtual void writeTest(const FixedLevelTest &) = 0;
    virtual void write(const BinocularGazeSamples &) = 0;
    virtual void close() = 0;
    virtual void save() = 0;
};

class TestMethod {
  public:
    virtual ~TestMethod() = default;
    virtual auto complete() -> bool = 0;
    virtual auto nextTarget() -> std::string = 0;
    virtual auto currentTarget() -> std::string = 0;
    virtual auto snr_dB() -> int = 0;
    virtual void submitCorrectResponse() = 0;
    virtual void submitIncorrectResponse() = 0;
    virtual void submit(const open_set::FreeResponse &) = 0;
    virtual void submit(const open_set::CorrectKeywords &) = 0;
    virtual void writeTestingParameters(OutputFile *) = 0;
    virtual void writeLastCoordinateResponse(OutputFile *) = 0;
    virtual void writeLastCorrectResponse(OutputFile *) = 0;
    virtual void writeLastCorrectKeywords(OutputFile *) = 0;
    virtual void writeLastIncorrectResponse(OutputFile *) = 0;
    virtual void submit(
        const coordinate_response_measure::Response &) = 0;
};
}

#endif
