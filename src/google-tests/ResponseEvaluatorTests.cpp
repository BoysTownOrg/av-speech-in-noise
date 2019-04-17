#include <recognition-test/ResponseEvaluatorImpl.hpp>
#include <gtest/gtest.h>

namespace {
    class ResponseEvaluatorTests : public ::testing::Test {
    protected:
        av_coordinate_response_measure::ResponseEvaluatorImpl evaluator{};
    };
    
    TEST_F(ResponseEvaluatorTests, blue) {
        EXPECT_TRUE(evaluator.correct(
            "blue1.mov",
            {
                1,
                av_coordinate_response_measure::Color::blue
            }
        ));
    }
    
    TEST_F(ResponseEvaluatorTests, red) {
        EXPECT_TRUE(evaluator.correct(
            "red1.mov",
            {
                1,
                av_coordinate_response_measure::Color::red
            }
        ));
    }
    
    TEST_F(ResponseEvaluatorTests, white) {
        EXPECT_TRUE(evaluator.correct(
            "white1.mov",
            {
                1,
                av_coordinate_response_measure::Color::white
            }
        ));
    }
    
    TEST_F(ResponseEvaluatorTests, green) {
        EXPECT_TRUE(evaluator.correct(
            "green1.mov",
            {
                1,
                av_coordinate_response_measure::Color::green
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
    
    TEST_F(ResponseEvaluatorTests, rightNumberWrongColor) {
        EXPECT_FALSE(evaluator.correct(
            "green1.mov",
            {
                1,
                av_coordinate_response_measure::Color::blue
            }
        ));
    }
    
    TEST_F(ResponseEvaluatorTests, wrongNumberWrongColor) {
        EXPECT_FALSE(evaluator.correct(
            "green2.mov",
            {
                1,
                av_coordinate_response_measure::Color::blue
            }
        ));
    }
    
    TEST_F(ResponseEvaluatorTests, ignoresLeadingPath) {
        EXPECT_TRUE(evaluator.correct(
            "a/green1.mov",
            {
                1,
                av_coordinate_response_measure::Color::green
            }
        ));
    }
}
