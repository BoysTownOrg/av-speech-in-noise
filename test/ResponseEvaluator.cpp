#include "assert-utility.hpp"

#include <av-speech-in-noise/core/ResponseEvaluator.hpp>

#include <gtest/gtest.h>

#include <string_view>

namespace av_speech_in_noise::coordinate_response_measure {
static auto correct(
    ResponseEvaluatorImpl &evaluator, std::string_view s, Response r) -> bool {
    return evaluator.correct(LocalUrl{std::string{s}}, r);
}

static void assertIncorrect(
    ResponseEvaluatorImpl &evaluator, const std::string &s, Response r) {
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(correct(evaluator, s, r));
}

static void assertCorrect(
    ResponseEvaluatorImpl &evaluator, const std::string &s, Response r) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(correct(evaluator, s, r));
}

namespace {
class CoordinateResponseEvaluatorTests : public ::testing::Test {
  protected:
    ResponseEvaluatorImpl evaluator{};
};

#define COORDINATE_RESPONSE_EVALUATOR_TEST(a)                                  \
    TEST_F(CoordinateResponseEvaluatorTests, a)

COORDINATE_RESPONSE_EVALUATOR_TEST(blue) {
    assertCorrect(evaluator, "blue1.mov", {1, Color::blue});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(red) {
    assertCorrect(evaluator, "red1.mov", {1, Color::red});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(white) {
    assertCorrect(evaluator, "white1.mov", {1, Color::white});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(green) {
    assertCorrect(evaluator, "green1.mov", {1, Color::green});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(rightColorWrongNumber) {
    assertIncorrect(evaluator, "blue2.mov", {1, Color::blue});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(rightNumberWrongColor) {
    assertIncorrect(evaluator, "green1.mov", {1, Color::blue});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(wrongNumberWrongColor) {
    assertIncorrect(evaluator, "green2.mov", {1, Color::blue});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(ignoresLeadingPath) {
    assertCorrect(evaluator, "a/green1.mov", {1, Color::green});
    assertCorrect(evaluator, "a/b/green1.mov", {1, Color::green});
    assertIncorrect(evaluator, "a/red1.mov", {1, Color::green});
    assertIncorrect(evaluator, "a/b/red1.mov", {1, Color::green});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(invalidFormatIsAlwaysIncorrect) {
    assertIncorrect(evaluator, "not-valid", {1, Color::blue});
    assertIncorrect(evaluator, "not-valid",
        {ResponseEvaluatorImpl::invalidNumber, Color::unknown});
    assertIncorrect(evaluator, "almost.mov",
        {ResponseEvaluatorImpl::invalidNumber, Color::unknown});
    assertIncorrect(evaluator, "white$.mov",
        {ResponseEvaluatorImpl::invalidNumber, Color::white});
    assertIncorrect(
        evaluator, "/", {ResponseEvaluatorImpl::invalidNumber, Color::unknown});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(fileNameReturnsEverythingAfterFinalSlash) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"b"}, evaluator.fileName({"a/b"}));
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, evaluator.fileName({"a"}));
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"c.txt"}, evaluator.fileName({"a/b/c.txt"}));
}

COORDINATE_RESPONSE_EVALUATOR_TEST(onlyEvaluatesFirstPartOfFileName) {
    assertCorrect(evaluator, "blue2_3.mov", {2, Color::blue});
    assertCorrect(evaluator, "a/blue2_3.mov", {2, Color::blue});
    assertIncorrect(evaluator, "blue2_3.mov", {3, Color::blue});
    assertIncorrect(evaluator, "a/blue2_3.mov", {3, Color::blue});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(miscellaneous) {
    assertCorrect(evaluator, "a/b/c/blue9-3.mov", {9, Color::blue});
    assertCorrect(evaluator, "a/b/c/red8 4.mov", {8, Color::red});
    assertIncorrect(evaluator, "a/b/c/blue9-3.mov", {3, Color::blue});
    assertIncorrect(evaluator, "a/b/c/red8 4.mov", {4, Color::red});
}
}
}
