#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_RECOGNITIONTESTMODEL_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_RECOGNITIONTESTMODEL_HPP_

#include "Model.hpp"
#include "Randomizer.hpp"
#include <gsl/gsl>
#include <string>
#include <vector>

namespace av_speech_in_noise {
class InvalidAudioDevice {};
class InvalidAudioFile {};

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

class MaskerPlayer {
  public:
    virtual ~MaskerPlayer() = default;

    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void fadeInComplete() = 0;
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
    virtual void seekSeconds(double) = 0;
    virtual auto fadeTimeSeconds() -> double = 0;
    virtual void useAllChannels() = 0;
    virtual void useFirstChannelOnly() = 0;
    virtual void clearChannelDelays() = 0;
    virtual void setChannelDelaySeconds(gsl::index channel, double seconds) = 0;
};

class RecognitionTestModelImpl : public TargetPlayer::EventListener,
                                 public MaskerPlayer::EventListener,
                                 public RecognitionTestModel {
  public:
    RecognitionTestModelImpl(TargetPlayer &, MaskerPlayer &,
        ResponseEvaluator &, OutputFile &, Randomizer &);
    void subscribe(Model::EventListener *) override;
    void initialize(TestMethod *, const Test &) override;
    void initializeWithSingleSpeaker(TestMethod *, const Test &) override;
    void initializeWithDelayedMasker(TestMethod *, const Test &) override;
    void playTrial(const AudioSettings &) override;
    void playCalibration(const Calibration &) override;
    void submit(const coordinate_response_measure::Response &) override;
    void submit(const FreeResponse &) override;
    void submit(const CorrectKeywords &) override;
    void submitCorrectResponse() override;
    void submitIncorrectResponse() override;
    auto testComplete() -> bool override;
    auto audioDevices() -> std::vector<std::string> override;
    auto trialNumber() -> int override;
    auto targetFileName() -> std::string override;
    void fadeInComplete() override;
    void fadeOutComplete() override;
    void playbackComplete() override;
    void prepareNextTrialIfNeeded() override;
    static constexpr auto maskerChannelDelaySeconds{0.004};

  private:
    void initialize_(TestMethod *, const Test &);
    void prepareTest(const Test &);
    void storeLevels(const Test &);
    void preparePlayersForNextTrial();
    auto currentTarget() -> std::string;
    auto correct(const coordinate_response_measure::Response &) -> bool;
    void submitResponse_(const coordinate_response_measure::Response &);
    void prepareMasker(const std::string &);
    auto noMoreTrials() -> bool;
    void prepareTargetPlayer();
    void seekRandomMaskerPosition();
    void prepareVideo(const Condition &);
    auto desiredMaskerLevel_dB() -> int;
    auto unalteredTargetLevel_dB() -> double;
    auto targetLevel_dB() -> double;
    auto maskerLevel_dB() -> double;
    auto findDeviceIndex(const AudioSettings &) -> int;

    MaskerPlayer &maskerPlayer;
    TargetPlayer &targetPlayer;
    ResponseEvaluator &evaluator;
    OutputFile &outputFile;
    Randomizer &randomizer;
    Model::EventListener *listener_{};
    TestMethod *testMethod{};
    int maskerLevel_dB_SPL{};
    int fullScaleLevel_dB_SPL{};
    int trialNumber_{};
    Condition condition{};
};
}

#endif
