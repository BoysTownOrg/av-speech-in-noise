#include "assert-utility.h"
#include <recognition-test/ResponseEvaluatorImpl.hpp>
#include <gtest/gtest.h>

namespace {
    using namespace av_speech_in_noise;
    using namespace av_speech_in_noise::coordinate_response_measure;
    
    class ResponseEvaluatorTests : public ::testing::Test {
    protected:
        ResponseEvaluatorImpl evaluator{};
        
        bool correct(std::string s, SubjectResponse r) {
            return evaluator.correct(std::move(s), r);
        }
        
        void assertCorrect(std::string s, SubjectResponse r) {
            EXPECT_TRUE(correct(std::move(s), r));
        }
        
        void assertIncorrect(std::string s, SubjectResponse r) {
            EXPECT_FALSE(correct(std::move(s), r));
        }
    };
    
    TEST_F(ResponseEvaluatorTests, blue) {
        assertCorrect("blue1.mov", { 1, Color::blue });
    }
    
    TEST_F(ResponseEvaluatorTests, red) {
        assertCorrect("red1.mov", { 1, Color::red });
    }
    
    TEST_F(ResponseEvaluatorTests, white) {
        assertCorrect("white1.mov", { 1, Color::white });
    }
    
    TEST_F(ResponseEvaluatorTests, green) {
        assertCorrect("green1.mov", { 1, Color::green });
    }
    
    TEST_F(ResponseEvaluatorTests, rightColorWrongNumber) {
        assertIncorrect("blue2.mov", { 1, Color::blue });
    }
    
    TEST_F(ResponseEvaluatorTests, rightNumberWrongColor) {
        assertIncorrect("green1.mov", { 1, Color::blue });
    }
    
    TEST_F(ResponseEvaluatorTests, wrongNumberWrongColor) {
        assertIncorrect("green2.mov", { 1, Color::blue });
    }
    
    TEST_F(ResponseEvaluatorTests, ignoresLeadingPath) {
        assertCorrect("a/green1.mov", { 1, Color::green });
        assertCorrect("a/b/green1.mov", { 1, Color::green });
        assertIncorrect("a/red1.mov", { 1, Color::green });
        assertIncorrect("a/b/red1.mov", { 1, Color::green });
    }
    
    TEST_F(ResponseEvaluatorTests, invalidFormatIsAlwaysIncorrect) {
        assertIncorrect("not-valid", { 1, Color::blue });
        assertIncorrect(
            "not-valid",
            {
                ResponseEvaluatorImpl::invalidNumber,
                Color::notAColor
            }
        );
        assertIncorrect(
            "almost.mov",
            {
                ResponseEvaluatorImpl::invalidNumber,
                Color::notAColor
            }
        );
        assertIncorrect(
            "white$.mov",
            {
                ResponseEvaluatorImpl::invalidNumber,
                Color::white
            }
        );
        assertIncorrect(
            "/",
            {
                ResponseEvaluatorImpl::invalidNumber,
                Color::notAColor
            }
        );
    }
    
    TEST_F(ResponseEvaluatorTests, fileNameReturnsEverythingAfterFinalSlash) {
        assertEqual("b", evaluator.fileName("a/b"));
        assertEqual("a", evaluator.fileName("a"));
    }
}
