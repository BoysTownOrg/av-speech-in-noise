#include "OutputFile.hpp"
#include "Model.hpp"
#include "RecognitionTestModel.hpp"
#include <algorithm>
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

    void writeMethod(const TestIdentity &p) {
        writeLabeledLine("method", p.method);
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

static void insert(FormattedStream &stream, HeadingItem item) {
    stream.insert(headingItemName(item));
}

static void insertCommaAndSpace(FormattedStream &stream) {
    stream.insertCommaAndSpace();
}

static void insertNewLine(FormattedStream &stream) { stream.insertNewLine(); }

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

static void writeSubjectId(
    FormattedStream &stream, const TestIdentity &identity) {
    stream.writeSubjectId(identity);
}

static void writeTester(FormattedStream &stream, const TestIdentity &identity) {
    stream.writeTester(identity);
}

static void writeSession(
    FormattedStream &stream, const TestIdentity &identity) {
    stream.writeSession(identity);
}

static void writeMethod(FormattedStream &stream, const TestIdentity &identity) {
    stream.writeMethod(identity);
}

static auto formatTest(const AdaptiveTest &test) -> std::string {
    FormattedStream stream;
    const auto &identity = test.identity;
    writeSubjectId(stream, identity);
    writeTester(stream, identity);
    writeSession(stream, identity);
    writeMethod(stream, identity);
    stream.writeMasker(test);
    stream.writeTargetList(test);
    stream.writeMaskerLevel(test);
    stream.writeLabeledLine("starting SNR (dB)", test.startingSnr_dB);
    stream.writeCondition(test);
    stream.insertNewLine();
    return stream.str();
}

static auto formatTest(const FixedLevelTest &test) -> std::string {
    FormattedStream stream;
    const auto &identity = test.identity;
    writeSubjectId(stream, identity);
    writeTester(stream, identity);
    writeSession(stream, identity);
    writeMethod(stream, identity);
    stream.writeMasker(test);
    stream.writeTargetList(test);
    stream.writeMaskerLevel(test);
    stream.writeLabeledLine("SNR (dB)", test.snr_dB);
    stream.writeCondition(test);
    stream.insertNewLine();
    return stream.str();
}

static auto format(const BinocularGazeSamples &gazes) -> std::string {
    FormattedStream stream;
    insert(stream, "system time (us)");
    insertCommaAndSpace(stream);
    insert(stream, "left gaze [x y]");
    insertCommaAndSpace(stream);
    insert(stream, "right gaze [x y]");
    std::for_each(gazes.begin(), gazes.end(), [&](auto g) {
        insertNewLine(stream);
        insert(stream, g.systemTimeMicroseconds);
        insertCommaAndSpace(stream);
        insert(stream, g.left.x);
        insert(stream, " ");
        insert(stream, g.left.y);
        insertCommaAndSpace(stream);
        insert(stream, g.right.x);
        insert(stream, " ");
        insert(stream, g.right.y);
    });
    insertNewLine(stream);
    return stream.str();
}

static auto format(const ConvertedAudioSampleSystemTime &time) -> std::string {
    FormattedStream stream;
    insert(stream, "fade in complete audio sample system time");
    insertNewLine(stream);
    insert(stream, "    ");
    stream.writeLabeledLine("system time (ns)", time.nanoseconds);
    insert(stream, "    ");
    stream.writeLabeledLine("offset (samples)", time.sampleOffset);
    return stream.str();
}

static auto formatTrial(
    const coordinate_response_measure::FixedLevelTrial &trial) -> std::string {
    FormattedStream stream;
    insert(stream, trial.correctNumber);
    insertCommaAndSpace(stream);
    insert(stream, trial.subjectNumber);
    insertCommaAndSpace(stream);
    insert(stream, colorName(trial.correctColor));
    insertCommaAndSpace(stream);
    insert(stream, colorName(trial.subjectColor));
    insertCommaAndSpace(stream);
    insert(stream, evaluation(trial));
    insertCommaAndSpace(stream);
    insert(stream, trial.target);
    insertNewLine(stream);
    return stream.str();
}

static auto formatTrial(const coordinate_response_measure::AdaptiveTrial &trial)
    -> std::string {
    FormattedStream stream;
    insert(stream, trial.SNR_dB);
    insertCommaAndSpace(stream);
    insert(stream, trial.correctNumber);
    insertCommaAndSpace(stream);
    insert(stream, trial.subjectNumber);
    insertCommaAndSpace(stream);
    insert(stream, colorName(trial.correctColor));
    insertCommaAndSpace(stream);
    insert(stream, colorName(trial.subjectColor));
    insertCommaAndSpace(stream);
    insert(stream, evaluation(trial));
    insertCommaAndSpace(stream);
    insert(stream, trial.reversals);
    insertNewLine(stream);
    return stream.str();
}

static auto formatTrial(const open_set::FreeResponseTrial &trial)
    -> std::string {
    FormattedStream stream;
    insert(stream, trial.target);
    insertCommaAndSpace(stream);
    insert(stream, trial.response);
    if (trial.flagged) {
        insertCommaAndSpace(stream);
        insert(stream, "FLAGGED");
    }
    insertNewLine(stream);
    return stream.str();
}

static auto formatTrial(const open_set::AdaptiveTrial &trial) -> std::string {
    FormattedStream stream;
    insert(stream, trial.SNR_dB);
    insertCommaAndSpace(stream);
    insert(stream, trial.target);
    insertCommaAndSpace(stream);
    insert(stream, evaluation(trial));
    insertCommaAndSpace(stream);
    insert(stream, trial.reversals);
    insertNewLine(stream);
    return stream.str();
}

static auto formatTrial(const open_set::CorrectKeywordsTrial &trial)
    -> std::string {
    FormattedStream stream;
    insert(stream, trial.SNR_dB);
    insertCommaAndSpace(stream);
    insert(stream, trial.target);
    insertCommaAndSpace(stream);
    insert(stream, trial.count);
    insertCommaAndSpace(stream);
    insert(stream, evaluation(trial));
    insertCommaAndSpace(stream);
    insert(stream, trial.reversals);
    insertNewLine(stream);
    return stream.str();
}

static auto formatOpenSetFreeResponseTrialHeading() -> std::string {
    FormattedStream stream;
    insert(stream, HeadingItem::target);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::freeResponse);
    insertNewLine(stream);
    return stream.str();
}

static auto formatCorrectKeywordsTrialHeading() -> std::string {
    FormattedStream stream;
    insert(stream, HeadingItem::snr_dB);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::target);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::correctKeywords);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::evaluation);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::reversals);
    insertNewLine(stream);
    return stream.str();
}

static auto formatAdaptiveCoordinateResponseTrialHeading() -> std::string {
    FormattedStream stream;
    insert(stream, HeadingItem::snr_dB);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::correctNumber);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::subjectNumber);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::correctColor);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::subjectColor);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::evaluation);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::reversals);
    insertNewLine(stream);
    return stream.str();
}

static auto formatFixedLevelCoordinateResponseTrialHeading() -> std::string {
    FormattedStream stream;
    insert(stream, HeadingItem::correctNumber);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::subjectNumber);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::correctColor);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::subjectColor);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::evaluation);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::stimulus);
    insertNewLine(stream);
    return stream.str();
}

static auto formatOpenSetAdaptiveTrialHeading() -> std::string {
    FormattedStream stream;
    insert(stream, HeadingItem::snr_dB);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::target);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::evaluation);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::reversals);
    insertNewLine(stream);
    return stream.str();
}

void OutputFileImpl::write(
    const coordinate_response_measure::AdaptiveTrial &trial) {
    if (!justWroteAdaptiveCoordinateResponseTrial)
        write(formatAdaptiveCoordinateResponseTrialHeading());
    write(formatTrial(trial));
    justWroteAdaptiveCoordinateResponseTrial = true;
}

void OutputFileImpl::write(
    const coordinate_response_measure::FixedLevelTrial &trial) {
    if (!justWroteFixedLevelCoordinateResponseTrial)
        write(formatFixedLevelCoordinateResponseTrialHeading());
    write(formatTrial(trial));
    justWroteFixedLevelCoordinateResponseTrial = true;
}

void OutputFileImpl::write(const open_set::FreeResponseTrial &trial) {
    if (!justWroteFreeResponseTrial)
        write(formatOpenSetFreeResponseTrialHeading());
    write(formatTrial(trial));
    justWroteFreeResponseTrial = true;
}

void OutputFileImpl::write(const open_set::CorrectKeywordsTrial &trial) {
    if (!justWroteCorrectKeywordsTrial)
        write(formatCorrectKeywordsTrialHeading());
    write(formatTrial(trial));
    justWroteCorrectKeywordsTrial = true;
}

void OutputFileImpl::write(const open_set::AdaptiveTrial &trial) {
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

void OutputFileImpl::write(const BinocularGazeSamples &gazes) {
    write(format(gazes));
}

void OutputFileImpl::writeFadeInComplete(
    const ConvertedAudioSampleSystemTime &time) {
    write(format(time));
}

void OutputFileImpl::openNewFile(const TestIdentity &test) {
    writer->open(generateNewFilePath(test));
    if (writer->failed())
        throw OpenFailure{};
    justWroteAdaptiveCoordinateResponseTrial = false;
    justWroteFixedLevelCoordinateResponseTrial = false;
    justWroteFreeResponseTrial = false;
    justWroteOpenSetAdaptiveTrial = false;
    justWroteCorrectKeywordsTrial = false;
}

auto OutputFileImpl::generateNewFilePath(const TestIdentity &test)
    -> std::string {
    return path->outputDirectory() + "/" + path->generateFileName(test) +
        ".txt";
}

void OutputFileImpl::close() { writer->close(); }

void OutputFileImpl::save() { writer->save(); }
}
