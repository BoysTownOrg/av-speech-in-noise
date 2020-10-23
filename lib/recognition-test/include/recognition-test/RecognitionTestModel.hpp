#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_RECOGNITIONTESTMODEL_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_RECOGNITIONTESTMODEL_HPP_

#include "Randomizer.hpp"
#include "Model.hpp"
#include <gsl/gsl>
#include <string>
#include <vector>
#include <cstdint>

namespace av_speech_in_noise {
class InvalidAudioDevice {};
class InvalidAudioFile {};

struct Duration {
    double seconds;
};

struct Delay : Duration {
    explicit constexpr Delay(double seconds = 0.) : Duration{seconds} {}
};

using player_system_time_type = std::uintmax_t;

struct PlayerTime {
    player_system_time_type system{};
};

struct PlayerTimeWithDelay {
    PlayerTime playerTime;
    Delay delay{};
};

struct DigitalLevel {
    double dBov;
};

struct LevelAmplification {
    double dB;
};

class TargetPlayer {
  public:
    class Observer {
      public:
        virtual ~Observer() = default;
        virtual void playbackComplete() = 0;
        virtual void notifyThatPreRollHasCompleted() = 0;
    };

    virtual ~TargetPlayer() = default;
    virtual void attach(Observer *) = 0;
    virtual void setAudioDevice(std::string) = 0;
    virtual void play() = 0;
    virtual void playAt(const PlayerTimeWithDelay &) = 0;
    virtual auto playing() -> bool = 0;
    virtual void loadFile(const LocalUrl &) = 0;
    virtual void hideVideo() = 0;
    virtual void showVideo() = 0;
    virtual auto digitalLevel() -> DigitalLevel = 0;
    virtual void apply(LevelAmplification) = 0;
    virtual void subscribeToPlaybackCompletion() = 0;
    virtual auto duration() -> Duration = 0;
    virtual void useAllChannels() = 0;
    virtual void useFirstChannelOnly() = 0;
    virtual void preRoll() = 0;
};

struct AudioSampleTimeWithOffset {
    PlayerTime playerTime;
    gsl::index sampleOffset;
};

class MaskerPlayer {
  public:
    class Observer {
      public:
        virtual ~Observer() = default;
        virtual void fadeInComplete(const AudioSampleTimeWithOffset &) = 0;
        virtual void fadeOutComplete() = 0;
    };

    virtual ~MaskerPlayer() = default;
    virtual void attach(Observer *) = 0;
    virtual auto outputAudioDeviceDescriptions()
        -> std::vector<std::string> = 0;
    virtual void setAudioDevice(std::string) = 0;
    virtual void fadeIn() = 0;
    virtual void fadeOut() = 0;
    virtual void loadFile(const LocalUrl &) = 0;
    virtual auto playing() -> bool = 0;
    virtual auto digitalLevel() -> DigitalLevel = 0;
    virtual void apply(LevelAmplification) = 0;
    virtual auto duration() -> Duration = 0;
    virtual auto sampleRateHz() -> double = 0;
    virtual void seekSeconds(double) = 0;
    virtual auto fadeTime() -> Duration = 0;
    virtual void useAllChannels() = 0;
    virtual void useFirstChannelOnly() = 0;
    virtual void useSecondChannelOnly() = 0;
    virtual void clearChannelDelays() = 0;
    virtual void setChannelDelaySeconds(gsl::index channel, double seconds) = 0;
    virtual auto nanoseconds(PlayerTime) -> std::uintmax_t = 0;
    virtual auto currentSystemTime() -> PlayerTime = 0;
    virtual void play() = 0;
    virtual void stop() = 0;
};

class EyeTracker {
  public:
    virtual ~EyeTracker() = default;
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
    void submitCorrectResponse() override;
    void submitIncorrectResponse() override;
    auto testComplete() -> bool override;
    auto audioDevices() -> AudioDevices override;
    auto trialNumber() -> int override;
    auto targetFileName() -> std::string override;
    void fadeInComplete(const AudioSampleTimeWithOffset &) override;
    void fadeOutComplete() override;
    void playbackComplete() override;
    void prepareNextTrialIfNeeded() override;
    void notifyThatPreRollHasCompleted() override;
    static constexpr Delay maskerChannelDelay{0.004};
    static constexpr Delay additionalTargetDelay{0.5};
    static constexpr Delay fringeTargetDelay{0.166};

  private:
    void initialize_(TestMethod *, const Test &);
    void preparePlayersForNextTrial();
    void seekRandomMaskerPosition();
    auto targetLevelAmplification() -> LevelAmplification;
    auto maskerLevelAmplification() -> LevelAmplification;

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
