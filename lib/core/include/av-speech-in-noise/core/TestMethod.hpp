#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_TESTMETHODHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_TESTMETHODHPP_

#include "IOutputFile.hpp"

#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/Interface.hpp>

namespace av_speech_in_noise {
class TestMethod : public Writable {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TestMethod);
    virtual auto complete() -> bool = 0;
    virtual auto nextTarget() -> LocalUrl = 0;
    virtual auto currentTarget() -> LocalUrl = 0;
    virtual auto snr() -> FloatSNR = 0;
    virtual void submit(const coordinate_response_measure::Response &) = 0;
    virtual void writeLastCoordinateResponse(OutputFile &) = 0;
    virtual void writeTestResult(OutputFile &) = 0;
};
}

#endif
