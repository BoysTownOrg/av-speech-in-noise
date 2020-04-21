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

using system_time = std::uintmax_t;

struct Delay {
    double seconds;
};

struct TargetTimeWithDelay {
    system_time system;
    Delay delay;
};

class TargetPlayer {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void playbackComplete() = 0;
    };

    virtual ~TargetPlayer() = default;
    virtual void subscribe(EventListener *) = 0;
    virtual void setAudioDevice(std::string) = 0;
    virtual void play() = 0;
    virtual void playAt(const TargetTimeWithDelay &) = 0;
    virtual auto playing() -> bool = 0;
    virtual void loadFile(std::string filePath) = 0;
    virtual void hideVideo() = 0;
    virtual void showVideo() = 0;
    virtual auto rms() -> double = 0;
    virtual void setLevel_dB(double) = 0;
    virtual void subscribeToPlaybackCompletion() = 0;
    virtual auto durationSeconds() -> double = 0;
    virtual void useAllChannels() = 0;
    virtual void useFirstChannelOnly() = 0;
};

struct AudioSampleTime {
    system_time system;
    gsl::index offset;
};

class MaskerPlayer {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void fadeInComplete(const AudioSampleTime &) = 0;
        virtual void fadeOutComplete() = 0;
    };

    virtual ~MaskerPlayer() = default;
    virtual void subscribe(EventListener *) = 0;
    virtual auto outputAudioDeviceDescriptions()
        -> std::vector<std::string> = 0;
    virtual void setAudioDevice(std::string) = 0;
    virtual void fadeIn() = 0;
    virtual void fadeOut() = 0;
    virtual void loadFile(std::string filePath) = 0;
    virtual auto playing() -> bool = 0;
    virtual auto rms() -> double = 0;
    virtual void setLevel_dB(double) = 0;
    virtual auto durationSeconds() -> double = 0;
    virtual auto sampleRateHz() -> double = 0;
    virtual void seekSeconds(double) = 0;
    virtual auto fadeTimeSeconds() -> double = 0;
    virtual void useAllChannels() = 0;
    virtual void useFirstChannelOnly() = 0;
    virtual void clearChannelDelays() = 0;
    virtual void setChannelDelaySeconds(gsl::index channel, double seconds) = 0;
    virtual auto nanoseconds(system_time) -> std::uintmax_t = 0;
};

class EyeTracker {
  public:
    virtual ~EyeTracker() = default;
    virtual void allocateRecordingTimeSeconds(double) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual auto gazeSamples() -> BinocularGazeSamples = 0;
};

class RecognitionTestModelImpl : public TargetPlayer::EventListener,
                                 public MaskerPlayer::EventListener,
                                 public RecognitionTestModel {
  public:
    RecognitionTestModelImpl(TargetPlayer &, MaskerPlayer &,
        ResponseEvaluator &, OutputFile &, Randomizer &, EyeTracker &);
    void subscribe(Model::EventListener *) override;
    void initialize(TestMethod *, const Test &) override;
    void initializeWithSingleSpeaker(TestMethod *, const Test &) override;
    void initializeWithDelayedMasker(TestMethod *, const Test &) override;
    void initializeWithEyeTracking(TestMethod *, const Test &) override;
    void playTrial(const AudioSettings &) override;
    void playCalibration(const Calibration &) override;
    void submit(const coordinate_response_measure::Response &) override;
    void submit(const FreeResponse &) override;
    void submit(const CorrectKeywords &) override;
    void submitCorrectResponse() override;
    void submitIncorrectResponse() override;
    auto testComplete() -> bool override;
    auto audioDevices() -> AudioDevices override;
    auto trialNumber() -> int override;
    auto targetFileName() -> std::string override;
    void fadeInComplete(const AudioSampleTime &) override;
    void fadeOutComplete() override;
    void playbackComplete() override;
    void prepareNextTrialIfNeeded() override;
    static constexpr auto maskerChannelDelaySeconds{0.004};
    static constexpr auto additionalTargetDelaySeconds{0.5};

  private:
    void initialize_(TestMethod *, const Test &);
    void preparePlayersForNextTrial();
    void seekRandomMaskerPosition();
    auto desiredMaskerLevel_dB() -> int;
    auto targetLevel_dB() -> double;
    auto maskerLevel_dB() -> double;

    MaskerPlayer &maskerPlayer;
    TargetPlayer &targetPlayer;
    ResponseEvaluator &evaluator;
    OutputFile &outputFile;
    Randomizer &randomizer;
    EyeTracker &eyeTracker;
    TargetTimeWithDelay lastTargetStartTime{};
    Model::EventListener *listener_{};
    TestMethod *testMethod{};
    int maskerLevel_dB_SPL{};
    int fullScaleLevel_dB_SPL{};
    int trialNumber_{};
    Condition condition{};
    bool eyeTracking{};
};
}

#endif
