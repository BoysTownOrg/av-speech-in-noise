#include "LogString.h"
#include "OutputFileStub.h"
#include "ResponseEvaluatorStub.h"
#include "TargetListStub.h"
#include "TestConcluderStub.h"
#include "assert-utility.h"
#include "av-speech-in-noise/Model.hpp"
#include <gtest/gtest.h>
#include <recognition-test/FixedLevelMethod.hpp>

namespace av_speech_in_noise::tests {
namespace {
class FiniteTargetListStub : public FiniteTargetList, public TargetListStub {};

class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run(FixedLevelMethodImpl &) = 0;
};

class InitializingMethod : public UseCase {
    FixedLevelTest test_;
    TargetList &list;
    TestConcluder &concluder;

  public:
    InitializingMethod(TargetList &list, TestConcluder &concluder)
        : list{list}, concluder{concluder} {}

    void run(FixedLevelMethodImpl &m) override {
        m.initialize(test_, &list, &concluder);
    }

    [[nodiscard]] auto test() const -> auto & { return test_; }

    void setTargetListDirectory(std::string s) {
        test_.targetListDirectory = std::move(s);
    }

    void setSnr(int x) { test_.snr_dB = x; }
};

class InitializingMethodWithFiniteTargetList : public UseCase {
  public:
    InitializingMethodWithFiniteTargetList(FiniteTargetList &list,
        TestConcluder &concluder, const FixedLevelTest &test)
        : list{list}, concluder{concluder}, test{test} {}

    void run(FixedLevelMethodImpl &m) override {
        m.initialize(test, &list, &concluder);
    }

  private:
    const FixedLevelTest &test;
    FiniteTargetList &list;
    TestConcluder &concluder;
};

class SubmittingCoordinateResponse : public UseCase {
    coordinate_response_measure::Response response_{};

  public:
    void run(FixedLevelMethodImpl &m) override { m.submitResponse(response_); }

    void setColor(coordinate_response_measure::Color c) { response_.color = c; }

    void setNumber(int n) { response_.number = n; }

    [[nodiscard]] auto response() const -> auto & { return response_; }
};

class SubmittingFreeResponse : public UseCase {
    open_set::FreeResponse response{};

  public:
    void run(FixedLevelMethodImpl &m) override { m.submitResponse(response); }
    void setFlagged() { response.flagged = true; }
};

auto blueColor() { return coordinate_response_measure::Color::blue; }

void run(UseCase &useCase, FixedLevelMethodImpl &method) {
    useCase.run(method);
}

auto nextTarget(FixedLevelMethodImpl &method) -> std::string {
    return method.nextTarget();
}

void assertNextTargetEquals(
    FixedLevelMethodImpl &method, const std::string &s) {
    assertEqual(s, nextTarget(method));
}

void setNext(TargetListStub &list, std::string s) {
    list.setNext(std::move(s));
}

class FixedLevelMethodTests : public ::testing::Test {
  protected:
    ResponseEvaluatorStub evaluator;
    TargetListStub targetList;
    TestConcluderStub testConcluder;
    OutputFileStub outputFile;
    FixedLevelMethodImpl method{&evaluator};
    SubmittingCoordinateResponse submittingCoordinateResponse;
    SubmittingFreeResponse submittingFreeResponse;
    InitializingMethod initializingMethod{targetList, testConcluder};

    FixedLevelMethodTests() { run(initializingMethod, method); }

    void writeLastCoordinateResponse() {
        method.writeLastCoordinateResponse(&outputFile);
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

    void assertTestIncomplete() { assertFalse(testComplete()); }

    auto testComplete() -> bool { return method.complete(); }

    void assertTestComplete() { assertTrue(testComplete()); }

    void setTestComplete() { testConcluder.setComplete(); }

    void setTestIncomplete() { testConcluder.setIncomplete(); }

    void assertTargetListPassedToConcluderAfter(UseCase &useCase) {
        run(useCase, method);
        assertTestConcluderPassedTargetList();
    }

    void assertTestConcluderPassedTargetList() {
        assertEqual(
            static_cast<TargetList *>(&targetList), testConcluder.targetList());
    }

    void assertTestCompleteOnlyWhenComplete(UseCase &useCase) {
        run(useCase, method);
        assertTestIncomplete();
        assertTestCompleteWhenComplete(useCase);
    }

    void assertTestCompleteWhenComplete(UseCase &useCase) {
        setTestComplete();
        run(useCase, method);
        assertTestComplete();
    }

    void assertTestConcluderLogContains(const std::string &s) {
        assertTrue(testConcluder.log().contains(s));
    }

    void assertTestConcluderLogContainsAfter(
        const std::string &s, UseCase &useCase) {
        run(useCase, method);
        assertTestConcluderLogContains(s);
    }

    auto reinsertCurrentCalled() -> bool {
        return targetList.reinsertCurrentCalled();
    }

    void assertCurrentTargetNotReinserted() {
        assertFalse(reinsertCurrentCalled());
    }

    void assertCurrentTargetReinserted() {
        assertTrue(reinsertCurrentCalled());
    }
};

#define FIXED_LEVEL_METHOD_TEST(a) TEST_F(FixedLevelMethodTests, a)

FIXED_LEVEL_METHOD_TEST(passesTestParametersToConcluder) {
    assertEqual(&initializingMethod.test(), testConcluder.test());
}

FIXED_LEVEL_METHOD_TEST(nextReturnsNextTarget) {
    setNext(targetList, "a");
    assertNextTargetEquals(method, "a");
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesSubjectColor) {
    submittingCoordinateResponse.setColor(blueColor());
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    assertEqual(blueColor(), writtenFixedLevelTrial().subjectColor);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesSubjectNumber) {
    submittingCoordinateResponse.setNumber(1);
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    assertEqual(1, writtenFixedLevelTrial().subjectNumber);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesCorrectColor) {
    evaluator.setCorrectColor(blueColor());
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    assertEqual(blueColor(), writtenFixedLevelTrial().correctColor);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesCorrectNumber) {
    evaluator.setCorrectNumber(1);
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    assertEqual(1, writtenFixedLevelTrial().correctNumber);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesStimulus) {
    setCurrentTarget("a");
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    assertEqual("a", writtenFixedLevelTrial().target);
}

FIXED_LEVEL_METHOD_TEST(writeCorrectCoordinateResponse) {
    evaluator.setCorrect();
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    assertTrue(writtenFixedLevelTrialCorrect());
}

FIXED_LEVEL_METHOD_TEST(writeIncorrectCoordinateResponse) {
    evaluator.setIncorrect();
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    assertFalse(writtenFixedLevelTrialCorrect());
}

FIXED_LEVEL_METHOD_TEST(writeTestPassesSettings) {
    method.writeTestingParameters(&outputFile);
    assertEqual(&initializingMethod.test(), outputFile.fixedLevelTest());
}

FIXED_LEVEL_METHOD_TEST(submitCoordinateResponsePassesResponse) {
    run(submittingCoordinateResponse, method);
    assertEqual(&submittingCoordinateResponse.response(), evaluator.response());
}

FIXED_LEVEL_METHOD_TEST(submitCoordinateResponseSubmitsResponseToConcluder) {
    run(submittingCoordinateResponse, method);
    assertTrue(testConcluder.responseSubmitted());
}

FIXED_LEVEL_METHOD_TEST(submitFreeResponseSubmitsResponseToConcluder) {
    run(submittingFreeResponse, method);
    assertTrue(testConcluder.responseSubmitted());
}

FIXED_LEVEL_METHOD_TEST(submitCoordinateResponsePassesCurrentToEvaluator) {
    setCurrentTarget("a");
    run(submittingCoordinateResponse, method);
    assertEqual("a", evaluator.correctColorFilePath());
    assertEqual("a", evaluator.correctNumberFilePath());
}

FIXED_LEVEL_METHOD_TEST(
    submitCoordinateResponsePassesCorrectTargetToEvaluator) {
    setCurrentTarget("a");
    run(submittingCoordinateResponse, method);
    assertEqual("a", evaluator.correctFilePath());
}

FIXED_LEVEL_METHOD_TEST(completeWhenTestCompleteAfterCoordinateResponse) {
    assertTestCompleteOnlyWhenComplete(submittingCoordinateResponse);
}

FIXED_LEVEL_METHOD_TEST(completeWhenTestCompleteAfterFreeResponse) {
    assertTestCompleteOnlyWhenComplete(submittingFreeResponse);
}

FIXED_LEVEL_METHOD_TEST(submitCoordinateResponsePassesTargetListToConcluder) {
    assertTargetListPassedToConcluderAfter(submittingCoordinateResponse);
}

FIXED_LEVEL_METHOD_TEST(submitFreeResponsePassesTargetListToConcluder) {
    assertTargetListPassedToConcluderAfter(submittingFreeResponse);
}

FIXED_LEVEL_METHOD_TEST(
    submitCoordinateResponseSubmitsResponsePriorToQueryingCompletion) {
    assertTestConcluderLogContainsAfter(
        "submitResponse complete", submittingCoordinateResponse);
}

FIXED_LEVEL_METHOD_TEST(submitFreeResponseDoesNotReinsertCurrentTarget) {
    run(submittingFreeResponse, method);
    assertCurrentTargetNotReinserted();
}

FIXED_LEVEL_METHOD_TEST(submitFreeResponseReinsertsCurrentTargetIfFlagged) {
    submittingFreeResponse.setFlagged();
    run(submittingFreeResponse, method);
    assertCurrentTargetReinserted();
}

FIXED_LEVEL_METHOD_TEST(initializesConcluderBeforeQueryingCompletion) {
    assertTestConcluderLogContains("initialize complete");
}

FIXED_LEVEL_METHOD_TEST(initializePassesTargetListToConcluder) {
    assertTestConcluderPassedTargetList();
}

FIXED_LEVEL_METHOD_TEST(completeWhenTestCompleteAfterInitializing) {
    assertTestIncomplete();
    assertTestCompleteWhenComplete(initializingMethod);
}

class PreInitializedFixedLevelMethodTests : public ::testing::Test {
  protected:
    ResponseEvaluatorStub evaluator;
    TargetListStub targetList;
    FiniteTargetListStub finiteTargetList;
    TestConcluderStub testConcluder;
    FixedLevelMethodImpl method{&evaluator};
    FixedLevelTest test{};
    InitializingMethod initializingMethod{targetList, testConcluder};
    InitializingMethodWithFiniteTargetList
        initializingMethodWithFiniteTargetList{
            finiteTargetList, testConcluder, test};
};

TEST_F(PreInitializedFixedLevelMethodTests, snrReturnsInitializedSnr) {
    initializingMethod.setSnr(1);
    run(initializingMethod, method);
    assertEqual(1, method.snr_dB());
}

TEST_F(
    PreInitializedFixedLevelMethodTests, initializePassesTargetListDirectory) {
    initializingMethod.setTargetListDirectory("a");
    run(initializingMethod, method);
    assertEqual("a", targetList.directory());
}

TEST_F(PreInitializedFixedLevelMethodTests,
    initializeWithFiniteTargetListPassesTargetListDirectory) {
    test.targetListDirectory = "a";
    run(initializingMethodWithFiniteTargetList, method);
    assertEqual("a", finiteTargetList.directory());
}

class FixedLevelMethodWithFiniteTargetListTests : public ::testing::Test {
  protected:
    ResponseEvaluatorStub evaluator;
    FiniteTargetListStub targetList;
    TestConcluderStub testConcluder;
    FixedLevelMethodImpl method{&evaluator};
    FixedLevelTest test{};
    InitializingMethodWithFiniteTargetList
        initializingMethodWithFiniteTargetList{targetList, testConcluder, test};

    FixedLevelMethodWithFiniteTargetListTests() {
        run(initializingMethodWithFiniteTargetList, method);
    }
};

#define FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(a)                     \
    TEST_F(FixedLevelMethodWithFiniteTargetListTests, a)

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    passesTestParametersToConcluder) {
    assertEqual(&std::as_const(test), testConcluder.test());
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(nextReturnsNextTarget) {
    setNext(targetList, "a");
    assertNextTargetEquals(method, "a");
}

class TargetListTestConcluderComboStub : public TargetList,
                                         public TestConcluder {
  public:
    void loadFromDirectory(std::string) override {}
    auto next() -> std::string override { return {}; }
    auto current() -> std::string override { return {}; }
    auto empty() -> bool override { return {}; }
    void reinsertCurrent() override { log_.insert("reinsertCurrent "); }
    auto complete(TargetList *) -> bool override {
        log_.insert("complete ");
        return {};
    }
    void submitResponse() override {}
    void initialize(const FixedLevelTest &) override {}
    auto log() const -> auto & { return log_; }

  private:
    LogString log_;
};

TEST(FixedLevelMethodTestsTBD,
    submitFreeResponseReinsertsCurrentTargetIfFlaggedBeforeQueryingCompletion) {
    ResponseEvaluatorStub evaluator;
    TargetListTestConcluderComboStub combo;
    FixedLevelMethodImpl method{&evaluator};
    FixedLevelTest test;
    method.initialize(test, &combo, &combo);
    open_set::FreeResponse response;
    response.flagged = true;
    method.submitResponse(response);
    assertEqual("complete reinsertCurrent complete ", combo.log());
}

class FixedTrialTestConcluderTests : public ::testing::Test {
  protected:
    FixedTrialTestConcluder testConcluder{};
    FixedLevelTest test;

    void initialize() { testConcluder.initialize(test); }

    void assertIncompleteAfterResponse() {
        submitResponse();
        assertIncomplete();
    }

    void submitResponse() { testConcluder.submitResponse(); }

    void assertCompleteAfterResponse() {
        submitResponse();
        assertComplete();
    }

    void assertIncomplete() { assertFalse(complete()); }

    auto complete() -> bool { return testConcluder.complete({}); }

    void assertComplete() { assertTrue(complete()); }
};

TEST_F(FixedTrialTestConcluderTests, completeWhenTrialsExhausted) {
    test.trials = 3;
    initialize();
    assertIncompleteAfterResponse();
    assertIncompleteAfterResponse();
    assertCompleteAfterResponse();
}

class EmptyTargetListTestConcluderTests : public ::testing::Test {
  protected:
    TargetListStub targetList;
    EmptyTargetListTestConcluder testConcluder;

    auto complete() -> bool { return testConcluder.complete(&targetList); }

    void assertIncomplete() { assertFalse(complete()); }

    void assertComplete() { assertTrue(complete()); }
};

TEST_F(EmptyTargetListTestConcluderTests, completeWhenTargetListComplete) {
    assertIncomplete();
    targetList.setEmpty();
    assertComplete();
}
}
}
