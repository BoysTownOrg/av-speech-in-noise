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
    SyllableResponse response;
    response.syllable = r;
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(correct(evaluator, s, response));
}

static void assertCorrect(
    ResponseEvaluatorImpl &evaluator, const std::string &s, Syllable r) {
    SyllableResponse response;
    response.syllable = r;
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(correct(evaluator, s, response));
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

SYLLABLE_RESPONSE_EVALUATOR_TEST(di) {
    assertCorrect(evaluator, "say_di_3-25.mov", Syllable::di);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(dji) {
    assertCorrect(evaluator, "say_dji_3-25.mov", Syllable::dji);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(fi) {
    assertCorrect(evaluator, "say_fi_2-25ao.mov", Syllable::fi);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(gi) {
    assertCorrect(evaluator, "say_gi_1-25ao.mov", Syllable::gi);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(hi) {
    assertCorrect(evaluator, "say_hi_2-25ao.mov", Syllable::hi);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(ji) {
    assertCorrect(evaluator, "say_ji_2-25.mov", Syllable::ji);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(ki) {
    assertCorrect(evaluator, "say_ki_3-25.mov", Syllable::ki);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(li) {
    assertCorrect(evaluator, "say_li_1-25.mov", Syllable::li);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(mi) {
    assertCorrect(evaluator, "say_mi_1-25ao.mov", Syllable::mi);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(ni) {
    assertCorrect(evaluator, "say_ni_2-25ao.mov", Syllable::ni);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(pi) {
    assertCorrect(evaluator, "say_pi_2-25ao.mov", Syllable::pi);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(ri) {
    assertCorrect(evaluator, "say_ri_2-25.mov", Syllable::ri);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(shi) {
    assertCorrect(evaluator, "say_shi_1-25.mov", Syllable::shi);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(si) {
    assertCorrect(evaluator, "say_si_3-25.mov", Syllable::si);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(thi) {
    assertCorrect(evaluator, "say_thi_3-25ao.mov", Syllable::thi);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(ti) {
    assertCorrect(evaluator, "say_ti_1-25ao.mov", Syllable::ti);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(tsi) {
    assertCorrect(evaluator, "say_tsi_1-25ao.mov", Syllable::tsi);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(vi) {
    assertCorrect(evaluator, "say_vi_2-25ao.mov", Syllable::vi);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(wi) {
    assertCorrect(evaluator, "say_wi_1-25ao.mov", Syllable::wi);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(zi) {
    assertCorrect(evaluator, "say_zi_1-25.mov", Syllable::zi);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(biAuditoryOnly) {
    assertCorrect(evaluator, "say_bi_2-25ao.mov", Syllable::bi);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(biIncorrect) {
    assertIncorrect(evaluator, "say_tsi_2-25.mov", Syllable::bi);
}

SYLLABLE_RESPONSE_EVALUATOR_TEST(withParentPath) {
    assertCorrect(evaluator, "/Users/bob/say_ti_2-25.mov", Syllable::ti);
}

CONSONANT_RESPONSE_EVALUATOR_TEST(parsesSyllable) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Syllable::fi, evaluator.correctSyllable(LocalUrl{"say_fi_3-25ao.mov"}));
}
}
