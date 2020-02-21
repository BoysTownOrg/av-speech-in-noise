#include "LogString.hpp"
#include "assert-utility.hpp"
#include "recognition-test/RecognitionTestModel.hpp"
#include <recognition-test/OutputFile.hpp>
#include <gtest/gtest.h>
#include <cstdint>
#include <algorithm>

namespace av_speech_in_noise {
namespace {
class WriterStub : public Writer {
    LogString written_;
    std::string filePath_;
    bool closed_{};
    bool saved_{};

  public:
    void save() override { saved_ = true; }

    auto saved() const { return saved_; }

    void close() override { closed_ = true; }

    void open(std::string f) override { filePath_ = std::move(f); }

    auto filePath() const { return filePath_; }

    auto closed() const { return closed_; }

    auto written() const -> auto & { return written_; }

    void write(std::string s) override { written_.insert(s); }

    auto failed() -> bool override { return {}; }
};

class OutputFilePathStub : public OutputFilePath {
    std::string fileName_;
    std::string homeDirectory_;
    std::string outputDirectory_;
    const TestIdentity *testInformation_{};

  public:
    auto outputDirectory() -> std::string override { return outputDirectory_; }

    void setOutputDirectory(std::string s) { outputDirectory_ = std::move(s); }

    void setFileName(std::string s) { fileName_ = std::move(s); }

    auto generateFileName(const TestIdentity &p) -> std::string override {
        testInformation_ = &p;
        return fileName_;
    }

    auto homeDirectory() -> std::string override { return homeDirectory_; }

    [[nodiscard]] auto testIdentity() const { return testInformation_; }
};

class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run(OutputFileImpl &) = 0;
};

class WritingTestUseCase : public virtual UseCase {
  public:
    virtual void setCondition(Condition) = 0;
    virtual void setTestIdentity(const TestIdentity &) = 0;
    virtual void setCommonTest(const Test &) = 0;
};

class WritingAdaptiveTest : public WritingTestUseCase {
    AdaptiveTest test{};

  public:
    void setCondition(Condition c) override { test.condition = c; }

    void setTestIdentity(const TestIdentity &p) override { test.identity = p; }

    void setCommonTest(const Test &p) override {
        static_cast<Test &>(test) = p;
    }

    void run(OutputFileImpl &file) override { file.writeTest(test); }
};

class WritingFixedLevelTest : public WritingTestUseCase {
    FixedLevelTest test{};

  public:
    void setCondition(Condition c) override { test.condition = c; }

    void setCommonTest(const Test &p) override {
        static_cast<Test &>(test) = p;
    }

    void setTestIdentity(const TestIdentity &p) override { test.identity = p; }

    void run(OutputFileImpl &file) override { file.writeTest(test); }
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
    open_set::CorrectKeywordsTrial trial_{};

  public:
    void incorrect() override { trial_.correct = false; }

    void correct() override { trial_.correct = true; }

    void run(OutputFileImpl &file) override { file.write(trial_); }

    auto evaluationEntryIndex() -> int override { return 4; }
};

void write(OutputFileImpl &file, const BinocularGazeSamples &gazeSamples) {
    file.write(gazeSamples);
}

void writeFadeInComplete(
    OutputFileImpl &file, const ConvertedAudioSampleSystemTime &time) {
    file.writeFadeInComplete(time);
}

void writeTargetStartTimeNanoseconds(OutputFileImpl &file, std::uintmax_t t) {
    file.writeTargetStartTimeNanoseconds(t);
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
    open_set::FreeResponseTrial freeResponseTrial;
    open_set::CorrectKeywordsTrial correctKeywordsTrial;
    open_set::AdaptiveTrial openSetAdaptiveTrial;
    AdaptiveTest adaptiveTest;
    FixedLevelTest fixedLevelTest;
    TestIdentity testIdentity;
    WritingFixedLevelTest writingFixedLevelTest;
    WritingAdaptiveTest writingAdaptiveTest;
    BinocularGazeSamples eyeGazes;
    ConvertedAudioSampleSystemTime audioSampleTime;

    void run(UseCase &useCase) { useCase.run(file); }

    void openNewFile() { file.openNewFile(testIdentity); }

    void writeFreeResponseTrial() { file.write(freeResponseTrial); }

    void writeCorrectKeywordsTrial() { file.write(correctKeywordsTrial); }

    void writeOpenSetAdaptiveTrial() { file.write(openSetAdaptiveTrial); }

    auto written() -> const auto & { return writer.written(); }

    void assertWriterContainsConditionName(Condition c) {
        assertColonDelimitedEntryWritten("condition", conditionName(c));
    }

    void assertWriterContains(const std::string &s) {
        assertTrue(written().contains(s));
    }

    void assertWrittenLast(const std::string &s) {
        assertTrue(written().endsWith(s));
    }

    auto nthCommaDelimitedEntryOfLine(int n, int line) -> std::string {
        std::string written_ = written();
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
        useCase.setCondition(c);
        useCase.run(file);
        assertWriterContainsConditionName(c);
    }

    void assertTestIdentityWritten(WritingTestUseCase &useCase) {
        TestIdentity identity;
        identity.subjectId = "a";
        identity.testerId = "b";
        identity.session = "c";
        identity.method = "d";
        useCase.setTestIdentity(identity);
        useCase.run(file);
        assertColonDelimitedEntryWritten("subject", "a");
        assertColonDelimitedEntryWritten("tester", "b");
        assertColonDelimitedEntryWritten("session", "c");
        assertColonDelimitedEntryWritten("method", "d");
    }

    void assertCommonTestWritten(WritingTestUseCase &useCase) {
        av_speech_in_noise::Test common;
        common.maskerFilePath = "a";
        common.targetListDirectory = "d";
        common.maskerLevel_dB_SPL = 1;
        useCase.setCommonTest(common);
        useCase.run(file);
        assertColonDelimitedEntryWritten("masker", "a");
        assertColonDelimitedEntryWritten("targets", "d");
        assertColonDelimitedEntryWritten("masker level (dB SPL)", "1");
        assertWrittenLast("\n\n");
    }

    void assertNthEntryOfSecondLine(const std::string &what, int n) {
        assertNthCommaDelimitedEntryOfLine(what, n, 2);
    }

    void assertEntriesOfSecondLine(int n) {
        std::string written_ = written();
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
        assertEqual(
            headingItemName(item), nthCommaDelimitedEntryOfLine(n, line));
    }

    void assertIncorrectTrialWritesEvaluation(WritingTrialUseCase &useCase) {
        useCase.incorrect();
        run(useCase);
        assertNthEntryOfSecondLine("incorrect", useCase.evaluationEntryIndex());
    }

    void assertCorrectTrialWritesEvaluation(WritingTrialUseCase &useCase) {
        useCase.correct();
        run(useCase);
        assertNthEntryOfSecondLine("correct", useCase.evaluationEntryIndex());
    }

    void assertFlaggedWritesFlagged() {
        freeResponseTrial.flagged = true;
        writeFreeResponseTrial();
        assertNthEntryOfSecondLine("FLAGGED", 3);
    }

    void assertNoFlagYieldsEntries(int n) {
        freeResponseTrial.flagged = false;
        writeFreeResponseTrial();
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
        run(writingAdaptiveCoordinateResponseTrial);
        assertNthCommaDelimitedEntryOfLine("1", 1, n);
        assertNthCommaDelimitedEntryOfLine("2", 2, n);
        assertNthCommaDelimitedEntryOfLine("3", 3, n);
        assertNthCommaDelimitedEntryOfLine(colorName(Color::green), 4, n);
        assertNthCommaDelimitedEntryOfLine(colorName(Color::red), 5, n);
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
        run(writingFixedLevelCoordinateResponseTrial);
        assertNthCommaDelimitedEntryOfLine("2", 1, n);
        assertNthCommaDelimitedEntryOfLine("3", 2, n);
        assertNthCommaDelimitedEntryOfLine(colorName(Color::green), 3, n);
        assertNthCommaDelimitedEntryOfLine(colorName(Color::red), 4, n);
        assertNthCommaDelimitedEntryOfLine("a", 6, n);
    }

    void assertWritesFreeResponseTrialOnLine(int n) {
        freeResponseTrial.target = "a";
        freeResponseTrial.response = "b";
        writeFreeResponseTrial();
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
        writeCorrectKeywordsTrial();
        assertNthCommaDelimitedEntryOfLine("11", 1, n);
        assertNthCommaDelimitedEntryOfLine("a", 2, n);
        assertNthCommaDelimitedEntryOfLine("22", 3, n);
        assertNthCommaDelimitedEntryOfLine("33", 5, n);
    }

    void assertColonDelimitedEntryWritten(
        const std::string &label, const std::string &what) {
        assertWriterContains(label + ": " + what + "\n");
    }

    void setEyeGazes(std::vector<std::uintmax_t> t, std::vector<EyeGaze> left,
        std::vector<EyeGaze> right) {
        eyeGazes.resize(t.size());
        std::generate(eyeGazes.begin(), eyeGazes.end(), [&, n = 0]() mutable {
            BinocularGazeSample gazeSamples{t.at(n), left.at(n), right.at(n)};
            ++n;
            return gazeSamples;
        });
    }

    void setAudioSampleTime(std::uintmax_t t, gsl::index offset) {
        audioSampleTime.nanoseconds = t;
        audioSampleTime.sampleOffset = offset;
    }
};

TEST_F(OutputFileTests, writeAdaptiveCoordinateResponseTrialHeading) {
    run(writingAdaptiveCoordinateResponseTrial);
    assertAdaptiveCoordinateHeadingAtLine(1);
}

TEST_F(OutputFileTests, writeFixedLevelCoordinateResponseTrialHeading) {
    run(writingFixedLevelCoordinateResponseTrial);
    assertFixedLevelCoordinateResponseHeadingAtLine(1);
}

TEST_F(OutputFileTests, writeFreeResponseTrialHeading) {
    writeFreeResponseTrial();
    assertFreeResponseHeadingAtLine(1);
}

TEST_F(OutputFileTests, writeCorrectKeywordsTrialHeading) {
    writeCorrectKeywordsTrial();
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
    run(writingAdaptiveCoordinateResponseTrial);
    assertWritesAdaptiveCoordinateResponseTrialOnLine(3);
}

TEST_F(OutputFileTests,
    writeFixedLevelCoordinateResponseTrialTwiceDoesNotWriteHeadingTwice) {
    run(writingFixedLevelCoordinateResponseTrial);
    assertWritesFixedLevelCoordinateResponseTrialOnLine(3);
}

TEST_F(OutputFileTests, writeFreeResponseTrialTwiceDoesNotWriteHeadingTwice) {
    writeFreeResponseTrial();
    assertWritesFreeResponseTrialOnLine(3);
}

TEST_F(
    OutputFileTests, writeOpenSetAdaptiveTrialTwiceDoesNotWriteHeadingTwice) {
    writeOpenSetAdaptiveTrial();
    assertWritesOpenSetAdaptiveTrialOnLine(3);
}

TEST_F(
    OutputFileTests, writeCorrectKeywordsTrialTwiceDoesNotWriteHeadingTwice) {
    writeCorrectKeywordsTrial();
    assertWritesCorrectKeywordsTrialOnLine(3);
}

TEST_F(OutputFileTests,
    writeAdaptiveCoordinateResponseTrialTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    run(writingAdaptiveCoordinateResponseTrial);
    openNewFile();
    run(writingAdaptiveCoordinateResponseTrial);
    assertAdaptiveCoordinateHeadingAtLine(3);
}

TEST_F(OutputFileTests,
    writeFixedLevelCoordinateResponseTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    run(writingFixedLevelCoordinateResponseTrial);
    openNewFile();
    run(writingFixedLevelCoordinateResponseTrial);
    assertFixedLevelCoordinateResponseHeadingAtLine(3);
}

TEST_F(OutputFileTests,
    writeFreeResponseTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    writeFreeResponseTrial();
    openNewFile();
    writeFreeResponseTrial();
    assertFreeResponseHeadingAtLine(3);
}

TEST_F(OutputFileTests,
    writeOpenSetAdaptiveTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    writeOpenSetAdaptiveTrial();
    openNewFile();
    writeOpenSetAdaptiveTrial();
    assertOpenSetAdaptiveHeadingAtLine(3);
}

TEST_F(OutputFileTests,
    writeCorrectKeywordsTrialTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    writeCorrectKeywordsTrial();
    openNewFile();
    writeCorrectKeywordsTrial();
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

TEST_F(OutputFileTests, writeCommonFixedLevelTest) {
    assertCommonTestWritten(writingFixedLevelTest);
}

TEST_F(OutputFileTests, writeAdaptiveTest) {
    adaptiveTest.startingSnr_dB = 2;
    file.writeTest(adaptiveTest);
    assertColonDelimitedEntryWritten("starting SNR (dB)", "2");
    assertWrittenLast("\n\n");
}

TEST_F(OutputFileTests, writeFixedLevelTest) {
    fixedLevelTest.snr_dB = 2;
    file.writeTest(fixedLevelTest);
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

TEST_F(OutputFileTests, writeEyeGazes) {
    setEyeGazes({1, 2, 3}, {{0.4, 0.44}, {0.5, 0.55}, {0.6, 0.66}},
        {{0.7, 0.77}, {0.8, 0.88}, {0.9, 0.99}});
    write(file, eyeGazes);
    assertNthCommaDelimitedEntryOfLine(
        headingItemName(HeadingItem::systemTime), 1, 1);
    assertNthCommaDelimitedEntryOfLine(
        headingItemName(HeadingItem::leftGaze), 2, 1);
    assertNthCommaDelimitedEntryOfLine(
        headingItemName(HeadingItem::rightGaze), 3, 1);
    assertNthCommaDelimitedEntryOfLine("1", 1, 2);
    assertNthCommaDelimitedEntryOfLine("0.4 0.44", 2, 2);
    assertNthCommaDelimitedEntryOfLine("0.7 0.77", 3, 2);
    assertNthCommaDelimitedEntryOfLine("2", 1, 3);
    assertNthCommaDelimitedEntryOfLine("0.5 0.55", 2, 3);
    assertNthCommaDelimitedEntryOfLine("0.8 0.88", 3, 3);
    assertNthCommaDelimitedEntryOfLine("3", 1, 4);
    assertNthCommaDelimitedEntryOfLine("0.6 0.66", 2, 4);
    assertNthCommaDelimitedEntryOfLine("0.9 0.99", 3, 4);
}

TEST_F(OutputFileTests, writeFadeInCompleteTime) {
    writeTargetStartTimeNanoseconds(file, 1);
    assertColonDelimitedEntryWritten("target start time (ns)", "1");
}

TEST_F(OutputFileTests, openPassesFormattedFilePath) {
    path.setFileName("a");
    path.setOutputDirectory("b");
    openNewFile();
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
    openNewFile();
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
