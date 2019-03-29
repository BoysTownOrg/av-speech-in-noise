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

    class OutputFileTests : public ::testing::Test {
    protected:
        WriterStub writer{};
        recognition_test::OutputFileImpl file{&writer};
        av_coordinated_response_measure::Trial trial{};
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
        openPassesFilePath
    ) {
        file.open("a");
        assertEqual("a", writer.filePath());
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
        recognition_test::OutputFileImpl file{&writer};
        try {
            file.open({});
            FAIL() << "Expected OutputFileImpl::OpenFailure";
        }
        catch (const recognition_test::OutputFileImpl::OpenFailure &) {
        }
    }
}
