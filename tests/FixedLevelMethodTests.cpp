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
};

class SubmittingCoordinateResponse : public UseCase {
    coordinate_response_measure::Response response{};

  public:
    void run(FixedLevelMethod &m) override { m.submitResponse(response); }
};

class SubmittingFreeResponse : public UseCase {
    FreeResponse response;

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
    FixedLevelTest test;
    coordinate_response_measure::Response coordinateResponse{};
    FreeResponse freeResponse;
    SubmittingCoordinateResponse submittingCoordinateResponse;
    SubmittingFreeResponse submittingFreeResponse;
    InitializingMethod initializingMethod{targetList, testConcluder};

    void initialize() { method.initialize(test, &targetList, &testConcluder); }

    void writeCoordinateResponse() {
        initialize();
        submitCoordinateResponse();
        writeLastCoordinateResponse();
    }

    void submitCoordinateResponse() {
        method.submitResponse(coordinateResponse);
    }

    void submitFreeResponse() { method.submitResponse(freeResponse); }

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

    void assertTestIncompleteAfter(UseCase &useCase) {
        run(useCase);
        assertTestIncomplete();
    }

    void assertTestCompleteAfter(UseCase &useCase) {
        run(useCase);
        assertTestComplete();
    }

    void assertTestIncomplete() { assertFalse(testComplete()); }

    auto testComplete() -> bool { return method.complete(); }

    void assertTestComplete() { assertTrue(testComplete()); }

    void setTestComplete() { testConcluder.setComplete(); }

    void setTestIncomplete() { testConcluder.setIncomplete(); }

    void run(UseCase &useCase) { useCase.run(method); }

    void assertTargetListPassedToConcluder(UseCase &useCase) {
        initialize();
        assertTargetListPassedToConcluder_(useCase);
    }

    void assertTargetListPassedToConcluder_(UseCase &useCase) {
        run(useCase);
        assertEqual(
            static_cast<TargetList *>(&targetList), testConcluder.targetList());
    }

    void assertTestCompleteWhenComplete(UseCase &useCase) {
        initialize();
        assertTestCompleteWhenComplete_(useCase);
    }

    void assertTestCompleteWhenComplete_(UseCase &useCase) {
        assertTestIncompleteAfter(useCase);
        setTestComplete();
        assertTestCompleteAfter(useCase);
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

FIXED_LEVEL_METHOD_TEST(initializePassesTestParametersToConcluder) {
    run(initializingMethod);
    assertEqual(&initializingMethod.test(), testConcluder.test());
}

FIXED_LEVEL_METHOD_TEST(initializePassesTargetListDirectory) {
    test.targetListDirectory = "a";
    initialize();
    assertEqual("a", targetList.directory());
}

FIXED_LEVEL_METHOD_TEST(nextReturnsNextTarget) {
    initialize();
    targetList.setNext("a");
    assertEqual("a", method.next());
}

FIXED_LEVEL_METHOD_TEST(snrReturnsInitializedSnr) {
    test.snr_dB = 1;
    initialize();
    assertEqual(1, method.snr_dB());
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesSubjectColor) {
    coordinateResponse.color = blueColor();
    writeCoordinateResponse();
    assertEqual(blueColor(), writtenFixedLevelTrial().subjectColor);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesSubjectNumber) {
    coordinateResponse.number = 1;
    writeCoordinateResponse();
    assertEqual(1, writtenFixedLevelTrial().subjectNumber);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesCorrectColor) {
    evaluator.setCorrectColor(blueColor());
    writeCoordinateResponse();
    assertEqual(blueColor(), writtenFixedLevelTrial().correctColor);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesCorrectNumber) {
    evaluator.setCorrectNumber(1);
    writeCoordinateResponse();
    assertEqual(1, writtenFixedLevelTrial().correctNumber);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesStimulus) {
    setCurrentTarget("a");
    writeCoordinateResponse();
    assertEqual("a", writtenFixedLevelTrial().target);
}

FIXED_LEVEL_METHOD_TEST(writeCorrectCoordinateResponse) {
    evaluator.setCorrect();
    writeCoordinateResponse();
    assertTrue(writtenFixedLevelTrialCorrect());
}

FIXED_LEVEL_METHOD_TEST(writeIncorrectCoordinateResponse) {
    evaluator.setIncorrect();
    writeCoordinateResponse();
    assertFalse(writtenFixedLevelTrialCorrect());
}

FIXED_LEVEL_METHOD_TEST(writeTestPassesSettings) {
    initialize();
    method.writeTestingParameters(&outputFile);
    assertEqual(&std::as_const(test), outputFile.fixedLevelTest());
}

FIXED_LEVEL_METHOD_TEST(submitCoordinateResponsePassesResponse) {
    initialize();
    submitCoordinateResponse();
    assertEqual(&std::as_const(coordinateResponse), evaluator.response());
}

FIXED_LEVEL_METHOD_TEST(submitCoordinateResponseSubmitsResponseToConcluder) {
    initialize();
    submitCoordinateResponse();
    assertTrue(testConcluder.responseSubmitted());
}

FIXED_LEVEL_METHOD_TEST(submitCoordinateResponsePassesCurrentToEvaluator) {
    initialize();
    setCurrentTarget("a");
    submitCoordinateResponse();
    assertEqual("a", evaluator.correctColorFilePath());
    assertEqual("a", evaluator.correctNumberFilePath());
}

FIXED_LEVEL_METHOD_TEST(
    submitCoordinateResponsePassesCorrectTargetToEvaluator) {
    initialize();
    setCurrentTarget("a");
    submitCoordinateResponse();
    assertEqual("a", evaluator.correctFilePath());
}

FIXED_LEVEL_METHOD_TEST(completeWhenTestCompleteAfterCoordinateResponse) {
    assertTestCompleteWhenComplete(submittingCoordinateResponse);
}

FIXED_LEVEL_METHOD_TEST(completeWhenTestCompleteAfterFreeResponse) {
    assertTestCompleteWhenComplete(submittingFreeResponse);
}

FIXED_LEVEL_METHOD_TEST(completeWhenTestCompleteAfterInitializing) {
    assertTestCompleteWhenComplete_(initializingMethod);
}

FIXED_LEVEL_METHOD_TEST(submitCoordinateResponsePassesTargetListToConcluder) {
    assertTargetListPassedToConcluder(submittingCoordinateResponse);
}

FIXED_LEVEL_METHOD_TEST(submitFreeResponsePassesTargetListToConcluder) {
    assertTargetListPassedToConcluder(submittingFreeResponse);
}

FIXED_LEVEL_METHOD_TEST(initializePassesTargetListToConcluder) {
    assertTargetListPassedToConcluder_(initializingMethod);
}

FIXED_LEVEL_METHOD_TEST(
    submitCoordinateResponseSubmitsResponsePriorToQueryingCompletion) {
    initialize();
    assertTestConcluderLogContainsAfter(
        "submitResponse complete", submittingCoordinateResponse);
}

FIXED_LEVEL_METHOD_TEST(
    initializeInitializesConcluderBeforeQueryingCompletion) {
    assertTestConcluderLogContainsAfter(
        "initialize complete", initializingMethod);
}

FIXED_LEVEL_METHOD_TEST(submitFreeResponseDoesNotReinsertCurrentTarget) {
    initialize();
    run(submittingFreeResponse);
    assertCurrentTargetNotReinserted();
}

FIXED_LEVEL_METHOD_TEST(submitFreeResponseReinsertsCurrentTargetIfFlagged) {
    initialize();
    submittingFreeResponse.setFlagged();
    run(submittingFreeResponse);
    assertCurrentTargetReinserted();
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
    FreeResponse response;
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

    bool complete() { return testConcluder.complete({}); }

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

    bool complete() { return testConcluder.complete(&targetList); }

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
