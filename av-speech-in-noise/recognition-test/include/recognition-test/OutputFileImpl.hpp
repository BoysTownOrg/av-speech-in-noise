#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_OUTPUTFILEIMPL_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_OUTPUTFILEIMPL_HPP_

#include "RecognitionTestModel.hpp"
#include <string>

namespace av_speech_in_noise {
enum class HeadingItem {
    snr_dB,
    correctNumber,
    subjectNumber,
    correctColor,
    subjectColor,
    evaluation,
    stimulus,
    reversals,
    target,
    freeResponse
};

constexpr const char *headingItemName(HeadingItem i) {
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
        case HeadingItem::stimulus:
            return "stimulus";
        case HeadingItem::target:
            return "target";
        case HeadingItem::reversals:
            return "reversals";
        case HeadingItem::freeResponse:
            return "response";
    }
}

constexpr const char *colorName(coordinate_response_measure::Color c) {
    switch (c) {
    case coordinate_response_measure::Color::green:
        return "green";
    case coordinate_response_measure::Color::red:
        return "red";
    case coordinate_response_measure::Color::blue:
        return "blue";
    case coordinate_response_measure::Color::white:
        return "white";
    case coordinate_response_measure::Color::notAColor:
        return "not a color";
    }
    return "unknown";
}

class Writer {
public:
    virtual ~Writer() = default;
    virtual void write(std::string) = 0;
    virtual void open(std::string) = 0;
    virtual bool failed() = 0;
    virtual void close() = 0;
    virtual void save() = 0;
};

class OutputFilePath {
public:
    virtual ~OutputFilePath() = default;
    virtual std::string generateFileName(const TestInformation &) = 0;
    virtual std::string homeDirectory() = 0;
    virtual std::string outputDirectory() = 0;
};

class OutputFileImpl : public OutputFile {
    Writer *writer;
    OutputFilePath *path;
    bool justWroteFixedLevelCoordinateResponseTrial{};
    bool justWroteAdaptiveCoordinateResponseTrial{};
    bool justWroteFreeResponseTrial{};
    bool justWroteOpenSetAdaptiveTrial{};
public:
    OutputFileImpl(Writer *, OutputFilePath *);
    void openNewFile(const TestInformation &) override;
    void close() override;
    void save() override;
    void writeTest(const AdaptiveTest &) override;
    void writeTest(const FixedLevelTest &) override;
    void writeTrial(
        const coordinate_response_measure::AdaptiveTrial &
    ) override;
    void writeTrial(
        const coordinate_response_measure::FixedLevelTrial &
    ) override;
    void writeTrial(const FreeResponseTrial &) override;
    void writeTrial(const open_set::AdaptiveTrial &) override;

private:
    void write(std::string);
    std::string formatTest(const AdaptiveTest &);
    std::string formatTest(const FixedLevelTest &);
    std::string generateNewFilePath(const TestInformation &);
};
}


#endif
