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
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"b"}, evaluator.fileName({"a/b"}));
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, evaluator.fileName({"a"}));
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"c.txt"}, evaluator.fileName({"a/b/c.txt"}));
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

CONSONANT_RESPONSE_EVALUATOR_TEST(b) {
    assertCorrect("choose_bi_1-25_Communicator.mp4", 'b');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(c) {
    assertCorrect("choose_si_2-25_FabricMask.mp4", 'c');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(d) {
    assertCorrect("choose_di_2-25_Communicator.mp4", 'd');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(h) {
    assertCorrect("choose_hi_1-25_Clear.mp4", 'h');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(k) {
    assertCorrect("choose_ki_2-25_FabricMask.mp4", 'k');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(m) {
    assertCorrect("choose_mi_1-25_HospitalMask.mp4", 'm');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(n) {
    assertCorrect("choose_ni_3-25_HospitalMask.mp4", 'n');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(p) {
    assertCorrect("choose_pi_2-25_NoMask.mp4", 'p');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(s) {
    assertCorrect("choose_shi_2-25_Clear.mp4", 's');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(t) {
    assertCorrect("choose_ti_3-25_Communicator.mp4", 't');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(v) {
    assertCorrect("choose_vi_3-25_FabricMask.mp4", 'v');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(z) {
    assertCorrect("choose_zi_3-25_NoMask.mp4", 'z');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(notB) { assertIncorrect("a.wav", 'b'); }

CONSONANT_RESPONSE_EVALUATOR_TEST(invalidFormatIsAlwaysIncorrect) {
    assertIncorrect("bwav", 'b');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(parsesConsonant) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL('b', evaluator.correctConsonant({"b.wav"}));
}
}
}
