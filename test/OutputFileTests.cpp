#include "LogString.hpp"
#include "assert-utility.hpp"
#include <recognition-test/OutputFile.hpp>
#include <av-speech-in-noise/name.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>

namespace av_speech_in_noise {
namespace {
class WriterStub : public Writer {
  public:
    void save() override { saved_ = true; }

    void close() override { closed_ = true; }

    void open(std::string f) override { filePath_ = std::move(f); }

    void write(std::string s) override { written_.insert(s); }

    auto failed() -> bool override { return {}; }

    auto saved() const -> bool { return saved_; }

    auto filePath() const -> std::string { return filePath_; }

    auto closed() const -> bool { return closed_; }

    auto written() const -> const LogString & { return written_; }

  private:
    LogString written_;
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

struct HeadingLabel {
    gsl::index index;
    HeadingItem headingItem;
};

class WritingTrial : public virtual UseCase {
  public:
    virtual auto headingLabels() -> std::vector<HeadingLabel> = 0;
};

class WritingEvaluatedTrial : public virtual WritingTrial {
  public:
    virtual void incorrect() = 0;
    virtual void correct() = 0;
    virtual auto evaluationEntryIndex() -> int = 0;
};

void setCorrect(coordinate_response_measure::Trial &trial) {
    trial.correct = true;
}

void setIncorrect(coordinate_response_measure::Trial &trial) {
    trial.correct = false;
}

class WritingAdaptiveCoordinateResponseTrial : public WritingEvaluatedTrial {
  public:
    auto trial() -> auto & { return trial_; }

    void incorrect() override { setIncorrect(trial_); }

    void correct() override { setCorrect(trial_); }

    auto evaluationEntryIndex() -> int override { return 6; }

    void run(OutputFileImpl &file) override { file.write(trial_); }

    auto headingLabels() -> std::vector<HeadingLabel> override {
        return {{1, HeadingItem::snr_dB}, {2, HeadingItem::correctNumber},
            {3, HeadingItem::subjectNumber}, {4, HeadingItem::correctColor},
            {5, HeadingItem::subjectColor}, {6, HeadingItem::evaluation},
            {7, HeadingItem::reversals}};
    }

  private:
    coordinate_response_measure::AdaptiveTrial trial_{};
};

class WritingFixedLevelCoordinateResponseTrial : public WritingEvaluatedTrial {
  public:
    auto trial() -> auto & { return trial_; }

    void incorrect() override { setIncorrect(trial_); }

    void correct() override { setCorrect(trial_); }

    void run(OutputFileImpl &file) override { file.write(trial_); }

    auto evaluationEntryIndex() -> int override { return 5; }

    auto headingLabels() -> std::vector<HeadingLabel> override {
        return {{1, HeadingItem::correctNumber},
            {2, HeadingItem::subjectNumber}, {3, HeadingItem::correctColor},
            {4, HeadingItem::subjectColor}, {5, HeadingItem::evaluation},
            {6, HeadingItem::stimulus}};
    }

  private:
    coordinate_response_measure::FixedLevelTrial trial_{};
};

class WritingOpenSetAdaptiveTrial : public WritingEvaluatedTrial {
  public:
    void incorrect() override { trial_.correct = false; }

    void correct() override { trial_.correct = true; }

    void run(OutputFileImpl &file) override { file.write(trial_); }

    auto evaluationEntryIndex() -> int override { return 3; }

    auto headingLabels() -> std::vector<HeadingLabel> override {
        return {{1, HeadingItem::snr_dB}, {2, HeadingItem::target},
            {3, HeadingItem::evaluation}, {4, HeadingItem::reversals}};
    }

  private:
    open_set::AdaptiveTrial trial_{};
};

class WritingCorrectKeywordsTrial : public WritingEvaluatedTrial {
  public:
    void incorrect() override { trial_.correct = false; }

    void correct() override { trial_.correct = true; }

    void run(OutputFileImpl &file) override { file.write(trial_); }

    auto evaluationEntryIndex() -> int override { return 4; }

    auto headingLabels() -> std::vector<HeadingLabel> override {
        return {{1, HeadingItem::snr_dB}, {2, HeadingItem::target},
            {3, HeadingItem::correctKeywords}, {4, HeadingItem::evaluation},
            {5, HeadingItem::reversals}};
    }

  private:
    CorrectKeywordsTrial trial_{};
};

class WritingFreeResponseTrial : public WritingTrial {
  public:
    void run(OutputFileImpl &file) override { file.write(FreeResponseTrial{}); }

    auto headingLabels() -> std::vector<HeadingLabel> override {
        return {{1, HeadingItem::target}, {2, HeadingItem::freeResponse}};
    }
};

void run(UseCase &useCase, OutputFileImpl &file) { useCase.run(file); }

auto written(WriterStub &writer) -> const LogString & {
    return writer.written();
}

void openNewFile(OutputFileImpl &file, const TestIdentity &identity = {}) {
    file.openNewFile(identity);
}

void write(OutputFileImpl &file, const FreeResponseTrial &trial) {
    file.write(trial);
}

void writeFreeResponseTrial(OutputFileImpl &file) {
    write(file, FreeResponseTrial{});
}

void write(OutputFileImpl &file, const CorrectKeywordsTrial &trial) {
    file.write(trial);
}

void writeCorrectKeywordsTrial(OutputFileImpl &file) {
    write(file, CorrectKeywordsTrial{});
}

void write(OutputFileImpl &file, const open_set::AdaptiveTrial &trial) {
    file.write(trial);
}

void writeOpenSetAdaptiveTrial(OutputFileImpl &file) {
    write(file, open_set::AdaptiveTrial{});
}

void write(OutputFileImpl &file, const BinocularGazeSamples &gazeSamples) {
    file.write(gazeSamples);
}

void writeTargetStartTimeNanoseconds(OutputFileImpl &file, std::uintmax_t t) {
    file.write(TargetStartTime{t});
}

void assertEndsWith(WriterStub &writer, const std::string &s) {
    assertTrue(written(writer).endsWith(s));
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

auto nthCommaDelimitedEntryOfLine(WriterStub &writer, gsl::index n, int line)
    -> std::string {
    std::string written_ = written(writer);
    auto precedingNewLine{find_nth_element(written_, line - 1, '\n')};
    auto line_{written_.substr(precedingNewLine + 1)};
    auto precedingComma{find_nth_element(line_, n - 1, ',')};
    auto entryBeginning =
        (precedingComma == std::string::npos) ? 0U : precedingComma + 2;
    return upUntilFirstOfAny(line_.substr(entryBeginning), {',', '\n'});
}

void assertContainsColonDelimitedEntry(
    WriterStub &writer, const std::string &label, const std::string &what) {
    assertTrue(written(writer).contains(label + ": " + what + '\n'));
}

void assertNthCommaDelimitedEntryOfLine(
    WriterStub &writer, const std::string &what, int n, int line) {
    assertEqual(what, nthCommaDelimitedEntryOfLine(writer, n, line));
}

void assertNthCommaDelimitedEntryOfLine(
    WriterStub &writer, HeadingItem item, gsl::index n, int line) {
    assertEqual(name(item), nthCommaDelimitedEntryOfLine(writer, n, line));
}

void assertNthEntryOfSecondLine(
    WriterStub &writer, const std::string &what, int n) {
    assertNthCommaDelimitedEntryOfLine(writer, what, n, 2);
}

auto test(WritingTest &useCase) -> Test & { return useCase.test(); }

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
    WritingFreeResponseTrial writingFreeResponseTrial;
    WritingFixedLevelTest writingFixedLevelTest;
    WritingAdaptiveTest writingAdaptiveTest;
    FreeResponseTrial freeResponseTrial;
    BinocularGazeSamples eyeGazes;

    void assertConditionNameWritten(WritingTest &useCase, Condition c) {
        useCase.test().condition = c;
        run(useCase, file);
        assertContainsColonDelimitedEntry(writer, "condition", name(c));
    }

    void assertHeadingAtLine(WritingTrial &useCase, int n) {
        for (auto label : useCase.headingLabels())
            assertNthCommaDelimitedEntryOfLine(
                writer, label.headingItem, label.index, n);
    }

    void assertTestIdentityWritten(WritingTest &useCase) {
        testIdentity(useCase).subjectId = "a";
        testIdentity(useCase).testerId = "b";
        testIdentity(useCase).session = "c";
        testIdentity(useCase).method = "d";
        testIdentity(useCase).rmeSetting = "e";
        testIdentity(useCase).transducer = Transducer::twoSpeakers;
        run(useCase, file);
        assertContainsColonDelimitedEntry(writer, "subject", "a");
        assertContainsColonDelimitedEntry(writer, "tester", "b");
        assertContainsColonDelimitedEntry(writer, "session", "c");
        assertContainsColonDelimitedEntry(writer, "method", "d");
        assertContainsColonDelimitedEntry(writer, "RME setting", "e");
        assertContainsColonDelimitedEntry(
            writer, "transducer", name(Transducer::twoSpeakers));
    }

    void assertCommonTestWritten(WritingTest &useCase) {
        test(useCase).maskerFilePath = "a";
        test(useCase).targetListDirectory = "d";
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

    void assertWritesAdaptiveCoordinateResponseTrialOnLine(int n) {
        using coordinate_response_measure::Color;
        auto &trial = writingAdaptiveCoordinateResponseTrial.trial();
        trial.SNR_dB = 1;
        trial.correctNumber = 2;
        trial.subjectNumber = 3;
        trial.correctColor = Color::green;
        trial.subjectColor = Color::red;
        trial.reversals = 4;
        run(writingAdaptiveCoordinateResponseTrial, file);
        assertNthCommaDelimitedEntryOfLine(writer, "1", 1, n);
        assertNthCommaDelimitedEntryOfLine(writer, "2", 2, n);
        assertNthCommaDelimitedEntryOfLine(writer, "3", 3, n);
        assertNthCommaDelimitedEntryOfLine(writer, name(Color::green), 4, n);
        assertNthCommaDelimitedEntryOfLine(writer, name(Color::red), 5, n);
        assertNthCommaDelimitedEntryOfLine(writer, "4", 7, n);
    }

    void assertWritesFixedLevelCoordinateResponseTrialOnLine(int n) {
        using coordinate_response_measure::Color;
        auto &trial = writingFixedLevelCoordinateResponseTrial.trial();
        trial.correctNumber = 2;
        trial.subjectNumber = 3;
        trial.correctColor = Color::green;
        trial.subjectColor = Color::red;
        trial.target = "a";
        run(writingFixedLevelCoordinateResponseTrial, file);
        assertNthCommaDelimitedEntryOfLine(writer, "2", 1, n);
        assertNthCommaDelimitedEntryOfLine(writer, "3", 2, n);
        assertNthCommaDelimitedEntryOfLine(writer, name(Color::green), 3, n);
        assertNthCommaDelimitedEntryOfLine(writer, name(Color::red), 4, n);
        assertNthCommaDelimitedEntryOfLine(writer, "a", 6, n);
    }

    void assertWritesFreeResponseTrialOnLine(int n) {
        freeResponseTrial.target = "a";
        freeResponseTrial.response = "b";
        write(file, freeResponseTrial);
        assertNthCommaDelimitedEntryOfLine(writer, "a", 1, n);
        assertNthCommaDelimitedEntryOfLine(writer, "b", 2, n);
    }

    void assertWritesOpenSetAdaptiveTrialOnLine(int n) {
        open_set::AdaptiveTrial openSetAdaptiveTrial;
        openSetAdaptiveTrial.SNR_dB = 11;
        openSetAdaptiveTrial.target = "a";
        openSetAdaptiveTrial.reversals = 22;
        write(file, openSetAdaptiveTrial);
        assertNthCommaDelimitedEntryOfLine(writer, "11", 1, n);
        assertNthCommaDelimitedEntryOfLine(writer, "a", 2, n);
        assertNthCommaDelimitedEntryOfLine(writer, "22", 4, n);
    }

    void assertWritesCorrectKeywordsTrialOnLine(int n) {
        CorrectKeywordsTrial correctKeywordsTrial;
        correctKeywordsTrial.SNR_dB = 11;
        correctKeywordsTrial.target = "a";
        correctKeywordsTrial.count = 22;
        correctKeywordsTrial.reversals = 33;
        write(file, correctKeywordsTrial);
        assertNthCommaDelimitedEntryOfLine(writer, "11", 1, n);
        assertNthCommaDelimitedEntryOfLine(writer, "a", 2, n);
        assertNthCommaDelimitedEntryOfLine(writer, "22", 3, n);
        assertNthCommaDelimitedEntryOfLine(writer, "33", 5, n);
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
            BinocularGazeSample gazeSamples{t.at(n), left.at(n), right.at(n)};
            ++n;
            return gazeSamples;
        });
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

OUTPUT_FILE_TEST(writingCorrectKeywordsTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingCorrectKeywordsTrial);
}

OUTPUT_FILE_TEST(writingOpenSetAdaptiveTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingOpenSetAdaptiveTrial);
}

OUTPUT_FILE_TEST(writeAdaptiveCoordinateResponseTrial) {
    assertWritesAdaptiveCoordinateResponseTrialOnLine(2);
}

OUTPUT_FILE_TEST(writeFixedLevelCoordinateResponseTrial) {
    assertWritesFixedLevelCoordinateResponseTrialOnLine(2);
}

OUTPUT_FILE_TEST(writeFreeResponseTrial) {
    assertWritesFreeResponseTrialOnLine(2);
}

OUTPUT_FILE_TEST(writeOpenSetAdaptiveTrial) {
    assertWritesOpenSetAdaptiveTrialOnLine(2);
}

OUTPUT_FILE_TEST(writeCorrectKeywordsTrial) {
    assertWritesCorrectKeywordsTrialOnLine(2);
}

OUTPUT_FILE_TEST(
    writeAdaptiveCoordinateResponseTrialTwiceDoesNotWriteHeadingTwice) {
    run(writingAdaptiveCoordinateResponseTrial, file);
    assertWritesAdaptiveCoordinateResponseTrialOnLine(3);
}

OUTPUT_FILE_TEST(
    writeFixedLevelCoordinateResponseTrialTwiceDoesNotWriteHeadingTwice) {
    run(writingFixedLevelCoordinateResponseTrial, file);
    assertWritesFixedLevelCoordinateResponseTrialOnLine(3);
}

OUTPUT_FILE_TEST(writeFreeResponseTrialTwiceDoesNotWriteHeadingTwice) {
    writeFreeResponseTrial(file);
    assertWritesFreeResponseTrialOnLine(3);
}

OUTPUT_FILE_TEST(writeOpenSetAdaptiveTrialTwiceDoesNotWriteHeadingTwice) {
    writeOpenSetAdaptiveTrial(file);
    assertWritesOpenSetAdaptiveTrialOnLine(3);
}

OUTPUT_FILE_TEST(writeCorrectKeywordsTrialTwiceDoesNotWriteHeadingTwice) {
    writeCorrectKeywordsTrial(file);
    assertWritesCorrectKeywordsTrialOnLine(3);
}

OUTPUT_FILE_TEST(
    writingAdaptiveCoordinateResponseTrialTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    assertWritesHeadingTwiceWhenNewFileOpened(
        writingAdaptiveCoordinateResponseTrial);
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
    std::string written_ = written(writer);
    auto precedingNewLine{find_nth_element(written_, 2 - 1, '\n')};
    auto line_{written_.substr(precedingNewLine + 1)};
    assertEqual(
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
    results.push_back({"a", 1.});
    results.push_back({"b", 2.});
    results.push_back({"c", 3.});
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
    adaptiveTest.startingSnr_dB = 2;
    file.write(adaptiveTest);
    assertContainsColonDelimitedEntry(writer, "starting SNR (dB)", "2");
    assertEndsWith(writer, "\n\n");
}

OUTPUT_FILE_TEST(writeFixedLevelTest) {
    FixedLevelTest fixedLevelTest;
    fixedLevelTest.snr_dB = 2;
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
    setEyeGazes({1, 2, 3}, {{0.4, 0.44}, {0.5, 0.55}, {0.6, 0.66}},
        {{0.7, 0.77}, {0.8, 0.88}, {0.9, 0.99}});
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

OUTPUT_FILE_TEST(writeFadeInCompleteTime) {
    writeTargetStartTimeNanoseconds(file, 1);
    assertContainsColonDelimitedEntry(writer, "target start time (ns)", "1");
}

OUTPUT_FILE_TEST(writeEyeTrackerPlayerSynchronization) {
    EyeTrackerTargetPlayerSynchronization s;
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
    assertEqual("b/a.txt", writer.filePath());
}

OUTPUT_FILE_TEST(closeClosesWriter) {
    file.close();
    assertTrue(writer.closed());
}

OUTPUT_FILE_TEST(saveSavesWriter) {
    file.save();
    assertTrue(writer.saved());
}

OUTPUT_FILE_TEST(openPassesTestInformation) {
    TestIdentity testIdentity;
    openNewFile(file, testIdentity);
    assertEqual(&std::as_const(testIdentity), path.testIdentity());
}

class FailingWriter : public Writer {
    bool failed_{};

  public:
    void open(std::string) override { failed_ = true; }

    auto failed() -> bool override { return failed_; }

    void close() override {}
    void write(std::string) override {}
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
