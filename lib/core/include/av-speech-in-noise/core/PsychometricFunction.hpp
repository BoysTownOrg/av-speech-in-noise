#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_PSYCHOMETRICFUNCTIONHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_PSYCHOMETRICFUNCTIONHPP_

#include <av-speech-in-noise/Interface.hpp>

struct Phi {
    double alpha;
    double beta;
    double gamma;
    double lambda;
};

class PsychometricFunction {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(PsychometricFunction);
    virtual auto operator()(Phi phi, double x) -> double = 0;
};

#endif
