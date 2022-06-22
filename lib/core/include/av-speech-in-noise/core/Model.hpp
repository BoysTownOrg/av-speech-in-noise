#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_MODELHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_MODELHPP_

#include "TargetPlaylist.hpp"
#include "TestMethod.hpp"
#include "IOutputFile.hpp"
#include "IAdaptiveMethod.hpp"
#include "IFixedLevelMethod.hpp"
#include "IRecognitionTestModel.hpp"
#include "IModel.hpp"

namespace av_speech_in_noise {
class ModelImpl : public Model {
  public:
    ModelImpl(AdaptiveMethod &, FixedLevelMethod &,
        TargetPlaylistReader &targetsWithReplacementReader,
        TargetPlaylistReader &cyclicTargetsReader,
        TargetPlaylist &targetsWithReplacement,
        FiniteTargetPlaylistWithRepeatables &silentIntervalTargets,
        FiniteTargetPlaylistWithRepeatables &everyTargetOnce,
        RepeatableFiniteTargetPlaylist &eachTargetNTimes,
        RecognitionTestModel &, OutputFile &);
    void attach(Model::Observer *) override;
    void initialize(const AdaptiveTest &) override;
    void initializeWithTargetReplacement(
        const FixedLevelFixedTrialsTest &) override;
    void initializeWithSilentIntervalTargets(const FixedLevelTest &) override;
    void initializeWithAllTargets(const FixedLevelTest &) override;
    void initialize(const FixedLevelTestWithEachTargetNTimes &) override;
    void initializeWithAllTargetsAndEyeTracking(
        const FixedLevelTest &) override;
    void initializeWithAllTargetsAndAudioRecording(
        const FixedLevelTest &) override;
    void initializeWithPredeterminedTargetsAndAudioRecording(
        const FixedLevelTest &) override {}
    void initializeWithSingleSpeaker(const AdaptiveTest &) override;
    void initializeWithDelayedMasker(const AdaptiveTest &) override;
    void initializeWithTargetReplacementAndEyeTracking(
        const FixedLevelFixedTrialsTest &) override;
    void initializeWithSilentIntervalTargetsAndEyeTracking(
        const FixedLevelTest &);
    void initializeWithEyeTracking(const AdaptiveTest &) override;
    void initializeWithCyclicTargets(const AdaptiveTest &) override;
    void initializeWithCyclicTargetsAndEyeTracking(
        const AdaptiveTest &) override;
    void playTrial(const AudioSettings &) override;
    void playCalibration(const Calibration &) override;
    void playLeftSpeakerCalibration(const Calibration &) override;
    void playRightSpeakerCalibration(const Calibration &) override;
    void submit(const coordinate_response_measure::Response &) override;
    auto testComplete() -> bool override;
    auto audioDevices() -> AudioDevices override;
    auto trialNumber() -> int override;
    auto targetFileName() -> std::string override;
    auto adaptiveTestResults() -> AdaptiveTestResults override;
    auto keywordsTestResults() -> KeywordsTestResults override;
    void restartAdaptiveTestWhilePreservingTargets() override;

  private:
    void initializeTest_(const AdaptiveTest &);

    AdaptiveMethod &adaptiveMethod;
    FixedLevelMethod &fixedLevelMethod;
    TargetPlaylistReader &targetsWithReplacementReader;
    TargetPlaylistReader &cyclicTargetsReader;
    TargetPlaylist &targetsWithReplacement;
    FiniteTargetPlaylistWithRepeatables &silentIntervalTargets;
    FiniteTargetPlaylistWithRepeatables &everyTargetOnce;
    RepeatableFiniteTargetPlaylist &eachTargetNTimes;
    RecognitionTestModel &model;
    OutputFile &outputFile;
};
}

#endif
