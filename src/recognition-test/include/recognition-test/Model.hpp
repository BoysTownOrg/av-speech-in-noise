#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_MODEL_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_MODEL_HPP_

#include "TargetList.hpp"
#include "TestMethod.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <gsl/gsl>
#include <cstdint>
#include <vector>
#include <string>
#include <memory>

namespace av_speech_in_noise {
struct EyeGaze {
    float x;
    float y;
};

struct EyeTrackerSystemTime {
    std::int_least64_t microseconds;
};

struct TargetPlayerSystemTime {
    std::uintmax_t nanoseconds;
};

struct EyeTrackerTargetPlayerSynchronization {
    EyeTrackerSystemTime eyeTrackerSystemTime;
    TargetPlayerSystemTime targetPlayerSystemTime;
};

struct BinocularGazeSample {
    EyeTrackerSystemTime systemTime;
    EyeGaze left;
    EyeGaze right;
};

using BinocularGazeSamples = typename std::vector<BinocularGazeSample>;

struct TargetStartTime : TargetPlayerSystemTime {};

struct Target {
    std::string target;
};

struct AdaptiveProgress {
    SNR snr{};
    int reversals{};
};

namespace open_set {
struct Trial : Target {};

struct AdaptiveTrial : AdaptiveProgress, Trial {
    bool correct{};
};
}

namespace coordinate_response_measure {
struct Trial : Target {
    int correctNumber{};
    int subjectNumber{};
    Color correctColor{};
    Color subjectColor{};
    bool correct{};
};

struct AdaptiveTrial : AdaptiveProgress, Trial {};

struct FixedLevelTrial : Trial {};
}

struct CorrectKeywordsTrial : CorrectKeywords, open_set::AdaptiveTrial {};

struct FreeResponseTrial : FreeResponse, open_set::Trial {};

class OutputFile {
  public:
    virtual ~OutputFile() = default;
    virtual void openNewFile(const TestIdentity &) = 0;
    class OpenFailure {};
    virtual void write(const coordinate_response_measure::AdaptiveTrial &) = 0;
    virtual void write(
        const coordinate_response_measure::FixedLevelTrial &) = 0;
    virtual void write(const FreeResponseTrial &) = 0;
    virtual void write(const CorrectKeywordsTrial &) = 0;
    virtual void write(const open_set::AdaptiveTrial &) = 0;
    virtual void write(const AdaptiveTest &) = 0;
    virtual void write(const FixedLevelTest &) = 0;
    virtual void write(const AdaptiveTestResults &) = 0;
    virtual void write(const BinocularGazeSamples &) = 0;
    virtual void write(TargetStartTime) = 0;
    virtual void write(const EyeTrackerTargetPlayerSynchronization &) = 0;
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
    virtual auto read(const LocalUrl &) -> lists_type = 0;
};

class AdaptiveMethod : public virtual TestMethod {
  public:
    virtual void initialize(const AdaptiveTest &, TargetListReader *) = 0;
    virtual void resetTracks() = 0;
    virtual auto testResults() -> AdaptiveTestResults = 0;
};

class FixedLevelMethod : public virtual TestMethod {
  public:
    virtual void initialize(const FixedLevelTest &, TargetList *) = 0;
    virtual void initialize(const FixedLevelTest &, FiniteTargetList *) = 0;
};

class RecognitionTestModel {
  public:
    virtual ~RecognitionTestModel() = default;
    virtual void subscribe(Model::EventListener *) = 0;
    virtual void initialize(TestMethod *, const Test &) = 0;
    virtual void initializeWithSingleSpeaker(TestMethod *, const Test &) = 0;
    virtual void initializeWithDelayedMasker(TestMethod *, const Test &) = 0;
    virtual void initializeWithEyeTracking(TestMethod *, const Test &) = 0;
    virtual void playTrial(const AudioSettings &) = 0;
    virtual void playCalibration(const Calibration &) = 0;
    virtual void submit(const coordinate_response_measure::Response &) = 0;
    virtual void submit(const FreeResponse &) = 0;
    virtual void submit(const CorrectKeywords &) = 0;
    virtual void submitCorrectResponse() = 0;
    virtual void submitIncorrectResponse() = 0;
    virtual auto testComplete() -> bool = 0;
    virtual auto audioDevices() -> AudioDevices = 0;
    virtual auto trialNumber() -> int = 0;
    virtual auto targetFileName() -> std::string = 0;
    virtual void prepareNextTrialIfNeeded() = 0;
};

class ModelImpl : public Model {
  public:
    ModelImpl(AdaptiveMethod &, FixedLevelMethod &,
        TargetListReader &targetsWithReplacementReader,
        TargetListReader &cyclicTargetsReader,
        TargetList &targetsWithReplacement,
        FiniteTargetList &silentIntervalTargets,
        FiniteTargetList &everyTargetOnce, RecognitionTestModel &);
    void subscribe(Model::EventListener *) override;
    void initialize(const AdaptiveTest &) override;
    void initializeWithTargetReplacement(const FixedLevelTest &) override;
    void initializeWithSilentIntervalTargets(const FixedLevelTest &) override;
    void initializeWithAllTargets(const FixedLevelTest &) override;
    void initializeWithAllTargetsAndEyeTracking(
        const FixedLevelTest &) override;
    void initializeWithSingleSpeaker(const AdaptiveTest &) override;
    void initializeWithDelayedMasker(const AdaptiveTest &) override;
    void initializeWithTargetReplacementAndEyeTracking(
        const FixedLevelTest &) override;
    void initializeWithSilentIntervalTargetsAndEyeTracking(
        const FixedLevelTest &);
    void initializeWithEyeTracking(const AdaptiveTest &) override;
    void initializeWithCyclicTargets(const AdaptiveTest &) override;
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
    auto adaptiveTestResults() -> AdaptiveTestResults override;
    void restartAdaptiveTestWhilePreservingTargets() override;

  private:
    void initializeTest_(const AdaptiveTest &);

    AdaptiveMethod &adaptiveMethod;
    FixedLevelMethod &fixedLevelMethod;
    TargetListReader &targetsWithReplacementReader;
    TargetListReader &cyclicTargetsReader;
    TargetList &targetsWithReplacement;
    FiniteTargetList &silentIntervalTargets;
    FiniteTargetList &everyTargetOnce;
    RecognitionTestModel &model;
};
}

#endif
