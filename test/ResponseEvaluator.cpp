#include "assert-utility.hpp"
#include <recognition-test/ResponseEvaluator.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::coordinate_response_measure {
namespace {
class CoordinateResponseEvaluatorTests : public ::testing::Test {
  protected:
    ResponseEvaluatorImpl evaluator{};

    auto correct(const std::string &s, Response r) -> bool {
        return evaluator.correct({s}, r);
    }

    void assertCorrect(const std::string &s, Response r) {
        EXPECT_TRUE(correct(s, r));
    }

    void assertIncorrect(const std::string &s, Response r) {
        EXPECT_FALSE(correct(s, r));
    }
};

#define COORDINATE_RESPONSE_EVALUATOR_TEST(a)                                  \
    TEST_F(CoordinateResponseEvaluatorTests, a)

COORDINATE_RESPONSE_EVALUATOR_TEST(blue) {
    assertCorrect("blue1.mov", {1, Color::blue});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(red) {
    assertCorrect("red1.mov", {1, Color::red});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(white) {
    assertCorrect("white1.mov", {1, Color::white});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(green) {
    assertCorrect("green1.mov", {1, Color::green});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(rightColorWrongNumber) {
    assertIncorrect("blue2.mov", {1, Color::blue});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(rightNumberWrongColor) {
    assertIncorrect("green1.mov", {1, Color::blue});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(wrongNumberWrongColor) {
    assertIncorrect("green2.mov", {1, Color::blue});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(ignoresLeadingPath) {
    assertCorrect("a/green1.mov", {1, Color::green});
    assertCorrect("a/b/green1.mov", {1, Color::green});
    assertIncorrect("a/red1.mov", {1, Color::green});
    assertIncorrect("a/b/red1.mov", {1, Color::green});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(invalidFormatIsAlwaysIncorrect) {
    assertIncorrect("not-valid", {1, Color::blue});
    assertIncorrect(
        "not-valid", {ResponseEvaluatorImpl::invalidNumber, Color::unknown});
    assertIncorrect(
        "almost.mov", {ResponseEvaluatorImpl::invalidNumber, Color::unknown});
    assertIncorrect(
        "white$.mov", {ResponseEvaluatorImpl::invalidNumber, Color::white});
    assertIncorrect(
        "/", {ResponseEvaluatorImpl::invalidNumber, Color::unknown});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(fileNameReturnsEverythingAfterFinalSlash) {
    assertEqual("b", evaluator.fileName({"a/b"}));
    assertEqual("a", evaluator.fileName({"a"}));
    assertEqual("c.txt", evaluator.fileName({"a/b/c.txt"}));
}

COORDINATE_RESPONSE_EVALUATOR_TEST(onlyEvaluatesFirstPartOfFileName) {
    assertCorrect("blue2_3.mov", {2, Color::blue});
    assertCorrect("a/blue2_3.mov", {2, Color::blue});
    assertIncorrect("blue2_3.mov", {3, Color::blue});
    assertIncorrect("a/blue2_3.mov", {3, Color::blue});
}

COORDINATE_RESPONSE_EVALUATOR_TEST(miscellaneous) {
    assertCorrect("a/b/c/blue9-3.mov", {9, Color::blue});
    assertCorrect("a/b/c/red8 4.mov", {8, Color::red});
    assertIncorrect("a/b/c/blue9-3.mov", {3, Color::blue});
    assertIncorrect("a/b/c/red8 4.mov", {4, Color::red});
}
}
}

namespace av_speech_in_noise {
namespace {
class ConsonantResponseEvaluatorTests : public ::testing::Test {
  protected:
    ResponseEvaluatorImpl evaluator{};

    auto correct(const std::string &s, ConsonantResponse r) -> bool {
        return evaluator.correct({s}, r);
    }

    void assertCorrect(const std::string &s, char r) {
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(correct(s, {r}));
    }

    void assertIncorrect(const std::string &s, char r) {
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(correct(s, {r}));
    }
};

#define CONSONANT_RESPONSE_EVALUATOR_TEST(a)                                   \
    TEST_F(ConsonantResponseEvaluatorTests, a)

CONSONANT_RESPONSE_EVALUATOR_TEST(b) { assertCorrect("b.wav", 'b'); }

CONSONANT_RESPONSE_EVALUATOR_TEST(notB) { assertIncorrect("a.wav", 'b'); }

CONSONANT_RESPONSE_EVALUATOR_TEST(invalidFormatIsAlwaysIncorrect) {
    assertIncorrect("bwav", 'b');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(parsesConsonant) {
    assertEqual('b', evaluator.correctConsonant({"b.wav"}));
}
}
}
