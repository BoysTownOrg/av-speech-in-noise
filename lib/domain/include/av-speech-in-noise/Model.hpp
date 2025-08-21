#ifndef AV_SPEECH_IN_NOISE_LIB_DOMAIN_INCLUDE_AVSPEECHINNOISE_MODELHPP_
#define AV_SPEECH_IN_NOISE_LIB_DOMAIN_INCLUDE_AVSPEECHINNOISE_MODELHPP_

#include <ostream>
#include <string>
#include <variant>
#include <vector>
#include <cstdint>

namespace av_speech_in_noise {
namespace coordinate_response_measure {
enum class Color : std::uint8_t { green, red, blue, white, unknown };

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

struct LocalUrl {
    std::string path;
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

enum class Consonant : std::uint8_t {
    bi,
    si,
    di,
    hi,
    ki,
    mi,
    ni,
    pi,
    shi,
    ti,
    vi,
    zi,
    fi,
    thi,
    unknown
};

struct ConsonantResponse {
    Consonant consonant{};
    double seconds{};
};

enum class Emotion : std::uint8_t {
    angry,
    disgusted,
    happy,
    neutral,
    sad,
    scared,
    surprised,
    unknown
};

struct EmotionResponse : Flaggable {
    Emotion emotion{};
    double reactionTimeMilliseconds{};
};

enum class KeyPressed : std::uint8_t { first, second, unknown };

struct KeyPressResponse : Flaggable {
    KeyPressed key{KeyPressed::unknown};
    double seconds{};
};

enum class Syllable : std::uint8_t {
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

struct RealLevel {
    int dB_SPL{};
};

enum class AudioChannelOption : std::uint8_t {
    all,
    singleSpeaker,
    delayedMasker
};

struct RationalNumber {
    int numerator;
    int denominator;
};

struct RealLevelDifference {
    int dB;
};

struct SNR : RealLevelDifference {
    explicit constexpr SNR(int dB = {}) : RealLevelDifference{dB} {}
};

struct FloatSNR {
    double dB;
};

struct Phi {
    double alpha;
    double beta;
    double gamma;
    double lambda;
};

inline auto operator<<(std::ostream &os, Phi phi) -> std::ostream & {
    return os << phi.alpha << ' ' << phi.beta << ' ' << phi.gamma << ' '
              << phi.lambda;
}

using Threshold = double;

struct AdaptiveTestResult {
    LocalUrl targetsUrl;
    std::variant<Threshold, Phi> result{};
};

struct KeywordsTestResults {
    double percentCorrect;
    int totalCorrect;
};

using AdaptiveTestResults = typename std::vector<AdaptiveTestResult>;

struct AudioSettings {
    std::string audioDevice;
};

struct Calibration : AudioSettings {};

using AudioDevices = typename std::vector<std::string>;

struct EyeGaze {
    float x;
    float y;
};

struct Point2D {
    float x;
    float y;
};

struct Point3D {
    float x;
    float y;
    float z;
};

struct GazeOrigin {
    Point3D relativeTrackbox{};
    bool valid{true};
};

struct GazePosition {
    Point3D relativeTrackbox{};
    Point2D relativeScreen{};
    bool valid{true};
};

struct Gaze {
    GazeOrigin origin;
    GazePosition position;
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
    EyeTrackerSystemTime systemTime{};
    Gaze left;
    Gaze right;
};

using BinocularGazeSamples = typename std::vector<BinocularGazeSample>;

struct TargetStartTime : TargetPlayerSystemTime {
    explicit constexpr TargetStartTime(std::uintmax_t nanoseconds = 0)
        : TargetPlayerSystemTime{nanoseconds} {}
};
}

#endif
