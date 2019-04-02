#include "assert-utility.h"
#include <recognition-test/OutputFileImpl.hpp>
#include <gtest/gtest.h>

namespace {
    class WriterStub : public recognition_test::Writer {
        std::string written_{};
        std::string filePath_{};
    public:
        void open(std::string f) override {
            filePath_ = std::move(f);
        }
        
        auto filePath() const {
            return filePath_;
        }
        
        auto written() const {
            return written_;
        }
        
        void write(std::string s) override {
            written_ = std::move(s);
        }
        
        bool failed() override {
            return {};
        }
    };
    
    class OutputFilePathStub : public ::recognition_test::OutputFilePath {
        av_coordinated_response_measure::Model::Test testParameters_{};
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
            const av_coordinated_response_measure::Model::Test &p
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
        WriterStub writer{};
        OutputFilePathStub path{};
        recognition_test::OutputFileImpl file{&writer, &path};
        av_coordinated_response_measure::Trial trial{};
        av_coordinated_response_measure::Model::Test test{};
        
        void openNewFile() {
            file.openNewFile(test);
        }
    };

    TEST_F(OutputFileTests, writeTrial) {
        trial.SNR_dB = 1;
        trial.correctNumber = 2;
        trial.subjectNumber = 3;
        trial.correctColor = av_coordinated_response_measure::Color::green;
        trial.subjectColor = av_coordinated_response_measure::Color::red;
        trial.reversals = 4;
        file.writeTrial(trial);
        assertEqual(
            "1, 2, 3, green, red, 4\n",
            writer.written()
        );
    }

    TEST_F(OutputFileTests, writeTest) {
        test.maskerFilePath = "a";
        test.session = "b";
        test.subjectId = "c";
        test.targetListDirectory = "d";
        test.testerId = "e";
        test.signalLevel_dB_SPL = 1;
        test.startingSnr_dB = 2;
        file.writeTest(test);
        assertEqual(
            "subject: c\n"
            "tester: e\n"
            "session: b\n"
            "masker: a\n"
            "targets: d\n"
            "signal level (dB SPL): 1\n"
            "starting SNR (dB): 2\n\n",
            writer.written()
        );
    }

    TEST_F(OutputFileTests, writeTrialHeading) {
        file.writeTrialHeading();
        assertEqual(
            "SNR (dB), correct number, subject number, correct color, subject color, reversals\n",
            writer.written()
        );
    }

    TEST_F(OutputFileTests, colorNameUninitializedColorDefined) {
        av_coordinated_response_measure::Trial uninitialized;
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
        openPassesTestParameters
    ) {
        test.testerId = "a";
        openNewFile();
        assertEqual("a", path.testParameters().testerId);
    }
    
    class FailingWriter : public recognition_test::Writer {
        bool failed_{};
    public:
        void write(std::string) override {
        
        }
        
        void open(std::string) override {
            failed_ = true;
        }
        
        bool failed() override {
            return failed_;
        }
    };
    
    TEST(
        FailingOutputFileTests,
        openThrowsOpenFailureWhenWriterFails
    ) {
        FailingWriter writer{};
        OutputFilePathStub path{};
        recognition_test::OutputFileImpl file{&writer, &path};
        try {
            file.openNewFile({});
            FAIL() << "Expected OutputFileImpl::OpenFailure";
        }
        catch (const recognition_test::OutputFileImpl::OpenFailure &) {
        }
    }
}
