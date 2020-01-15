#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_MODEL_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_MODEL_HPP_

#include "TargetList.hpp"
#include "TestMethod.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <vector>
#include <string>

namespace av_speech_in_noise {
class ResponseEvaluator {
  public:
    virtual ~ResponseEvaluator() = default;
    virtual auto correct(const std::string &filePath,
        const coordinate_response_measure::Response &) -> bool = 0;
    virtual auto correctColor(const std::string &filePath)
        -> coordinate_response_measure::Color = 0;
    virtual auto correctNumber(const std::string &filePath) -> int = 0;
    virtual auto fileName(const std::string &filePath) -> std::string = 0;
};

class Randomizer {
  public:
    virtual ~Randomizer() = default;
    virtual auto randomFloatBetween(double, double) -> double = 0;
    virtual auto randomIntBetween(int, int) -> int = 0;
};

class TestConcluder {
  public:
    virtual ~TestConcluder() = default;
    virtual auto complete(TargetList *) -> bool = 0;
    virtual void submitResponse() = 0;
    virtual void initialize(const FixedLevelTest &) = 0;
};

class AdaptiveMethod : public virtual TestMethod {
  public:
    virtual void initialize(const AdaptiveTest &) = 0;
};

class FixedLevelMethod : public virtual TestMethod {
  public:
    virtual void initialize(
        const FixedLevelTest &, TargetList *, TestConcluder *) = 0;
};

class RecognitionTestModel {
  public:
    virtual ~RecognitionTestModel() = default;
    virtual void initialize(TestMethod *, const Test &) = 0;
    virtual void initializeWithSingleSpeaker(TestMethod *, const Test &) = 0;
    virtual void initializeWithDelayedMasker(TestMethod *, const Test &) = 0;
    virtual void playTrial(const AudioSettings &) = 0;
    virtual void submitResponse(
        const coordinate_response_measure::Response &) = 0;
    virtual auto testComplete() -> bool = 0;
    virtual auto audioDevices() -> std::vector<std::string> = 0;
    virtual void subscribe(Model::EventListener *) = 0;
    virtual void playCalibration(const Calibration &) = 0;
    virtual void submitCorrectResponse() = 0;
    virtual void submitIncorrectResponse() = 0;
    virtual void submitResponse(const open_set::FreeResponse &) = 0;
    virtual void throwIfTrialInProgress() = 0;
    virtual auto trialNumber() -> int = 0;
};

class ModelImpl : public Model {
  public:
    ModelImpl(AdaptiveMethod *, FixedLevelMethod *,
        TargetList *infiniteTargetList, TestConcluder *fixedTrialTestConcluder,
        TargetList *silentIntervals, TestConcluder *completesWhenTargetsEmpty,
        TargetList *allStimuli, RecognitionTestModel *);
    void initializeTest(const AdaptiveTest &) override;
    void initializeTest(const FixedLevelTest &) override;
    void initializeSilentIntervalsTest(const FixedLevelTest &) override;
    void initializeAllStimuliTest(const FixedLevelTest &) override;
    void initializeTestWithSingleSpeaker(const AdaptiveTest &) override;
    void initializeTestWithDelayedMasker(const AdaptiveTest &) override;
    void playTrial(const AudioSettings &) override;
    void submitResponse(const coordinate_response_measure::Response &) override;
    auto testComplete() -> bool override;
    auto audioDevices() -> std::vector<std::string> override;
    void subscribe(Model::EventListener *) override;
    void playCalibration(const Calibration &) override;
    void submitCorrectResponse() override;
    void submitIncorrectResponse() override;
    void submitResponse(const open_set::FreeResponse &) override;
    auto trialNumber() -> int override;

  private:
    void initializeTest_(const AdaptiveTest &);

    AdaptiveMethod *adaptiveMethod;
    FixedLevelMethod *fixedLevelMethod;
    TargetList *infiniteTargetList;
    TestConcluder *fixedTrialTestConcluder;
    TargetList *silentIntervals;
    TestConcluder *completesWhenTargetsEmpty;
    TargetList *allStimuli;
    RecognitionTestModel *model;
};
}

#endif
