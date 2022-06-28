#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_RECOGNITIONTESTMODELHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_RECOGNITIONTESTMODELHPP_

#include "Randomizer.hpp"
#include "IResponseEvaluator.hpp"
#include "ITargetPlayer.hpp"
#include "IMaskerPlayer.hpp"
#include "IRecognitionTestModel.hpp"
#include "IOutputFile.hpp"
#include "av-speech-in-noise/Model.hpp"
#include <av-speech-in-noise/Interface.hpp>
#include <string>
#include <string_view>

namespace av_speech_in_noise {
class EyeTracker : public Writable {
  public:
    virtual void allocateRecordingTimeSeconds(double) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual auto gazeSamples() -> BinocularGazeSamples = 0;
    virtual auto currentSystemTime() -> EyeTrackerSystemTime = 0;
};

class Clock {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Clock);
    virtual auto time() -> std::string = 0;
};

class AudioRecorder {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(AudioRecorder);
    virtual void initialize(const LocalUrl &) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};

class EyeTracking : public RunningATest::Observer {
  public:
    EyeTracking(EyeTracker &, MaskerPlayer &, TargetPlayer &, OutputFile &);
    void notifyThatNewTestIsReady(std::string_view session) override {}
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

class AudioRecording : public RunningATest::Observer {
  public:
    AudioRecording(AudioRecorder &, OutputFile &);
    void notifyThatNewTestIsReady(std::string_view session) override {
        this->session = session;
    }
    void notifyThatTrialWillBegin(int trialNumber) override;
    void notifyThatTargetWillPlayAt(const PlayerTimeWithDelay &) override;
    void notifyThatStimulusHasEnded() override;
    void notifyThatSubjectHasResponded() override;

  private:
    std::string session;
    AudioRecorder &audioRecorder;
    OutputFile &outputFile;
};

class RunningATestImpl : public TargetPlayer::Observer,
                         public MaskerPlayer::Observer,
                         public RunningATest {
  public:
    RunningATestImpl(TargetPlayer &, MaskerPlayer &, AudioRecorder &,
        ResponseEvaluator &, OutputFile &, Randomizer &, EyeTracker &, Clock &);
    void attach(Model::Observer *) override;
    void initialize(
        TestMethod *, const Test &, RunningATest::Observer *) override;
    void initializeWithSingleSpeaker(TestMethod *, const Test &) override;
    void initializeWithDelayedMasker(TestMethod *, const Test &) override;
    void initializeWithEyeTracking(TestMethod *, const Test &) override;
    void initializeWithAudioRecording(TestMethod *, const Test &) override;
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
    AudioRecording audioRecording;
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
