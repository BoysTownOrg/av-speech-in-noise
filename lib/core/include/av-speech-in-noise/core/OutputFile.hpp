#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_OUTPUTFILEHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_OUTPUTFILEHPP_

#include "IOutputFile.hpp"

#include <av-speech-in-noise/Interface.hpp>

#include <filesystem>
#include <string>

namespace av_speech_in_noise {
enum class HeadingItem {
    snr_dB,
    correctNumber,
    subjectNumber,
    correctColor,
    subjectColor,
    evaluation,
    reversals,
    target,
    correctKeywords,
    freeResponse,
    leftGazePositionRelativeScreen,
    rightGazePositionRelativeScreen,
    leftGazePositionRelativeScreenIsValid,
    rightGazePositionRelativeScreenIsValid,
    leftGazePositionRelativeTracker,
    rightGazePositionRelativeTracker,
    leftGazePositionRelativeTrackerIsValid,
    rightGazePositionRelativeTrackerIsValid,
    leftGazeOriginRelativeTracker,
    rightGazeOriginRelativeTracker,
    leftGazeOriginRelativeTrackerIsValid,
    rightGazeOriginRelativeTrackerIsValid,
    eyeTrackerTime,
    targetPlayerTime,
    correctConsonant,
    subjectConsonant,
    firstKeywordEvaluation,
    secondKeywordEvaluation,
    thirdKeywordEvaluation,
    subjectSyllable,
    correctSyllable,
    time,
    keyPressed,
    reactionTime,
    vibrotactileDuration,
    vibrotactileDelay
};

constexpr auto name(HeadingItem i) -> const char * {
    switch (i) {
    case HeadingItem::snr_dB:
        return "SNR (dB)";
    case HeadingItem::correctNumber:
        return "correct number";
    case HeadingItem::subjectNumber:
        return "subject number";
    case HeadingItem::correctConsonant:
        return "correct consonant";
    case HeadingItem::subjectConsonant:
        return "subject consonant";
    case HeadingItem::correctColor:
        return "correct color";
    case HeadingItem::subjectColor:
        return "subject color";
    case HeadingItem::evaluation:
        return "evaluation";
    case HeadingItem::target:
        return "target";
    case HeadingItem::reversals:
        return "reversals";
    case HeadingItem::freeResponse:
        return "response";
    case HeadingItem::correctKeywords:
        return "# correct keywords";
    case HeadingItem::leftGazePositionRelativeScreen:
        return "left gaze position relative screen [x y]";
    case HeadingItem::rightGazePositionRelativeScreen:
        return "right gaze position relative screen [x y]";
    case HeadingItem::leftGazePositionRelativeTracker:
        return "left gaze position relative tracker [x y z]";
    case HeadingItem::rightGazePositionRelativeTracker:
        return "right gaze position relative tracker [x y z]";
    case HeadingItem::leftGazeOriginRelativeTracker:
        return "left gaze origin relative tracker [x y z]";
    case HeadingItem::rightGazeOriginRelativeTracker:
        return "right gaze origin relative tracker [x y z]";
    case HeadingItem::eyeTrackerTime:
        return "eye tracker time (us)";
    case HeadingItem::targetPlayerTime:
        return "target player time (ns)";
    case HeadingItem::firstKeywordEvaluation:
        return "first keyword evaluation";
    case HeadingItem::secondKeywordEvaluation:
        return "second keyword evaluation";
    case HeadingItem::thirdKeywordEvaluation:
        return "third keyword evaluation";
    case HeadingItem::subjectSyllable:
        return "subject syllable";
    case HeadingItem::correctSyllable:
        return "correct syllable";
    case HeadingItem::time:
        return "time";
    case HeadingItem::keyPressed:
        return "key pressed";
    case HeadingItem::reactionTime:
        return "reaction time (ms)";
    case HeadingItem::vibrotactileDuration:
        return "vibrotactile duration (s)";
    case HeadingItem::vibrotactileDelay:
        return "vibrotactile delay (s)";
    case HeadingItem::leftGazePositionRelativeScreenIsValid:
        return "valid left gaze position relative screen";
    case HeadingItem::rightGazePositionRelativeScreenIsValid:
        return "valid right gaze position relative screen";
    case HeadingItem::leftGazePositionRelativeTrackerIsValid:
        return "valid left gaze position relative tracker";
    case HeadingItem::rightGazePositionRelativeTrackerIsValid:
        return "valid right gaze position relative tracker";
    case HeadingItem::leftGazeOriginRelativeTrackerIsValid:
        return "valid left gaze origin relative tracker";
    case HeadingItem::rightGazeOriginRelativeTrackerIsValid:
        return "valid right gaze origin relative tracker";
    }
}

constexpr auto name(Consonant c) -> const char * {
    switch (c) {
    case Consonant::bi:
        return "bi";
    case Consonant::si:
        return "si";
    case Consonant::di:
        return "di";
    case Consonant::hi:
        return "hi";
    case Consonant::ki:
        return "ki";
    case Consonant::mi:
        return "mi";
    case Consonant::ni:
        return "ni";
    case Consonant::pi:
        return "pi";
    case Consonant::shi:
        return "shi";
    case Consonant::ti:
        return "ti";
    case Consonant::vi:
        return "vi";
    case Consonant::zi:
        return "zi";
    case Consonant::thi:
        return "thi";
    case Consonant::fi:
        return "fi";
    case Consonant::unknown:
        return "?";
    }
};

constexpr auto name(Syllable i) -> const char * {
    switch (i) {
    case Syllable::bi:
        return "bi";
    case Syllable::di:
        return "di";
    case Syllable::dji:
        return "dji";
    case Syllable::fi:
        return "fi";
    case Syllable::gi:
        return "gi";
    case Syllable::hi:
        return "hi";
    case Syllable::ji:
        return "ji";
    case Syllable::ki:
        return "ki";
    case Syllable::li:
        return "li";
    case Syllable::mi:
        return "mi";
    case Syllable::ni:
        return "ni";
    case Syllable::pi:
        return "pi";
    case Syllable::ri:
        return "ri";
    case Syllable::shi:
        return "shi";
    case Syllable::si:
        return "si";
    case Syllable::thi:
        return "thi";
    case Syllable::ti:
        return "ti";
    case Syllable::tsi:
        return "tsi";
    case Syllable::vi:
        return "vi";
    case Syllable::wi:
        return "wi";
    case Syllable::zi:
        return "zi";
    case Syllable::unknown:
        return "?";
    }
}

constexpr auto name(coordinate_response_measure::Color c) -> const char * {
    switch (c) {
    case coordinate_response_measure::Color::green:
        return "green";
    case coordinate_response_measure::Color::red:
        return "red";
    case coordinate_response_measure::Color::blue:
        return "blue";
    case coordinate_response_measure::Color::white:
        return "white";
    case coordinate_response_measure::Color::unknown:
        return "unknown";
    }
}

constexpr auto name(KeyPressed k) -> const char * {
    switch (k) {
    case KeyPressed::first:
        return "first";
    case KeyPressed::second:
        return "second";
    case KeyPressed::unknown:
        return "unknown";
    }
}

class Writer {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Writer);
    virtual void write(const std::string &) = 0;
    virtual void write(Writable &) = 0;
    virtual void open(const std::string &) = 0;
    virtual auto failed() -> bool = 0;
    virtual void close() = 0;
    virtual void save() = 0;
};

class OutputFilePath {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(OutputFilePath);
    virtual auto generateFileName(const TestIdentity &) -> std::string = 0;
    virtual auto outputDirectory() -> std::string = 0;
    virtual void setRelativeOutputDirectory(std::filesystem::path) = 0;
};

class OutputFileImpl : public OutputFile {
  public:
    OutputFileImpl(Writer &, OutputFilePath &);
    void openNewFile(const TestIdentity &) override;
    void close() override;
    void save() override;
    void write(const AdaptiveTest &) override;
    void write(const FixedLevelTest &) override;
    void write(const coordinate_response_measure::AdaptiveTrial &) override;
    void write(const open_set::AdaptiveTrial &) override;
    void write(const coordinate_response_measure::FixedLevelTrial &) override;
    void write(const FreeResponseTrial &) override;
    void write(const CorrectKeywordsTrial &) override;
    void write(const ConsonantTrial &) override;
    void write(const ThreeKeywordsTrial &) override;
    void write(const AdaptiveTestResults &) override;
    void write(const BinocularGazeSamples &) override;
    void write(TargetStartTime) override;
    void write(const EyeTrackerTargetPlayerSynchronization &) override;
    void write(const SyllableTrial &) override;
    void write(const KeyPressTrial &) override;
    void write(Writable &) override;
    auto parentPath() -> std::filesystem::path override;

    enum class Trial : int;

  private:
    void write(const std::string &);
    auto generateNewFilePath(const TestIdentity &) -> std::string;

    Writer &writer;
    OutputFilePath &path;
    Trial currentTrial;
};
}

#endif
