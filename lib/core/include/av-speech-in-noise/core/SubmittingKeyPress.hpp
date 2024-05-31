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
    auto submits(const std::vector<KeyPressResponse> &) -> bool override;
    void forceSubmit(const std::vector<KeyPressResponse> &) override;
    void notifyThatTargetWillPlayAt(const PlayerTimeWithDelay &) override;
    void notifyThatTrialWillBegin(int) override;
    void notifyThatStimulusHasEnded() override;

  private:
    void writeSaveAndReadyNextTrial(KeyPressTrial &);

    VibrotactileStimulus lastVibrotactileStimulus;
    FixedLevelMethod &method;
    RunningATest &model;
    OutputFile &outputFile;
    MaskerPlayer &maskerPlayer;
    Randomizer &randomizer;
    double vibrotactileStartTimeMilliseconds{};
    bool readyForResponse{};
};
}

#endif
