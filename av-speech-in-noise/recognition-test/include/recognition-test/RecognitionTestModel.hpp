#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_RECOGNITIONTESTMODEL_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_RECOGNITIONTESTMODEL_HPP_

#include <av-speech-in-noise/Model.hpp>
#include <vector>
#include <string>

namespace av_speech_in_noise {
class TargetList {
  public:
    virtual ~TargetList() = default;
    virtual void loadFromDirectory(std::string directory) = 0;
    virtual std::string next() = 0;
    virtual std::string current() = 0;
    virtual bool empty() = 0;
};

class ResponseEvaluator {
  public:
    virtual ~ResponseEvaluator() = default;
    virtual bool correct(const std::string &filePath,
        const coordinate_response_measure::Response &) = 0;
    virtual coordinate_response_measure::Color correctColor(
        const std::string &filePath) = 0;
    virtual int correctNumber(const std::string &filePath) = 0;
    virtual std::string fileName(const std::string &filePath) = 0;
};

class Randomizer {
  public:
    virtual ~Randomizer() = default;
    virtual double randomFloatBetween(double, double) = 0;
    virtual int randomIntBetween(int, int) = 0;
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
    virtual void writeTrial(const FreeResponseTrial &) = 0;
    virtual void writeTrial(const open_set::AdaptiveTrial &) = 0;
    virtual void writeTest(const AdaptiveTest &) = 0;
    virtual void writeTest(const FixedLevelTest &) = 0;
    virtual void close() = 0;
    virtual void save() = 0;
};

class TestMethod {
  public:
    virtual ~TestMethod() = default;
    virtual bool complete() = 0;
    virtual std::string next() = 0;
    virtual std::string current() = 0;
    virtual int snr_dB() = 0;
    virtual void submitCorrectResponse() = 0;
    virtual void submitIncorrectResponse() = 0;
    virtual void submitResponse(const FreeResponse &) = 0;
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
    virtual bool complete(TargetList *) = 0;
    virtual void submitResponse() = 0;
    virtual void initialize(const FixedLevelTest &) = 0;
};

class AdaptiveMethod : public virtual TestMethod {
  public:
    virtual void initialize(const AdaptiveTest &) = 0;
};

class IFixedLevelMethod : public virtual TestMethod {
  public:
    virtual void initialize(
        const FixedLevelTest &, TargetList *, TestConcluder *) = 0;
};

class IRecognitionTestModel_Internal {
  public:
    virtual ~IRecognitionTestModel_Internal() = default;
    virtual void initialize(
        TestMethod *, const Test &, const TestIdentity &) = 0;
    virtual void playTrial(const AudioSettings &) = 0;
    virtual void submitResponse(
        const coordinate_response_measure::Response &) = 0;
    virtual bool testComplete() = 0;
    virtual std::vector<std::string> audioDevices() = 0;
    virtual void subscribe(Model::EventListener *) = 0;
    virtual void playCalibration(const Calibration &) = 0;
    virtual void submitCorrectResponse() = 0;
    virtual void submitIncorrectResponse() = 0;
    virtual void submitResponse(const FreeResponse &) = 0;
    virtual void throwIfTrialInProgress() = 0;
    virtual int trialNumber() = 0;
};

class RecognitionTestModel : public Model {
    AdaptiveMethod *adaptiveMethod;
    IFixedLevelMethod *fixedLevelMethod;
    TargetList *infiniteTargetList;
    TestConcluder *fixedTrialTestConcluder;
    TargetList *finiteTargetList;
    TestConcluder *completesWhenTargetsEmpty;
    IRecognitionTestModel_Internal *model;

  public:
    RecognitionTestModel(AdaptiveMethod *, IFixedLevelMethod *,
        TargetList *infiniteTargetList, TestConcluder *fixedTrialTestConcluder,
        TargetList *finiteTargetList, TestConcluder *completesWhenTargetsEmpty,
        IRecognitionTestModel_Internal *);
    void initializeTest(const AdaptiveTest &) override;
    void initializeTest(const FixedLevelTest &) override;
    void initializeTestWithFiniteTargets(const FixedLevelTest &) override;
    void playTrial(const AudioSettings &) override;
    void submitResponse(const coordinate_response_measure::Response &) override;
    bool testComplete() override;
    std::vector<std::string> audioDevices() override;
    void subscribe(Model::EventListener *) override;
    void playCalibration(const Calibration &) override;
    void submitCorrectResponse() override;
    void submitIncorrectResponse() override;
    void submitResponse(const FreeResponse &) override;
    int trialNumber() override;
};
}

#endif
