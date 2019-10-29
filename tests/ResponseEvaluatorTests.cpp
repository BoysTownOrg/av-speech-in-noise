#include "assert-utility.h"
#include <gtest/gtest.h>
#include <recognition-test/ResponseEvaluator.hpp>

namespace av_speech_in_noise::tests::response_evaluation {
using coordinate_response_measure::Color;
using coordinate_response_measure::Response;

class ResponseEvaluatorTests : public ::testing::Test {
  protected:
    ResponseEvaluatorImpl evaluator{};

    auto correct(const std::string &s, Response r) -> bool {
        return evaluator.correct(s, r);
    }

    void assertCorrect(const std::string &s, Response r) {
        EXPECT_TRUE(correct(s, r));
    }

    void assertIncorrect(const std::string &s, Response r) {
        EXPECT_FALSE(correct(s, r));
    }
};

TEST_F(ResponseEvaluatorTests, blue) {
    assertCorrect("blue1.mov", {1, Color::blue});
}

TEST_F(ResponseEvaluatorTests, red) {
    assertCorrect("red1.mov", {1, Color::red});
}

TEST_F(ResponseEvaluatorTests, white) {
    assertCorrect("white1.mov", {1, Color::white});
}

TEST_F(ResponseEvaluatorTests, green) {
    assertCorrect("green1.mov", {1, Color::green});
}

TEST_F(ResponseEvaluatorTests, rightColorWrongNumber) {
    assertIncorrect("blue2.mov", {1, Color::blue});
}

TEST_F(ResponseEvaluatorTests, rightNumberWrongColor) {
    assertIncorrect("green1.mov", {1, Color::blue});
}

TEST_F(ResponseEvaluatorTests, wrongNumberWrongColor) {
    assertIncorrect("green2.mov", {1, Color::blue});
}

TEST_F(ResponseEvaluatorTests, ignoresLeadingPath) {
    assertCorrect("a/green1.mov", {1, Color::green});
    assertCorrect("a/b/green1.mov", {1, Color::green});
    assertIncorrect("a/red1.mov", {1, Color::green});
    assertIncorrect("a/b/red1.mov", {1, Color::green});
}

TEST_F(ResponseEvaluatorTests, invalidFormatIsAlwaysIncorrect) {
    assertIncorrect("not-valid", {1, Color::blue});
    assertIncorrect(
        "not-valid", {ResponseEvaluatorImpl::invalidNumber, Color::notAColor});
    assertIncorrect(
        "almost.mov", {ResponseEvaluatorImpl::invalidNumber, Color::notAColor});
    assertIncorrect(
        "white$.mov", {ResponseEvaluatorImpl::invalidNumber, Color::white});
    assertIncorrect(
        "/", {ResponseEvaluatorImpl::invalidNumber, Color::notAColor});
}

TEST_F(ResponseEvaluatorTests, fileNameReturnsEverythingAfterFinalSlash) {
    assertEqual("b", evaluator.fileName("a/b"));
    assertEqual("a", evaluator.fileName("a"));
    assertEqual("c.txt", evaluator.fileName("a/b/c.txt"));
}

TEST_F(ResponseEvaluatorTests, onlyEvaluatesFirstPartOfFileName) {
    assertCorrect("blue2_3.mov", {2, Color::blue});
    assertCorrect("a/blue2_3.mov", {2, Color::blue});
    assertIncorrect("blue2_3.mov", {3, Color::blue});
    assertIncorrect("a/blue2_3.mov", {3, Color::blue});
}

TEST_F(ResponseEvaluatorTests, miscellaneous) {
    assertCorrect("a/b/c/blue9-3.mov", {9, Color::blue});
    assertCorrect("a/b/c/red8 4.mov", {8, Color::red});
    assertIncorrect("a/b/c/blue9-3.mov", {3, Color::blue});
    assertIncorrect("a/b/c/red8 4.mov", {4, Color::red});
}
}
