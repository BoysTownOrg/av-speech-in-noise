#include "RecognitionTestModelTests.hpp"

namespace av_speech_in_noise::tests::recognition_test {
    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestWritesTestSettings
    ) {
        run(initializingFixedLevelTest);
        assertEqual(outputFile.fixedLevelTest(), &fixedLevelTestSettings());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestPassesTargetListDirectory
    ) {
        assertTargetListPassed(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestPassesNextTargetToTargetPlayerAfterLoadingFromDirectory
    ) {
        run(initializingFixedLevelTest);
        assertEqual("loadFromDirectory next ", targetList.log());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestPassesNextTargetToTargetPlayer
    ) {
        assertNextTargetPassedToPlayer(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponseLoadsNextTargetForFixedLevelTest
    ) {
        assertNextTargetPassedToPlayer(
            initializingFixedLevelTest,
            submittingCorrectResponse
        );
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponseLoadsNextTargetForAdaptiveTest
    ) {
        assertNextTargetPassedToPlayer(
            initializingAdaptiveTest,
            submittingCorrectResponse
        );
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponseLoadsNextTargetForFixedLevelTest
    ) {
        assertNextTargetPassedToPlayer(
            initializingFixedLevelTest,
            submittingIncorrectResponse
        );
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponseLoadsNextTargetForAdaptiveTest
    ) {
        assertNextTargetPassedToPlayer(
            initializingAdaptiveTest,
            submittingIncorrectResponse
        );
    }

    TEST_F(
        RecognitionTestModelTests,
        submitTypedResponseLoadsNextTargetForFixedLevelTest
    ) {
        assertNextTargetPassedToPlayer(
            initializingFixedLevelTest,
            submittingFreeResponse
        );
    }

    TEST_F(
        RecognitionTestModelTests,
        submitTypedResponseLoadsNextTargetForAdaptiveTest
    ) {
        assertNextTargetPassedToPlayer(
            initializingAdaptiveTest,
            submittingFreeResponse
        );
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponseSelectsRandomListInRangeAfterRemovingCompleteTracks
    ) {
        assertSelectsRandomListInRangeAfterRemovingCompleteTracks(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponseSubscribesToTargetPlaybackCompletionNotification
    ) {
        assertTargetPlayerPlaybackCompletionSubscribed(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponseSubscribesToTargetPlaybackCompletionNotification
    ) {
        assertTargetPlayerPlaybackCompletionSubscribed(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitTypedResponseSubscribesToTargetPlaybackCompletionNotification
    ) {
        assertTargetPlayerPlaybackCompletionSubscribed(submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponseSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        assertSeeksToRandomMaskerPositionWithinTrialDuration(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponseSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        assertSeeksToRandomMaskerPositionWithinTrialDuration(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitTypedResponseSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        assertSeeksToRandomMaskerPositionWithinTrialDuration(submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponseSeeksToRandomMaskerPosition
    ) {
        assertMaskerPlayerSeekedToRandomTime(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponseSeeksToRandomMaskerPosition
    ) {
        assertMaskerPlayerSeekedToRandomTime(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitTypedResponseSeeksToRandomMaskerPosition
    ) {
        assertMaskerPlayerSeekedToRandomTime(submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestSetsTargetPlayerLevel
    ) {
        assertSetsTargetLevel(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitTypedResponseSetsTargetPlayerLevelForAdaptiveTest
    ) {
        assertSetsTargetLevel(initializingAdaptiveTest, submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponseSetsTargetPlayerLevelForAdaptiveTest
    ) {
        assertSetsTargetLevel(initializingAdaptiveTest, submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponseSetsTargetPlayerLevelForAdaptiveTest
    ) {
        assertSetsTargetLevel(initializingAdaptiveTest, submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitFreeResponseWritesResponse
    ) {
        submittingFreeResponse.setResponse("a");
        run(submittingFreeResponse);
        assertEqual("a", writtenFreeResponseTrial().response);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitFreeResponsePassesCurrentTargetToEvaluatorBeforeAdvancingTargetForFixedLevelTest
    ) {
        run(initializingFixedLevelTest);
        initializingFixedLevelTest.setCurrentTarget("a");
        initializingFixedLevelTest.setCurrentTargetWhenNext("b");
        run(submittingFreeResponse);
        assertEqual("a", evaluator.filePathForFileName());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitFreeResponsePassesCurrentTargetToEvaluatorForAdaptiveTest
    ) {
        assertCurrentTargetPassedToEvaluator(
            initializingAdaptiveTest,
            submittingFreeResponse
        );
    }

    TEST_F(
        RecognitionTestModelTests,
        submitFreeResponseWritesTarget
    ) {
        evaluator.setFileName("a");
        run(submittingFreeResponse);
        assertEqual("a", writtenFreeResponseTrial().target);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesColorForFixedLevelTest
    ) {
        assertWritesSubjectColor(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesNumberForFixedLevelTest
    ) {
        assertWritesSubjectNumber(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesCorrectColorForFixedLevelTest
    ) {
        assertWritesCorrectColor(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesCorrectNumberForFixedLevelTest
    ) {
        assertWritesCorrectNumber(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesCorrectTrialForFixedLevelTest
    ) {
        assertWritesCorrectEvaluation(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesIncorrectTrialForAdaptiveTest
    ) {
        assertWritesIncorrectEvaluation(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesIncorrectTrialForFixedLevelTest
    ) {
        assertWritesIncorrectEvaluation(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitFreeResponseSavesOutputFileAfterWritingTrial
    ) {
        assertSavesOutputFileAfterWritingTrial(submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponsePassesSubjectResponseToEvaluatorForAdaptiveTest
    ) {
        assertCoordinateResponsePassedToEvaluator(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponsePassesSubjectResponseToEvaluatorForFixedLevelTest
    ) {
        assertCoordinateResponsePassedToEvaluator(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponsePassesPreviousTargetToEvaluatorForNumberAndColorForAdaptiveTest
    ) {
        initializingAdaptiveTest.selectList(1);
        run(initializingAdaptiveTest);
        initializingAdaptiveTest.setTargetListCurrent(1, "a");
        initializingAdaptiveTest.selectList(2);
        submitCoordinateResponse();
        assertEqual("a", evaluator.correctColorFilePath());
        assertEqual("a", evaluator.correctNumberFilePath());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponsePassesTargetToEvaluatorForNumberAndColorForFixedLevelTest
    ) {
        run(initializingFixedLevelTest);
        initializingFixedLevelTest.setCurrentTarget("a");
        initializingFixedLevelTest.setCurrentTargetWhenNext("b");
        submitCoordinateResponse();
        assertEqual("a", evaluator.correctColorFilePath());
        assertEqual("a", evaluator.correctNumberFilePath());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponsePassesTargetToEvaluatorForAdaptiveTest
    ) {
        initializingAdaptiveTest.selectList(1);
        run(initializingAdaptiveTest);
        initializingAdaptiveTest.setTargetListCurrent(1, "a");
        initializingAdaptiveTest.selectList(2);
        submitCoordinateResponse();
        assertEqual("a", evaluator.correctFilePath());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponsePassesTargetToEvaluatorForFixedLevelTest
    ) {
        run(initializingFixedLevelTest);
        initializingFixedLevelTest.setCurrentTarget("a");
        initializingFixedLevelTest.setCurrentTargetWhenNext("b");
        submitCoordinateResponse();
        assertEqual("a", evaluator.correctFilePath());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponsePushesSnrDownWhenEvaluationIsCorrectForAdaptiveTest
    ) {
        setCorrectResponse();
        assertPushesSnrTrackDown(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponsePushesSnrDownForAdaptiveTest
    ) {
        assertPushesSnrTrackDown(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponsePushesSnrUpWhenEvaluationIsIncorrectForAdaptiveTest
    ) {
        setIncorrectResponse();
        assertPushesSnrTrackUp(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponsePushesSnrUpForAdaptiveTest
    ) {
        assertPushesSnrTrackUp(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseSelectsNextListAmongThoseWithIncompleteTracksForAdaptiveTest
    ) {
        assertSelectsListAmongThoseWithIncompleteTracks(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponseSelectsNextListAmongThoseWithIncompleteTracksForAdaptiveTest
    ) {
        assertSelectsListAmongThoseWithIncompleteTracks(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponseSelectsNextListAmongThoseWithIncompleteTracksForAdaptiveTest
    ) {
        assertSelectsListAmongThoseWithIncompleteTracks(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        testCompleteWhenAllTracksComplete
    ) {
        run(initializingAdaptiveTest);
        initializingAdaptiveTest.setSnrTrackComplete(0);
        assertTestIncompleteAfterCoordinateResponse();
        initializingAdaptiveTest.setSnrTrackComplete(1);
        assertTestIncompleteAfterCoordinateResponse();
        initializingAdaptiveTest.setSnrTrackComplete(2);
        assertTestCompleteAfterCoordinateResponse();
    }

    TEST_F(
        RecognitionTestModelTests,
        fixedLevelTestCompleteAfterTrials
    ) {
        initializingFixedLevelTest.setTrials(3);
        run(initializingFixedLevelTest);
        assertTestIncompleteAfterCoordinateResponse();
        assertTestIncompleteAfterCoordinateResponse();
        assertTestCompleteAfterCoordinateResponse();
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseDoesNotLoadNextTargetWhenCompleteForFixedLevelTest
    ) {
        initializingFixedLevelTest.setNextTarget("a");
        initializingFixedLevelTest.setTrials(1);
        run(initializingFixedLevelTest);
        initializingFixedLevelTest.setNextTarget("b");
        submitCoordinateResponse();
        assertTargetFilePathEquals("a");
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseDoesNotLoadNextTargetWhenCompleteForAdaptiveTest
    ) {
        initializingAdaptiveTest.setNextTarget("a");
        run(initializingAdaptiveTest);
        initializingAdaptiveTest.setComplete();
        initializingAdaptiveTest.setNextTarget("b");
        submitCoordinateResponse();
        assertTargetFilePathEquals("a");
    }
}

