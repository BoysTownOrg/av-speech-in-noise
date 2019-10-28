#include "OutputFileImpl.hpp"
#include <sstream>

namespace av_speech_in_noise {
namespace {
class FormattedStream {
    std::stringstream stream;

  public:
    template <typename T> void writeLabeledLine(std::string label, T thing) {
        stream << std::move(label);
        stream << ": ";
        stream << thing;
        stream << '\n';
    }

    template <typename T> void insert(T item) { stream << item; }

    void insertCommaAndSpace() { stream << ", "; }

    void insertNewLine() { stream << '\n'; }

    auto str() const { return stream.str(); }

    void writeSubjectId(const TestIdentity &p) {
        writeLabeledLine("subject", p.subjectId);
    }

    void writeTester(const TestIdentity &p) {
        writeLabeledLine("tester", p.testerId);
    }

    void writeSession(const TestIdentity &p) {
        writeLabeledLine("session", p.session);
    }

    void writeMasker(const Test &p) {
        writeLabeledLine("masker", p.maskerFilePath);
    }

    void writeTargetList(const Test &p) {
        writeLabeledLine("targets", p.targetListDirectory);
    }

    void writeMaskerLevel(const Test &p) {
        writeLabeledLine("masker level (dB SPL)", p.maskerLevel_dB_SPL);
    }

    void writeCondition(const Test &p) {
        writeLabeledLine("condition", conditionName(p.condition));
    }
};
}

OutputFileImpl::OutputFileImpl(Writer *writer, OutputFilePath *path)
    : writer{writer}, path{path} {}

void OutputFileImpl::write(std::string s) { writer->write(std::move(s)); }

constexpr const char *correct() { return "correct"; }

constexpr const char *incorrect() { return "incorrect"; }

static std::string evaluation(const open_set::AdaptiveTrial &trial) {
    return trial.correct ? correct() : incorrect();
}

static std::string evaluation(const coordinate_response_measure::Trial &trial) {
    return trial.correct ? correct() : incorrect();
}

static std::string formatTrial(
    const coordinate_response_measure::FixedLevelTrial &trial) {
    FormattedStream stream;
    stream.insert(trial.correctNumber);
    stream.insertCommaAndSpace();
    stream.insert(trial.subjectNumber);
    stream.insertCommaAndSpace();
    stream.insert(colorName(trial.correctColor));
    stream.insertCommaAndSpace();
    stream.insert(colorName(trial.subjectColor));
    stream.insertCommaAndSpace();
    stream.insert(evaluation(trial));
    stream.insertCommaAndSpace();
    stream.insert(trial.target);
    stream.insertNewLine();
    return stream.str();
}

static std::string formatTrial(
    const coordinate_response_measure::AdaptiveTrial &trial) {
    FormattedStream stream;
    stream.insert(trial.SNR_dB);
    stream.insertCommaAndSpace();
    stream.insert(trial.correctNumber);
    stream.insertCommaAndSpace();
    stream.insert(trial.subjectNumber);
    stream.insertCommaAndSpace();
    stream.insert(colorName(trial.correctColor));
    stream.insertCommaAndSpace();
    stream.insert(colorName(trial.subjectColor));
    stream.insertCommaAndSpace();
    stream.insert(evaluation(trial));
    stream.insertCommaAndSpace();
    stream.insert(trial.reversals);
    stream.insertNewLine();
    return stream.str();
}

static std::string formatTrial(const FreeResponseTrial &trial) {
    FormattedStream stream;
    stream.insert(trial.target);
    stream.insertCommaAndSpace();
    stream.insert(trial.response);
    stream.insertNewLine();
    return stream.str();
}

static std::string formatTrial(const open_set::AdaptiveTrial &trial) {
    FormattedStream stream;
    stream.insert(trial.SNR_dB);
    stream.insertCommaAndSpace();
    stream.insert(trial.target);
    stream.insertCommaAndSpace();
    stream.insert(evaluation(trial));
    stream.insertCommaAndSpace();
    stream.insert(trial.reversals);
    stream.insertNewLine();
    return stream.str();
}

static std::string formatOpenSetFreeResponseTrialHeading() {
    FormattedStream stream;
    stream.insert(headingItemName(HeadingItem::target));
    stream.insertCommaAndSpace();
    stream.insert(headingItemName(HeadingItem::freeResponse));
    stream.insertNewLine();
    return stream.str();
}

static std::string formatAdaptiveCoordinateResponseTrialHeading() {
    FormattedStream stream;
    stream.insert(headingItemName(HeadingItem::snr_dB));
    stream.insertCommaAndSpace();
    stream.insert(headingItemName(HeadingItem::correctNumber));
    stream.insertCommaAndSpace();
    stream.insert(headingItemName(HeadingItem::subjectNumber));
    stream.insertCommaAndSpace();
    stream.insert(headingItemName(HeadingItem::correctColor));
    stream.insertCommaAndSpace();
    stream.insert(headingItemName(HeadingItem::subjectColor));
    stream.insertCommaAndSpace();
    stream.insert(headingItemName(HeadingItem::evaluation));
    stream.insertCommaAndSpace();
    stream.insert(headingItemName(HeadingItem::reversals));
    stream.insertNewLine();
    return stream.str();
}

static std::string formatFixedLevelCoordinateResponseTrialHeading() {
    FormattedStream stream;
    stream.insert(headingItemName(HeadingItem::correctNumber));
    stream.insertCommaAndSpace();
    stream.insert(headingItemName(HeadingItem::subjectNumber));
    stream.insertCommaAndSpace();
    stream.insert(headingItemName(HeadingItem::correctColor));
    stream.insertCommaAndSpace();
    stream.insert(headingItemName(HeadingItem::subjectColor));
    stream.insertCommaAndSpace();
    stream.insert(headingItemName(HeadingItem::evaluation));
    stream.insertCommaAndSpace();
    stream.insert(headingItemName(HeadingItem::stimulus));
    stream.insertNewLine();
    return stream.str();
}

static std::string formatOpenSetAdaptiveTrialHeading() {
    FormattedStream stream;
    stream.insert(headingItemName(HeadingItem::snr_dB));
    stream.insertCommaAndSpace();
    stream.insert(headingItemName(HeadingItem::target));
    stream.insertCommaAndSpace();
    stream.insert(headingItemName(HeadingItem::evaluation));
    stream.insertCommaAndSpace();
    stream.insert(headingItemName(HeadingItem::reversals));
    stream.insertNewLine();
    return stream.str();
}

void OutputFileImpl::writeTrial(
    const coordinate_response_measure::AdaptiveTrial &trial) {
    if (!justWroteAdaptiveCoordinateResponseTrial)
        write(formatAdaptiveCoordinateResponseTrialHeading());
    write(formatTrial(trial));
    justWroteAdaptiveCoordinateResponseTrial = true;
}

void OutputFileImpl::writeTrial(
    const coordinate_response_measure::FixedLevelTrial &trial) {
    if (!justWroteFixedLevelCoordinateResponseTrial)
        write(formatFixedLevelCoordinateResponseTrialHeading());
    write(formatTrial(trial));
    justWroteFixedLevelCoordinateResponseTrial = true;
}

void OutputFileImpl::writeTrial(const FreeResponseTrial &trial) {
    if (!justWroteFreeResponseTrial)
        write(formatOpenSetFreeResponseTrialHeading());
    write(formatTrial(trial));
    justWroteFreeResponseTrial = true;
}

void OutputFileImpl::writeTrial(const open_set::AdaptiveTrial &trial) {
    if (!justWroteOpenSetAdaptiveTrial)
        write(formatOpenSetAdaptiveTrialHeading());
    write(formatTrial(trial));
    justWroteOpenSetAdaptiveTrial = true;
}

void OutputFileImpl::writeTest(const AdaptiveTest &test) {
    write(formatTest(test));
}

void OutputFileImpl::writeTest(const FixedLevelTest &test) {
    write(formatTest(test));
}

std::string OutputFileImpl::formatTest(const AdaptiveTest &test) {
    FormattedStream stream;
    auto information = test.identity;
    stream.writeSubjectId(information);
    stream.writeTester(information);
    stream.writeSession(information);
    auto common = test;
    stream.writeMasker(common);
    stream.writeTargetList(common);
    stream.writeMaskerLevel(common);
    stream.writeLabeledLine("starting SNR (dB)", test.startingSnr_dB);
    stream.writeCondition(common);
    stream.insertNewLine();
    return stream.str();
}

std::string OutputFileImpl::formatTest(const FixedLevelTest &test) {
    FormattedStream stream;
    auto information = test.identity;
    stream.writeSubjectId(information);
    stream.writeTester(information);
    stream.writeSession(information);
    auto common = test;
    stream.writeMasker(common);
    stream.writeTargetList(common);
    stream.writeMaskerLevel(common);
    stream.writeLabeledLine("SNR (dB)", test.snr_dB);
    stream.writeCondition(common);
    stream.insertNewLine();
    return stream.str();
}

void OutputFileImpl::openNewFile(const TestIdentity &test) {
    writer->open(generateNewFilePath(test));
    if (writer->failed())
        throw OpenFailure{};
    justWroteAdaptiveCoordinateResponseTrial = false;
    justWroteFixedLevelCoordinateResponseTrial = false;
    justWroteFreeResponseTrial = false;
    justWroteOpenSetAdaptiveTrial = false;
}

std::string OutputFileImpl::generateNewFilePath(const TestIdentity &test) {
    return path->outputDirectory() + "/" + path->generateFileName(test) +
        ".txt";
}

void OutputFileImpl::close() { writer->close(); }

void OutputFileImpl::save() { writer->save(); }
}
