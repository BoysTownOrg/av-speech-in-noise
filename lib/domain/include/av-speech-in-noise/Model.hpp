#ifndef AV_SPEECH_IN_NOISE_LIB_DOMAIN_INCLUDE_AVSPEECHINNOISE_MODELHPP_
#define AV_SPEECH_IN_NOISE_LIB_DOMAIN_INCLUDE_AVSPEECHINNOISE_MODELHPP_

#include <string>
#include <vector>
#include <cstdint>

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

struct Flaggable {
    bool flagged{};
};

struct ThreeKeywordsResponse : Flaggable {
    bool firstCorrect{};
    bool secondCorrect{};
    bool thirdCorrect{};
};

struct FreeResponse : Flaggable {
    std::string response;
};

struct TestIdentity {
    std::string subjectId;
    std::string testerId;
    std::string session;
    std::string method;
    std::string rmeSetting;
    std::string transducer;
    std::string meta;
};

struct ConsonantResponse {
    char consonant{};
};

enum class Syllable {
    bi,
    di,
    dji,
    fi,
    gi,
    hi,
    ji,
    ki,
    li,
    mi,
    ni,
    pi,
    ri,
    shi,
    si,
    thi,
    ti,
    tsi,
    vi,
    wi,
    zi,
    unknown
};

struct SyllableResponse : Flaggable {
    Syllable syllable{};
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

struct KeywordsTestResults {
    double percentCorrect;
    int totalCorrect;
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

struct TargetStartTime : TargetPlayerSystemTime {
    explicit constexpr TargetStartTime(std::uintmax_t nanoseconds = 0)
        : TargetPlayerSystemTime{nanoseconds} {}
};

constexpr auto name(Condition c) -> const char * {
    switch (c) {
    case Condition::auditoryOnly:
        return "auditory-only";
    case Condition::audioVisual:
        return "audio-visual";
    }
}
}

#endif
