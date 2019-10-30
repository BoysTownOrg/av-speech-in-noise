#include "OutputFile.hpp"
#include <sstream>

namespace av_speech_in_noise {
namespace {
class FormattedStream {
  public:
    template <typename T>
    void writeLabeledLine(const std::string &label, T thing) {
        stream << label;
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

  private:
    std::stringstream stream;
};
}

template <typename T> void insert(FormattedStream &stream, T item) {
    stream.insert(item);
}

OutputFileImpl::OutputFileImpl(Writer *writer, OutputFilePath *path)
    : writer{writer}, path{path} {}

void OutputFileImpl::write(std::string s) { writer->write(std::move(s)); }

constexpr auto correct() -> const char * { return "correct"; }

constexpr auto incorrect() -> const char * { return "incorrect"; }

static auto evaluation(const open_set::AdaptiveTrial &trial) -> std::string {
    return trial.correct ? correct() : incorrect();
}

static auto evaluation(const coordinate_response_measure::Trial &trial)
    -> std::string {
    return trial.correct ? correct() : incorrect();
}

static auto formatTest(const AdaptiveTest &test) -> std::string {
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

static auto formatTest(const FixedLevelTest &test) -> std::string {
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

static auto formatTrial(
    const coordinate_response_measure::FixedLevelTrial &trial) -> std::string {
    FormattedStream stream;
    insert(stream, trial.correctNumber);
    stream.insertCommaAndSpace();
    insert(stream, trial.subjectNumber);
    stream.insertCommaAndSpace();
    insert(stream, colorName(trial.correctColor));
    stream.insertCommaAndSpace();
    insert(stream, colorName(trial.subjectColor));
    stream.insertCommaAndSpace();
    insert(stream, evaluation(trial));
    stream.insertCommaAndSpace();
    insert(stream, trial.target);
    stream.insertNewLine();
    return stream.str();
}

static auto formatTrial(const coordinate_response_measure::AdaptiveTrial &trial)
    -> std::string {
    FormattedStream stream;
    insert(stream, trial.SNR_dB);
    stream.insertCommaAndSpace();
    insert(stream, trial.correctNumber);
    stream.insertCommaAndSpace();
    insert(stream, trial.subjectNumber);
    stream.insertCommaAndSpace();
    insert(stream, colorName(trial.correctColor));
    stream.insertCommaAndSpace();
    insert(stream, colorName(trial.subjectColor));
    stream.insertCommaAndSpace();
    insert(stream, evaluation(trial));
    stream.insertCommaAndSpace();
    insert(stream, trial.reversals);
    stream.insertNewLine();
    return stream.str();
}

static auto formatTrial(const FreeResponseTrial &trial) -> std::string {
    FormattedStream stream;
    insert(stream, trial.target);
    stream.insertCommaAndSpace();
    insert(stream, trial.response);
    if (trial.flagged) {
        stream.insertCommaAndSpace();
        insert(stream, "FLAGGED");
    }
    stream.insertNewLine();
    return stream.str();
}

static auto formatTrial(const open_set::AdaptiveTrial &trial) -> std::string {
    FormattedStream stream;
    insert(stream, trial.SNR_dB);
    stream.insertCommaAndSpace();
    insert(stream, trial.target);
    stream.insertCommaAndSpace();
    insert(stream, evaluation(trial));
    stream.insertCommaAndSpace();
    insert(stream, trial.reversals);
    stream.insertNewLine();
    return stream.str();
}

static auto formatOpenSetFreeResponseTrialHeading() -> std::string {
    FormattedStream stream;
    insert(stream, headingItemName(HeadingItem::target));
    stream.insertCommaAndSpace();
    insert(stream, headingItemName(HeadingItem::freeResponse));
    stream.insertNewLine();
    return stream.str();
}

static auto formatAdaptiveCoordinateResponseTrialHeading() -> std::string {
    FormattedStream stream;
    insert(stream, headingItemName(HeadingItem::snr_dB));
    stream.insertCommaAndSpace();
    insert(stream, headingItemName(HeadingItem::correctNumber));
    stream.insertCommaAndSpace();
    insert(stream, headingItemName(HeadingItem::subjectNumber));
    stream.insertCommaAndSpace();
    insert(stream, headingItemName(HeadingItem::correctColor));
    stream.insertCommaAndSpace();
    insert(stream, headingItemName(HeadingItem::subjectColor));
    stream.insertCommaAndSpace();
    insert(stream, headingItemName(HeadingItem::evaluation));
    stream.insertCommaAndSpace();
    insert(stream, headingItemName(HeadingItem::reversals));
    stream.insertNewLine();
    return stream.str();
}

static auto formatFixedLevelCoordinateResponseTrialHeading() -> std::string {
    FormattedStream stream;
    insert(stream, headingItemName(HeadingItem::correctNumber));
    stream.insertCommaAndSpace();
    insert(stream, headingItemName(HeadingItem::subjectNumber));
    stream.insertCommaAndSpace();
    insert(stream, headingItemName(HeadingItem::correctColor));
    stream.insertCommaAndSpace();
    insert(stream, headingItemName(HeadingItem::subjectColor));
    stream.insertCommaAndSpace();
    insert(stream, headingItemName(HeadingItem::evaluation));
    stream.insertCommaAndSpace();
    insert(stream, headingItemName(HeadingItem::stimulus));
    stream.insertNewLine();
    return stream.str();
}

static auto formatOpenSetAdaptiveTrialHeading() -> std::string {
    FormattedStream stream;
    insert(stream, headingItemName(HeadingItem::snr_dB));
    stream.insertCommaAndSpace();
    insert(stream, headingItemName(HeadingItem::target));
    stream.insertCommaAndSpace();
    insert(stream, headingItemName(HeadingItem::evaluation));
    stream.insertCommaAndSpace();
    insert(stream, headingItemName(HeadingItem::reversals));
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

void OutputFileImpl::openNewFile(const TestIdentity &test) {
    writer->open(generateNewFilePath(test));
    if (writer->failed())
        throw OpenFailure{};
    justWroteAdaptiveCoordinateResponseTrial = false;
    justWroteFixedLevelCoordinateResponseTrial = false;
    justWroteFreeResponseTrial = false;
    justWroteOpenSetAdaptiveTrial = false;
}

auto OutputFileImpl::generateNewFilePath(const TestIdentity &test)
    -> std::string {
    return path->outputDirectory() + "/" + path->generateFileName(test) +
        ".txt";
}

void OutputFileImpl::close() { writer->close(); }

void OutputFileImpl::save() { writer->save(); }
}
