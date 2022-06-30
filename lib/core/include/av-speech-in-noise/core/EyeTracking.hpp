#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_EYETRACKINGHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_EYETRACKINGHPP_

#include "IMaskerPlayer.hpp"
#include "IOutputFile.hpp"
#include "IRecognitionTestModel.hpp"
#include "ITargetPlayer.hpp"

#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
class EyeTracker : public Writable {
  public:
    virtual void allocateRecordingTimeSeconds(double) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual auto gazeSamples() -> BinocularGazeSamples = 0;
    virtual auto currentSystemTime() -> EyeTrackerSystemTime = 0;
};

class EyeTracking : public RunningATest::Observer {
  public:
    EyeTracking(EyeTracker &, MaskerPlayer &, TargetPlayer &, OutputFile &);
    void notifyThatNewTestIsReady(std::string_view session) override;
    void notifyThatTrialWillBegin(int trialNumber) override;
    void notifyThatTargetWillPlayAt(const PlayerTimeWithDelay &) override;
    void notifyThatStimulusHasEnded() override;
    void notifyThatSubjectHasResponded() override;

  private:
    EyeTrackerTargetPlayerSynchronization
        lastEyeTrackerTargetPlayerSynchronization{};
    TargetStartTime lastTargetStartTime{};
    EyeTracker &eyeTracker;
    MaskerPlayer &maskerPlayer;
    TargetPlayer &targetPlayer;
    OutputFile &outputFile;
};
}

#endif
