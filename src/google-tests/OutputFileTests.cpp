#include "LogString.h"
#include "assert-utility.h"
#include <recognition-test/OutputFileImpl.hpp>
#include <gtest/gtest.h>

namespace {
    class WriterStub : public av_speech_in_noise::Writer {
        LogString written_{};
        std::string filePath_{};
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
    
    class OutputFilePathStub :
        public av_speech_in_noise::OutputFilePath
    {
        av_speech_in_noise::TestInformation testParameters_{};
        std::string fileName_{};
        std::string homeDirectory_{};
        std::string outputDirectory_{};
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
        
        std::string generateFileName(
            const av_speech_in_noise::TestInformation &p
        ) override {
            testParameters_ = p;
            return fileName_;
        }
        
        std::string homeDirectory() override {
            return homeDirectory_;
        }
        
        auto &testParameters() const {
            return testParameters_;
        }
    };

    class OutputFileTests : public ::testing::Test {
    protected:
        WriterStub writer;
        OutputFilePathStub path;
        av_speech_in_noise::OutputFileImpl file{&writer, &path};
        av_speech_in_noise::coordinate_response_measure::Trial trial{};
        av_speech_in_noise::AdaptiveTest adaptiveTest{};
        
        void openNewFile() {
            file.openNewFile(adaptiveTest.information);
        }
        
        void assertWriterContainsConditionName(
            av_speech_in_noise::Condition c
        ) {
            adaptiveTest.condition = c;
            file.writeTest(adaptiveTest);
            std::string name = conditionName(c);
            EXPECT_TRUE(writer.written().contains("condition: " + name + "\n"));
        }
    };

    TEST_F(OutputFileTests, writeTrial) {
        trial.SNR_dB = 1;
        trial.correctNumber = 2;
        trial.subjectNumber = 3;
        trial.correctColor = av_speech_in_noise::coordinate_response_measure::Color::green;
        trial.subjectColor = av_speech_in_noise::coordinate_response_measure::Color::red;
        trial.reversals = 4;
        trial.correct = false;
        file.writeTrial(trial);
        assertEqual(
            "1, 2, 3, green, red, incorrect, 4\n",
            writer.written()
        );
    }

    TEST_F(OutputFileTests, writeTrialIncorrect) {
        trial.correct = false;
        file.writeTrial(trial);
        EXPECT_TRUE(writer.written().contains(" incorrect, "));
    }

    TEST_F(OutputFileTests, writeTrialCorrect) {
        trial.correct = true;
        file.writeTrial(trial);
        EXPECT_TRUE(writer.written().contains(" correct, "));
    }

    TEST_F(OutputFileTests, writeTest) {
        adaptiveTest.maskerFilePath = "a";
        adaptiveTest.information.session = "b";
        adaptiveTest.information.subjectId = "c";
        adaptiveTest.targetListDirectory = "d";
        adaptiveTest.information.testerId = "e";
        adaptiveTest.maskerLevel_dB_SPL = 1;
        adaptiveTest.startingSnr_dB = 2;
        file.writeTest(adaptiveTest);
        EXPECT_TRUE(writer.written().contains("subject: c\n"));
        EXPECT_TRUE(writer.written().contains("tester: e\n"));
        EXPECT_TRUE(writer.written().contains("session: b\n"));
        EXPECT_TRUE(writer.written().contains("masker: a\n"));
        EXPECT_TRUE(writer.written().contains("targets: d\n"));
        EXPECT_TRUE(writer.written().contains("masker level (dB SPL): 1\n"));
        EXPECT_TRUE(writer.written().contains("starting SNR (dB): 2\n"));
        EXPECT_TRUE(writer.written().endsWith("\n\n"));
    }

    TEST_F(OutputFileTests, writeTestWithAvCondition) {
        assertWriterContainsConditionName(
            av_speech_in_noise::Condition::audioVisual
        );
    }

    TEST_F(OutputFileTests, writeTestWithAuditoryOnlyCondition) {
        assertWriterContainsConditionName(
            av_speech_in_noise::Condition::auditoryOnly
        );
    }

    TEST_F(OutputFileTests, writeTrialHeading) {
        file.writeCoordinateResponseTrialHeading();
        assertEqual(
            "SNR (dB), correct number, subject number, "
            "correct color, subject color, evaluation, reversals\n",
            writer.written()
        );
    }

    TEST_F(OutputFileTests, colorNameUninitializedColorDefined) {
        av_speech_in_noise::coordinate_response_measure::Trial uninitialized;
        file.writeTrial(uninitialized);
    }

    TEST_F(
        OutputFileTests,
        openPassesFormattedFilePath
    ) {
        path.setFileName("a");
        path.setOutputDirectory("b");
        openNewFile();
        assertEqual("b/a.txt", writer.filePath());
    }

    TEST_F(
        OutputFileTests,
        closeClosesWriter
    ) {
        file.close();
        EXPECT_TRUE(writer.closed());
    }

    TEST_F(
        OutputFileTests,
        openPassesTestParameters
    ) {
        adaptiveTest.information.testerId = "a";
        openNewFile();
        assertEqual("a", path.testParameters().testerId);
    }
    
    class FailingWriter : public av_speech_in_noise::Writer {
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
        FailingWriter writer{};
        OutputFilePathStub path{};
        av_speech_in_noise::OutputFileImpl file{&writer, &path};
        try {
            file.openNewFile({});
            FAIL() << "Expected OutputFileImpl::OpenFailure";
        }
        catch (
            const av_speech_in_noise::
            OutputFileImpl::
            OpenFailure &
        ) {
        }
    }
}
