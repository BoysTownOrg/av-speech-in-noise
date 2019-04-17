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
        auto correctColor_ = correctColor(filePath);
        return
            correctNumber == r.number &&
            correctColor_ == r.color;
    }
    
    av_coordinate_response_measure::Color correctColor(std::string filePath) {
        auto fileSeparator = filePath.find_last_of("/");
        auto extension = filePath.find(".");
        auto colorName = filePath.substr(fileSeparator+1, extension-1-fileSeparator-1);
        if (colorName == "green")
            return av_coordinate_response_measure::Color::green;
        else
            return av_coordinate_response_measure::Color::blue;
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
