#include "ConfigurationRegistryStub.hpp"
#include "LogString.hpp"
#include "OutputFileStub.hpp"
#include "ResponseEvaluatorStub.hpp"
#include "RunningATestStub.hpp"
#include "TargetPlaylistStub.hpp"
#include "assert-utility.hpp"

#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/core/FixedLevelMethod.hpp>

#include <gtest/gtest.h>

#include <stdexcept>

namespace av_speech_in_noise {
namespace {
class UseCase {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(UseCase);
    virtual void run(FixedLevelMethodImpl &) = 0;
};

class InitializingMethod : public UseCase {
    TargetPlaylist &list;
    const FixedLevelFixedTrialsTest &test;

  public:
    InitializingMethod(
        TargetPlaylist &list, const FixedLevelFixedTrialsTest &test)
        : list{list}, test{test} {}

    void run(FixedLevelMethodImpl &m) override {
        m.attach(test, &list);
        m.initialize();
    }
};

class InitializingMethodWithFiniteTargetPlaylistWithRepeatables
    : public UseCase {
  public:
    explicit InitializingMethodWithFiniteTargetPlaylistWithRepeatables(
        FiniteTargetPlaylistWithRepeatables &list)
        : list{list} {}

    void run(FixedLevelMethodImpl &m) override {
        m.attach(&list);
        m.initialize();
    }

  private:
    FiniteTargetPlaylistWithRepeatables &list;
};

class InitializingMethodWithFiniteTargetPlaylist : public UseCase {
  public:
    explicit InitializingMethodWithFiniteTargetPlaylist(
        FiniteTargetPlaylist &list)
        : list{list} {}

    void run(FixedLevelMethodImpl &m) override {
        m.attach(&list);
        m.initialize();
    }

  private:
    FiniteTargetPlaylist &list;
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

class SubmittingKeyPressResponse : public UseCase {
  public:
    void run(FixedLevelMethodImpl &m) override { m.submit(response); }

    KeyPressResponse response;
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
    void setFlagged() { response.flagged = true; }

  private:
    SyllableResponse response{};
};

class SubmittingConsonant : public UseCase {
  public:
    void run(FixedLevelMethodImpl &m) override { m.submit(Flaggable{false}); }
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
    ConfigurationRegistryStub registry;
    RunningATestStub runningATest;
    FixedLevelMethodImpl method{registry, evaluator, runningATest};
    FixedLevelFixedTrialsTest testWithFixedTrials{};
    InitializingMethod initializingMethod{targetList, testWithFixedTrials};
    InitializingMethodWithFiniteTargetPlaylistWithRepeatables
        initializingMethodWithFiniteTargetPlaylistWithRepeatables{
            finiteTargetPlaylistWithRepeatables};
    InitializingMethodWithFiniteTargetPlaylist
        initializingMethodWithFiniteTargetPlaylist{finiteTargetPlaylist};
};

#define PRE_INITIALIZED_FIXED_LEVEL_METHOD_TEST(a)                             \
    TEST_F(PreInitializedFixedLevelMethodTests, a)

PRE_INITIALIZED_FIXED_LEVEL_METHOD_TEST(snrReturnsInitializedSnr) {
    method.configure("starting SNR (dB)", "1");
    run(initializingMethod, method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, method.snr().dB);
}

PRE_INITIALIZED_FIXED_LEVEL_METHOD_TEST(
    snrReturnsInitializedWithFiniteTargetPlaylistWithRepeatablesSnr) {
    method.configure("starting SNR (dB)", "1");
    run(initializingMethodWithFiniteTargetPlaylistWithRepeatables, method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, method.snr().dB);
}

PRE_INITIALIZED_FIXED_LEVEL_METHOD_TEST(
    snrReturnsInitializedWithFiniteTargetPlaylistSnr) {
    method.configure("starting SNR (dB)", "1");
    run(initializingMethodWithFiniteTargetPlaylist, method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, method.snr().dB);
}

PRE_INITIALIZED_FIXED_LEVEL_METHOD_TEST(
    initializePassesTargetPlaylistDirectory) {
    method.configure("targets", "a");
    run(initializingMethod, method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, targetList.directory().path);
}

PRE_INITIALIZED_FIXED_LEVEL_METHOD_TEST(
    initializeWithFiniteTargetPlaylistPassesTargetPlaylistDirectory) {
    method.configure("targets", "a");
    run(initializingMethodWithFiniteTargetPlaylist, method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, finiteTargetPlaylist.directory().path);
}

PRE_INITIALIZED_FIXED_LEVEL_METHOD_TEST(
    initializeWithFiniteTargetPlaylistWithRepeatablesPassesTargetPlaylistDirectory) {
    method.configure("targets", "a");
    run(initializingMethodWithFiniteTargetPlaylistWithRepeatables, method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, finiteTargetPlaylistWithRepeatables.directory().path);
}

PRE_INITIALIZED_FIXED_LEVEL_METHOD_TEST(
    failingTargetListLoadThrowsRequestFailure) {
    FailingTargetPlaylistStub targetPlaylist;
    try {
        method.attach({}, &targetPlaylist);
        method.initialize();
        FAIL() << "Expected std::runtime_error";
    } catch (const std::runtime_error &) {
    }
}

PRE_INITIALIZED_FIXED_LEVEL_METHOD_TEST(attaches) {
    method.configure("method", "fixed-level consonants");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(&method, runningATest.testMethod);
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
    ConfigurationRegistryStub registry;
    RunningATestStub runningATest;
    FixedLevelMethodImpl method{registry, evaluator, runningATest};
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

class FixedLevelMethodWithFiniteTargetPlaylistTests : public ::testing::Test {
  protected:
    ResponseEvaluatorStub evaluator;
    FiniteTargetPlaylistStub targetList;
    ConfigurationRegistryStub registry;
    RunningATestStub runningATest;
    FixedLevelMethodImpl method{registry, evaluator, runningATest};
    OutputFileStub outputFile;
    InitializingMethodWithFiniteTargetPlaylist initializingMethod{targetList};
    SubmittingCoordinateResponse submittingCoordinateResponse;
    SubmittingFreeResponse submittingFreeResponse;
    SubmittingThreeKeywords submittingThreeKeywords;
    SubmittingSyllable submittingSyllable;
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
    completeWhenTestCompleteAfterSyllable) {
    assertTestCompleteOnlyAfter(submittingSyllable, method, targetList);
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
    ConfigurationRegistryStub registry;
    RunningATestStub runningATest;
    FixedLevelMethodImpl method{registry, evaluator, runningATest};
    InitializingMethodWithFiniteTargetPlaylistWithRepeatables
        initializingMethod{targetList};
    SubmittingCoordinateResponse submittingCoordinateResponse;
    SubmittingFreeResponse submittingFreeResponse;
    SubmittingKeyPressResponse submittingKeyPressResponse;
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
    completeWhenTestCompleteAfterCoordinateResponse) {
    assertTestCompleteOnlyAfter(
        submittingCoordinateResponse, method, targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(
    completeWhenTestCompleteAfterKeyPressResponse) {
    assertTestCompleteOnlyAfter(submittingKeyPressResponse, method, targetList);
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
    submitSyllableDoesNotReinsertCurrentTarget) {
    run(submittingSyllable, method);
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
    submitSyllableReinsertsCurrentTargetIfFlagged) {
    submittingSyllable.setFlagged();
    run(submittingSyllable, method);
    assertCurrentTargetReinserted(targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(
    submitThreeKeywordsTracksTotalCorrectAndPercentCorrect) {
    method.submit(ThreeKeywordsResponse{{}, true, true, true});
    method.submit(ThreeKeywordsResponse{{}, true, true, false});
    method.submit(ThreeKeywordsResponse{{}, true, false, true});
    method.submit(ThreeKeywordsResponse{{}, false, false, true});
    method.submit(ThreeKeywordsResponse{{}, true, true, false});
    method.submit(ThreeKeywordsResponse{{}, true, true, true});
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(
        13, method.keywordsTestResults().totalCorrect);
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(
        13 * 100. / 18, method.keywordsTestResults().percentCorrect);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(
    submitThreeKeywordsResetsTotalCorrectAndPercentCorrect) {
    method.submit(ThreeKeywordsResponse{{}, true, true, true});
    method.submit(ThreeKeywordsResponse{{}, true, true, false});
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

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_WITH_REPEATABLES_TEST(tbd) {
    method.configure("starting SNR (dB)", "2");
    method.configure("targets", "a");
    run(initializingMethod, method);
    std::stringstream stream;
    method.write(stream);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(R"(targets: a
SNR (dB): 2
)",
        stream.str());
}
}
}
