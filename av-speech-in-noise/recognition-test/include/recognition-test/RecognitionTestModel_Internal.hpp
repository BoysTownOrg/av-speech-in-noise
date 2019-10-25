#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_RECOGNITIONTESTMODEL_INTERNAL_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_RECOGNITIONTESTMODEL_INTERNAL_HPP_

#include "RecognitionTestModel.hpp"
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
    virtual bool playing() = 0;
    virtual void loadFile(std::string filePath) = 0;
    virtual void hideVideo() = 0;
    virtual void showVideo() = 0;
    virtual double rms() = 0;
    virtual void setLevel_dB(double) = 0;
    virtual void subscribeToPlaybackCompletion() = 0;
    virtual double durationSeconds() = 0;
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
    virtual std::vector<std::string> outputAudioDeviceDescriptions() = 0;
    virtual void setAudioDevice(std::string) = 0;
    virtual void fadeIn() = 0;
    virtual void fadeOut() = 0;
    virtual void loadFile(std::string filePath) = 0;
    virtual bool playing() = 0;
    virtual double rms() = 0;
    virtual void setLevel_dB(double) = 0;
    virtual double durationSeconds() = 0;
    virtual void seekSeconds(double) = 0;
    virtual double fadeTimeSeconds() = 0;
};

class RecognitionTestModel_Internal : public TargetPlayer::EventListener,
                                      public MaskerPlayer::EventListener,
                                      public IRecognitionTestModel_Internal {
    MaskerPlayer *maskerPlayer;
    TargetPlayer *targetPlayer;
    ResponseEvaluator *evaluator;
    OutputFile *outputFile;
    Randomizer *randomizer;
    Model::EventListener *listener_{};
    TestMethod *testMethod{};
    int maskerLevel_dB_SPL{};
    int fullScaleLevel_dB_SPL{};
    int trialNumber_{};
    Condition condition;

  public:
    RecognitionTestModel_Internal(TargetPlayer *, MaskerPlayer *,
        ResponseEvaluator *, OutputFile *, Randomizer *);
    void initialize(
        TestMethod *, const CommonTest &, const TestInformation &) override;
    void playTrial(const AudioSettings &) override;
    void submitResponse(const coordinate_response_measure::Response &) override;
    bool testComplete() override;
    std::vector<std::string> audioDevices() override;
    int trialNumber() override;
    void subscribe(Model::EventListener *) override;
    void playCalibration(const Calibration &) override;
    void submitCorrectResponse() override;
    void submitIncorrectResponse() override;
    void submitResponse(const FreeResponse &) override;
    void throwIfTrialInProgress() override;
    void fadeInComplete() override;
    void fadeOutComplete() override;
    void playbackComplete() override;

  private:
    void submitCorrectResponse_();
    void submitIncorrectResponse_();
    void writeTrial(const FreeResponse &p);
    void prepareCommonTest(const CommonTest &, const TestInformation &);
    void storeLevels(const CommonTest &common);
    void preparePlayersForNextTrial();
    std::string currentTarget();
    bool correct(const coordinate_response_measure::Response &);
    void submitResponse_(const coordinate_response_measure::Response &);
    void setTargetPlayerDevice(const Calibration &);
    double calibrationLevel_dB(const Calibration &);
    void trySettingTargetLevel(const Calibration &);
    void playCalibration_(const Calibration &);
    void prepareMasker(const std::string &);
    void tryOpeningOutputFile_(const TestInformation &);
    void tryOpeningOutputFile(const TestInformation &);
    void loadMaskerFile(const std::string &);
    void playTarget();
    bool noMoreTrials();
    bool trialInProgress();
    void prepareTargetPlayer();
    void seekRandomMaskerPosition();
    void preparePlayersToPlay(const AudioSettings &);
    void startTrial();
    bool auditoryOnly(const Condition &);
    void prepareVideo(const Condition &);
    int desiredMaskerLevel_dB();
    double unalteredTargetLevel_dB();
    double targetLevel_dB();
    double maskerLevel_dB();
    void setTargetPlayerDevice_(const std::string &);
    void setAudioDevices_(const std::string &);
    void setAudioDevices(const AudioSettings &);
    int findDeviceIndex(const AudioSettings &);
    void throwInvalidAudioDeviceOnErrorSettingDevice(
        void (RecognitionTestModel_Internal::*f)(const std::string &),
        const std::string &);
    void throwInvalidAudioFileOnErrorLoading(
        void (RecognitionTestModel_Internal::*f)(const std::string &),
        const std::string &file);
    void loadTargetFile(std::string);
    void setTargetLevel_dB(double);
    void prepareNextTrialIfNeeded();
};
}

#endif
