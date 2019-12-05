#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_MODEL_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_MODEL_HPP_

#include <av-speech-in-noise/Model.hpp>
#include <vector>
#include <string>

namespace av_speech_in_noise {
class TargetList {
  public:
    virtual ~TargetList() = default;
    virtual void loadFromDirectory(std::string directory) = 0;
    virtual auto next() -> std::string = 0;
    virtual auto current() -> std::string = 0;
    virtual auto empty() -> bool = 0;
    virtual void reinsertCurrent() = 0;
};

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

class OutputFile {
  public:
    virtual ~OutputFile() = default;
    virtual void openNewFile(const TestIdentity &) = 0;
    class OpenFailure {};
    virtual void writeTrial(
        const coordinate_response_measure::AdaptiveTrial &) = 0;
    virtual void writeTrial(
        const coordinate_response_measure::FixedLevelTrial &) = 0;
    virtual void writeTrial(const open_set::FreeResponseTrial &) = 0;
    virtual void writeTrial(const open_set::AdaptiveTrial &) = 0;
    virtual void writeTest(const AdaptiveTest &) = 0;
    virtual void writeTest(const FixedLevelTest &) = 0;
    virtual void close() = 0;
    virtual void save() = 0;
};

class TestMethod {
  public:
    virtual ~TestMethod() = default;
    virtual auto complete() -> bool = 0;
    virtual auto next() -> std::string = 0;
    virtual auto current() -> std::string = 0;
    virtual auto snr_dB() -> int = 0;
    virtual void submitCorrectResponse() = 0;
    virtual void submitIncorrectResponse() = 0;
    virtual void submitResponse(const open_set::FreeResponse &) = 0;
    virtual void writeTestingParameters(OutputFile *) = 0;
    virtual void writeLastCoordinateResponse(OutputFile *) = 0;
    virtual void writeLastCorrectResponse(OutputFile *) = 0;
    virtual void writeLastIncorrectResponse(OutputFile *) = 0;
    virtual void submitResponse(
        const coordinate_response_measure::Response &) = 0;
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
        TargetList *finiteTargetList, TestConcluder *completesWhenTargetsEmpty,
        RecognitionTestModel *);
    void initializeTest(const AdaptiveTest &) override;
    void initializeTest(const FixedLevelTest &) override;
    void initializeTestWithFiniteTargets(const FixedLevelTest &) override;
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
    AdaptiveMethod *adaptiveMethod;
    FixedLevelMethod *fixedLevelMethod;
    TargetList *infiniteTargetList;
    TestConcluder *fixedTrialTestConcluder;
    TargetList *finiteTargetList;
    TestConcluder *completesWhenTargetsEmpty;
    RecognitionTestModel *model;
};
}

#endif
