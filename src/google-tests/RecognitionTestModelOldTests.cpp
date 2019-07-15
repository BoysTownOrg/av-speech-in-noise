#include "RecognitionTestModelOldTests.hpp"

namespace av_speech_in_noise::tests::recognition_test {
    TEST_F(
        RecognitionTestModelOldTests,
        submitCorrectResponseLoadsNextTargetForFixedLevelTest
    ) {
        assertNextTargetPassedToPlayer(
            initializingFixedLevelTest,
            submittingCorrectResponse
        );
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitCorrectResponseLoadsNextTargetForAdaptiveTest
    ) {
        assertNextTargetPassedToPlayer(
            initializingAdaptiveTest,
            submittingCorrectResponse
        );
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitIncorrectResponseLoadsNextTargetForFixedLevelTest
    ) {
        assertNextTargetPassedToPlayer(
            initializingFixedLevelTest,
            submittingIncorrectResponse
        );
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitIncorrectResponseLoadsNextTargetForAdaptiveTest
    ) {
        assertNextTargetPassedToPlayer(
            initializingAdaptiveTest,
            submittingIncorrectResponse
        );
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitTypedResponseLoadsNextTargetForFixedLevelTest
    ) {
        assertNextTargetPassedToPlayer(
            initializingFixedLevelTest,
            submittingFreeResponse
        );
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitTypedResponseLoadsNextTargetForAdaptiveTest
    ) {
        assertNextTargetPassedToPlayer(
            initializingAdaptiveTest,
            submittingFreeResponse
        );
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitIncorrectResponseSelectsRandomListInRangeAfterRemovingCompleteTracks
    ) {
        assertSelectsRandomListInRangeAfterRemovingCompleteTracks(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitCorrectResponseSubscribesToTargetPlaybackCompletionNotification
    ) {
        assertTargetPlayerPlaybackCompletionSubscribed(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitIncorrectResponseSubscribesToTargetPlaybackCompletionNotification
    ) {
        assertTargetPlayerPlaybackCompletionSubscribed(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitTypedResponseSubscribesToTargetPlaybackCompletionNotification
    ) {
        assertTargetPlayerPlaybackCompletionSubscribed(submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitCorrectResponseSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        assertSeeksToRandomMaskerPositionWithinTrialDuration(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitIncorrectResponseSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        assertSeeksToRandomMaskerPositionWithinTrialDuration(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitTypedResponseSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        assertSeeksToRandomMaskerPositionWithinTrialDuration(submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitCorrectResponseSeeksToRandomMaskerPosition
    ) {
        assertMaskerPlayerSeekedToRandomTime(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitIncorrectResponseSeeksToRandomMaskerPosition
    ) {
        assertMaskerPlayerSeekedToRandomTime(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitTypedResponseSeeksToRandomMaskerPosition
    ) {
        assertMaskerPlayerSeekedToRandomTime(submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitTypedResponseSetsTargetPlayerLevelForAdaptiveTest
    ) {
        assertSetsTargetLevel(initializingAdaptiveTest, submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitCorrectResponseSetsTargetPlayerLevelForAdaptiveTest
    ) {
        assertSetsTargetLevel(initializingAdaptiveTest, submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitIncorrectResponseSetsTargetPlayerLevelForAdaptiveTest
    ) {
        assertSetsTargetLevel(initializingAdaptiveTest, submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitFreeResponseWritesResponse
    ) {
        submittingFreeResponse.setResponse("a");
        run(submittingFreeResponse);
        assertEqual("a", writtenFreeResponseTrial().response);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitFreeResponsePassesCurrentTargetToEvaluatorBeforeAdvancingTargetForFixedLevelTest
    ) {
        run(initializingFixedLevelTest);
        initializingFixedLevelTest.setCurrentTarget("a");
        initializingFixedLevelTest.setCurrentTargetWhenNext("b");
        run(submittingFreeResponse);
        assertEqual("a", evaluator.filePathForFileName());
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitFreeResponsePassesCurrentTargetToEvaluatorForAdaptiveTest
    ) {
        assertCurrentTargetPassedToEvaluator(
            initializingAdaptiveTest,
            submittingFreeResponse
        );
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitFreeResponseWritesTarget
    ) {
        evaluator.setFileName("a");
        run(submittingFreeResponse);
        assertEqual("a", writtenFreeResponseTrial().target);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitFreeResponseSavesOutputFileAfterWritingTrial
    ) {
        assertSavesOutputFileAfterWritingTrial(submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
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
        RecognitionTestModelOldTests,
        submitCoordinateResponsePassesTargetToEvaluatorForFixedLevelTest
    ) {
        run(initializingFixedLevelTest);
        initializingFixedLevelTest.setCurrentTarget("a");
        initializingFixedLevelTest.setCurrentTargetWhenNext("b");
        submitCoordinateResponse();
        assertEqual("a", evaluator.correctFilePath());
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitCorrectResponsePushesSnrDownForAdaptiveTest
    ) {
        assertPushesSnrTrackDown(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitIncorrectResponsePushesSnrUpForAdaptiveTest
    ) {
        assertPushesSnrTrackUp(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitCorrectResponseSelectsNextListAmongThoseWithIncompleteTracksForAdaptiveTest
    ) {
        assertSelectsListAmongThoseWithIncompleteTracks(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitIncorrectResponseSelectsNextListAmongThoseWithIncompleteTracksForAdaptiveTest
    ) {
        assertSelectsListAmongThoseWithIncompleteTracks(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        fixedLevelTestCompleteAfterTrials
    ) {
        initializingFixedLevelTest.setTrials(3);
        run(initializingFixedLevelTest);
        assertTestIncompleteAfterCoordinateResponse();
        assertTestIncompleteAfterCoordinateResponse();
        assertTestCompleteAfterCoordinateResponse();
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitCoordinateResponseDoesNotLoadNextTargetWhenCompleteForFixedLevelTest
    ) {
        initializingFixedLevelTest.setNextTarget("a");
        initializingFixedLevelTest.setTrials(1);
        run(initializingFixedLevelTest);
        initializingFixedLevelTest.setNextTarget("b");
        submitCoordinateResponse();
        assertTargetFilePathEquals("a");
    }
}

