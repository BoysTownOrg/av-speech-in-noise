#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_TEST_METHOD_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_TEST_METHOD_HPP_

#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/Interface.hpp>

namespace av_speech_in_noise {
class OutputFile;

class TestMethod {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TestMethod);
    virtual auto complete() -> bool = 0;
    virtual auto nextTarget() -> LocalUrl = 0;
    virtual auto currentTarget() -> LocalUrl = 0;
    virtual auto snr() -> SNR = 0;
    virtual void submit(const FreeResponse &) = 0;
    virtual void submit(const coordinate_response_measure::Response &) = 0;
    virtual void submit(const ThreeKeywordsResponse &) {}
    virtual void submit(const SyllableResponse &) {}
    virtual void writeTestingParameters(OutputFile &) = 0;
    virtual void writeLastCoordinateResponse(OutputFile &) = 0;
    virtual void writeTestResult(OutputFile &) = 0;
};
}

#endif
