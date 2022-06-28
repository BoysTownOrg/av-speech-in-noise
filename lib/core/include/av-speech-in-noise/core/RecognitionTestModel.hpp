#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_RECOGNITIONTESTMODELHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_RECOGNITIONTESTMODELHPP_

#include "Randomizer.hpp"
#include "IMaskerPlayer.hpp"
#include "IOutputFile.hpp"
#include "IRecognitionTestModel.hpp"
#include "IResponseEvaluator.hpp"
#include "ITargetPlayer.hpp"
#include "EyeTracking.hpp"

#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>

#include <string>
#include <string_view>

namespace av_speech_in_noise {
class Clock {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Clock);
    virtual auto time() -> std::string = 0;
};

class RunningATestImpl : public TargetPlayer::Observer,
                         public MaskerPlayer::Observer,
                         public RunningATest {
  public:
    RunningATestImpl(TargetPlayer &, MaskerPlayer &, ResponseEvaluator &,
        OutputFile &, Randomizer &, EyeTracker &, Clock &);
    void attach(Model::Observer *) override;
    void initialize(
        TestMethod *, const Test &, RunningATest::Observer *) override;
    void initializeWithSingleSpeaker(TestMethod *, const Test &) override;
    void initializeWithDelayedMasker(TestMethod *, const Test &) override;
    void initializeWithEyeTracking(TestMethod *, const Test &) override;
    void playTrial(const AudioSettings &) override;
    void playCalibration(const Calibration &) override;
    void playLeftSpeakerCalibration(const Calibration &) override;
    void playRightSpeakerCalibration(const Calibration &) override;
    void submit(const coordinate_response_measure::Response &) override;
    auto testComplete() -> bool override;
    auto audioDevices() -> AudioDevices override;
    auto trialNumber() -> int override;
    auto targetFileName() -> std::string override;
    void fadeInComplete(const AudioSampleTimeWithOffset &) override;
    void fadeOutComplete() override;
    void prepareNextTrialIfNeeded() override;
    void notifyThatPreRollHasCompleted() override;
    auto playTrialTime() -> std::string override;
    static constexpr Delay maskerChannelDelay{0.004};
    static constexpr Duration targetOnsetFringeDuration{0.166};
    static constexpr Duration targetOffsetFringeDuration{
        targetOnsetFringeDuration};

  private:
    void initialize_(TestMethod *, const Test &, RunningATest::Observer *);
    void seekRandomMaskerPosition();

    EyeTracking eyeTracking;
    RunningATest::Observer *observer;
    MaskerPlayer &maskerPlayer;
    TargetPlayer &targetPlayer;
    ResponseEvaluator &evaluator;
    OutputFile &outputFile;
    Randomizer &randomizer;
    Clock &clock;
    std::string playTrialTime_;
    Model::Observer *listener_{};
    TestMethod *testMethod{};
    RealLevel maskerLevel_{};
    RealLevel fullScaleLevel_{};
    int trialNumber_{};
    Condition condition{};
    bool trialInProgress_{};
};
}

#endif
