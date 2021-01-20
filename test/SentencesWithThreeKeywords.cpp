#include <presentation/ChooseKeywords.hpp>
#include <map>
#include <string_view>
#include <sstream>

namespace av_speech_in_noise {
auto sentencesWithThreeKeywords(std::string_view s)
    -> std::map<std::string, SentenceWithThreeKeywords> {
    std::map<std::string, SentenceWithThreeKeywords> map;
    std::stringstream stream{std::string{s}};
    while (!stream.eof()) {
        std::string maybeFileName;
        std::getline(stream, maybeFileName);
        if (maybeFileName.empty())
            continue;
        SentenceWithThreeKeywords sentenceWithThreeKeywords;
        std::getline(stream, sentenceWithThreeKeywords.sentence);
        std::getline(stream, sentenceWithThreeKeywords.firstKeyword);
        std::getline(stream, sentenceWithThreeKeywords.secondKeyword);
        std::getline(stream, sentenceWithThreeKeywords.thirdKeyword);
        map[maybeFileName] = sentenceWithThreeKeywords;
    }
    return map;
}

static auto operator==(const SentenceWithThreeKeywords &a,
    const SentenceWithThreeKeywords &b) -> bool {
    return a.sentence == b.sentence && a.firstKeyword == b.firstKeyword &&
        a.secondKeyword == b.secondKeyword && a.thirdKeyword == b.thirdKeyword;
}
}

#include "assert-utility.hpp"
#include <gtest/gtest.h>

namespace av_speech_in_noise {
namespace {

class SentencesWithThreeKeywordsTests : public ::testing::Test {
  protected:
};

TEST_F(SentencesWithThreeKeywordsTests, a) {
    const auto actual{sentencesWithThreeKeywords(
        R"(
filenameA.wav
check this out it has lots of words.
check
has
words

filenameB.wav
you have no idea what this can do?
have
no
can

filenameC.wav
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
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(expectedA, actual.at("filenameA.wav"));
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(expectedB, actual.at("filenameB.wav"));
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(expectedC, actual.at("filenameC.wav"));
}
}
}
