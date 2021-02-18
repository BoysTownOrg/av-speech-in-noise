#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_IOUTPUTFILE_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_IOUTPUTFILE_HPP_

#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
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

struct ConsonantTrial : Target {
    char subjectConsonant{};
    char correctConsonant{};
    bool correct{};
};

struct ThreeKeywordsTrial : ThreeKeywordsResponse, open_set::Trial {};

struct SyllableTrial : open_set::Trial {
    Syllable correctSyllable{};
    Syllable subjectSyllable{};
    bool correct{};
    bool flagged{};
};

struct FreeResponseTrial : FreeResponse, open_set::Trial {};

class OutputFile {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(OutputFile);
    virtual void openNewFile(const TestIdentity &) = 0;
    class OpenFailure {};
    virtual void write(const coordinate_response_measure::AdaptiveTrial &) = 0;
    virtual void write(
        const coordinate_response_measure::FixedLevelTrial &) = 0;
    virtual void write(const FreeResponseTrial &) = 0;
    virtual void write(const CorrectKeywordsTrial &) = 0;
    virtual void write(const open_set::AdaptiveTrial &) = 0;
    virtual void write(const ConsonantTrial &) = 0;
    virtual void write(const AdaptiveTest &) = 0;
    virtual void write(const FixedLevelTest &) = 0;
    virtual void write(const AdaptiveTestResults &) = 0;
    virtual void write(const BinocularGazeSamples &) = 0;
    virtual void write(TargetStartTime) = 0;
    virtual void write(const EyeTrackerTargetPlayerSynchronization &) = 0;
    virtual void write(const ThreeKeywordsTrial &) {}
    virtual void write(const SyllableTrial &) {}
    virtual void close() = 0;
    virtual void save() = 0;
};
}

#endif
