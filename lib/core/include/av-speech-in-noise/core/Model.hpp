#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_MODELHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_MODELHPP_

#include "TargetPlaylist.hpp"
#include "IAdaptiveMethod.hpp"
#include "IFixedLevelMethod.hpp"
#include "IModel.hpp"
#include "IOutputFile.hpp"
#include "IRecognitionTestModel.hpp"
#include "TestMethod.hpp"
#include "av-speech-in-noise/Model.hpp"

namespace av_speech_in_noise {
class RunningATestFacadeImpl : public RunningATestFacade {
  public:
    RunningATestFacadeImpl(AdaptiveMethod &, FixedLevelMethod &,
        TargetPlaylistReader &targetsWithReplacementReader,
        TargetPlaylistReader &cyclicTargetsReader,
        TargetPlaylist &targetsWithReplacement,
        FiniteTargetPlaylistWithRepeatables &silentIntervalTargets,
        FiniteTargetPlaylistWithRepeatables &everyTargetOnce,
        RepeatableFiniteTargetPlaylist &eachTargetNTimes,
        FiniteTargetPlaylistWithRepeatables &predeterminedTargets,
        RunningATest &, OutputFile &, RunningATest::Observer &audioRecording,
        RunningATest::Observer &eyeTracking);
    void initialize(const AdaptiveTest &) override;
    void initializeWithCyclicTargets(const AdaptiveTest &) override;

  private:
    void initializeTest_(const AdaptiveTest &);
    void initializeTestWithPossiblePeripheral(TestMethod &, const Test &);

    RunningATest::Observer &audioRecording;
    RunningATest::Observer &eyeTracking;
    AdaptiveMethod &adaptiveMethod;
    FixedLevelMethod &fixedLevelMethod;
    TargetPlaylistReader &targetsWithReplacementReader;
    TargetPlaylistReader &cyclicTargetsReader;
    TargetPlaylist &targetsWithReplacement;
    FiniteTargetPlaylistWithRepeatables &silentIntervalTargets;
    FiniteTargetPlaylistWithRepeatables &everyTargetOnce;
    FiniteTargetPlaylistWithRepeatables &predeterminedTargets;
    RepeatableFiniteTargetPlaylist &eachTargetNTimes;
    RunningATest &runningATest;
    OutputFile &outputFile;
};
}

#endif
