#include "LogString.h"
#include "OutputFileStub.h"
#include "ResponseEvaluatorStub.h"
#include "TargetListStub.h"
#include "assert-utility.h"
#include "av-speech-in-noise/Model.hpp"
#include <gtest/gtest.h>
#include <recognition-test/FixedLevelMethod.hpp>

namespace av_speech_in_noise::tests {
namespace {
class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run(FixedLevelMethodImpl &) = 0;
};

class InitializingMethod : public UseCase {
    TargetList &list;
    const FixedLevelTest &test;

  public:
    InitializingMethod(TargetList &list, const FixedLevelTest &test)
        : list{list}, test{test} {}

    void run(FixedLevelMethodImpl &m) override { m.initialize(test, &list); }
};

class InitializingMethodWithFiniteTargetList : public UseCase {
  public:
    InitializingMethodWithFiniteTargetList(
        FiniteTargetList &list, const FixedLevelTest &test)
        : list{list}, test{test} {}

    void run(FixedLevelMethodImpl &m) override { m.initialize(test, &list); }

  private:
    const FixedLevelTest &test;
    FiniteTargetList &list;
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
    open_set::FreeResponse response{};

  public:
    void run(FixedLevelMethodImpl &m) override { m.submit(response); }
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
    OutputFileStub outputFile;
    FixedLevelMethodImpl method{&evaluator};
    FixedLevelTest test{};
    SubmittingCoordinateResponse submittingCoordinateResponse;
    SubmittingFreeResponse submittingFreeResponse;
    InitializingMethod initializingMethod{targetList, test};

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
    assertEqual(&std::as_const(test), outputFile.fixedLevelTest());
}

FIXED_LEVEL_METHOD_TEST(submitCoordinateResponsePassesResponse) {
    run(submittingCoordinateResponse, method);
    assertEqual(&submittingCoordinateResponse.response(), evaluator.response());
}

FIXED_LEVEL_METHOD_TEST(completeWhenTrialsExhausted) {
    test.trials = 3;
    run(initializingMethod, method);
    run(submittingCoordinateResponse, method);
    assertFalse(method.complete());
    run(submittingFreeResponse, method);
    assertFalse(method.complete());
    run(submittingCoordinateResponse, method);
    assertTrue(method.complete());
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

class PreInitializedFixedLevelMethodTests : public ::testing::Test {
  protected:
    ResponseEvaluatorStub evaluator;
    TargetListStub targetList;
    FiniteTargetListStub finiteTargetList;
    FixedLevelMethodImpl method{&evaluator};
    FixedLevelTest test{};
    InitializingMethod initializingMethod{targetList, test};
    InitializingMethodWithFiniteTargetList
        initializingMethodWithFiniteTargetList{finiteTargetList, test};
};

TEST_F(PreInitializedFixedLevelMethodTests, snrReturnsInitializedSnr) {
    test.snr_dB = 1;
    run(initializingMethod, method);
    assertEqual(1, method.snr_dB());
}

TEST_F(PreInitializedFixedLevelMethodTests,
    snrReturnsInitializedWithFiniteTargetListSnr) {
    test.snr_dB = 1;
    run(initializingMethodWithFiniteTargetList, method);
    assertEqual(1, method.snr_dB());
}

TEST_F(
    PreInitializedFixedLevelMethodTests, initializePassesTargetListDirectory) {
    test.targetListDirectory = "a";
    run(initializingMethod, method);
    assertEqual("a", targetList.directory());
}

TEST_F(PreInitializedFixedLevelMethodTests,
    initializeWithFiniteTargetListPassesTargetListDirectory) {
    test.targetListDirectory = "a";
    run(initializingMethodWithFiniteTargetList, method);
    assertEqual("a", finiteTargetList.directory());
}

void assertComplete(FixedLevelMethodImpl &method) {
    assertTrue(method.complete());
}

void assertIncomplete(FixedLevelMethodImpl &method) {
    assertFalse(method.complete());
}

void assertTestCompleteOnlyAfter(UseCase &useCase, FixedLevelMethodImpl &method,
    FiniteTargetListStub &list) {
    list.setEmpty();
    assertIncomplete(method);
    run(useCase, method);
    assertComplete(method);
}

auto reinsertCurrentCalled(FiniteTargetListStub &list) -> bool {
    return list.reinsertCurrentCalled();
}

void assertCurrentTargetNotReinserted(FiniteTargetListStub &list) {
    assertFalse(reinsertCurrentCalled(list));
}

void assertCurrentTargetReinserted(FiniteTargetListStub &list) {
    assertTrue(reinsertCurrentCalled(list));
}

class FixedLevelMethodWithFiniteTargetListTests : public ::testing::Test {
  protected:
    ResponseEvaluatorStub evaluator;
    FiniteTargetListStub targetList;
    FixedLevelMethodImpl method{&evaluator};
    FixedLevelTest test{};
    InitializingMethodWithFiniteTargetList
        initializingMethodWithFiniteTargetList{targetList, test};
    SubmittingCoordinateResponse submittingCoordinateResponse;
    SubmittingFreeResponse submittingFreeResponse;

    FixedLevelMethodWithFiniteTargetListTests() {
        run(initializingMethodWithFiniteTargetList, method);
    }
};

#define FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(a)                     \
    TEST_F(FixedLevelMethodWithFiniteTargetListTests, a)

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(nextReturnsNextTarget) {
    setNext(targetList, "a");
    assertNextTargetEquals(method, "a");
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(writeTestPassesSettings) {
    OutputFileStub outputFile;
    method.writeTestingParameters(&outputFile);
    assertEqual(&std::as_const(test), outputFile.fixedLevelTest());
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    completeWhenTestCompleteAfterCoordinateResponse) {
    assertTestCompleteOnlyAfter(
        submittingCoordinateResponse, method, targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    completeWhenTestCompleteAfterFreeResponse) {
    assertTestCompleteOnlyAfter(submittingFreeResponse, method, targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    completeWhenTestCompleteAfterInitializing) {
    assertTestCompleteOnlyAfter(
        initializingMethodWithFiniteTargetList, method, targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    submitFreeResponseDoesNotReinsertCurrentTarget) {
    run(submittingFreeResponse, method);
    assertCurrentTargetNotReinserted(targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    submitFreeResponseReinsertsCurrentTargetIfFlagged) {
    submittingFreeResponse.setFlagged();
    run(submittingFreeResponse, method);
    assertCurrentTargetReinserted(targetList);
}

class TargetListTestConcluderComboStub : public FiniteTargetList {
  public:
    void loadFromDirectory(std::string) override {}
    auto next() -> std::string override { return {}; }
    auto current() -> std::string override { return {}; }
    auto empty() -> bool override {
        log_.insert("empty ");
        return {};
    }
    void reinsertCurrent() override { log_.insert("reinsertCurrent "); }
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
    method.initialize(test, &combo);
    open_set::FreeResponse response;
    response.flagged = true;
    method.submit(response);
    assertTrue(combo.log().endsWith("reinsertCurrent empty "));
}
}
}
