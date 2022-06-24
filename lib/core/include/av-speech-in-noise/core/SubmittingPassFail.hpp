#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGPASSFAILHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGPASSFAILHPP_

#include "IModel.hpp"
#include "IAdaptiveMethod.hpp"
#include "IOutputFile.hpp"
#include "IRecognitionTestModel.hpp"

namespace av_speech_in_noise::submitting_pass_fail {
class InteractorImpl : public Interactor {
  public:
    InteractorImpl(AdaptiveMethod &, RunningATest &, OutputFile &);
    void submitCorrectResponse() override;
    void submitIncorrectResponse() override;

  private:
    AdaptiveMethod &adaptiveMethod;
    RunningATest &model;
    OutputFile &outputFile;
};
}

#endif
