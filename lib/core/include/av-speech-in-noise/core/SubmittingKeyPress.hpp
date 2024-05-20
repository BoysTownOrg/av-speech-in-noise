#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGKEYPRESSHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGKEYPRESSHPP_

#include "IFixedLevelMethod.hpp"
#include "IMaskerPlayer.hpp"
#include "IModel.hpp"
#include "IRunningATest.hpp"
#include "IOutputFile.hpp"
#include "Randomizer.hpp"

namespace av_speech_in_noise::submitting_keypress {
class InteractorImpl : public Interactor, public RunningATest::TestObserver {
  public:
    InteractorImpl(FixedLevelMethod &, RunningATest &, OutputFile &,
        MaskerPlayer &, Randomizer &);
    void submit(const std::vector<KeyPressResponse> &) override;
    void notifyThatTargetWillPlayAt(const PlayerTimeWithDelay &) override;
    void notifyThatTrialWillBegin(int) override;

  private:
    FixedLevelMethod &method;
    RunningATest &model;
    OutputFile &outputFile;
    MaskerPlayer &maskerPlayer;
    Randomizer &randomizer;
    double targetStartTimeMilliseconds{};
};
}

#endif
