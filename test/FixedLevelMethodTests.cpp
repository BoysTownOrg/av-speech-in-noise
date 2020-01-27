#include "LogString.hpp"
#include "OutputFileStub.hpp"
#include "ResponseEvaluatorStub.hpp"
#include "TargetListStub.h"
#include "TestConcluderStub.h"
#include "assert-utility.hpp"
#include "av-speech-in-noise/Model.hpp"
#include <gtest/gtest.h>
#include <recognition-test/FixedLevelMethod.hpp>

namespace av_speech_in_noise::tests {
namespace {
class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run(FixedLevelMethod &) = 0;
};

class InitializingMethod : public UseCase {
    FixedLevelTest test_;
    TargetList &list;
    TestConcluder &concluder;

  public:
    InitializingMethod(TargetList &list, TestConcluder &concluder)
        : list{list}, concluder{concluder} {}

    void run(FixedLevelMethod &m) override {
        m.initialize(test_, &list, &concluder);
    }

    [[nodiscard]] auto test() const -> auto & { return test_; }

    void setTargetListDirectory(std::string s) {
        test_.targetListDirectory = std::move(s);
    }

    void setSnr(int x) { test_.snr_dB = x; }
};

class SubmittingCoordinateResponse : public UseCase {
    coordinate_response_measure::Response response_{};

  public:
    void run(FixedLevelMethod &m) override { m.submitResponse(response_); }

    void setColor(coordinate_response_measure::Color c) { response_.color = c; }

    void setNumber(int n) { response_.number = n; }

    [[nodiscard]] auto response() const -> auto & { return response_; }
};

class SubmittingFreeResponse : public UseCase {
    open_set::FreeResponse response{};

  public:
    void run(FixedLevelMethod &m) override { m.submitResponse(response); }
    void setFlagged() { response.flagged = true; }
};

auto blueColor() { return coordinate_response_measure::Color::blue; }

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

    FixedLevelMethodTests() { run(initializingMethod); }

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

    void run(UseCase &useCase) { useCase.run(method); }

    void assertTargetListPassedToConcluderAfter(UseCase &useCase) {
        run(useCase);
        assertTestConcluderPassedTargetList();
    }

    void assertTestConcluderPassedTargetList() {
        assertEqual(
            static_cast<TargetList *>(&targetList), testConcluder.targetList());
    }

    void assertTestCompleteOnlyWhenComplete(UseCase &useCase) {
        run(useCase);
        assertTestIncomplete();
        assertTestCompleteWhenComplete(useCase);
    }

    void assertTestCompleteWhenComplete(UseCase &useCase) {
        setTestComplete();
        run(useCase);
        assertTestComplete();
    }

    void assertTestConcluderLogContains(const std::string &s) {
        assertTrue(testConcluder.log().contains(s));
    }

    void assertTestConcluderLogContainsAfter(
        const std::string &s, UseCase &useCase) {
        run(useCase);
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
    targetList.setNext("a");
    assertEqual("a", method.nextTarget());
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesSubjectColor) {
    submittingCoordinateResponse.setColor(blueColor());
    run(submittingCoordinateResponse);
    writeLastCoordinateResponse();
    assertEqual(blueColor(), writtenFixedLevelTrial().subjectColor);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesSubjectNumber) {
    submittingCoordinateResponse.setNumber(1);
    run(submittingCoordinateResponse);
    writeLastCoordinateResponse();
    assertEqual(1, writtenFixedLevelTrial().subjectNumber);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesCorrectColor) {
    evaluator.setCorrectColor(blueColor());
    run(submittingCoordinateResponse);
    writeLastCoordinateResponse();
    assertEqual(blueColor(), writtenFixedLevelTrial().correctColor);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesCorrectNumber) {
    evaluator.setCorrectNumber(1);
    run(submittingCoordinateResponse);
    writeLastCoordinateResponse();
    assertEqual(1, writtenFixedLevelTrial().correctNumber);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesStimulus) {
    setCurrentTarget("a");
    run(submittingCoordinateResponse);
    writeLastCoordinateResponse();
    assertEqual("a", writtenFixedLevelTrial().target);
}

FIXED_LEVEL_METHOD_TEST(writeCorrectCoordinateResponse) {
    evaluator.setCorrect();
    run(submittingCoordinateResponse);
    writeLastCoordinateResponse();
    assertTrue(writtenFixedLevelTrialCorrect());
}

FIXED_LEVEL_METHOD_TEST(writeIncorrectCoordinateResponse) {
    evaluator.setIncorrect();
    run(submittingCoordinateResponse);
    writeLastCoordinateResponse();
    assertFalse(writtenFixedLevelTrialCorrect());
}

FIXED_LEVEL_METHOD_TEST(writeTestPassesSettings) {
    method.writeTestingParameters(&outputFile);
    assertEqual(&initializingMethod.test(), outputFile.fixedLevelTest());
}

FIXED_LEVEL_METHOD_TEST(submitCoordinateResponsePassesResponse) {
    run(submittingCoordinateResponse);
    assertEqual(&submittingCoordinateResponse.response(), evaluator.response());
}

FIXED_LEVEL_METHOD_TEST(submitCoordinateResponseSubmitsResponseToConcluder) {
    run(submittingCoordinateResponse);
    assertTrue(testConcluder.responseSubmitted());
}

FIXED_LEVEL_METHOD_TEST(submitFreeResponseSubmitsResponseToConcluder) {
    run(submittingFreeResponse);
    assertTrue(testConcluder.responseSubmitted());
}

FIXED_LEVEL_METHOD_TEST(submitCoordinateResponsePassesCurrentToEvaluator) {
    setCurrentTarget("a");
    run(submittingCoordinateResponse);
    assertEqual("a", evaluator.correctColorFilePath());
    assertEqual("a", evaluator.correctNumberFilePath());
}

FIXED_LEVEL_METHOD_TEST(
    submitCoordinateResponsePassesCorrectTargetToEvaluator) {
    setCurrentTarget("a");
    run(submittingCoordinateResponse);
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
    run(submittingFreeResponse);
    assertCurrentTargetNotReinserted();
}

FIXED_LEVEL_METHOD_TEST(submitFreeResponseReinsertsCurrentTargetIfFlagged) {
    submittingFreeResponse.setFlagged();
    run(submittingFreeResponse);
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
    TestConcluderStub testConcluder;
    FixedLevelMethodImpl method{&evaluator};
    InitializingMethod initializingMethod{targetList, testConcluder};

    void run(UseCase &useCase) { useCase.run(method); }
};

TEST_F(PreInitializedFixedLevelMethodTests, snrReturnsInitializedSnr) {
    initializingMethod.setSnr(1);
    run(initializingMethod);
    assertEqual(1, method.snr_dB());
}

TEST_F(
    PreInitializedFixedLevelMethodTests, initializePassesTargetListDirectory) {
    initializingMethod.setTargetListDirectory("a");
    run(initializingMethod);
    assertEqual("a", targetList.directory());
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
