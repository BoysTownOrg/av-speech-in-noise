#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGEMOTIONHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGEMOTIONHPP_

#include "IModel.hpp"
#include "IFixedLevelMethod.hpp"
#include "IRunningATest.hpp"
#include "IOutputFile.hpp"

namespace av_speech_in_noise::submitting_emotion {
class InteractorImpl : public Interactor {
  public:
    InteractorImpl(FixedLevelMethod &, RunningATest &, OutputFile &);
    void submit(const EmotionResponse &) override;

  private:
    FixedLevelMethod &method;
    RunningATest &model;
    OutputFile &outputFile;
};
}

#endif
