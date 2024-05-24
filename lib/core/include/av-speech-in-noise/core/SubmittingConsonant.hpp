#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGCONSONANTHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGCONSONANTHPP_

#include "IModel.hpp"
#include "IFixedLevelMethod.hpp"
#include "IRunningATest.hpp"
#include "IOutputFile.hpp"

#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise::submitting_consonant {
class InteractorImpl : public Interactor {
  public:
    InteractorImpl(FixedLevelMethod &, RunningATest &, OutputFile &);
    void submit(const ConsonantResponse &r) override;

  private:
    FixedLevelMethod &method;
    RunningATest &model;
    OutputFile &outputFile;
};
}

#endif
