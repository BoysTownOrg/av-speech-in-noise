#ifndef AV_SPEECH_IN_NOISE_AV_SPEECH_IN_NOISE_INCLUDE_AV_SPEECH_IN_NOISE_MODEL_HPP_
#define AV_SPEECH_IN_NOISE_AV_SPEECH_IN_NOISE_INCLUDE_AV_SPEECH_IN_NOISE_MODEL_HPP_

#include <stdexcept>
#include <string>
#include <vector>
#include <cstdint>

namespace av_speech_in_noise {
struct AdaptiveProgress {
    int SNR_dB{};
    int reversals{};
};

struct Target {
    std::string target;
};

namespace coordinate_response_measure {
enum class Color { green, red, blue, white, unknown };

struct Response {
    int number{};
    Color color{};
};

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

namespace open_set {
struct Trial : Target {};
}

struct CorrectKeywords {
    int count{};
};

struct FreeResponse {
    std::string response;
    bool flagged{};
};

enum class Condition { auditoryOnly, audioVisual };

struct TrackingSequence {
    int runCount{};
    int stepSize{};
    int down{};
    int up{};
};

using TrackingRule = typename std::vector<TrackingSequence>;

enum class Transducer { headphone, oneSpeaker, twoSpeakers, unknown };

struct TestIdentity {
    std::string subjectId;
    std::string testerId;
    std::string session;
    std::string method;
    std::string rmeSetting;
    Transducer transducer{};
};

struct Test {
    TestIdentity identity;
    std::string targetListDirectory;
    std::string maskerFilePath;
    int maskerLevel_dB_SPL{};
    int fullScaleLevel_dB_SPL{};
    Condition condition{};
};

struct AdaptiveTest : Test {
    TrackingRule trackingRule;
    int startingSnr_dB{};
    int ceilingSnr_dB{};
    int floorSnr_dB{};
    int trackBumpLimit{};
    int thresholdReversals{};
};

struct AdaptiveTestResult {
    std::string targetListDirectory;
    double threshold;
};

using AdaptiveTestResults = typename std::vector<AdaptiveTestResult>;

struct FixedLevelTest : Test {
    int snr_dB{};
    int trials{30};
};

struct AudioSettings {
    std::string audioDevice;
};

struct Calibration : AudioSettings {
    std::string filePath;
    int level_dB_SPL{};
    int fullScaleLevel_dB_SPL{};
};

using AudioDevices = typename std::vector<std::string>;

class Model {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void trialComplete() = 0;
    };

    class RequestFailure : public std::runtime_error {
      public:
        explicit RequestFailure(const std::string &s) : std::runtime_error{s} {}
    };

    virtual ~Model() = default;
    virtual void subscribe(EventListener *) = 0;
    virtual void initialize(const AdaptiveTest &) = 0;
    virtual void initializeWithTargetReplacement(const FixedLevelTest &) = 0;
    virtual void initializeWithTargetReplacementAndEyeTracking(
        const FixedLevelTest &) = 0;
    virtual void initializeWithSingleSpeaker(const AdaptiveTest &) = 0;
    virtual void initializeWithDelayedMasker(const AdaptiveTest &) = 0;
    virtual void initializeWithEyeTracking(const AdaptiveTest &) = 0;
    virtual void initializeWithCyclicTargets(const AdaptiveTest &) = 0;
    virtual void initializeWithSilentIntervalTargets(
        const FixedLevelTest &) = 0;
    virtual void initializeWithAllTargets(const FixedLevelTest &) = 0;
    virtual void initializeWithAllTargetsAndEyeTracking(
        const FixedLevelTest &) = 0;
    virtual void playCalibration(const Calibration &) = 0;
    virtual void playTrial(const AudioSettings &) = 0;
    virtual void submit(const coordinate_response_measure::Response &) = 0;
    virtual void submit(const FreeResponse &) = 0;
    virtual void submit(const CorrectKeywords &) = 0;
    virtual void submitCorrectResponse() = 0;
    virtual void submitIncorrectResponse() = 0;
    virtual auto testComplete() -> bool = 0;
    virtual auto audioDevices() -> AudioDevices = 0;
    virtual auto trialNumber() -> int = 0;
    virtual auto targetFileName() -> std::string = 0;
    virtual void restartAdaptiveTestWhilePreservingTargets() = 0;
    virtual auto adaptiveTestResults() -> AdaptiveTestResults = 0;
};
}

#endif
