#ifndef AV_SPEECH_IN_NOISE_LIB_DOMAIN_INCLUDE_AVSPEECHINNOISE_MODELHPP_
#define AV_SPEECH_IN_NOISE_LIB_DOMAIN_INCLUDE_AVSPEECHINNOISE_MODELHPP_

#include <cstddef>
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
    LocalUrl relativeOutputUrl;
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

enum class Condition : std::uint8_t { auditoryOnly, audioVisual };

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

struct Test {
    TestIdentity identity;
    LocalUrl targetsUrl;
    LocalUrl maskerFileUrl;
    RationalNumber videoScale{2, 3};
    RealLevel maskerLevel;
    RealLevel fullScaleLevel;
    Condition condition{};
    AudioChannelOption audioChannelOption{AudioChannelOption::all};
    bool keepVideoShown{};
    bool enableVibrotactileStimulus{};
};

struct TrackingSequence {
    int runCount{};
    int stepSize{};
    int down{};
    int up{};
};

using TrackingRule = typename std::vector<TrackingSequence>;

enum class PriorProbabilityKind : std::uint8_t { LinearNorm, LogNorm, Flat };

enum class ParameterSpace : std::uint8_t { Linear, Log };

struct PriorProbabilitySetting {
    double mu;
    double sigma;
    PriorProbabilityKind kind;
};

struct ParameterSpaceSetting {
    double lower;
    double upper;
    std::size_t N;
    ParameterSpace space;
};

struct PhiParameterSetting {
    ParameterSpaceSetting space;
    PriorProbabilitySetting priorProbability;
};

struct UmlSettings {
    PhiParameterSetting alpha{{-30, 30, 61, ParameterSpace::Linear},
        {0, 10, PriorProbabilityKind::LinearNorm}};
    PhiParameterSetting beta{{0.1, 10, 41, ParameterSpace::Log},
        {-0.5, 0.4, PriorProbabilityKind::LogNorm}};
    PhiParameterSetting gamma{{0.02, 0.2, 11, ParameterSpace::Linear},
        {{}, {}, PriorProbabilityKind::Flat}};
    PhiParameterSetting lambda{{0.02, 0.2, 11, ParameterSpace::Linear},
        {{}, {}, PriorProbabilityKind::Flat}};
    int trials{};
};

struct LevittSettings {
    TrackingRule trackingRule;
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

struct AdaptiveTest : Test {
    UmlSettings umlSettings;
    LevittSettings levittSettings;
    SNR startingSnr{};
    SNR ceilingSnr{};
    SNR floorSnr{};
    int trackBumpLimit{};
    int thresholdReversals{};
    bool uml{false};
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
