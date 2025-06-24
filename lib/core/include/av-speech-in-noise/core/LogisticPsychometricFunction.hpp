#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_LOGISTICPSYCHOMETRICFUNCTIONHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_LOGISTICPSYCHOMETRICFUNCTIONHPP_

#include "PsychometricFunction.hpp"

class LogisticPsychometricFunction : public PsychometricFunction {
  public:
    auto operator()(Phi phi, double x) -> double override;
};

#endif
