#include <recognition-test/RecognitionTestModel.hpp>

class ResponseEvaluatorImpl :
    public av_coordinate_response_measure::ResponseEvaluator
{
public:
    bool correct(
        std::string filePath,
        const av_coordinate_response_measure::SubjectResponse &
    ) override {
        return true;
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
}
