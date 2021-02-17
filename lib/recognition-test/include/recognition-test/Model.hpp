#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_MODEL_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_MODEL_HPP_

#include "TargetPlaylist.hpp"
#include "TestMethod.hpp"
#include "IOutputFile.hpp"
#include "IAdaptiveMethod.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <gsl/gsl>
#include <cstdint>
#include <vector>
#include <string>
#include <memory>

namespace av_speech_in_noise {
class FixedLevelMethod : public virtual TestMethod {
  public:
    virtual void initialize(const FixedLevelTest &, FiniteTargetPlaylist *) = 0;
    virtual void initialize(
        const FixedLevelTest &, FiniteTargetPlaylistWithRepeatables *) = 0;
    virtual void initialize(
        const FixedLevelFixedTrialsTest &, TargetPlaylist *) = 0;
    virtual void submit(const ConsonantResponse &) = 0;
    virtual void writeLastConsonant(OutputFile &) = 0;
    virtual auto keywordsTestResults() -> KeywordsTestResults = 0;
};

class RecognitionTestModel {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(RecognitionTestModel);
    virtual void attach(Model::Observer *) = 0;
    virtual void initialize(TestMethod *, const Test &) = 0;
    virtual void initializeWithSingleSpeaker(TestMethod *, const Test &) = 0;
    virtual void initializeWithDelayedMasker(TestMethod *, const Test &) = 0;
    virtual void initializeWithEyeTracking(TestMethod *, const Test &) = 0;
    virtual void playTrial(const AudioSettings &) = 0;
    virtual void playCalibration(const Calibration &) = 0;
    virtual void playLeftSpeakerCalibration(const Calibration &) = 0;
    virtual void playRightSpeakerCalibration(const Calibration &) = 0;
    virtual void submit(const coordinate_response_measure::Response &) = 0;
    virtual void submit(const FreeResponse &) = 0;
    virtual void submit(const ThreeKeywordsResponse &) = 0;
    virtual void submit(const CorrectKeywords &) = 0;
    virtual void submit(const ConsonantResponse &) = 0;
    virtual void submit(const SyllableResponse &) {}
    virtual void submitCorrectResponse() = 0;
    virtual void submitIncorrectResponse() = 0;
    virtual auto testComplete() -> bool = 0;
    virtual auto audioDevices() -> AudioDevices = 0;
    virtual auto trialNumber() -> int = 0;
    virtual auto targetFileName() -> std::string = 0;
    virtual void prepareNextTrialIfNeeded() = 0;
};

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
    void submit(const FreeResponse &) override;
    void submit(const CorrectKeywords &) override;
    void submit(const ConsonantResponse &) override;
    void submit(const ThreeKeywordsResponse &) override;
    void submit(const SyllableResponse &) override;
    void submitCorrectResponse() override;
    void submitIncorrectResponse() override;
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
