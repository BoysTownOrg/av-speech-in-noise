#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGKEYPRESSHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGKEYPRESSHPP_

#include "IFixedLevelMethod.hpp"
#include "IMaskerPlayer.hpp"
#include "IModel.hpp"
#include "IRunningATest.hpp"
#include "IOutputFile.hpp"

namespace av_speech_in_noise::submitting_keypress {
class InteractorImpl : public Interactor {
  public:
    InteractorImpl(
        FixedLevelMethod &, RunningATest &, OutputFile &, MaskerPlayer &);
    void submit(const KeyPressResponse &) override;
    void notifyThatTargetWillPlayAt(const PlayerTimeWithDelay &);

  private:
    FixedLevelMethod &method;
    RunningATest &model;
    OutputFile &outputFile;
    MaskerPlayer &maskerPlayer;
    double targetStartTimeMilliseconds{};
};
}

#endif
