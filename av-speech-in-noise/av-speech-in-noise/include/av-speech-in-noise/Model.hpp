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

struct Trial {
    std::string target;
    int correctNumber{};
    int subjectNumber{};
    Color correctColor{};
    Color subjectColor{};
    bool correct{};
};

struct AdaptiveTrial : Trial {
    int SNR_dB{};
    int reversals{};
};

struct FixedLevelTrial : Trial {};
}

namespace open_set {
struct FreeResponse {
    std::string response;
    bool flagged{};
};

struct Trial {
    std::string target;
};

struct FreeResponseTrial : Trial {
    std::string response;
    bool flagged{};
};

struct AdaptiveTrial : Trial {
    int SNR_dB{};
    int reversals{};
    bool correct{};
};
}

enum class Condition { auditoryOnly, audioVisual };

constexpr auto conditionName(Condition c) -> const char * {
    switch (c) {
    case Condition::auditoryOnly:
        return "auditory-only";
    case Condition::audioVisual:
        return "audio-visual";
    }
}

struct TrackingSequence {
    int runCount{};
    int stepSize{};
    int down{};
    int up{};
};

constexpr auto operator==(const TrackingSequence &a, const TrackingSequence &b)
    -> bool {
    return a.down == b.down && a.up == b.up && a.runCount == b.runCount &&
        a.stepSize == b.stepSize;
}

using TrackingRule = typename std::vector<TrackingSequence>;

struct TestIdentity {
    std::string subjectId;
    std::string testerId;
    std::string session;
    std::string method;
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
    std::string trackSettingsFile;
    int startingSnr_dB{};
    int ceilingSnr_dB{};
    int floorSnr_dB{};
    int trackBumpLimit{};
};

struct FixedLevelTest : Test {
    int snr_dB{};
    int trials{30};
};

struct AudioSettings {
    std::string audioDevice;
};

struct Calibration {
    AudioSettings audioSettings;
    std::string filePath;
    int level_dB_SPL{};
    int fullScaleLevel_dB_SPL{};
    Condition condition{};
};

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
    virtual void initialize(const FixedLevelTest &) = 0;
    virtual void initializeWithSingleSpeaker(const AdaptiveTest &) = 0;
    virtual void initializeWithDelayedMasker(const AdaptiveTest &) = 0;
    virtual void initializeWithEyeTracking(const AdaptiveTest &) = 0;
    virtual void initializeWithSilentIntervalTargets(const FixedLevelTest &) = 0;
    virtual void initializeWithAllTargets(const FixedLevelTest &) = 0;
    virtual void playCalibration(const Calibration &) = 0;
    virtual void playTrial(const AudioSettings &) = 0;
    virtual void submitResponse(
        const coordinate_response_measure::Response &) = 0;
    virtual void submitResponse(const open_set::FreeResponse &) = 0;
    virtual void submitCorrectResponse() = 0;
    virtual void submitIncorrectResponse() = 0;
    virtual auto testComplete() -> bool = 0;
    virtual auto audioDevices() -> std::vector<std::string> = 0;
    virtual auto trialNumber() -> int = 0;
};
}

#endif
