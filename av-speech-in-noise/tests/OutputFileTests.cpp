#include "LogString.h"
#include "assert-utility.h"
#include <recognition-test/OutputFileImpl.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests { namespace {
class WriterStub : public Writer {
    LogString written_;
    std::string filePath_;
    bool closed_{};
    bool saved_{};
public:
    void save() override {
        saved_ = true;
    }

    auto saved() const {
        return saved_;
    }

    void close() override {
        closed_ = true;
    }

    void open(std::string f) override {
        filePath_ = std::move(f);
    }

    auto filePath() const {
        return filePath_;
    }

    auto closed() const {
        return closed_;
    }

    auto &written() const {
        return written_;
    }

    void write(std::string s) override {
        written_.insert(std::move(s));
    }

    bool failed() override {
        return {};
    }
};

class OutputFilePathStub : public OutputFilePath {
    std::string fileName_;
    std::string homeDirectory_;
    std::string outputDirectory_;
    const TestInformation *testInformation_{};
public:
    std::string outputDirectory() override {
        return outputDirectory_;
    }

    void setOutputDirectory(std::string s) {
        outputDirectory_ = std::move(s);
    }

    void setFileName(std::string s) {
        fileName_ = std::move(s);
    }

    std::string generateFileName(const TestInformation &p) override {
        testInformation_ = &p;
        return fileName_;
    }

    std::string homeDirectory() override {
        return homeDirectory_;
    }

    auto testInformation() const {
        return testInformation_;
    }
};

class UseCase {
public:
    virtual ~UseCase() = default;
    virtual void run(OutputFileImpl &) = 0;
};

class WritingTestUseCase : public virtual UseCase {
public:
    virtual void setCondition(Condition) = 0;
    virtual void setTestInfo(const TestInformation &) = 0;
    virtual void setCommonTest(const CommonTest &) = 0;
};

class WritingAdaptiveTest : public WritingTestUseCase {
    AdaptiveTest test{};
public:
    void setCondition(Condition c) override {
        test.common.condition = c;
    }

    void setTestInfo(const TestInformation &p) override {
        test.information = p;
    }

    void setCommonTest(const CommonTest &p) override {
        test.common = p;
    }

    void run(OutputFileImpl &file) override {
        file.writeTest(test);
    }
};

class WritingFixedLevelTest : public WritingTestUseCase {
    FixedLevelTest test{};
public:
    void setCondition(Condition c) override {
        test.common.condition = c;
    }

    void setCommonTest(const CommonTest &p) override {
        test.common = p;
    }

    void setTestInfo(const TestInformation &p) override {
        test.information = p;
    }

    void run(OutputFileImpl &file) override {
        file.writeTest(test);
    }
};

class WritingTrialUseCase : public virtual UseCase {
public:
    virtual void incorrect() = 0;
    virtual void correct() = 0;
    virtual int evaluationEntryIndex() = 0;
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
    auto &trial() {
        return trial_;
    }

    void incorrect() override {
        setIncorrect(trial_.trial);
    }

    void correct() override {
        setCorrect(trial_.trial);
    }

    int evaluationEntryIndex() override {
        return 6;
    }

    void run(av_speech_in_noise::OutputFileImpl &file) override {
        file.writeTrial(trial_);
    }
};

class WritingFixedLevelCoordinateResponseTrial : public WritingTrialUseCase {
    coordinate_response_measure::FixedLevelTrial trial_{};
public:
    auto &trial() {
        return trial_;
    }

    void incorrect() override {
        setIncorrect(trial_.trial);
    }

    void correct() override {
        setCorrect(trial_.trial);
    }

    void run(av_speech_in_noise::OutputFileImpl &file) override {
        file.writeTrial(trial_);
    }

    int evaluationEntryIndex() override {
        return 5;
    }
};

class OutputFileTests : public ::testing::Test {
protected:
    WriterStub writer;
    OutputFilePathStub path;
    OutputFileImpl file{&writer, &path};
    WritingAdaptiveCoordinateResponseTrial
        writingAdaptiveCoordinateResponseTrial;
    WritingFixedLevelCoordinateResponseTrial
        writingFixedLevelCoordinateResponseTrial;
    FreeResponseTrial freeResponseTrial;
    open_set::AdaptiveTrial openSetAdaptiveTrial;
    AdaptiveTest adaptiveTest;
    FixedLevelTest fixedLevelTest;
    TestInformation testInformation;
    WritingFixedLevelTest writingFixedLevelTest;
    WritingAdaptiveTest writingAdaptiveTest;

    void run(UseCase &useCase) {
        useCase.run(file);
    }

    void openNewFile() {
        file.openNewFile(testInformation);
    }

    void writeFreeResponseTrial() {
        file.writeTrial(freeResponseTrial);
    }

    void writeOpenSetAdaptiveTrial() {
        file.writeTrial(openSetAdaptiveTrial);
    }

    const auto &written() {
        return writer.written();
    }

    void assertWriterContainsConditionName(Condition c) {
        assertColonDelimitedEntryWritten("condition", conditionName(c));
    }

    void assertWriterContains(std::string s) {
        assertTrue(written().contains(std::move(s)));
    }

    void assertWrittenLast(std::string s) {
        assertTrue(written().endsWith(std::move(s)));
    }

    std::string nthCommaDelimitedEntryOfLine(int n, int line) {
        std::string written_ = written();
        auto precedingNewLine = find_nth_element(written_, line - 1, '\n');
        auto line_ = written_.substr(precedingNewLine + 1);
        auto precedingComma = find_nth_element(line_, n - 1, ',');
        auto entryBeginning = (precedingComma == std::string::npos)
            ? 0U
            : precedingComma + 2;
        return upUntilFirstOfAny(line_.substr(entryBeginning), {',', '\n'});
    }

    std::string::size_type find_nth_element(
        const std::string &content,
        int n,
        char what
    ) {
        auto found = std::string::npos;
        for (int i = 0; i < n; ++i)
            found = content.find(what, found + 1U);
        return found;
    }

    std::string upUntilFirstOfAny(
        const std::string &content,
        std::vector<char> v
    ) {
        return content.substr(0, content.find_first_of({v.begin(), v.end()}));
    }

    void assertConditionNameWritten(
        WritingTestUseCase &useCase,
        Condition c
    ) {
        useCase.setCondition(c);
        useCase.run(file);
        assertWriterContainsConditionName(c);
    }

    void assertTestInformationWritten(WritingTestUseCase &useCase) {
        TestInformation info;
        info.subjectId = "a";
        info.testerId = "b";
        info.session = "c";
        useCase.setTestInfo(info);
        useCase.run(file);
        assertColonDelimitedEntryWritten("subject", "a");
        assertColonDelimitedEntryWritten("tester", "b");
        assertColonDelimitedEntryWritten("session", "c");
    }

    void assertCommonTestWritten(WritingTestUseCase &useCase) {
        CommonTest common;
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

    void assertNthEntryOfFirstLine(std::string what, int n) {
        assertNthCommaDelimitedEntryOfLine(std::move(what), n, 1);
    }

    void assertNthEntryOfSecondLine(std::string what, int n) {
        assertNthCommaDelimitedEntryOfLine(std::move(what), n, 2);
    }

    void assertNthEntryOfThirdLine(std::string what, int n) {
        assertNthCommaDelimitedEntryOfLine(std::move(what), n, 3);
    }

    void assertNthCommaDelimitedEntryOfLine(std::string what, int n, int line) {
        assertEqual(std::move(what), nthCommaDelimitedEntryOfLine(n, line));
    }

    void assertNthCommaDelimitedEntryOfLine(HeadingItem item, int n, int line) {
        assertEqual(
            headingItemName(item),
            nthCommaDelimitedEntryOfLine(n, line)
        );
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

    void assertAdaptiveCoordinateHeadingAtLine(int n) {
        assertNthCommaDelimitedEntryOfLine(HeadingItem::snr_dB, 1, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::correctNumber, 2, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::subjectNumber, 3, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::correctColor, 4, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::subjectColor, 5, n);
        assertNthCommaDelimitedEntryOfLine(
            HeadingItem::evaluation,
            writingAdaptiveCoordinateResponseTrial.evaluationEntryIndex(),
            n
        );
        assertNthCommaDelimitedEntryOfLine(HeadingItem::reversals, 7, n);
    }

    void assertFixedLevelCoordinateResponseHeadingAtLine(int n) {
        assertNthCommaDelimitedEntryOfLine(HeadingItem::correctNumber, 1, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::subjectNumber, 2, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::correctColor, 3, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::subjectColor, 4, n);
        assertNthCommaDelimitedEntryOfLine(
            HeadingItem::evaluation,
            writingFixedLevelCoordinateResponseTrial.evaluationEntryIndex(),
            n
        );
        assertNthCommaDelimitedEntryOfLine(HeadingItem::stimulus, 6, n);
    }

    void assertFreeResponseHeadingAtLine(int n) {
        assertNthCommaDelimitedEntryOfLine(HeadingItem::target, 1, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::freeResponse, 2, n);
    }

    void assertOpenSetAdaptiveHeadingAtLine(int n) {
        assertNthCommaDelimitedEntryOfLine(HeadingItem::target, 1, n);
        assertNthCommaDelimitedEntryOfLine(HeadingItem::evaluation, 2, n);
    }

    void assertWritesAdaptiveCoordinateResponseTrialOnLine(int n) {
        using coordinate_response_measure::Color;
        auto &trial = writingAdaptiveCoordinateResponseTrial.trial();
        trial.SNR_dB = 1;
        trial.trial.correctNumber = 2;
        trial.trial.subjectNumber = 3;
        trial.trial.correctColor = Color::green;
        trial.trial.subjectColor = Color::red;
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
        trial.trial.correctNumber = 2;
        trial.trial.subjectNumber = 3;
        trial.trial.correctColor = Color::green;
        trial.trial.subjectColor = Color::red;
        trial.trial.stimulus = "a";
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
        openSetAdaptiveTrial.target = "a";
        writeOpenSetAdaptiveTrial();
        assertNthCommaDelimitedEntryOfLine("a", 1, n);
    }

    void assertColonDelimitedEntryWritten(std::string label, std::string what) {
        assertWriterContains(label + ": " + what + "\n");
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

TEST_F(
    OutputFileTests,
    writeAdaptiveCoordinateResponseTrialTwiceDoesNotWriteHeadingTwice
) {
    run(writingAdaptiveCoordinateResponseTrial);
    assertWritesAdaptiveCoordinateResponseTrialOnLine(3);
}

TEST_F(
    OutputFileTests,
    writeFixedLevelCoordinateResponseTrialTwiceDoesNotWriteHeadingTwice
) {
    run(writingFixedLevelCoordinateResponseTrial);
    assertWritesFixedLevelCoordinateResponseTrialOnLine(3);
}

TEST_F(OutputFileTests, writeFreeResponseTrialTwiceDoesNotWriteHeadingTwice) {
    writeFreeResponseTrial();
    assertWritesFreeResponseTrialOnLine(3);
}

TEST_F(OutputFileTests, writeOpenSetAdaptiveTrialTwiceDoesNotWriteHeadingTwice) {
    writeOpenSetAdaptiveTrial();
    assertWritesOpenSetAdaptiveTrialOnLine(3);
}

TEST_F(
    OutputFileTests,
    writeAdaptiveCoordinateResponseTrialTwiceWritesTrialHeadingTwiceWhenNewFileOpened
) {
    run(writingAdaptiveCoordinateResponseTrial);
    openNewFile();
    run(writingAdaptiveCoordinateResponseTrial);
    assertAdaptiveCoordinateHeadingAtLine(3);
}

TEST_F(
    OutputFileTests,
    writeFixedLevelCoordinateResponseTwiceWritesTrialHeadingTwiceWhenNewFileOpened
) {
    run(writingFixedLevelCoordinateResponseTrial);
    openNewFile();
    run(writingFixedLevelCoordinateResponseTrial);
    assertFixedLevelCoordinateResponseHeadingAtLine(3);
}

TEST_F(
    OutputFileTests,
    writeFreeResponseTwiceWritesTrialHeadingTwiceWhenNewFileOpened
) {
    writeFreeResponseTrial();
    openNewFile();
    writeFreeResponseTrial();
    assertFreeResponseHeadingAtLine(3);
}

TEST_F(
    OutputFileTests,
    writeOpenSetAdaptiveTwiceWritesTrialHeadingTwiceWhenNewFileOpened
) {
    writeOpenSetAdaptiveTrial();
    openNewFile();
    writeOpenSetAdaptiveTrial();
    assertOpenSetAdaptiveHeadingAtLine(3);
}

TEST_F(OutputFileTests, writeIncorrectAdaptiveCoordinateResponseTrial) {
    assertIncorrectTrialWritesEvaluation(writingAdaptiveCoordinateResponseTrial);
}

TEST_F(OutputFileTests, writeCorrectAdaptiveCoordinateResponseTrial) {
    assertCorrectTrialWritesEvaluation(writingAdaptiveCoordinateResponseTrial);
}

TEST_F(OutputFileTests, writeIncorrectFixedLevelCoordinateResponseTrial) {
    assertIncorrectTrialWritesEvaluation(writingFixedLevelCoordinateResponseTrial);
}

TEST_F(OutputFileTests, writeCorrectFixedLevelCoordinateResponseTrial) {
    assertCorrectTrialWritesEvaluation(writingFixedLevelCoordinateResponseTrial);
}

TEST_F(OutputFileTests, uninitializedColorDoesNotBreak) {
    coordinate_response_measure::AdaptiveTrial uninitialized;
    file.writeTrial(uninitialized);
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
    assertTestInformationWritten(writingAdaptiveTest);
}

TEST_F(OutputFileTests, writeFixedLevelTestInformation) {
    assertTestInformationWritten(writingFixedLevelTest);
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
    EXPECT_EQ(&testInformation, path.testInformation());
}

class FailingWriter : public Writer {
    bool failed_{};
public:
    void open(std::string) override {
        failed_ = true;
    }

    bool failed() override {
        return failed_;
    }

    void close() override {}
    void write(std::string) override {}
    void save() override {}
};

TEST(
    FailingOutputFileTests,
    openThrowsOpenFailureWhenWriterFails
) {
    FailingWriter writer;
    OutputFilePathStub path;
    OutputFileImpl file{&writer, &path};
    try {
        file.openNewFile({});
        FAIL() << "Expected OutputFileImpl::OpenFailure";
    }
    catch (const OutputFileImpl::OpenFailure &) {
    }
}
}}
