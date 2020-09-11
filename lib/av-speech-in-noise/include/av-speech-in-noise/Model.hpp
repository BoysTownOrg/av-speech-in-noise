#ifndef AV_SPEECH_IN_NOISE_AV_SPEECH_IN_NOISE_INCLUDE_AV_SPEECH_IN_NOISE_MODEL_HPP_
#define AV_SPEECH_IN_NOISE_AV_SPEECH_IN_NOISE_INCLUDE_AV_SPEECH_IN_NOISE_MODEL_HPP_

#include <stdexcept>
#include <string>
#include <vector>

namespace av_speech_in_noise {
namespace coordinate_response_measure {
enum class Color { green, red, blue, white, unknown };

struct Response {
    int number{};
    Color color{};
};
}

struct CorrectKeywords {
    int count{};
};

struct FreeResponse {
    std::string response;
    bool flagged{};
};

struct TestIdentity {
    std::string subjectId;
    std::string testerId;
    std::string session;
    std::string method;
    std::string rmeSetting;
    std::string transducer;
};

struct ConsonantResponse {
    char consonant{};
};

enum class Condition { auditoryOnly, audioVisual };

struct RealLevel {
    int dB_SPL{};
};

struct LocalUrl {
    std::string path;
};

struct Test {
    TestIdentity identity;
    LocalUrl targetsUrl;
    LocalUrl maskerFileUrl;
    RealLevel maskerLevel;
    RealLevel fullScaleLevel;
    Condition condition{};
};

struct TrackingSequence {
    int runCount{};
    int stepSize{};
    int down{};
    int up{};
};

using TrackingRule = typename std::vector<TrackingSequence>;

struct RealLevelDifference {
    int dB;
};

struct SNR : RealLevelDifference {
    explicit constexpr SNR(int dB = {}) : RealLevelDifference{dB} {}
};

struct AdaptiveTest : Test {
    TrackingRule trackingRule;
    SNR startingSnr{};
    SNR ceilingSnr{};
    SNR floorSnr{};
    int trackBumpLimit{};
    int thresholdReversals{};
};

struct AdaptiveTestResult {
    LocalUrl targetsUrl;
    double threshold{};
};

using AdaptiveTestResults = typename std::vector<AdaptiveTestResult>;

struct FixedLevelTest : Test {
    SNR snr{};
};

struct FixedLevelFixedTrialsTest : FixedLevelTest {
    int trials{30};
};

struct FixedLevelTestWithEachTargetNTimes : FixedLevelTest {
    int timesEachTargetIsPlayed{1};
};

struct AudioSettings {
    std::string audioDevice;
};

struct Calibration : AudioSettings {
    LocalUrl fileUrl;
    RealLevel level;
    RealLevel fullScaleLevel;
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
    virtual void attach(EventListener *) = 0;
    virtual void initialize(const AdaptiveTest &) = 0;
    virtual void initializeWithTargetReplacement(
        const FixedLevelFixedTrialsTest &) = 0;
    virtual void initializeWithTargetReplacementAndEyeTracking(
        const FixedLevelFixedTrialsTest &) = 0;
    virtual void initializeWithSingleSpeaker(const AdaptiveTest &) = 0;
    virtual void initializeWithDelayedMasker(const AdaptiveTest &) = 0;
    virtual void initializeWithEyeTracking(const AdaptiveTest &) = 0;
    virtual void initializeWithCyclicTargets(const AdaptiveTest &) = 0;
    virtual void initializeWithCyclicTargetsAndEyeTracking(
        const AdaptiveTest &) = 0;
    virtual void initializeWithSilentIntervalTargets(
        const FixedLevelTest &) = 0;
    virtual void initializeWithAllTargets(const FixedLevelTest &) = 0;
    virtual void initialize(const FixedLevelTestWithEachTargetNTimes &) = 0;
    virtual void initializeWithAllTargetsAndEyeTracking(
        const FixedLevelTest &) = 0;
    virtual void playCalibration(const Calibration &) = 0;
    virtual void playTrial(const AudioSettings &) = 0;
    virtual void submit(const coordinate_response_measure::Response &) = 0;
    virtual void submit(const FreeResponse &) = 0;
    virtual void submit(const CorrectKeywords &) = 0;
    virtual void submit(const ConsonantResponse &) = 0;
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
