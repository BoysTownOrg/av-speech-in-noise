#include <recognition-test/RecognitionTestModel.hpp>

class ResponseEvaluatorImpl :
    public av_coordinate_response_measure::ResponseEvaluator
{
public:
    bool correct(
        std::string filePath,
        const av_coordinate_response_measure::SubjectResponse &r
    ) override {
        auto found = filePath.find(".");
        auto correctNumber = std::stoi(filePath.substr(found-1, 1));
        return correctNumber == r.number;
    }
};

#include <gtest/gtest.h>

namespace {
    class ResponseEvaluatorTests : public ::testing::Test {
    protected:
        ResponseEvaluatorImpl evaluator{};
    };
    
    TEST_F(ResponseEvaluatorTests, tbd) {
        EXPECT_TRUE(evaluator.correct(
            "blue1.mov",
            {
                1,
                av_coordinate_response_measure::Color::blue
            }
        ));
    }
    
    TEST_F(ResponseEvaluatorTests, rightColorWrongNumber) {
        EXPECT_FALSE(evaluator.correct(
            "blue2.mov",
            {
                1,
                av_coordinate_response_measure::Color::blue
            }
        ));
    }
}
