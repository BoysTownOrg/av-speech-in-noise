#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_IOUTPUTFILEHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_IOUTPUTFILEHPP_

#include "Player.hpp"

#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>

#include <exception>
#include <filesystem>
#include <optional>
#include <ostream>
#include <string>

namespace av_speech_in_noise {
struct Target {
    std::string target;
};

struct AdaptiveProgress {
    std::optional<Phi> phi;
    FloatSNR snr{};
    int reversals{};
};

struct Evaluative {
    bool correct{};
};

namespace open_set {
struct Trial : Target {};

struct AdaptiveTrial : AdaptiveProgress, Trial, Evaluative {};
}

namespace coordinate_response_measure {
struct Trial : Target, Evaluative {
    int correctNumber{};
    int subjectNumber{};
    Color correctColor{};
    Color subjectColor{};
};

struct AdaptiveTrial : AdaptiveProgress, Trial {};

struct FixedLevelTrial : Trial {};
}

struct CorrectKeywordsTrial : CorrectKeywords, open_set::AdaptiveTrial {};

struct ReactionTime {
    double milliseconds;
};

struct ConsonantTrial : Target, Evaluative {
    Consonant subjectConsonant{};
    Consonant correctConsonant{};
    ReactionTime rt{};
};

struct EmotionTrial : EmotionResponse, Target {};

struct ThreeKeywordsTrial : ThreeKeywordsResponse, open_set::Trial {};

struct SyllableTrial : open_set::Trial, Evaluative, Flaggable {
    Syllable correctSyllable{};
    Syllable subjectSyllable{};
};

struct FreeResponseTrial : FreeResponse, open_set::Trial {
    std::string time;
};

struct KeyPressTrial : KeyPressResponse, open_set::Trial {
    VibrotactileStimulus vibrotactileStimulus;
    ReactionTime rt{};
};

struct PassFailTrial : Evaluative, Target {};

class Writable {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Writable);
    virtual void write(std::ostream &) = 0;
};

class OutputFile {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(OutputFile);
    virtual void openNewFile(const TestIdentity &) = 0;
    class OpenFailure : std::exception {};
    virtual void write(const coordinate_response_measure::AdaptiveTrial &) = 0;
    virtual void write(
        const coordinate_response_measure::FixedLevelTrial &) = 0;
    virtual void write(const FreeResponseTrial &) = 0;
    virtual void write(const KeyPressTrial &) = 0;
    virtual void write(const CorrectKeywordsTrial &) = 0;
    virtual void write(const open_set::AdaptiveTrial &) = 0;
    virtual void write(const ConsonantTrial &) = 0;
    virtual void write(const EmotionTrial &) = 0;
    virtual void write(const AdaptiveTest &) = 0;
    virtual void write(const FixedLevelTest &) = 0;
    virtual void write(const AdaptiveTestResults &) = 0;
    virtual void write(const BinocularGazeSamples &) = 0;
    virtual void write(TargetStartTime) = 0;
    virtual void write(const EyeTrackerTargetPlayerSynchronization &) = 0;
    virtual void write(const ThreeKeywordsTrial &) = 0;
    virtual void write(const SyllableTrial &) = 0;
    virtual void write(const PassFailTrial &) = 0;
    virtual void write(Writable &) = 0;
    virtual void close() = 0;
    virtual void save() = 0;
    virtual auto parentPath() -> std::filesystem::path = 0;
};
}

#endif
