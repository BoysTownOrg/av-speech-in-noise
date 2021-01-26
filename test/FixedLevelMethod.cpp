#include "LogString.hpp"
#include "OutputFileStub.hpp"
#include "ResponseEvaluatorStub.hpp"
#include "TargetPlaylistStub.hpp"
#include "assert-utility.hpp"
#include <recognition-test/FixedLevelMethod.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise {
namespace {
class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run(FixedLevelMethodImpl &) = 0;
};

class InitializingMethod : public UseCase {
    TargetPlaylist &list;
    const FixedLevelFixedTrialsTest &test;

  public:
    InitializingMethod(
        TargetPlaylist &list, const FixedLevelFixedTrialsTest &test)
        : list{list}, test{test} {}

    void run(FixedLevelMethodImpl &m) override { m.initialize(test, &list); }
};

class InitializingMethodWithFiniteTargetPlaylistWithRepeatables
    : public UseCase {
  public:
    InitializingMethodWithFiniteTargetPlaylistWithRepeatables(
        FiniteTargetPlaylistWithRepeatables &list, const FixedLevelTest &test)
        : list{list}, test{test} {}

    void run(FixedLevelMethodImpl &m) override { m.initialize(test, &list); }

  private:
    FiniteTargetPlaylistWithRepeatables &list;
    const FixedLevelTest &test;
};

class InitializingMethodWithFiniteTargetPlaylist : public UseCase {
  public:
    InitializingMethodWithFiniteTargetPlaylist(
        FiniteTargetPlaylist &list, const FixedLevelTest &test)
        : list{list}, test{test} {}

    void run(FixedLevelMethodImpl &m) override { m.initialize(test, &list); }

  private:
    FiniteTargetPlaylist &list;
    const FixedLevelTest &test;
};

class SubmittingCoordinateResponse : public UseCase {
    coordinate_response_measure::Response response_{};

  public:
    void run(FixedLevelMethodImpl &m) override { m.submit(response_); }

    void setColor(coordinate_response_measure::Color c) { response_.color = c; }

    void setNumber(int n) { response_.number = n; }

    [[nodiscard]] auto response() const -> auto & { return response_; }
};

class SubmittingFreeResponse : public UseCase {
    FreeResponse response{};

  public:
    void run(FixedLevelMethodImpl &m) override { m.submit(response); }
    void setFlagged() { response.flagged = true; }
};

class SubmittingThreeKeywords : public UseCase {

  public:
    void run(FixedLevelMethodImpl &m) override { m.submit(response); }
    void setFlagged() { response.flagged = true; }

  private:
    ThreeKeywordsResponse response{};
};

class SubmittingSyllable : public UseCase {

  public:
    void run(FixedLevelMethodImpl &m) override { m.submit(response); }

  private:
    SyllableResponse response{};
};

class SubmittingConsonant : public UseCase {
    ConsonantResponse response{};

  public:
    void run(FixedLevelMethodImpl &m) override { m.submit(response); }

    void setConsonant(char c) { response.consonant = c; }
};

auto blueColor() { return coordinate_response_measure::Color::blue; }

void run(UseCase &useCase, FixedLevelMethodImpl &method) {
    useCase.run(method);
}

auto nextTarget(FixedLevelMethodImpl &method) -> std::string {
    return method.nextTarget().path;
}

void assertNextTargetEquals(
    FixedLevelMethodImpl &method, const std::string &s) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(s, nextTarget(method));
}

void setNext(TargetPlaylistStub &list, std::string s) {
    list.setNext(std::move(s));
}

class PreInitializedFixedLevelMethodTests : public ::testing::Test {
  protected:
    ResponseEvaluatorStub evaluator;
    TargetPlaylistStub targetList;
    FiniteTargetPlaylistWithRepeatablesStub finiteTargetPlaylistWithRepeatables;
    FiniteTargetPlaylistStub finiteTargetPlaylist;
    FixedLevelMethodImpl method{evaluator};
    FixedLevelTest test{};
    FixedLevelFixedTrialsTest testWithFixedTrials{};
    InitializingMethod initializingMethod{targetList, testWithFixedTrials};
    InitializingMethodWithFiniteTargetPlaylistWithRepeatables
        initializingMethodWithFiniteTargetPlaylistWithRepeatables{
            finiteTargetPlaylistWithRepeatables, test};
    InitializingMethodWithFiniteTargetPlaylist
        initializingMethodWithFiniteTargetPlaylist{finiteTargetPlaylist, test};
};

#define PRE_INITIALIZED_FIXED_LEVEL_METHOD_TEST(a)                             \
    TEST_F(PreInitializedFixedLevelMethodTests, a)

PRE_INITIALIZED_FIXED_LEVEL_METHOD_TEST(snrReturnsInitializedSnr) {
    testWithFixedTrials.snr.dB = 1;
    run(initializingMethod, method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, method.snr().dB);
}

PRE_INITIALIZED_FIXED_LEVEL_METHOD_TEST(
    snrReturnsInitializedWithFiniteTargetPlaylistWithRepeatablesSnr) {
    test.snr.dB = 1;
    run(initializingMethodWithFiniteTargetPlaylistWithRepeatables, method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, method.snr().dB);
}

PRE_INITIALIZED_FIXED_LEVEL_METHOD_TEST(
    snrReturnsInitializedWithFiniteTargetPlaylistSnr) {
    test.snr.dB = 1;
    run(initializingMethodWithFiniteTargetPlaylist, method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, method.snr().dB);
}

PRE_INITIALIZED_FIXED_LEVEL_METHOD_TEST(
    initializePassesTargetPlaylistDirectory) {
    testWithFixedTrials.targetsUrl.path = "a";
    run(initializingMethod, method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, targetList.directory().path);
}

PRE_INITIALIZED_FIXED_LEVEL_METHOD_TEST(
    initializeWithFiniteTargetPlaylistPassesTargetPlaylistDirectory) {
    test.targetsUrl.path = "a";
    run(initializingMethodWithFiniteTargetPlaylist, method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, finiteTargetPlaylist.directory().path);
}

PRE_INITIALIZED_FIXED_LEVEL_METHOD_TEST(
    initializeWithFiniteTargetPlaylistWithRepeatablesPassesTargetPlaylistDirectory) {
    test.targetsUrl.path = "a";
    run(initializingMethodWithFiniteTargetPlaylistWithRepeatables, method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, finiteTargetPlaylistWithRepeatables.directory().path);
}

void assertComplete(FixedLevelMethodImpl &method) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(method.complete());
}

void assertIncomplete(FixedLevelMethodImpl &method) {
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(method.complete());
}

void assertTestCompleteOnlyAfter(UseCase &useCase, FixedLevelMethodImpl &method,
    FiniteTargetPlaylistStub &list) {
    list.setEmpty();
    assertIncomplete(method);
    run(useCase, method);
    assertComplete(method);
}

auto reinsertCurrentCalled(FiniteTargetPlaylistWithRepeatablesStub &list)
    -> bool {
    return list.reinsertCurrentCalled();
}

void assertCurrentTargetNotReinserted(
    FiniteTargetPlaylistWithRepeatablesStub &list) {
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(reinsertCurrentCalled(list));
}

void assertCurrentTargetReinserted(
    FiniteTargetPlaylistWithRepeatablesStub &list) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(reinsertCurrentCalled(list));
}

class FixedLevelMethodTests : public ::testing::Test {
  protected:
    ResponseEvaluatorStub evaluator;
    TargetPlaylistStub targetList;
    OutputFileStub outputFile;
    FixedLevelMethodImpl method{evaluator};
    FixedLevelFixedTrialsTest test{};
    SubmittingCoordinateResponse submittingCoordinateResponse;
    SubmittingFreeResponse submittingFreeResponse;
    InitializingMethod initializingMethod{targetList, test};

    FixedLevelMethodTests() { run(initializingMethod, method); }

    void writeLastCoordinateResponse() {
        method.writeLastCoordinateResponse(outputFile);
    }

    auto writtenFixedLevelTrial() {
        return outputFile.writtenFixedLevelTrial();
    }

    auto writtenFixedLevelTrialCorrect() -> bool {
        return writtenFixedLevelTrial().correct;
    }

    void setCurrentTarget(std::string s) {
        targetList.setCurrent(std::move(s));
    }
};

#define FIXED_LEVEL_METHOD_TEST(a) TEST_F(FixedLevelMethodTests, a)

FIXED_LEVEL_METHOD_TEST(nextReturnsNextTarget) {
    setNext(targetList, "a");
    assertNextTargetEquals(method, "a");
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesSubjectColor) {
    submittingCoordinateResponse.setColor(blueColor());
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        blueColor(), writtenFixedLevelTrial().subjectColor);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesSubjectNumber) {
    submittingCoordinateResponse.setNumber(1);
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, writtenFixedLevelTrial().subjectNumber);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesCorrectColor) {
    evaluator.setCorrectColor(blueColor());
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        blueColor(), writtenFixedLevelTrial().correctColor);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesCorrectNumber) {
    evaluator.setCorrectNumber(1);
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, writtenFixedLevelTrial().correctNumber);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesStimulus) {
    setCurrentTarget("a");
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, writtenFixedLevelTrial().target);
}

FIXED_LEVEL_METHOD_TEST(writeCorrectCoordinateResponse) {
    evaluator.setCorrect();
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(writtenFixedLevelTrialCorrect());
}

FIXED_LEVEL_METHOD_TEST(writeIncorrectCoordinateResponse) {
    evaluator.setIncorrect();
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(writtenFixedLevelTrialCorrect());
}

FIXED_LEVEL_METHOD_TEST(writeTestPassesSettings) {
    method.writeTestingParameters(outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &static_cast<const FixedLevelTest &>(std::as_const(test)),
        outputFile.fixedLevelTest());
}

FIXED_LEVEL_METHOD_TEST(submitCoordinateResponsePassesResponse) {
    run(submittingCoordinateResponse, method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &submittingCoordinateResponse.response(), evaluator.response());
}

FIXED_LEVEL_METHOD_TEST(completeWhenTrialsExhausted) {
    test.trials = 3;
    run(initializingMethod, method);
    run(submittingCoordinateResponse, method);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(method.complete());
    run(submittingFreeResponse, method);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(method.complete());
    run(submittingCoordinateResponse, method);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(method.complete());
}

FIXED_LEVEL_METHOD_TEST(submitCoordinateResponsePassesCurrentToEvaluator) {
    setCurrentTarget("a");
    run(submittingCoordinateResponse, method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, evaluator.correctColorFilePath());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, evaluator.correctNumberFilePath());
}

FIXED_LEVEL_METHOD_TEST(
    submitCoordinateResponsePassesCorrectTargetToEvaluator) {
    setCurrentTarget("a");
    run(submittingCoordinateResponse, method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, evaluator.correctFilePath());
}

void writeLastConsonant(FixedLevelMethodImpl &method, OutputFile &outputFile) {
    method.writeLastConsonant(outputFile);
}

class FixedLevelMethodWithFiniteTargetPlaylistTests : public ::testing::Test {
  protected:
    ResponseEvaluatorStub evaluator;
    FiniteTargetPlaylistStub targetList;
    FixedLevelMethodImpl method{evaluator};
    OutputFileStub outputFile;
    FixedLevelTest test{};
    InitializingMethodWithFiniteTargetPlaylist initializingMethod{
        targetList, test};
    SubmittingCoordinateResponse submittingCoordinateResponse;
    SubmittingFreeResponse submittingFreeResponse;
    SubmittingThreeKeywords submittingThreeKeywords;
    SubmittingConsonant submittingConsonant;

    FixedLevelMethodWithFiniteTargetPlaylistTests() {
        run(initializingMethod, method);
    }
};

#define FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(a)                     \
    TEST_F(FixedLevelMethodWithFiniteTargetPlaylistTests, a)

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(nextReturnsNextTarget) {
    setNext(targetList, "a");
    assertNextTargetEquals(method, "a");
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(writeTestPassesSettings) {
    method.writeTestingParameters(outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &std::as_const(test), outputFile.fixedLevelTest());
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(writeConsonantPassesConsonant) {
    submittingConsonant.setConsonant('b');
    run(submittingConsonant, method);
    writeLastConsonant(method, outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        'b', outputFile.consonantTrial().subjectConsonant);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(writeConsonantPassesTarget) {
    targetList.setCurrent("a");
    run(submittingConsonant, method);
    writeLastConsonant(method, outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, outputFile.consonantTrial().target);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    writeConsonantPassesCorrectConsonant) {
    evaluator.setCorrectConsonant('b');
    run(submittingConsonant, method);
    writeLastConsonant(method, outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        'b', outputFile.consonantTrial().correctConsonant);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    writeCorrectCoordinateResponse) {
    evaluator.setCorrect();
    run(submittingConsonant, method);
    writeLastConsonant(method, outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputFile.consonantTrial().correct);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    writeIncorrectCoordinateResponse) {
    evaluator.setIncorrect();
    run(submittingConsonant, method);
    writeLastConsonant(method, outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(outputFile.consonantTrial().correct);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    submitConsonantPassesCurrentTargetToEvaluator) {
    targetList.setCurrent("a");
    run(submittingConsonant, method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, evaluator.correctConsonantUrl().path);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    submitConsonantPassesCorrectTargetToEvaluator) {
    targetList.setCurrent("a");
    run(submittingConsonant, method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, evaluator.correctUrlForConsonantResponse().path);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    completeWhenTestCompleteAfterCoordinateResponse) {
    assertTestCompleteOnlyAfter(
        submittingCoordinateResponse, method, targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    completeWhenTestCompleteAfterConsonantResponse) {
    assertTestCompleteOnlyAfter(submittingConsonant, method, targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    completeWhenTestCompleteAfterFreeResponse) {
    assertTestCompleteOnlyAfter(submittingFreeResponse, method, targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    completeWhenTestCompleteAfterThreeKeywords) {
    assertTestCompleteOnlyAfter(submittingThreeKeywords, method, targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    completeWhenTestCompleteAfterInitializing) {
    assertTestCompleteOnlyAfter(initializingMethod, method, targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    initializeLoadsBeforeQueryingCompletion) {
    run(initializingMethod, method);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        endsWith(targetList.log(), "loadFromDirectory empty "));
}

class FixedLevelMethodWithFiniteTargetPlaylistWithRepeatablesTests
    : public ::testing::Test {
  protected:
    ResponseEvaluatorStub evaluator;
    FiniteTargetPlaylistWithRepeatablesStub targetList;
    FixedLevelMethodImpl method{evaluator};
    FixedLevelTest test{};
    InitializingMethodWithFiniteTargetPlaylistWithRepeatables
        initializingMethod{targetList, test};
    SubmittingCoordinateResponse submittingCoordinateResponse;
    SubmittingFreeResponse submittingFreeResponse;
    SubmittingThreeKeywords submittingThreeKeywords;
    SubmittingSyllable submittingSyllable;

    FixedLevelMethodWithFiniteTargetPlaylistWithRepeatablesTests() {
        run(initializingMethod, method);
    }
};

#define FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(a)    \
    TEST_F(FixedLevelMethodWithFiniteTargetPlaylistWithRepeatablesTests, a)

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(
    nextReturnsNextTarget) {
    setNext(targetList, "a");
    assertNextTargetEquals(method, "a");
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(
    writeTestPassesSettings) {
    OutputFileStub outputFile;
    method.writeTestingParameters(outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &std::as_const(test), outputFile.fixedLevelTest());
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(
    completeWhenTestCompleteAfterCoordinateResponse) {
    assertTestCompleteOnlyAfter(
        submittingCoordinateResponse, method, targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(
    completeWhenTestCompleteAfterFreeResponse) {
    assertTestCompleteOnlyAfter(submittingFreeResponse, method, targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(
    completeWhenTestCompleteAfterThreeKeywords) {
    assertTestCompleteOnlyAfter(submittingThreeKeywords, method, targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(
    completeWhenTestCompleteAfterSyllableResponse) {
    assertTestCompleteOnlyAfter(submittingSyllable, method, targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(
    completeWhenTestCompleteAfterInitializing) {
    assertTestCompleteOnlyAfter(initializingMethod, method, targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(
    submitFreeResponseDoesNotReinsertCurrentTarget) {
    run(submittingFreeResponse, method);
    assertCurrentTargetNotReinserted(targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(
    submitThreeKeywordsDoesNotReinsertCurrentTarget) {
    run(submittingThreeKeywords, method);
    assertCurrentTargetNotReinserted(targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(
    submitFreeResponseReinsertsCurrentTargetIfFlagged) {
    submittingFreeResponse.setFlagged();
    run(submittingFreeResponse, method);
    assertCurrentTargetReinserted(targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(
    submitThreeKeywordsReinsertsCurrentTargetIfFlagged) {
    submittingThreeKeywords.setFlagged();
    run(submittingThreeKeywords, method);
    assertCurrentTargetReinserted(targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(
    submitThreeKeywordsTracksTotalCorrectAndPercentCorrect) {
    method.submit(ThreeKeywordsResponse{true, true, true, false});
    method.submit(ThreeKeywordsResponse{true, true, false, false});
    method.submit(ThreeKeywordsResponse{true, false, true, false});
    method.submit(ThreeKeywordsResponse{false, false, true, false});
    method.submit(ThreeKeywordsResponse{true, true, false, false});
    method.submit(ThreeKeywordsResponse{true, true, true, false});
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(
        13, method.keywordsTestResults().totalCorrect);
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(
        13 / 18., method.keywordsTestResults().percentCorrect);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(
    submitThreeKeywordsResetsTotalCorrectAndPercentCorrect) {
    method.submit(ThreeKeywordsResponse{true, true, true, false});
    method.submit(ThreeKeywordsResponse{true, true, false, false});
    run(initializingMethod, method);
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(
        0, method.keywordsTestResults().totalCorrect);
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(
        0, method.keywordsTestResults().percentCorrect);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(
    submitFreeResponseReinsertsCurrentTargetIfFlaggedBeforeQueryingCompletion) {
    run(initializingMethod, method);
    FreeResponse response;
    response.flagged = true;
    method.submit(response);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        endsWith(targetList.log(), "reinsertCurrent empty "));
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(
    initializeLoadsBeforeQueryingCompletion) {
    run(initializingMethod, method);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        endsWith(targetList.log(), "loadFromDirectory empty "));
}
}
}
