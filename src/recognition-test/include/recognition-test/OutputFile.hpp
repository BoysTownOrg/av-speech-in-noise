#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_OUTPUTFILE_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_OUTPUTFILE_HPP_

#include "Model.hpp"
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
    leftGaze,
    rightGaze,
    eyeTrackerTime,
    targetPlayerTime
};

constexpr auto name(HeadingItem i) -> const char * {
    switch (i) {
    case HeadingItem::snr_dB:
        return "SNR (dB)";
    case HeadingItem::correctNumber:
        return "correct number";
    case HeadingItem::subjectNumber:
        return "subject number";
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
    case HeadingItem::leftGaze:
        return "left gaze [x y]";
    case HeadingItem::rightGaze:
        return "right gaze [x y]";
    case HeadingItem::eyeTrackerTime:
        return "eye tracker time (us)";
    case HeadingItem::targetPlayerTime:
        return "target player time (ns)";
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

class Writer {
  public:
    virtual ~Writer() = default;
    virtual void write(const std::string &) = 0;
    virtual void open(const std::string &) = 0;
    virtual auto failed() -> bool = 0;
    virtual void close() = 0;
    virtual void save() = 0;
};

class OutputFilePath {
  public:
    virtual ~OutputFilePath() = default;
    virtual auto generateFileName(const TestIdentity &) -> std::string = 0;
    virtual auto homeDirectory() -> std::string = 0;
    virtual auto outputDirectory() -> std::string = 0;
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
    void write(const ConsonantTrial &) override {}
    void write(const AdaptiveTestResults &) override;
    void write(const BinocularGazeSamples &) override;
    void write(TargetStartTime) override;
    void write(const EyeTrackerTargetPlayerSynchronization &) override;

  private:
    void write(std::string);
    auto generateNewFilePath(const TestIdentity &) -> std::string;

    Writer &writer;
    OutputFilePath &path;
    bool justWroteFixedLevelCoordinateResponseTrial{};
    bool justWroteAdaptiveCoordinateResponseTrial{};
    bool justWroteFreeResponseTrial{};
    bool justWroteOpenSetAdaptiveTrial{};
    bool justWroteCorrectKeywordsTrial{};
};
}

#endif
