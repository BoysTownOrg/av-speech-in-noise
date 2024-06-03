#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGFIXEDPASSFAILHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGFIXEDPASSFAILHPP_

#include "IFixedLevelMethod.hpp"
#include "IModel.hpp"
#include "IOutputFile.hpp"
#include "IRunningATest.hpp"

namespace av_speech_in_noise::submitting_fixed_pass_fail {
class InteractorImpl : public Interactor {
  public:
    InteractorImpl(FixedLevelMethod &, RunningATest &, OutputFile &);
    void submitCorrectResponse() override;
    void submitIncorrectResponse() override;

  private:
    void submit(bool correct);

    FixedLevelMethod &method;
    RunningATest &model;
    OutputFile &outputFile;
};
}

#endif
