#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_RECOGNITIONTESTMODELHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_RECOGNITIONTESTMODELHPP_

#include "Randomizer.hpp"
#include "IResponseEvaluator.hpp"
#include "ITargetPlayer.hpp"
#include "IMaskerPlayer.hpp"
#include "IRecognitionTestModel.hpp"
#include "IOutputFile.hpp"

namespace av_speech_in_noise {
class EyeTracker : public Writable {
  public:
    virtual void allocateRecordingTimeSeconds(double) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual auto gazeSamples() -> BinocularGazeSamples = 0;
    virtual auto currentSystemTime() -> EyeTrackerSystemTime = 0;
};

class RecognitionTestModelImpl : public TargetPlayer::Observer,
                                 public MaskerPlayer::Observer,
                                 public RecognitionTestModel {
  public:
    RecognitionTestModelImpl(TargetPlayer &, MaskerPlayer &,
        ResponseEvaluator &, OutputFile &, Randomizer &, EyeTracker &);
    void attach(Model::Observer *) override;
    void initialize(TestMethod *, const Test &) override;
    void initializeWithSingleSpeaker(TestMethod *, const Test &) override;
    void initializeWithDelayedMasker(TestMethod *, const Test &) override;
    void initializeWithEyeTracking(TestMethod *, const Test &) override;
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
    void fadeInComplete(const AudioSampleTimeWithOffset &) override;
    void fadeOutComplete() override;
    void prepareNextTrialIfNeeded() override;
    void notifyThatPreRollHasCompleted() override;
    static constexpr Delay maskerChannelDelay{0.004};
    static constexpr Duration targetOnsetFringeDuration{0.166};
    static constexpr Duration targetOffsetFringeDuration{
        targetOnsetFringeDuration};

  private:
    void initialize_(TestMethod *, const Test &);
    void seekRandomMaskerPosition();

    MaskerPlayer &maskerPlayer;
    TargetPlayer &targetPlayer;
    ResponseEvaluator &evaluator;
    OutputFile &outputFile;
    Randomizer &randomizer;
    EyeTracker &eyeTracker;
    EyeTrackerTargetPlayerSynchronization
        lastEyeTrackerTargetPlayerSynchronization{};
    TargetStartTime lastTargetStartTime{};
    Model::Observer *listener_{};
    TestMethod *testMethod{};
    RealLevel maskerLevel_{};
    RealLevel fullScaleLevel_{};
    int trialNumber_{};
    Condition condition{};
    bool eyeTracking{};
    bool trialInProgress_{};
};
}

#endif
