#include "assert-utility.hpp"
#include <av-speech-in-noise/ui/ChooseKeywords.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise {
namespace {

class SentencesWithThreeKeywordsTests : public ::testing::Test {
  protected:
};

TEST_F(SentencesWithThreeKeywordsTests, a) {
    const auto actual{sentencesWithThreeKeywords(
        R"(
check this out it has lots of words.
check
has
words

you have no idea what this can do?
have
no
can

nope is am are was were be being.
are
be
being
)")};
    const auto expectedA{SentenceWithThreeKeywords{
        "check this out it has lots of words.", "check", "has", "words"}};
    const auto expectedB{SentenceWithThreeKeywords{
        "you have no idea what this can do?", "have", "no", "can"}};
    const auto expectedC{SentenceWithThreeKeywords{
        "nope is am are was were be being.", "are", "be", "being"}};
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(expectedA, actual.at(0));
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(expectedB, actual.at(1));
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(expectedC, actual.at(2));
}
}
}
