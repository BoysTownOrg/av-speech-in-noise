#include "LogString.hpp"
#include "assert-utility.hpp"
#include <recognition-test/OutputFile.hpp>
#include <av-speech-in-noise/name.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>
#include <vector>
#include <map>

namespace av_speech_in_noise {
namespace {
class WriterStub : public Writer {
  public:
    void save() override { saved_ = true; }

    void close() override { closed_ = true; }

    void open(const std::string &f) override { filePath_ = f; }

    void write(const std::string &s) override { insert(written_, s); }

    auto failed() -> bool override { return {}; }

    auto saved() const -> bool { return saved_; }

    auto filePath() const -> std::string { return filePath_; }

    auto closed() const -> bool { return closed_; }

    auto written() const -> const std::stringstream & { return written_; }

  private:
    std::stringstream written_;
    std::string filePath_;
    bool closed_{};
    bool saved_{};
};

class OutputFilePathStub : public OutputFilePath {
  public:
    auto outputDirectory() -> std::string override { return outputDirectory_; }

    auto generateFileName(const TestIdentity &p) -> std::string override {
        testIdentity_ = &p;
        return fileName_;
    }

    auto homeDirectory() -> std::string override { return homeDirectory_; }

    void setFileName(std::string s) { fileName_ = std::move(s); }

    [[nodiscard]] auto testIdentity() const -> const TestIdentity * {
        return testIdentity_;
    }

    void setOutputDirectory(std::string s) { outputDirectory_ = std::move(s); }

  private:
    std::string fileName_;
    std::string homeDirectory_;
    std::string outputDirectory_;
    const TestIdentity *testIdentity_{};
};

class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run(OutputFileImpl &) = 0;
};

class WritingTest : public virtual UseCase {
  public:
    virtual auto test() -> Test & = 0;
};

class WritingAdaptiveTest : public WritingTest {
  public:
    void run(OutputFileImpl &file) override { file.write(test_); }

    auto test() -> Test & override { return test_; }

  private:
    AdaptiveTest test_{};
};

class WritingFixedLevelTest : public WritingTest {
  public:
    void run(OutputFileImpl &file) override { file.write(test_); }

    auto test() -> Test & override { return test_; }

  private:
    FixedLevelTest test_{};
};

class WritingTrial : public virtual UseCase {
  public:
    virtual auto headingLabels() -> std::map<HeadingItem, gsl::index> = 0;
    virtual void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &, gsl::index line) = 0;
};

class WritingEvaluatedTrial : public virtual WritingTrial {
  public:
    virtual void incorrect() = 0;
    virtual void correct() = 0;
    virtual auto evaluationEntryIndex() -> gsl::index = 0;
};

void setCorrect(coordinate_response_measure::Trial &trial) {
    trial.correct = true;
}

void setIncorrect(coordinate_response_measure::Trial &trial) {
    trial.correct = false;
}

void run(UseCase &useCase, OutputFileImpl &file) { useCase.run(file); }

auto written(WriterStub &writer) -> const std::stringstream & {
    return writer.written();
}

auto writtenString(WriterStub &writer) -> std::string {
    return string(written(writer));
}

void openNewFile(OutputFileImpl &file, const TestIdentity &identity = {}) {
    file.openNewFile(identity);
}

void write(OutputFileImpl &file, const FreeResponseTrial &trial) {
    file.write(trial);
}

void write(OutputFileImpl &file, const BinocularGazeSamples &gazeSamples) {
    file.write(gazeSamples);
}

void writeTargetStartTimeNanoseconds(OutputFileImpl &file, std::uintmax_t t) {
    file.write(TargetStartTime{t});
}

void assertEndsWith(WriterStub &writer, const std::string &s) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(endsWith(written(writer), s));
}

auto find_nth_element(const std::string &content, gsl::index n, char what)
    -> std::string::size_type {
    auto found{std::string::npos};
    for (int i = 0; i < n; ++i)
        found = content.find(what, found + 1U);
    return found;
}

auto upUntilFirstOfAny(const std::string &content, std::vector<char> v)
    -> std::string {
    return content.substr(0, content.find_first_of({v.begin(), v.end()}));
}

auto testIdentity(WritingTest &useCase) -> TestIdentity & {
    return useCase.test().identity;
}

auto nthCommaDelimitedEntryOfLine(
    WriterStub &writer, gsl::index n, gsl::index line) -> std::string {
    const auto precedingNewLine{
        find_nth_element(writtenString(writer), line - 1, '\n')};
    const auto line_{writtenString(writer).substr(precedingNewLine + 1)};
    const auto precedingComma{find_nth_element(line_, n - 1, ',')};
    const auto entryBeginning =
        (precedingComma == std::string::npos) ? 0U : precedingComma + 2;
    return upUntilFirstOfAny(line_.substr(entryBeginning), {',', '\n'});
}

void assertContainsColonDelimitedEntry(
    WriterStub &writer, const std::string &label, const std::string &what) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        contains(written(writer), label + ": " + what + '\n'));
}

void assertNthCommaDelimitedEntryOfLine(WriterStub &writer,
    const std::string &what, gsl::index n, gsl::index line) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        what, nthCommaDelimitedEntryOfLine(writer, n, line));
}

void assertNthCommaDelimitedEntryOfLine(WriterStub &writer,
    coordinate_response_measure::Color what, gsl::index n, gsl::index line) {
    assertNthCommaDelimitedEntryOfLine(writer, name(what), n, line);
}

void assertNthCommaDelimitedEntryOfLine(
    WriterStub &writer, HeadingItem item, gsl::index n, gsl::index line) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        name(item), nthCommaDelimitedEntryOfLine(writer, n, line));
}

void assertNthEntryOfSecondLine(
    WriterStub &writer, const std::string &what, gsl::index n) {
    assertNthCommaDelimitedEntryOfLine(writer, what, n, 2);
}

auto test(WritingTest &useCase) -> Test & { return useCase.test(); }

auto at(const std::map<HeadingItem, gsl::index> &m, HeadingItem item)
    -> gsl::index {
    return m.at(item);
}

class WritingAdaptiveCoordinateResponseTrial : public WritingEvaluatedTrial {
  public:
    WritingAdaptiveCoordinateResponseTrial() {
        trial.snr.dB = 1;
        trial.correctNumber = 2;
        trial.subjectNumber = 3;
        trial.correctColor = coordinate_response_measure::Color::green;
        trial.subjectColor = coordinate_response_measure::Color::red;
        trial.reversals = 4;
    }

    void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &writer, gsl::index line) override {
        assertNthCommaDelimitedEntryOfLine(
            writer, "1", at(headingLabels_, HeadingItem::snr_dB), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "2", at(headingLabels_, HeadingItem::correctNumber), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "3", at(headingLabels_, HeadingItem::subjectNumber), line);
        assertNthCommaDelimitedEntryOfLine(writer,
            coordinate_response_measure::Color::green,
            at(headingLabels_, HeadingItem::correctColor), line);
        assertNthCommaDelimitedEntryOfLine(writer,
            coordinate_response_measure::Color::red,
            at(headingLabels_, HeadingItem::subjectColor), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "4", at(headingLabels_, HeadingItem::reversals), line);
    }

    void incorrect() override { setIncorrect(trial); }

    void correct() override { setCorrect(trial); }

    auto evaluationEntryIndex() -> gsl::index override {
        return at(headingLabels_, HeadingItem::evaluation);
    }

    void run(OutputFileImpl &file) override { file.write(trial); }

    auto headingLabels() -> std::map<HeadingItem, gsl::index> override {
        return headingLabels_;
    }

  private:
    coordinate_response_measure::AdaptiveTrial trial{};
    std::map<HeadingItem, gsl::index> headingLabels_{{HeadingItem::snr_dB, 1},
        {HeadingItem::correctNumber, 2}, {HeadingItem::subjectNumber, 3},
        {HeadingItem::correctColor, 4}, {HeadingItem::subjectColor, 5},
        {HeadingItem::evaluation, 6}, {HeadingItem::reversals, 7}};
};

class WritingFixedLevelCoordinateResponseTrial : public WritingEvaluatedTrial {
  public:
    WritingFixedLevelCoordinateResponseTrial() {
        trial.correctNumber = 2;
        trial.subjectNumber = 3;
        trial.correctColor = coordinate_response_measure::Color::green;
        trial.subjectColor = coordinate_response_measure::Color::red;
        trial.target = "a";
    }

    void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &writer, gsl::index line) override {
        assertNthCommaDelimitedEntryOfLine(
            writer, "2", at(headingLabels_, HeadingItem::correctNumber), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "3", at(headingLabels_, HeadingItem::subjectNumber), line);
        assertNthCommaDelimitedEntryOfLine(writer,
            coordinate_response_measure::Color::green,
            at(headingLabels_, HeadingItem::correctColor), line);
        assertNthCommaDelimitedEntryOfLine(writer,
            coordinate_response_measure::Color::red,
            at(headingLabels_, HeadingItem::subjectColor), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "a", at(headingLabels_, HeadingItem::target), line);
    }

    void incorrect() override { setIncorrect(trial); }

    void correct() override { setCorrect(trial); }

    void run(OutputFileImpl &file) override { file.write(trial); }

    auto evaluationEntryIndex() -> gsl::index override {
        return at(headingLabels_, HeadingItem::evaluation);
    }

    auto headingLabels() -> std::map<HeadingItem, gsl::index> override {
        return headingLabels_;
    }

  private:
    coordinate_response_measure::FixedLevelTrial trial{};
    std::map<HeadingItem, gsl::index> headingLabels_{
        {HeadingItem::correctNumber, 1}, {HeadingItem::subjectNumber, 2},
        {HeadingItem::correctColor, 3}, {HeadingItem::subjectColor, 4},
        {HeadingItem::evaluation, 5}, {HeadingItem::target, 6}};
};

class WritingOpenSetAdaptiveTrial : public WritingEvaluatedTrial {
  public:
    WritingOpenSetAdaptiveTrial() {
        trial.snr.dB = 11;
        trial.target = "a";
        trial.reversals = 22;
    }

    void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &writer, gsl::index line) override {
        assertNthCommaDelimitedEntryOfLine(
            writer, "11", at(headingLabels_, HeadingItem::snr_dB), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "a", at(headingLabels_, HeadingItem::target), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "22", at(headingLabels_, HeadingItem::reversals), line);
    }

    void incorrect() override { trial.correct = false; }

    void correct() override { trial.correct = true; }

    void run(OutputFileImpl &file) override { file.write(trial); }

    auto evaluationEntryIndex() -> gsl::index override {
        return at(headingLabels_, HeadingItem::evaluation);
    }

    auto headingLabels() -> std::map<HeadingItem, gsl::index> override {
        return headingLabels_;
    }

  private:
    open_set::AdaptiveTrial trial{};
    std::map<HeadingItem, gsl::index> headingLabels_{{HeadingItem::snr_dB, 1},
        {HeadingItem::target, 2}, {HeadingItem::evaluation, 3},
        {HeadingItem::reversals, 4}};
};

class WritingCorrectKeywordsTrial : public WritingEvaluatedTrial {
  public:
    WritingCorrectKeywordsTrial() {
        trial.snr.dB = 11;
        trial.target = "a";
        trial.count = 22;
        trial.reversals = 33;
    }

    void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &writer, gsl::index line) override {
        assertNthCommaDelimitedEntryOfLine(
            writer, "11", at(headingLabels_, HeadingItem::snr_dB), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "a", at(headingLabels_, HeadingItem::target), line);
        assertNthCommaDelimitedEntryOfLine(writer, "22",
            at(headingLabels_, HeadingItem::correctKeywords), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "33", at(headingLabels_, HeadingItem::reversals), line);
    }

    void incorrect() override { trial.correct = false; }

    void correct() override { trial.correct = true; }

    void run(OutputFileImpl &file) override { file.write(trial); }

    auto evaluationEntryIndex() -> gsl::index override {
        return at(headingLabels_, HeadingItem::evaluation);
    }

    auto headingLabels() -> std::map<HeadingItem, gsl::index> override {
        return headingLabels_;
    }

  private:
    CorrectKeywordsTrial trial{};
    std::map<HeadingItem, gsl::index> headingLabels_{{HeadingItem::snr_dB, 1},
        {HeadingItem::target, 2}, {HeadingItem::correctKeywords, 3},
        {HeadingItem::evaluation, 4}, {HeadingItem::reversals, 5}};
};

class WritingConsonantTrial : public WritingEvaluatedTrial {
  public:
    WritingConsonantTrial() {
        trial.correctConsonant = 'a';
        trial.subjectConsonant = 'b';
        trial.target = "c";
    }

    void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &writer, gsl::index line) override {
        assertNthCommaDelimitedEntryOfLine(writer, "a",
            at(headingLabels_, HeadingItem::correctConsonant), line);
        assertNthCommaDelimitedEntryOfLine(writer, "b",
            at(headingLabels_, HeadingItem::subjectConsonant), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "c", at(headingLabels_, HeadingItem::target), line);
    }

    void incorrect() override { trial.correct = false; }

    void correct() override { trial.correct = true; }

    void run(OutputFileImpl &file) override { file.write(trial); }

    auto evaluationEntryIndex() -> gsl::index override {
        return at(headingLabels_, HeadingItem::evaluation);
    }

    auto headingLabels() -> std::map<HeadingItem, gsl::index> override {
        return headingLabels_;
    }

  private:
    ConsonantTrial trial{};
    std::map<HeadingItem, gsl::index> headingLabels_{
        {HeadingItem::correctConsonant, 1}, {HeadingItem::subjectConsonant, 2},
        {HeadingItem::evaluation, 3}, {HeadingItem::target, 4}};
};

class WritingFreeResponseTrial : public WritingTrial {
  public:
    WritingFreeResponseTrial() {
        trial.target = "a";
        trial.response = "b";
    }

    void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &writer, gsl::index line) override {
        assertNthCommaDelimitedEntryOfLine(
            writer, "a", at(headingLabels_, HeadingItem::target), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "b", at(headingLabels_, HeadingItem::freeResponse), line);
    }

    void run(OutputFileImpl &file) override { file.write(trial); }

    auto headingLabels() -> std::map<HeadingItem, gsl::index> override {
        return headingLabels_;
    }

  private:
    FreeResponseTrial trial{};
    std::map<HeadingItem, gsl::index> headingLabels_{
        {HeadingItem::target, 1}, {HeadingItem::freeResponse, 2}};
};

class WritingThreeKeywordsTrial : public WritingTrial {
  public:
    WritingThreeKeywordsTrial() {
        trial.target = "a";
        trial.firstCorrect = true;
        trial.secondCorrect = false;
        trial.thirdCorrect = true;
    }

    void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &writer, gsl::index line) override {
        assertNthCommaDelimitedEntryOfLine(
            writer, "a", at(headingLabels_, HeadingItem::target), line);
        assertNthCommaDelimitedEntryOfLine(writer, "correct",
            at(headingLabels_, HeadingItem::firstKeywordEvaluation), line);
        assertNthCommaDelimitedEntryOfLine(writer, "incorrect",
            at(headingLabels_, HeadingItem::secondKeywordEvaluation), line);
        assertNthCommaDelimitedEntryOfLine(writer, "correct",
            at(headingLabels_, HeadingItem::thirdKeywordEvaluation), line);
    }

    void run(OutputFileImpl &file) override { file.write(trial); }

    auto headingLabels() -> std::map<HeadingItem, gsl::index> override {
        return headingLabels_;
    }

  private:
    ThreeKeywordsTrial trial{};
    std::map<HeadingItem, gsl::index> headingLabels_{{HeadingItem::target, 1},
        {HeadingItem::firstKeywordEvaluation, 2},
        {HeadingItem::secondKeywordEvaluation, 3},
        {HeadingItem::thirdKeywordEvaluation, 4}};
};

class OutputFileTests : public ::testing::Test {
  protected:
    WriterStub writer;
    OutputFilePathStub path;
    OutputFileImpl file{writer, path};
    WritingAdaptiveCoordinateResponseTrial
        writingAdaptiveCoordinateResponseTrial;
    WritingFixedLevelCoordinateResponseTrial
        writingFixedLevelCoordinateResponseTrial;
    WritingOpenSetAdaptiveTrial writingOpenSetAdaptiveTrial;
    WritingCorrectKeywordsTrial writingCorrectKeywordsTrial;
    WritingConsonantTrial writingConsonantTrial;
    WritingFreeResponseTrial writingFreeResponseTrial;
    WritingThreeKeywordsTrial writingThreeKeywordsTrial;
    WritingFixedLevelTest writingFixedLevelTest;
    WritingAdaptiveTest writingAdaptiveTest;
    FreeResponseTrial freeResponseTrial;
    BinocularGazeSamples eyeGazes;

    void assertConditionNameWritten(WritingTest &useCase, Condition c) {
        useCase.test().condition = c;
        run(useCase, file);
        assertContainsColonDelimitedEntry(writer, "condition", name(c));
    }

    void assertHeadingAtLine(WritingTrial &useCase, gsl::index line) {
        for (auto [headingItem, index] : useCase.headingLabels())
            assertNthCommaDelimitedEntryOfLine(
                writer, headingItem, index, line);
    }

    void assertTestIdentityWritten(WritingTest &useCase) {
        testIdentity(useCase).subjectId = "a";
        testIdentity(useCase).testerId = "b";
        testIdentity(useCase).session = "c";
        testIdentity(useCase).method = "d";
        testIdentity(useCase).rmeSetting = "e";
        testIdentity(useCase).transducer = "f";
        run(useCase, file);
        assertContainsColonDelimitedEntry(writer, "subject", "a");
        assertContainsColonDelimitedEntry(writer, "tester", "b");
        assertContainsColonDelimitedEntry(writer, "session", "c");
        assertContainsColonDelimitedEntry(writer, "method", "d");
        assertContainsColonDelimitedEntry(writer, "RME setting", "e");
        assertContainsColonDelimitedEntry(writer, "transducer", "f");
    }

    void assertCommonTestWritten(WritingTest &useCase) {
        test(useCase).maskerFileUrl.path = "a";
        test(useCase).targetsUrl.path = "d";
        test(useCase).maskerLevel.dB_SPL = 1;
        run(useCase, file);
        assertContainsColonDelimitedEntry(writer, "masker", "a");
        assertContainsColonDelimitedEntry(writer, "targets", "d");
        assertContainsColonDelimitedEntry(writer, "masker level (dB SPL)", "1");
        assertEndsWith(writer, "\n\n");
    }

    void assertIncorrectTrialWritesEvaluation(WritingEvaluatedTrial &useCase) {
        useCase.incorrect();
        run(useCase, file);
        assertNthEntryOfSecondLine(
            writer, "incorrect", useCase.evaluationEntryIndex());
    }

    void assertCorrectTrialWritesEvaluation(WritingEvaluatedTrial &useCase) {
        useCase.correct();
        run(useCase, file);
        assertNthEntryOfSecondLine(
            writer, "correct", useCase.evaluationEntryIndex());
    }

    void assertWritesTrialOnLine(WritingTrial &useCase, gsl::index line) {
        run(useCase, file);
        useCase.assertContainsCommaDelimitedTrialOnLine(writer, line);
    }

    void assertWritesHeadingOnFirstLine(WritingTrial &useCase) {
        run(useCase, file);
        assertHeadingAtLine(useCase, 1);
    }

    void assertWritesHeadingTwiceWhenNewFileOpened(WritingTrial &useCase) {
        run(useCase, file);
        openNewFile(file);
        run(useCase, file);
        assertHeadingAtLine(useCase, 3);
    }

    void setEyeGazes(std::vector<std::int_least64_t> t,
        std::vector<EyeGaze> left, std::vector<EyeGaze> right) {
        eyeGazes.resize(t.size());
        std::generate(eyeGazes.begin(), eyeGazes.end(), [&, n = 0]() mutable {
            BinocularGazeSample gazeSamples{{t.at(n)}, left.at(n), right.at(n)};
            ++n;
            return gazeSamples;
        });
    }

    void assertWritesTrialOnLineAfterWritingTwice(
        WritingTrial &useCase, gsl::index line) {
        run(useCase, file);
        assertWritesTrialOnLine(useCase, line);
    }
};

#define OUTPUT_FILE_TEST(a) TEST_F(OutputFileTests, a)

OUTPUT_FILE_TEST(
    writingAdaptiveCoordinateResponseTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingAdaptiveCoordinateResponseTrial);
}

OUTPUT_FILE_TEST(
    writingFixedLevelCoordinateResponseTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingFixedLevelCoordinateResponseTrial);
}

OUTPUT_FILE_TEST(writingFreeResponseTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingFreeResponseTrial);
}

OUTPUT_FILE_TEST(writingThreeKeywordsTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingThreeKeywordsTrial);
}

OUTPUT_FILE_TEST(writingCorrectKeywordsTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingCorrectKeywordsTrial);
}

OUTPUT_FILE_TEST(writingConsonantTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingConsonantTrial);
}

OUTPUT_FILE_TEST(writingOpenSetAdaptiveTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingOpenSetAdaptiveTrial);
}

OUTPUT_FILE_TEST(writeAdaptiveCoordinateResponseTrialWritesTrialOnSecondLine) {
    assertWritesTrialOnLine(writingAdaptiveCoordinateResponseTrial, 2);
}

OUTPUT_FILE_TEST(
    writeFixedLevelCoordinateResponseTrialWritesTrialOnSecondLine) {
    assertWritesTrialOnLine(writingFixedLevelCoordinateResponseTrial, 2);
}

OUTPUT_FILE_TEST(writingConsonantTrialWritesTrialOnSecondLine) {
    assertWritesTrialOnLine(writingConsonantTrial, 2);
}

OUTPUT_FILE_TEST(writeFreeResponseTrialWritesTrialOnSecondLine) {
    assertWritesTrialOnLine(writingFreeResponseTrial, 2);
}

OUTPUT_FILE_TEST(writeThreeKeywordsTrialWritesTrialOnSecondLine) {
    assertWritesTrialOnLine(writingThreeKeywordsTrial, 2);
}

OUTPUT_FILE_TEST(writeOpenSetAdaptiveTrialWritesTrialOnSecondLine) {
    assertWritesTrialOnLine(writingOpenSetAdaptiveTrial, 2);
}

OUTPUT_FILE_TEST(writeCorrectKeywordsTrialWritesTrialOnSecondLine) {
    assertWritesTrialOnLine(writingCorrectKeywordsTrial, 2);
}

OUTPUT_FILE_TEST(
    writeAdaptiveCoordinateResponseTrialTwiceDoesNotWriteHeadingTwice) {
    assertWritesTrialOnLineAfterWritingTwice(
        writingAdaptiveCoordinateResponseTrial, 3);
}

OUTPUT_FILE_TEST(
    writeFixedLevelCoordinateResponseTrialTwiceDoesNotWriteHeadingTwice) {
    assertWritesTrialOnLineAfterWritingTwice(
        writingFixedLevelCoordinateResponseTrial, 3);
}

OUTPUT_FILE_TEST(writingConsonantTrialTwiceDoesNotWriteHeadingTwice) {
    assertWritesTrialOnLineAfterWritingTwice(writingConsonantTrial, 3);
}

OUTPUT_FILE_TEST(writeFreeResponseTrialTwiceDoesNotWriteHeadingTwice) {
    assertWritesTrialOnLineAfterWritingTwice(writingFreeResponseTrial, 3);
}

OUTPUT_FILE_TEST(writeOpenSetAdaptiveTrialTwiceDoesNotWriteHeadingTwice) {
    assertWritesTrialOnLineAfterWritingTwice(writingOpenSetAdaptiveTrial, 3);
}

OUTPUT_FILE_TEST(writeCorrectKeywordsTrialTwiceDoesNotWriteHeadingTwice) {
    assertWritesTrialOnLineAfterWritingTwice(writingCorrectKeywordsTrial, 3);
}

OUTPUT_FILE_TEST(
    writingAdaptiveCoordinateResponseTrialTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    assertWritesHeadingTwiceWhenNewFileOpened(
        writingAdaptiveCoordinateResponseTrial);
}

OUTPUT_FILE_TEST(
    writingConsonantTrialTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    assertWritesHeadingTwiceWhenNewFileOpened(writingConsonantTrial);
}

OUTPUT_FILE_TEST(
    writingFixedLevelCoordinateResponseTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    assertWritesHeadingTwiceWhenNewFileOpened(
        writingFixedLevelCoordinateResponseTrial);
}

OUTPUT_FILE_TEST(
    writingFreeResponseTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    assertWritesHeadingTwiceWhenNewFileOpened(writingFreeResponseTrial);
}

OUTPUT_FILE_TEST(
    writingOpenSetAdaptiveTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    assertWritesHeadingTwiceWhenNewFileOpened(writingOpenSetAdaptiveTrial);
}

OUTPUT_FILE_TEST(
    writingCorrectKeywordsTrialTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    assertWritesHeadingTwiceWhenNewFileOpened(writingCorrectKeywordsTrial);
}

OUTPUT_FILE_TEST(writeIncorrectAdaptiveCoordinateResponseTrial) {
    assertIncorrectTrialWritesEvaluation(
        writingAdaptiveCoordinateResponseTrial);
}

OUTPUT_FILE_TEST(writingIncorrectConsonantTrial) {
    assertIncorrectTrialWritesEvaluation(writingConsonantTrial);
}

OUTPUT_FILE_TEST(writeIncorrectFixedLevelCoordinateResponseTrial) {
    assertIncorrectTrialWritesEvaluation(
        writingFixedLevelCoordinateResponseTrial);
}

OUTPUT_FILE_TEST(writeIncorrectOpenSetAdaptiveTrial) {
    assertIncorrectTrialWritesEvaluation(writingOpenSetAdaptiveTrial);
}

OUTPUT_FILE_TEST(writeIncorrectKeywordsTrial) {
    assertIncorrectTrialWritesEvaluation(writingCorrectKeywordsTrial);
}

OUTPUT_FILE_TEST(writeCorrectAdaptiveCoordinateResponseTrial) {
    assertCorrectTrialWritesEvaluation(writingAdaptiveCoordinateResponseTrial);
}

OUTPUT_FILE_TEST(writingCorrectConsonantTrial) {
    assertCorrectTrialWritesEvaluation(writingConsonantTrial);
}

OUTPUT_FILE_TEST(writeCorrectFixedLevelCoordinateResponseTrial) {
    assertCorrectTrialWritesEvaluation(
        writingFixedLevelCoordinateResponseTrial);
}

OUTPUT_FILE_TEST(writeCorrectOpenSetAdaptiveTrial) {
    assertCorrectTrialWritesEvaluation(writingOpenSetAdaptiveTrial);
}

OUTPUT_FILE_TEST(writeCorrectKeywordsTrialWritesCorrectEvaluation) {
    assertCorrectTrialWritesEvaluation(writingCorrectKeywordsTrial);
}

OUTPUT_FILE_TEST(writeFlaggedFreeResponseTrial) {
    freeResponseTrial.flagged = true;
    write(file, freeResponseTrial);
    assertNthEntryOfSecondLine(writer, "FLAGGED", 3);
}

OUTPUT_FILE_TEST(writeNoFlagFreeResponseTrialOnlyTwoEntries) {
    freeResponseTrial.flagged = false;
    write(file, freeResponseTrial);
    const auto precedingNewLine{
        find_nth_element(writtenString(writer), 2 - 1, '\n')};
    const auto line_{writtenString(writer).substr(precedingNewLine + 1)};
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::iterator_traits<std::string::iterator>::difference_type{2 - 1},
        std::count(line_.begin(), line_.end(), ','));
}

OUTPUT_FILE_TEST(uninitializedColorDoesNotBreak) {
    coordinate_response_measure::AdaptiveTrial uninitialized;
    file.write(uninitialized);
}

OUTPUT_FILE_TEST(writeCommonAdaptiveTest) {
    assertCommonTestWritten(writingAdaptiveTest);
}

OUTPUT_FILE_TEST(writesTrackSettings) {
    AdaptiveTest test;
    TrackingSequence first;
    first.up = 1;
    first.down = 2;
    first.runCount = 3;
    first.stepSize = 4;
    TrackingSequence second;
    second.up = 5;
    second.down = 6;
    second.runCount = 7;
    second.stepSize = 8;
    test.trackingRule.push_back(first);
    test.trackingRule.push_back(second);
    test.thresholdReversals = 9;
    file.write(test);
    assertContainsColonDelimitedEntry(writer, "up", "1 5");
    assertContainsColonDelimitedEntry(writer, "down", "2 6");
    assertContainsColonDelimitedEntry(writer, "reversals per step size", "3 7");
    assertContainsColonDelimitedEntry(writer, "step sizes (dB)", "4 8");
    assertContainsColonDelimitedEntry(writer, "threshold reversals", "9");
    assertEndsWith(writer, "\n\n");
}

OUTPUT_FILE_TEST(writeAdaptiveTestResult) {
    AdaptiveTestResults results{};
    results.push_back({{"a"}, 1.});
    results.push_back({{"b"}, 2.});
    results.push_back({{"c"}, 3.});
    file.write(results);
    assertContainsColonDelimitedEntry(writer, "threshold for a", "1");
    assertContainsColonDelimitedEntry(writer, "threshold for b", "2");
    assertContainsColonDelimitedEntry(writer, "threshold for c", "3");
}

OUTPUT_FILE_TEST(writeCommonFixedLevelTest) {
    assertCommonTestWritten(writingFixedLevelTest);
}

OUTPUT_FILE_TEST(writeAdaptiveTest) {
    AdaptiveTest adaptiveTest;
    adaptiveTest.startingSnr.dB = 2;
    file.write(adaptiveTest);
    assertContainsColonDelimitedEntry(writer, "starting SNR (dB)", "2");
    assertEndsWith(writer, "\n\n");
}

OUTPUT_FILE_TEST(writeFixedLevelTest) {
    FixedLevelTest fixedLevelTest;
    fixedLevelTest.snr.dB = 2;
    file.write(fixedLevelTest);
    assertContainsColonDelimitedEntry(writer, "SNR (dB)", "2");
    assertEndsWith(writer, "\n\n");
}

OUTPUT_FILE_TEST(writeAdaptiveTestInformation) {
    assertTestIdentityWritten(writingAdaptiveTest);
}

OUTPUT_FILE_TEST(writeFixedLevelTestInformation) {
    assertTestIdentityWritten(writingFixedLevelTest);
}

OUTPUT_FILE_TEST(writeAdaptiveTestWithAvCondition) {
    assertConditionNameWritten(writingAdaptiveTest, Condition::audioVisual);
}

OUTPUT_FILE_TEST(writeAdaptiveTestWithAuditoryOnlyCondition) {
    assertConditionNameWritten(writingAdaptiveTest, Condition::auditoryOnly);
}

OUTPUT_FILE_TEST(writeFixedLevelTestWithAvCondition) {
    assertConditionNameWritten(writingFixedLevelTest, Condition::audioVisual);
}

OUTPUT_FILE_TEST(writeFixedLevelTestWithAuditoryOnlyCondition) {
    assertConditionNameWritten(writingFixedLevelTest, Condition::auditoryOnly);
}

OUTPUT_FILE_TEST(writeEyeGazes) {
    setEyeGazes({1, 2, 3}, {{0.4F, 0.44F}, {0.5F, 0.55F}, {0.6F, 0.66F}},
        {{0.7F, 0.77F}, {0.8F, 0.88F}, {0.9F, 0.99F}});
    write(file, eyeGazes);
    assertNthCommaDelimitedEntryOfLine(
        writer, HeadingItem::eyeTrackerTime, 1, 1);
    assertNthCommaDelimitedEntryOfLine(writer, HeadingItem::leftGaze, 2, 1);
    assertNthCommaDelimitedEntryOfLine(writer, HeadingItem::rightGaze, 3, 1);
    assertNthCommaDelimitedEntryOfLine(writer, "1", 1, 2);
    assertNthCommaDelimitedEntryOfLine(writer, "0.4 0.44", 2, 2);
    assertNthCommaDelimitedEntryOfLine(writer, "0.7 0.77", 3, 2);
    assertNthCommaDelimitedEntryOfLine(writer, "2", 1, 3);
    assertNthCommaDelimitedEntryOfLine(writer, "0.5 0.55", 2, 3);
    assertNthCommaDelimitedEntryOfLine(writer, "0.8 0.88", 3, 3);
    assertNthCommaDelimitedEntryOfLine(writer, "3", 1, 4);
    assertNthCommaDelimitedEntryOfLine(writer, "0.6 0.66", 2, 4);
    assertNthCommaDelimitedEntryOfLine(writer, "0.9 0.99", 3, 4);
}

OUTPUT_FILE_TEST(writeTargetStartTime) {
    writeTargetStartTimeNanoseconds(file, 1);
    assertContainsColonDelimitedEntry(writer, "target start time (ns)", "1");
}

OUTPUT_FILE_TEST(writeEyeTrackerTargetPlayerSynchronization) {
    EyeTrackerTargetPlayerSynchronization s{};
    s.eyeTrackerSystemTime.microseconds = 1;
    s.targetPlayerSystemTime.nanoseconds = 2;
    file.write(s);
    assertNthCommaDelimitedEntryOfLine(
        writer, HeadingItem::eyeTrackerTime, 1, 1);
    assertNthCommaDelimitedEntryOfLine(
        writer, HeadingItem::targetPlayerTime, 2, 1);
    assertNthCommaDelimitedEntryOfLine(writer, "1", 1, 2);
    assertNthCommaDelimitedEntryOfLine(writer, "2", 2, 2);
}

OUTPUT_FILE_TEST(openPassesFormattedFilePath) {
    path.setFileName("a");
    path.setOutputDirectory("b");
    openNewFile(file);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"b/a.txt"}, writer.filePath());
}

OUTPUT_FILE_TEST(closeClosesWriter) {
    file.close();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(writer.closed());
}

OUTPUT_FILE_TEST(saveSavesWriter) {
    file.save();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(writer.saved());
}

OUTPUT_FILE_TEST(openPassesTestInformation) {
    TestIdentity testIdentity;
    openNewFile(file, testIdentity);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &std::as_const(testIdentity), path.testIdentity());
}

class FailingWriter : public Writer {
    bool failed_{};

  public:
    void open(const std::string &) override { failed_ = true; }

    auto failed() -> bool override { return failed_; }

    void close() override {}
    void write(const std::string &) override {}
    void save() override {}
};

TEST(FailingOutputFileTests, openThrowsOpenFailureWhenWriterFails) {
    FailingWriter writer;
    OutputFilePathStub path;
    OutputFileImpl file{writer, path};
    try {
        openNewFile(file);
        FAIL() << "Expected OutputFileImpl::OpenFailure";
    } catch (const OutputFileImpl::OpenFailure &) {
    }
}
}
}
