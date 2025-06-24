#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_LOGISTICPSYCHOMETRICFUNCTIONHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_LOGISTICPSYCHOMETRICFUNCTIONHPP_

#include <av-speech-in-noise/Interface.hpp>
#include <vector>

namespace av_speech_in_noise {
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

class LogisticPsychometricFunction : public PsychometricFunction {
  public:
    auto operator()(Phi phi, double x) -> double override;
};

class SweetPoints {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(SweetPoints);
    virtual auto operator()(Phi phi) -> std::vector<double> = 0;
};

class LogisticSweetPoints : public SweetPoints {
  public:
    auto operator()(Phi phi) -> std::vector<double> override;

  private:
    auto alphaEstimate(Phi phi, double x) -> double;
    auto betaEstimate(Phi phi, double x) -> double;
};
}

#endif
