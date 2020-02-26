#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_MODEL_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_MODEL_HPP_

#include "TargetList.hpp"
#include "TestMethod.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <gsl/gsl>
#include <cstdint>
#include <vector>
#include <string>

namespace av_speech_in_noise {
class OutputFile {
  public:
    virtual ~OutputFile() = default;
    virtual void openNewFile(const TestIdentity &) = 0;
    class OpenFailure {};
    virtual void write(const coordinate_response_measure::AdaptiveTrial &) = 0;
    virtual void write(
        const coordinate_response_measure::FixedLevelTrial &) = 0;
    virtual void write(const open_set::FreeResponseTrial &) = 0;
    virtual void write(const open_set::CorrectKeywordsTrial &) = 0;
    virtual void write(const open_set::AdaptiveTrial &) = 0;
    virtual void writeTest(const AdaptiveTest &) = 0;
    virtual void writeTest(const FixedLevelTest &) = 0;
    virtual void close() = 0;
    virtual void save() = 0;
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

class TargetListReader {
  public:
    virtual ~TargetListReader() = default;
    using lists_type = typename std::vector<std::shared_ptr<TargetList>>;
    virtual auto read(std::string directory) -> lists_type = 0;
};

class AdaptiveMethod : public virtual TestMethod {
  public:
    virtual void initialize(const AdaptiveTest &, TargetListReader *) = 0;
};

class FixedLevelMethod : public virtual TestMethod {
  public:
    virtual void initialize(const FixedLevelTest &, TargetList *) = 0;
    virtual void initialize(const FixedLevelTest &, FiniteTargetList *) = 0;
};

class RecognitionTestModel {
  public:
    virtual ~RecognitionTestModel() = default;
    virtual void initialize(TestMethod *, const Test &) = 0;
    virtual void initializeWithSingleSpeaker(TestMethod *, const Test &) = 0;
    virtual void initializeWithDelayedMasker(TestMethod *, const Test &) = 0;
    virtual void playTrial(const AudioSettings &) = 0;
    virtual void submit(const coordinate_response_measure::Response &) = 0;
    virtual auto testComplete() -> bool = 0;
    virtual auto audioDevices() -> std::vector<std::string> = 0;
    virtual void subscribe(Model::EventListener *) = 0;
    virtual void playCalibration(const Calibration &) = 0;
    virtual void submitCorrectResponse() = 0;
    virtual void submitIncorrectResponse() = 0;
    virtual void submit(const open_set::FreeResponse &) = 0;
    virtual void submit(const open_set::CorrectKeywords &) = 0;
    virtual void throwIfTrialInProgress() = 0;
    virtual auto trialNumber() -> int = 0;
    virtual auto targetFileName() -> std::string = 0;
};

class ModelImpl : public Model {
  public:
    ModelImpl(AdaptiveMethod &, FixedLevelMethod &,
        TargetListReader &targetsWithReplacementReader,
        TargetList &targetsWithReplacement,
        FiniteTargetList &silentIntervalTargets,
        FiniteTargetList &everyTargetOnce, RecognitionTestModel &);
    void initialize(const AdaptiveTest &) override;
    void initializeWithTargetReplacement(const FixedLevelTest &) override;
    void initializeWithSilentIntervalTargets(const FixedLevelTest &) override;
    void initializeWithAllTargets(const FixedLevelTest &) override;
    void initializeWithAllTargetsAndEyeTracking(const FixedLevelTest &);
    void initializeWithSingleSpeaker(const AdaptiveTest &) override;
    void initializeWithDelayedMasker(const AdaptiveTest &) override;
    void initializeWithTargetReplacementAndEyeTracking(const FixedLevelTest &);
    void initializeWithSilentIntervalTargetsAndEyeTracking(
        const FixedLevelTest &);
    void playTrial(const AudioSettings &) override;
    void submit(const coordinate_response_measure::Response &) override;
    auto testComplete() -> bool override;
    auto audioDevices() -> std::vector<std::string> override;
    void subscribe(Model::EventListener *) override;
    void playCalibration(const Calibration &) override;
    void submitCorrectResponse() override;
    void submitIncorrectResponse() override;
    void submit(const open_set::FreeResponse &) override;
    void submit(const open_set::CorrectKeywords &) override;
    auto trialNumber() -> int override;
    auto targetFileName() -> std::string override;

  private:
    void initializeTest_(const AdaptiveTest &);

    AdaptiveMethod &adaptiveMethod;
    FixedLevelMethod &fixedLevelMethod;
    TargetListReader &targetsWithReplacementReader;
    TargetList &targetsWithReplacement;
    FiniteTargetList &silentIntervalTargets;
    FiniteTargetList &everyTargetOnce;
    RecognitionTestModel &model;
};
}

#endif
