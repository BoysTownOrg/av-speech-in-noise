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

struct SystemTimeWithDelay {
    system_time time;
    double secondsDelayed;
};

class TargetPlayer {
  public:
    virtual ~TargetPlayer() = default;

    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void playbackComplete() = 0;
    };

    virtual void subscribe(EventListener *) = 0;
    virtual void setAudioDevice(std::string) = 0;
    virtual void play() = 0;
    virtual void playAt(const SystemTimeWithDelay &) = 0;
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

struct AudioSampleSystemTime {
    system_time time;
    gsl::index sampleOffset;
};

class MaskerPlayer {
  public:
    virtual ~MaskerPlayer() = default;

    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void fadeInComplete(const AudioSampleSystemTime &) = 0;
        virtual void fadeOutComplete() = 0;
    };

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
    void initialize(TestMethod *, const Test &) override;
    void initializeWithSingleSpeaker(TestMethod *, const Test &) override;
    void initializeWithDelayedMasker(TestMethod *, const Test &) override;
    void initializeWithEyeTracking(TestMethod *, const Test &) override;
    void playTrial(const AudioSettings &) override;
    void submit(const coordinate_response_measure::Response &) override;
    auto testComplete() -> bool override;
    auto audioDevices() -> std::vector<std::string> override;
    auto trialNumber() -> int override;
    auto targetFileName() -> std::string override;
    void subscribe(Model::EventListener *) override;
    void playCalibration(const Calibration &) override;
    void submitCorrectResponse() override;
    void submitIncorrectResponse() override;
    void submit(const open_set::FreeResponse &) override;
    void submit(const open_set::CorrectKeywords &) override;
    void throwIfTrialInProgress() override;
    void fadeInComplete(const AudioSampleSystemTime &) override;
    void fadeOutComplete() override;
    void playbackComplete() override;
    static constexpr auto maskerChannelDelaySeconds{0.004};
    static constexpr auto additionalTargetDelaySeconds{0.5};

  private:
    void initialize_(TestMethod *, const Test &);
    void initializeWithDefaultAudioConfiguration(TestMethod *, const Test &);
    void submitCorrectResponse_();
    void submitIncorrectResponse_();
    void write(const open_set::FreeResponse &p);
    void write(const open_set::CorrectKeywords &p);
    void prepareTest(const Test &);
    void storeLevels(const Test &common);
    void preparePlayersForNextTrial();
    auto currentTarget() -> std::string;
    auto correct(const coordinate_response_measure::Response &) -> bool;
    void submitResponse_(const coordinate_response_measure::Response &);
    void setTargetPlayerDevice(const Calibration &);
    auto calibrationLevel_dB(const Calibration &) -> double;
    void trySettingTargetLevel(const Calibration &);
    void playCalibration_(const Calibration &);
    void prepareMasker(const std::string &);
    void tryOpeningOutputFile_(const TestIdentity &);
    void tryOpeningOutputFile(const TestIdentity &);
    void loadMaskerFile(const std::string &);
    void playTarget();
    auto noMoreTrials() -> bool;
    auto trialInProgress() -> bool;
    void prepareTargetPlayer();
    void seekRandomMaskerPosition();
    void preparePlayersToPlay(const AudioSettings &);
    void startTrial();
    void prepareVideo(const Condition &);
    auto desiredMaskerLevel_dB() -> int;
    auto unalteredTargetLevel_dB() -> double;
    auto targetLevel_dB() -> double;
    auto maskerLevel_dB() -> double;
    void setTargetPlayerDevice_(const std::string &);
    void setAudioDevices_(const std::string &);
    void setAudioDevices(const AudioSettings &);
    auto findDeviceIndex(const AudioSettings &) -> int;
    void throwInvalidAudioDeviceOnErrorSettingDevice(
        void (RecognitionTestModelImpl::*f)(const std::string &),
        const std::string &);
    void throwInvalidAudioFileOnErrorLoading(
        void (RecognitionTestModelImpl::*f)(const std::string &),
        const std::string &file);
    void loadTargetFile(std::string);
    void setTargetLevel_dB(double);
    void prepareNextTrialIfNeeded();

    MaskerPlayer &maskerPlayer;
    TargetPlayer &targetPlayer;
    ResponseEvaluator &evaluator;
    OutputFile &outputFile;
    Randomizer &randomizer;
    EyeTracker &eyeTracker;
    SystemTimeWithDelay lastTargetStartTime{};
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
