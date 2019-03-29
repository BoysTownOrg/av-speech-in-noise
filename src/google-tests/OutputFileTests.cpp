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
/*
    TEST(
        TestDocumenterWithInitializationFailingWriter,
        initializeTestThrowsInitializationFailureWhenDocumenterFailsToInitialize
    ) {
        InitializationFailingWriter writer{};
        writer.setErrorMessage("error.");
        TestDocumenterImpl documenter{ &writer };
        try {
            documenter.initialize({});
            FAIL() << "Expected TestDocumenter::InitializationFailure";
        }
        catch (const TestDocumenter::InitializationFailure &e) {
            assertEqual(std::string{ "error." }, e.what());
        }
    }
    */
}
