#include "LogString.hpp"
#include "assert-utility.h"
#include <recognition-test/OutputFile.hpp>
#include <gtest/gtest.h>

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

class WritingTestUseCase : public virtual UseCase {
  public:
    virtual auto test() -> Test & = 0;
};

class WritingAdaptiveTest : public WritingTestUseCase {
  public:
    AdaptiveTest test_{};

    void run(OutputFileImpl &file) override { file.write(test_); }

    auto test() -> Test & override { return test_; }
};

class WritingFixedLevelTest : public WritingTestUseCase {
    FixedLevelTest test_{};

  public:
    void run(OutputFileImpl &file) override { file.write(test_); }

    auto test() -> Test & override { return test_; }
};

class WritingTrialUseCase : public virtual UseCase {
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

class WritingAdaptiveCoordinateResponseTrial : public WritingTrialUseCase {
    coordinate_response_measure::AdaptiveTrial trial_{};

  public:
    auto trial() -> auto & { return trial_; }

    void incorrect() override { setIncorrect(trial_); }

    void correct() override { setCorrect(trial_); }

    auto evaluationEntryIndex() -> int override { return 6; }

    void run(OutputFileImpl &file) override { file.write(trial_); }
};

class WritingFixedLevelCoordinateResponseTrial : public WritingTrialUseCase {
    coordinate_response_measure::FixedLevelTrial trial_{};

  public:
    auto trial() -> auto & { return trial_; }

    void incorrect() override { setIncorrect(trial_); }

    void correct() override { setCorrect(trial_); }

    void run(OutputFileImpl &file) override { file.write(trial_); }

    auto evaluationEntryIndex() -> int override { return 5; }
};

class WritingOpenSetAdaptiveTrial : public WritingTrialUseCase {
    open_set::AdaptiveTrial trial_{};

  public:
    void incorrect() override { trial_.correct = false; }

    void correct() override { trial_.correct = true; }

    void run(OutputFileImpl &file) override { file.write(trial_); }

    auto evaluationEntryIndex() -> int override { return 3; }
};

class WritingCorrectKeywordsTrial : public WritingTrialUseCase {
    CorrectKeywordsTrial trial_{};

  public:
    void incorrect() override { trial_.correct = false; }

    void correct() override { trial_.correct = true; }

    void run(OutputFileImpl &file) override { file.write(trial_); }

    auto evaluationEntryIndex() -> int override { return 4; }
};

void run(OutputFileImpl &file, UseCase &useCase) { useCase.run(file); }

auto written(WriterStub &writer) -> const LogString & {
    return writer.written();
}

void openNewFile(OutputFileImpl &file, const TestIdentity &identity) {
    file.openNewFile(identity);
}

void write(OutputFileImpl &file, const FreeResponseTrial &trial) {
    file.write(trial);
}

void write(OutputFileImpl &file, const CorrectKeywordsTrial &trial) {
    file.write(trial);
}

class OutputFileTests : public ::testing::Test {
  protected:
    WriterStub writer;
    OutputFilePathStub path;
    OutputFileImpl file{&writer, &path};
    WritingAdaptiveCoordinateResponseTrial
        writingAdaptiveCoordinateResponseTrial;
    WritingFixedLevelCoordinateResponseTrial
        writingFixedLevelCoordinateResponseTrial;
    WritingOpenSetAdaptiveTrial writingOpenSetAdaptiveTrial;
    WritingCorrectKeywordsTrial writingCorrectKeywordsTrial;
    FreeResponseTrial freeResponseTrial;
    CorrectKeywordsTrial correctKeywordsTrial;
    open_set::AdaptiveTrial openSetAdaptiveTrial;
    AdaptiveTest adaptiveTest;
    FixedLevelTest fixedLevelTest;
    TestIdentity testIdentity;
    WritingFixedLevelTest writingFixedLevelTest;
    WritingAdaptiveTest writingAdaptiveTest;

    void writeOpenSetAdaptiveTrial() { file.write(openSetAdaptiveTrial); }

    void assertWriterContainsConditionName(Condition c) {
        assertColonDelimitedEntryWritten("condition", name(c));
    }

    void assertWriterContains(const std::string &s) {
        assertTrue(written(writer).contains(s));
    }

    void assertWrittenLast(const std::string &s) {
        assertTrue(written(writer).endsWith(s));
    }

    auto nthCommaDelimitedEntryOfLine(int n, int line) -> std::string {
        std::string written_ = written(writer);
        auto precedingNewLine = find_nth_element(written_, line - 1, '\n');
        auto line_ = written_.substr(precedingNewLine + 1);
        auto precedingComma = find_nth_element(line_, n - 1, ',');
        auto entryBeginning =
            (precedingComma == std::string::npos) ? 0U : precedingComma + 2;
        return upUntilFirstOfAny(line_.substr(entryBeginning), {',', '\n'});
    }

    static auto find_nth_element(const std::string &content, int n, char what)
        -> std::string::size_type {
        auto found = std::string::npos;
        for (int i = 0; i < n; ++i)
            found = content.find(what, found + 1U);
        return found;
    }

    static auto upUntilFirstOfAny(
        const std::string &content, std::vector<char> v) -> std::string {
        return content.substr(0, content.find_first_of({v.begin(), v.end()}));
    }

    void assertConditionNameWritten(WritingTestUseCase &useCase, Condition c) {
        useCase.test().condition = c;
        run(file, useCase);
        assertWriterContainsConditionName(c);
    }

    void assertTestIdentityWritten(WritingTestUseCase &useCase) {
        useCase.test().identity.subjectId = "a";
        useCase.test().identity.testerId = "b";
        useCase.test().identity.session = "c";
        useCase.test().identity.method = "d";
        useCase.test().identity.rmeSetting = "e";
        useCase.test().identity.transducer = Transducer::twoSpeakers;
        run(file, useCase);
        assertColonDelimitedEntryWritten("subject", "a");
        assertColonDelimitedEntryWritten("tester", "b");
        assertColonDelimitedEntryWritten("session", "c");
        assertColonDelimitedEntryWritten("method", "d");
        assertColonDelimitedEntryWritten("RME setting", "e");
        assertColonDelimitedEntryWritten(
            "transducer", name(Transducer::twoSpeakers));
    }

    void assertCommonTestWritten(WritingTestUseCase &useCase) {
        useCase.test().maskerFilePath = "a";
        useCase.test().targetListDirectory = "d";
        useCase.test().maskerLevel_dB_SPL = 1;
        run(file, useCase);
        assertColonDelimitedEntryWritten("masker", "a");
        assertColonDelimitedEntryWritten("targets", "d");
        assertColonDelimitedEntryWritten("masker level (dB SPL)", "1");
        assertWrittenLast("\n\n");
    }

    void assertNthEntryOfSecondLine(const std::string &what, int n) {
        assertNthCommaDelimitedEntryOfLine(what, n, 2);
    }

    void assertEntriesOfSecondLine(int n) {
        std::string written_ = written(writer);
        auto precedingNewLine = find_nth_element(written_, 2 - 1, '\n');
        auto line_ = written_.substr(precedingNewLine + 1);
        assertEqual(
            std::iterator_traits<std::string::iterator>::difference_type{n - 1},
            std::count(line_.begin(), line_.end(), ','));
    }

    void assertNthCommaDelimitedEntryOfLine(
        const std::string &what, int n, int line) {
        assertEqual(what, nthCommaDelimitedEntryOfLine(n, line));
    }

    void assertNthCommaDelimitedEntryOfLine(HeadingItem item, int n, int line) {
        assertEqual(name(item), nthCommaDelimitedEntryOfLine(n, line));
    }

    void assertIncorrectTrialWritesEvaluation(WritingTrialUseCase &useCase) {
        useCase.incorrect();
        run(file, useCase);
        assertNthEntryOfSecondLine("incorrect", useCase.evaluationEntryIndex());
    }

    void assertCorrectTrialWritesEvaluation(WritingTrialUseCase &useCase) {
        useCase.correct();
        run(file, useCase);
        assertNthEntryOfSecondLine("correct", useCase.evaluationEntryIndex());
    }

    void assertFlaggedWritesFlagged() {
        freeResponseTrial.flagged = true;
        write(file, freeResponseTrial);
        assertNthEntryOfSecondLine("FLAGGED", 3);
    }

    void assertNoFlagYieldsEntries(int n) {
        freeResponseTrial.flagged = false;
        write(file, freeResponseTrial);
        assertEntriesOfSecondLine(n);
    }

    void assertAdaptiveCoordinateHeadingAtLine(int n) {
        assertNthCommaDelimitedEntryOfLine(HeadingItem::snr_dB, 1, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::correctNumber, 2, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::subjectNumber, 3, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::correctColor, 4, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::subjectColor, 5, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::evaluation,
            writingAdaptiveCoordinateResponseTrial.evaluationEntryIndex(), n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::reversals, 7, n);
    }

    void assertFixedLevelCoordinateResponseHeadingAtLine(int n) {
        assertNthCommaDelimitedEntryOfLine(HeadingItem::correctNumber, 1, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::subjectNumber, 2, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::correctColor, 3, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::subjectColor, 4, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::evaluation,
            writingFixedLevelCoordinateResponseTrial.evaluationEntryIndex(), n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::stimulus, 6, n);
    }

    void assertFreeResponseHeadingAtLine(int n) {
        assertNthCommaDelimitedEntryOfLine(HeadingItem::target, 1, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::freeResponse, 2, n);
    }

    void assertCorrectKeywordsHeadingAtLine(int n) {
        assertNthCommaDelimitedEntryOfLine(HeadingItem::snr_dB, 1, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::target, 2, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::correctKeywords, 3, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::evaluation, 4, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::reversals, 5, n);
    }

    void assertOpenSetAdaptiveHeadingAtLine(int n) {
        assertNthCommaDelimitedEntryOfLine(HeadingItem::snr_dB, 1, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::target, 2, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::evaluation, 3, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::reversals, 4, n);
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
        run(file, writingAdaptiveCoordinateResponseTrial);
        assertNthCommaDelimitedEntryOfLine("1", 1, n);
        assertNthCommaDelimitedEntryOfLine("2", 2, n);
        assertNthCommaDelimitedEntryOfLine("3", 3, n);
        assertNthCommaDelimitedEntryOfLine(name(Color::green), 4, n);
        assertNthCommaDelimitedEntryOfLine(name(Color::red), 5, n);
        assertNthCommaDelimitedEntryOfLine("4", 7, n);
    }

    void assertWritesFixedLevelCoordinateResponseTrialOnLine(int n) {
        using coordinate_response_measure::Color;
        auto &trial = writingFixedLevelCoordinateResponseTrial.trial();
        trial.correctNumber = 2;
        trial.subjectNumber = 3;
        trial.correctColor = Color::green;
        trial.subjectColor = Color::red;
        trial.target = "a";
        run(file, writingFixedLevelCoordinateResponseTrial);
        assertNthCommaDelimitedEntryOfLine("2", 1, n);
        assertNthCommaDelimitedEntryOfLine("3", 2, n);
        assertNthCommaDelimitedEntryOfLine(name(Color::green), 3, n);
        assertNthCommaDelimitedEntryOfLine(name(Color::red), 4, n);
        assertNthCommaDelimitedEntryOfLine("a", 6, n);
    }

    void assertWritesFreeResponseTrialOnLine(int n) {
        freeResponseTrial.target = "a";
        freeResponseTrial.response = "b";
        write(file, freeResponseTrial);
        assertNthCommaDelimitedEntryOfLine("a", 1, n);
        assertNthCommaDelimitedEntryOfLine("b", 2, n);
    }

    void assertWritesOpenSetAdaptiveTrialOnLine(int n) {
        openSetAdaptiveTrial.SNR_dB = 11;
        openSetAdaptiveTrial.target = "a";
        openSetAdaptiveTrial.reversals = 22;
        writeOpenSetAdaptiveTrial();
        assertNthCommaDelimitedEntryOfLine("11", 1, n);
        assertNthCommaDelimitedEntryOfLine("a", 2, n);
        assertNthCommaDelimitedEntryOfLine("22", 4, n);
    }

    void assertWritesCorrectKeywordsTrialOnLine(int n) {
        correctKeywordsTrial.SNR_dB = 11;
        correctKeywordsTrial.target = "a";
        correctKeywordsTrial.count = 22;
        correctKeywordsTrial.reversals = 33;
        write(file, correctKeywordsTrial);
        assertNthCommaDelimitedEntryOfLine("11", 1, n);
        assertNthCommaDelimitedEntryOfLine("a", 2, n);
        assertNthCommaDelimitedEntryOfLine("22", 3, n);
        assertNthCommaDelimitedEntryOfLine("33", 5, n);
    }

    void assertColonDelimitedEntryWritten(
        const std::string &label, const std::string &what) {
        assertWriterContains(label + ": " + what + "\n");
    }
};

TEST_F(OutputFileTests, writeAdaptiveCoordinateResponseTrialHeading) {
    run(file, writingAdaptiveCoordinateResponseTrial);
    assertAdaptiveCoordinateHeadingAtLine(1);
}

TEST_F(OutputFileTests, writeFixedLevelCoordinateResponseTrialHeading) {
    run(file, writingFixedLevelCoordinateResponseTrial);
    assertFixedLevelCoordinateResponseHeadingAtLine(1);
}

TEST_F(OutputFileTests, writeFreeResponseTrialHeading) {
    write(file, freeResponseTrial);
    assertFreeResponseHeadingAtLine(1);
}

TEST_F(OutputFileTests, writeCorrectKeywordsTrialHeading) {
    write(file, correctKeywordsTrial);
    assertCorrectKeywordsHeadingAtLine(1);
}

TEST_F(OutputFileTests, writeOpenSetAdaptiveTrialHeading) {
    writeOpenSetAdaptiveTrial();
    assertOpenSetAdaptiveHeadingAtLine(1);
}

TEST_F(OutputFileTests, writeAdaptiveCoordinateResponseTrial) {
    assertWritesAdaptiveCoordinateResponseTrialOnLine(2);
}

TEST_F(OutputFileTests, writeFixedLevelCoordinateResponseTrial) {
    assertWritesFixedLevelCoordinateResponseTrialOnLine(2);
}

TEST_F(OutputFileTests, writeFreeResponseTrial) {
    assertWritesFreeResponseTrialOnLine(2);
}

TEST_F(OutputFileTests, writeOpenSetAdaptiveTrial) {
    assertWritesOpenSetAdaptiveTrialOnLine(2);
}

TEST_F(OutputFileTests, writeCorrectKeywordsTrial) {
    assertWritesCorrectKeywordsTrialOnLine(2);
}

TEST_F(OutputFileTests,
    writeAdaptiveCoordinateResponseTrialTwiceDoesNotWriteHeadingTwice) {
    run(file, writingAdaptiveCoordinateResponseTrial);
    assertWritesAdaptiveCoordinateResponseTrialOnLine(3);
}

TEST_F(OutputFileTests,
    writeFixedLevelCoordinateResponseTrialTwiceDoesNotWriteHeadingTwice) {
    run(file, writingFixedLevelCoordinateResponseTrial);
    assertWritesFixedLevelCoordinateResponseTrialOnLine(3);
}

TEST_F(OutputFileTests, writeFreeResponseTrialTwiceDoesNotWriteHeadingTwice) {
    write(file, freeResponseTrial);
    assertWritesFreeResponseTrialOnLine(3);
}

TEST_F(
    OutputFileTests, writeOpenSetAdaptiveTrialTwiceDoesNotWriteHeadingTwice) {
    writeOpenSetAdaptiveTrial();
    assertWritesOpenSetAdaptiveTrialOnLine(3);
}

TEST_F(
    OutputFileTests, writeCorrectKeywordsTrialTwiceDoesNotWriteHeadingTwice) {
    write(file, correctKeywordsTrial);
    assertWritesCorrectKeywordsTrialOnLine(3);
}

TEST_F(OutputFileTests,
    writeAdaptiveCoordinateResponseTrialTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    run(file, writingAdaptiveCoordinateResponseTrial);
    openNewFile(file, testIdentity);
    run(file, writingAdaptiveCoordinateResponseTrial);
    assertAdaptiveCoordinateHeadingAtLine(3);
}

TEST_F(OutputFileTests,
    writeFixedLevelCoordinateResponseTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    run(file, writingFixedLevelCoordinateResponseTrial);
    openNewFile(file, testIdentity);
    run(file, writingFixedLevelCoordinateResponseTrial);
    assertFixedLevelCoordinateResponseHeadingAtLine(3);
}

TEST_F(OutputFileTests,
    writeFreeResponseTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    write(file, freeResponseTrial);
    openNewFile(file, testIdentity);
    write(file, freeResponseTrial);
    assertFreeResponseHeadingAtLine(3);
}

TEST_F(OutputFileTests,
    writeOpenSetAdaptiveTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    writeOpenSetAdaptiveTrial();
    openNewFile(file, testIdentity);
    writeOpenSetAdaptiveTrial();
    assertOpenSetAdaptiveHeadingAtLine(3);
}

TEST_F(OutputFileTests,
    writeCorrectKeywordsTrialTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    write(file, correctKeywordsTrial);
    openNewFile(file, testIdentity);
    write(file, correctKeywordsTrial);
    assertCorrectKeywordsHeadingAtLine(3);
}

TEST_F(OutputFileTests, writeIncorrectAdaptiveCoordinateResponseTrial) {
    assertIncorrectTrialWritesEvaluation(
        writingAdaptiveCoordinateResponseTrial);
}

TEST_F(OutputFileTests, writeIncorrectFixedLevelCoordinateResponseTrial) {
    assertIncorrectTrialWritesEvaluation(
        writingFixedLevelCoordinateResponseTrial);
}

TEST_F(OutputFileTests, writeIncorrectOpenSetAdaptiveTrial) {
    assertIncorrectTrialWritesEvaluation(writingOpenSetAdaptiveTrial);
}

TEST_F(OutputFileTests, writeIncorrectKeywordsTrial) {
    assertIncorrectTrialWritesEvaluation(writingCorrectKeywordsTrial);
}

TEST_F(OutputFileTests, writeCorrectAdaptiveCoordinateResponseTrial) {
    assertCorrectTrialWritesEvaluation(writingAdaptiveCoordinateResponseTrial);
}

TEST_F(OutputFileTests, writeCorrectFixedLevelCoordinateResponseTrial) {
    assertCorrectTrialWritesEvaluation(
        writingFixedLevelCoordinateResponseTrial);
}

TEST_F(OutputFileTests, writeCorrectOpenSetAdaptiveTrial) {
    assertCorrectTrialWritesEvaluation(writingOpenSetAdaptiveTrial);
}

TEST_F(OutputFileTests, writeCorrectKeywordsTrialWritesCorrectEvaluation) {
    assertCorrectTrialWritesEvaluation(writingCorrectKeywordsTrial);
}

TEST_F(OutputFileTests, writeFlaggedFreeResponseTrial) {
    assertFlaggedWritesFlagged();
}

TEST_F(OutputFileTests, writeNoFlagFreeResponseTrialOnlyTwoEntries) {
    assertNoFlagYieldsEntries(2);
}

TEST_F(OutputFileTests, uninitializedColorDoesNotBreak) {
    coordinate_response_measure::AdaptiveTrial uninitialized;
    file.write(uninitialized);
}

TEST_F(OutputFileTests, writeCommonAdaptiveTest) {
    assertCommonTestWritten(writingAdaptiveTest);
}

TEST_F(OutputFileTests, writesTrackSettings) {
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
    file.write(test);
    assertColonDelimitedEntryWritten("up", "1 5");
    assertColonDelimitedEntryWritten("down", "2 6");
    assertColonDelimitedEntryWritten("reversals per step size", "3 7");
    assertColonDelimitedEntryWritten("step sizes (dB)", "4 8");
    assertWrittenLast("\n\n");
}

TEST_F(OutputFileTests, writeAdaptiveTestResult) {
    AdaptiveTestResults results{};
    results.push_back({"a", 1.});
    results.push_back({"b", 2.});
    results.push_back({"c", 3.});
    file.write(results);
    assertColonDelimitedEntryWritten("threshold for a", "1");
    assertColonDelimitedEntryWritten("threshold for b", "2");
    assertColonDelimitedEntryWritten("threshold for c", "3");
}

TEST_F(OutputFileTests, writeCommonFixedLevelTest) {
    assertCommonTestWritten(writingFixedLevelTest);
}

TEST_F(OutputFileTests, writeAdaptiveTest) {
    adaptiveTest.startingSnr_dB = 2;
    file.write(adaptiveTest);
    assertColonDelimitedEntryWritten("starting SNR (dB)", "2");
    assertWrittenLast("\n\n");
}

TEST_F(OutputFileTests, writeFixedLevelTest) {
    fixedLevelTest.snr_dB = 2;
    file.write(fixedLevelTest);
    assertColonDelimitedEntryWritten("SNR (dB)", "2");
    assertWrittenLast("\n\n");
}

TEST_F(OutputFileTests, writeAdaptiveTestInformation) {
    assertTestIdentityWritten(writingAdaptiveTest);
}

TEST_F(OutputFileTests, writeFixedLevelTestInformation) {
    assertTestIdentityWritten(writingFixedLevelTest);
}

TEST_F(OutputFileTests, writeAdaptiveTestWithAvCondition) {
    assertConditionNameWritten(writingAdaptiveTest, Condition::audioVisual);
}

TEST_F(OutputFileTests, writeAdaptiveTestWithAuditoryOnlyCondition) {
    assertConditionNameWritten(writingAdaptiveTest, Condition::auditoryOnly);
}

TEST_F(OutputFileTests, writeFixedLevelTestWithAvCondition) {
    assertConditionNameWritten(writingFixedLevelTest, Condition::audioVisual);
}

TEST_F(OutputFileTests, writeFixedLevelTestWithAuditoryOnlyCondition) {
    assertConditionNameWritten(writingFixedLevelTest, Condition::auditoryOnly);
}

TEST_F(OutputFileTests, openPassesFormattedFilePath) {
    path.setFileName("a");
    path.setOutputDirectory("b");
    openNewFile(file, testIdentity);
    assertEqual("b/a.txt", writer.filePath());
}

TEST_F(OutputFileTests, closeClosesWriter) {
    file.close();
    assertTrue(writer.closed());
}

TEST_F(OutputFileTests, saveSavesWriter) {
    file.save();
    assertTrue(writer.saved());
}

TEST_F(OutputFileTests, openPassesTestInformation) {
    openNewFile(file, testIdentity);
    EXPECT_EQ(&testIdentity, path.testIdentity());
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
    OutputFileImpl file{&writer, &path};
    try {
        file.openNewFile({});
        FAIL() << "Expected OutputFileImpl::OpenFailure";
    } catch (const OutputFileImpl::OpenFailure &) {
    }
}
}
}
