#include "assert-utility.hpp"
#include <recognition-test/ResponseEvaluator.hpp>
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

namespace av_speech_in_noise {
static auto correct(ResponseEvaluatorImpl &evaluator, std::string_view s,
    ConsonantResponse r) -> bool {
    return evaluator.correct(LocalUrl{std::string{s}}, r);
}

static void assertIncorrect(
    ResponseEvaluatorImpl &evaluator, const std::string &s, char r) {
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(
        correct(evaluator, s, ConsonantResponse{r}));
}

static void assertCorrect(
    ResponseEvaluatorImpl &evaluator, const std::string &s, char r) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(correct(evaluator, s, ConsonantResponse{r}));
}

static auto correct(ResponseEvaluatorImpl &evaluator, std::string_view s,
    SyllableResponse r) -> bool {
    return evaluator.correct(LocalUrl{std::string{s}}, r);
}

static void assertIncorrect(
    ResponseEvaluatorImpl &evaluator, const std::string &s, Syllable r) {
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(correct(evaluator, s, SyllableResponse{r}));
}

static void assertCorrect(
    ResponseEvaluatorImpl &evaluator, const std::string &s, Syllable r) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(correct(evaluator, s, SyllableResponse{r}));
}

namespace {
class ConsonantResponseEvaluatorTests : public ::testing::Test {
  protected:
    ResponseEvaluatorImpl evaluator{};
};

#define CONSONANT_RESPONSE_EVALUATOR_TEST(a)                                   \
    TEST_F(ConsonantResponseEvaluatorTests, a)

CONSONANT_RESPONSE_EVALUATOR_TEST(b) {
    assertCorrect(evaluator, "choose_bi_1-25_Communicator.mp4", 'b');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(c) {
    assertCorrect(evaluator, "choose_si_2-25_FabricMask.mp4", 'c');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(d) {
    assertCorrect(evaluator, "choose_di_2-25_Communicator.mp4", 'd');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(h) {
    assertCorrect(evaluator, "choose_hi_1-25_Clear.mp4", 'h');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(k) {
    assertCorrect(evaluator, "choose_ki_2-25_FabricMask.mp4", 'k');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(m) {
    assertCorrect(evaluator, "choose_mi_1-25_HospitalMask.mp4", 'm');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(n) {
    assertCorrect(evaluator, "choose_ni_3-25_HospitalMask.mp4", 'n');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(p) {
    assertCorrect(evaluator, "choose_pi_2-25_NoMask.mp4", 'p');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(s) {
    assertCorrect(evaluator, "choose_shi_2-25_Clear.mp4", 's');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(t) {
    assertCorrect(evaluator, "choose_ti_3-25_Communicator.mp4", 't');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(v) {
    assertCorrect(evaluator, "choose_vi_3-25_FabricMask.mp4", 'v');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(z) {
    assertCorrect(evaluator, "choose_zi_3-25_NoMask.mp4", 'z');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(notB) {
    assertIncorrect(evaluator, "choose_zi_3-25_NoMask.mp4", 'b');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(invalidFormatIsAlwaysIncorrect) {
    assertIncorrect(evaluator, "idontknowb", 'b');
}

CONSONANT_RESPONSE_EVALUATOR_TEST(parsesConsonant) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        'b', evaluator.correctConsonant({"choose_bi_1-25_Communicator.mp4"}));
}

class SyllableResponseEvaluatorTests : public ::testing::Test {
  protected:
    ResponseEvaluatorImpl evaluator{};
};

#define SYLLABLE_RESPONSE_EVALUATOR_TEST(a)                                    \
    TEST_F(SyllableResponseEvaluatorTests, a)
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(bi) {
    assertCorrect(evaluator, "say_bi_1-25.mov", Syllable::bi);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(biAuditoryOnly) {
    assertCorrect(evaluator, "say_bi_2-25ao.mov", Syllable::bi);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(biIncorrect) {
    assertIncorrect(evaluator, "say_tsi_2-25.mov", Syllable::bi);
}
}
