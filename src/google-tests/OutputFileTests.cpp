#include "LogString.h"
#include "assert-utility.h"
#include <recognition-test/OutputFileImpl.hpp>
#include <gtest/gtest.h>

namespace {
    using namespace av_speech_in_noise;
    
    class WriterStub : public Writer {
        LogString written_;
        std::string filePath_;
        bool closed_{};
    public:
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
            fileName_ = s;
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

    class OutputFileTests : public ::testing::Test {
    protected:
        WriterStub writer;
        OutputFilePathStub path;
        OutputFileImpl file{&writer, &path};
        coordinate_response_measure::Trial coordinateResponseTrial{};
        AdaptiveTest adaptiveTest{};
        FixedLevelTest fixedLevelTest{};
        TestInformation testInformation{};
        
        void openNewFile() {
            file.openNewFile(testInformation);
        }
        
        void assertWriterContainsConditionName(Condition c) {
            adaptiveTest.condition = c;
            file.writeTest(adaptiveTest);
            std::string name = conditionName(c);
            assertWriterContains("condition: " + name + "\n");
        }
        
        void assertWriterContains(std::string s) {
            EXPECT_TRUE(writer.written().contains(std::move(s)));
        }
        
        void writeCoordinateResponseTrial() {
            file.writeTrial(coordinateResponseTrial);
        }
    };

    TEST_F(OutputFileTests, writeCoordinateResponseTrialHeading) {
        file.writeCoordinateResponseTrialHeading();
        assertEqual(
            "SNR (dB), correct number, subject number, "
            "correct color, subject color, evaluation, reversals\n",
            writer.written()
        );
    }

    TEST_F(OutputFileTests, writeCoordinateResponseTrial) {
        coordinateResponseTrial.SNR_dB = 1;
        coordinateResponseTrial.correctNumber = 2;
        coordinateResponseTrial.subjectNumber = 3;
        coordinateResponseTrial.correctColor = coordinate_response_measure::Color::green;
        coordinateResponseTrial.subjectColor = coordinate_response_measure::Color::red;
        coordinateResponseTrial.reversals = 4;
        coordinateResponseTrial.correct = false;
        writeCoordinateResponseTrial();
        assertEqual(
            "1, 2, 3, green, red, incorrect, 4\n",
            writer.written()
        );
    }

    TEST_F(OutputFileTests, writeIncorrectCoordinateResponseTrial) {
        coordinateResponseTrial.correct = false;
        writeCoordinateResponseTrial();
        assertWriterContains(" incorrect, ");
    }

    TEST_F(OutputFileTests, writeCorrectCoordinateResponseTrial) {
        coordinateResponseTrial.correct = true;
        writeCoordinateResponseTrial();
        assertWriterContains(" correct, ");
    }

    TEST_F(OutputFileTests, colorNameUninitializedColorDefined) {
        coordinate_response_measure::Trial uninitialized;
        file.writeTrial(uninitialized);
    }

    TEST_F(OutputFileTests, writeAdaptiveTest) {
        adaptiveTest.maskerFilePath = "a";
        adaptiveTest.information.session = "b";
        adaptiveTest.information.subjectId = "c";
        adaptiveTest.targetListDirectory = "d";
        adaptiveTest.information.testerId = "e";
        adaptiveTest.maskerLevel_dB_SPL = 1;
        adaptiveTest.startingSnr_dB = 2;
        file.writeTest(adaptiveTest);
        assertWriterContains("subject: c\n");
        assertWriterContains("tester: e\n");
        assertWriterContains("session: b\n");
        assertWriterContains("masker: a\n");
        assertWriterContains("targets: d\n");
        assertWriterContains("masker level (dB SPL): 1\n");
        assertWriterContains("starting SNR (dB): 2\n");
        EXPECT_TRUE(writer.written().endsWith("\n\n"));
    }

    TEST_F(OutputFileTests, writeTestWithAvCondition) {
        assertWriterContainsConditionName(Condition::audioVisual);
    }

    TEST_F(OutputFileTests, writeTestWithAuditoryOnlyCondition) {
        assertWriterContainsConditionName(Condition::auditoryOnly);
    }

    TEST_F(OutputFileTests, writeFixedLevelTest) {
        fixedLevelTest.maskerFilePath = "a";
        fixedLevelTest.information.session = "b";
        fixedLevelTest.information.subjectId = "c";
        fixedLevelTest.targetListDirectory = "d";
        fixedLevelTest.information.testerId = "e";
        fixedLevelTest.maskerLevel_dB_SPL = 1;
        fixedLevelTest.snr_dB = 2;
        file.writeTest(fixedLevelTest);
        assertWriterContains("subject: c\n");
        assertWriterContains("tester: e\n");
        assertWriterContains("session: b\n");
        assertWriterContains("masker: a\n");
        assertWriterContains("targets: d\n");
        assertWriterContains("masker level (dB SPL): 1\n");
        assertWriterContains("SNR (dB): 2\n");
        EXPECT_TRUE(writer.written().endsWith("\n\n"));
    }

    TEST_F(OutputFileTests, openPassesFormattedFilePath) {
        path.setFileName("a");
        path.setOutputDirectory("b");
        openNewFile();
        assertEqual("b/a.txt", writer.filePath());
    }

    TEST_F(OutputFileTests, closeClosesWriter) {
        file.close();
        EXPECT_TRUE(writer.closed());
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
}
