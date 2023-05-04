#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGFREERESPONSEHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGFREERESPONSEHPP_

#include "IModel.hpp"
#include "FixedLevelMethod.hpp"
#include "IRunningATest.hpp"
#include "IOutputFile.hpp"

namespace av_speech_in_noise::submitting_free_response {
class InteractorImpl : public Interactor {
  public:
    InteractorImpl(FixedLevelMethod &, RunningATest &, OutputFile &);
    void submit(const FreeResponse &) override;

  private:
    FixedLevelMethod &method;
    RunningATest &model;
    OutputFile &outputFile;
};
}

#endif
