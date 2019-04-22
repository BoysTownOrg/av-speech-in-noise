#include <recognition-test/ResponseEvaluatorImpl.hpp>
#include <gtest/gtest.h>

namespace {
    class ResponseEvaluatorTests : public ::testing::Test {
    protected:
        av_coordinate_response_measure::ResponseEvaluatorImpl evaluator{};
        
        bool correct(
            std::string s,
            av_coordinate_response_measure::SubjectResponse r
        ) {
            return evaluator.correct(std::move(s), r);
        }
        
        void assertCorrect(
            std::string s,
            av_coordinate_response_measure::SubjectResponse r
        ) {
            EXPECT_TRUE(correct(std::move(s), r));
        }
        
        void assertIncorrect(
            std::string s,
            av_coordinate_response_measure::SubjectResponse r
        ) {
            EXPECT_FALSE(correct(std::move(s), r));
        }
    };
    
    TEST_F(ResponseEvaluatorTests, blue) {
        assertCorrect(
            "blue1.mov",
            {
                1,
                av_coordinate_response_measure::Color::blue
            }
        );
    }
    
    TEST_F(ResponseEvaluatorTests, red) {
        assertCorrect(
            "red1.mov",
            {
                1,
                av_coordinate_response_measure::Color::red
            }
        );
    }
    
    TEST_F(ResponseEvaluatorTests, white) {
        assertCorrect(
            "white1.mov",
            {
                1,
                av_coordinate_response_measure::Color::white
            }
        );
    }
    
    TEST_F(ResponseEvaluatorTests, green) {
        assertCorrect(
            "green1.mov",
            {
                1,
                av_coordinate_response_measure::Color::green
            }
        );
    }
    
    TEST_F(ResponseEvaluatorTests, rightColorWrongNumber) {
        assertIncorrect(
            "blue2.mov",
            {
                1,
                av_coordinate_response_measure::Color::blue
            }
        );
    }
    
    TEST_F(ResponseEvaluatorTests, rightNumberWrongColor) {
        assertIncorrect(
            "green1.mov",
            {
                1,
                av_coordinate_response_measure::Color::blue
            }
        );
    }
    
    TEST_F(ResponseEvaluatorTests, wrongNumberWrongColor) {
        assertIncorrect(
            "green2.mov",
            {
                1,
                av_coordinate_response_measure::Color::blue
            }
        );
    }
    
    TEST_F(ResponseEvaluatorTests, ignoresLeadingPath) {
        assertCorrect(
            "a/green1.mov",
            {
                1,
                av_coordinate_response_measure::Color::green
            }
        );
        assertCorrect(
            "a/b/green1.mov",
            {
                1,
                av_coordinate_response_measure::Color::green
            }
        );
        assertIncorrect(
            "a/red1.mov",
            {
                1,
                av_coordinate_response_measure::Color::green
            }
        );
        assertIncorrect(
            "a/b/red1.mov",
            {
                1,
                av_coordinate_response_measure::Color::green
            }
        );
    }
    
    TEST_F(ResponseEvaluatorTests, invalidFormatIsAlwaysIncorrect) {
        assertIncorrect(
            "not-valid",
            {
                1,
                av_coordinate_response_measure::Color::blue
            }
        );
        assertIncorrect(
            "almost.mov",
            {
                -1,
                av_coordinate_response_measure::Color::white
            }
        );
    }
}
