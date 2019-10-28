#include "OutputFileStub.h"
#include "ResponseEvaluatorStub.h"
#include "TargetListStub.h"
#include "TestConcluderStub.h"
#include "assert-utility.h"
#include <gtest/gtest.h>
#include <recognition-test/FixedLevelMethod.hpp>

namespace av_speech_in_noise::tests {
class UseCase {
public:
    virtual ~UseCase() = default;
    virtual void run(FixedLevelMethod &) = 0;
};

class InitializingMethod : public UseCase {
    FixedLevelTest test;
    TargetList &list;
    TestConcluder &concluder;
public:
    InitializingMethod(TargetList &list, TestConcluder &concluder) :
        list{list},
        concluder{concluder} {}

    void run(FixedLevelMethod &m) override {
        m.initialize(test, &list, &concluder);
    }
};

class SubmittingCoordinateResponse : public UseCase {
    coordinate_response_measure::Response response;
public:
    void run(FixedLevelMethod &m) override {
        m.submitResponse(response);
    }
};

class SubmittingFreeResponse : public UseCase {
    FreeResponse response;
public:
    void run(FixedLevelMethod &m) override {
        m.submitResponse(response);
    }
};

namespace {
class FixedLevelMethodTests : public ::testing::Test {
  protected:
    ResponseEvaluatorStub evaluator;
    TargetListStub targetList;
    TestConcluderStub testConcluder;
    OutputFileStub outputFile;
    FixedLevelMethodImpl method{&evaluator};
    FixedLevelTest test;
    coordinate_response_measure::Response coordinateResponse;
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

    void submitFreeResponse() {
        method.submitResponse(freeResponse);
    }

    void writeLastCoordinateResponse() {
        method.writeLastCoordinateResponse(&outputFile);
    }

    auto blueColor() { return coordinate_response_measure::Color::blue; }

    auto writtenFixedLevelTrial() {
        return outputFile.writtenFixedLevelTrial();
    }

    bool writtenFixedLevelTrialCorrect() {
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

    bool testComplete() { return method.complete(); }

    void assertTestComplete() { assertTrue(testComplete()); }

    void setTestComplete() { testConcluder.setComplete(); }

    void setTestIncomplete() { testConcluder.setIncomplete(); }

    void run(UseCase &useCase) {
        useCase.run(method);
    }

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

    void assertConcluderSubmitResponseCalledBeforeComplete() {
        assertTrue(testConcluder.log().contains("submitResponse complete"));
    }

    void assertConcluderInitializeBeforeComplete() {
        assertTrue(testConcluder.log().contains("initialize complete"));
    }
};

TEST_F(FixedLevelMethodTests, initializePassesTestParametersToConcluder) {
    initialize();
    assertEqual(&std::as_const(test), testConcluder.test());
}

TEST_F(FixedLevelMethodTests, initializePassesTargetListDirectory) {
    test.targetListDirectory = "a";
    initialize();
    assertEqual("a", targetList.directory());
}

TEST_F(FixedLevelMethodTests, nextReturnsNextTarget) {
    initialize();
    targetList.setNext("a");
    assertEqual("a", method.next());
}

TEST_F(FixedLevelMethodTests, snrReturnsInitializedSnr) {
    test.snr_dB = 1;
    initialize();
    assertEqual(1, method.snr_dB());
}

TEST_F(FixedLevelMethodTests, writeCoordinateResponsePassesSubjectColor) {
    coordinateResponse.color = blueColor();
    writeCoordinateResponse();
    assertEqual(blueColor(), writtenFixedLevelTrial().subjectColor);
}

TEST_F(FixedLevelMethodTests, writeCoordinateResponsePassesSubjectNumber) {
    coordinateResponse.number = 1;
    writeCoordinateResponse();
    assertEqual(1, writtenFixedLevelTrial().subjectNumber);
}

TEST_F(FixedLevelMethodTests, writeCoordinateResponsePassesCorrectColor) {
    evaluator.setCorrectColor(blueColor());
    writeCoordinateResponse();
    assertEqual(blueColor(), writtenFixedLevelTrial().correctColor);
}

TEST_F(FixedLevelMethodTests, writeCoordinateResponsePassesCorrectNumber) {
    evaluator.setCorrectNumber(1);
    writeCoordinateResponse();
    assertEqual(1, writtenFixedLevelTrial().correctNumber);
}

TEST_F(FixedLevelMethodTests, writeCoordinateResponsePassesStimulus) {
    setCurrentTarget("a");
    writeCoordinateResponse();
    assertEqual("a", writtenFixedLevelTrial().target);
}

TEST_F(FixedLevelMethodTests, writeCorrectCoordinateResponse) {
    evaluator.setCorrect();
    writeCoordinateResponse();
    assertTrue(writtenFixedLevelTrialCorrect());
}

TEST_F(FixedLevelMethodTests, writeIncorrectCoordinateResponse) {
    evaluator.setIncorrect();
    writeCoordinateResponse();
    assertFalse(writtenFixedLevelTrialCorrect());
}

TEST_F(FixedLevelMethodTests, writeTestPassesSettings) {
    initialize();
    method.writeTestingParameters(&outputFile);
    assertEqual(&std::as_const(test), outputFile.fixedLevelTest());
}

TEST_F(FixedLevelMethodTests, submitCoordinateResponsePassesResponse) {
    initialize();
    submitCoordinateResponse();
    assertEqual(&std::as_const(coordinateResponse), evaluator.response());
}

TEST_F(
    FixedLevelMethodTests, submitCoordinateResponseSubmitsResponseToConcluder) {
    initialize();
    submitCoordinateResponse();
    assertTrue(testConcluder.responseSubmitted());
}

TEST_F(
    FixedLevelMethodTests, submitCoordinateResponsePassesCurrentToEvaluator) {
    initialize();
    setCurrentTarget("a");
    submitCoordinateResponse();
    assertEqual("a", evaluator.correctColorFilePath());
    assertEqual("a", evaluator.correctNumberFilePath());
}

TEST_F(FixedLevelMethodTests,
    submitCoordinateResponsePassesCorrectTargetToEvaluator) {
    initialize();
    setCurrentTarget("a");
    submitCoordinateResponse();
    assertEqual("a", evaluator.correctFilePath());
}

TEST_F(FixedLevelMethodTests, completeWhenTestCompleteAfterCoordinateResponse) {
    assertTestCompleteWhenComplete(submittingCoordinateResponse);
}

TEST_F(FixedLevelMethodTests, completeWhenTestCompleteAfterFreeResponse) {
    assertTestCompleteWhenComplete(submittingFreeResponse);
}

TEST_F(FixedLevelMethodTests, completeWhenTestCompleteAfterInitializing) {
    assertTestCompleteWhenComplete_(initializingMethod);
}

TEST_F(FixedLevelMethodTests, submitCoordinateResponsePassesTargetListToConcluder) {
    assertTargetListPassedToConcluder(submittingCoordinateResponse);
}

TEST_F(FixedLevelMethodTests, submitFreeResponsePassesTargetListToConcluder) {
    assertTargetListPassedToConcluder(submittingFreeResponse);
}

TEST_F(FixedLevelMethodTests, initializePassesTargetListToConcluder) {
    assertTargetListPassedToConcluder_(initializingMethod);
}

TEST_F(FixedLevelMethodTests, submitCoordinateResponseSubmitsResponsePriorToQueryingCompletion) {
    initialize();
    run(submittingCoordinateResponse);
    assertConcluderSubmitResponseCalledBeforeComplete();
}

TEST_F(FixedLevelMethodTests, initializeInitializesConcluderBeforeQueryingCompletion) {
    run(initializingMethod);
    assertConcluderInitializeBeforeComplete();
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
} // namespace
} // namespace av_speech_in_noise::tests
