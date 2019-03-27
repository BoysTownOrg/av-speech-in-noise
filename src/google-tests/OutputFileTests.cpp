#include "assert-utility.h"
#include <recognition-test/OutputFileImpl.hpp>
#include <gtest/gtest.h>

class WriterStub : public recognition_test::Writer {
    std::string written_{};
public:
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
