#include "RecognitionTestModelTests.hpp"

namespace av_speech_in_noise::tests {
    TEST_F(RecognitionTestModelTests, subscribesToPlayerEvents) {
        assertEqual(
            static_cast<TargetPlayer::EventListener *>(&model),
            targetPlayer.listener()
        );
        assertEqual(
            static_cast<MaskerPlayer::EventListener *>(&model),
            maskerPlayer.listener()
        );
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestHidesTargetVideoWhenAuditoryOnly
    ) {
        assertTargetVideoHiddenWhenAuditoryOnly(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestHidesTargetVideoWhenAuditoryOnly
    ) {
        assertTargetVideoHiddenWhenAuditoryOnly(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationHidesTargetVideoWhenAuditoryOnly
    ) {
        assertTargetVideoHiddenWhenAuditoryOnly(playingCalibration);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestShowsTargetVideoWhenAudioVisual
    ) {
        assertTargetVideoShownWhenAudioVisual(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestShowsTargetVideoWhenAudioVisual
    ) {
        assertTargetVideoShownWhenAudioVisual(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationShowsTargetVideoWhenAudioVisual
    ) {
        assertTargetVideoShownWhenAudioVisual(playingCalibration);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestCreatesEachSnrTrackWithStartingSnr
    ) {
        assertSnrPassedToTrackFactory(settingStartingSnr);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestCreatesEachSnrTrackWithCeilingSnr
    ) {
        assertSnrPassedToTrackFactory(settingCeilingSnr);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestClosesOutputFileOpensAndWritesTestInOrder
    ) {
        assertClosesOutputFileOpensAndWritesTestInOrder(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestClosesOutputFileOpensAndWritesTestInOrder
    ) {
        assertClosesOutputFileOpensAndWritesTestInOrder(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestOpensNewOutputFilePassingTestInformation
    ) {
        assertOutputFilePassedTestInformation(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestOpensNewOutputFilePassingTestInformation
    ) {
        assertOutputFilePassedTestInformation(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestWritesTestSettings
    ) {
        run(initializingAdaptiveTest);
        assertEqual(outputFile.adaptiveTest(), &adaptiveTestSettings());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestWritesTestSettings
    ) {
        run(initializingFixedLevelTest);
        assertEqual(outputFile.fixedLevelTest(), &fixedLevelTestSettings());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestPassesTargetListDirectory
    ) {
        assertTargetListPassed(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestPassesTargetListDirectory
    ) {
        assertTargetListPassed(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialPassesAudioDeviceToTargetPlayer
    ) {
        assertDevicePassedToTargetPlayer(playingTrial);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationPassesAudioDeviceToTargetPlayer
    ) {
        assertDevicePassedToTargetPlayer(playingCalibration);
    }

    TEST_F(RecognitionTestModelTests, playTrialPassesAudioDeviceToMaskerPlayer) {
        assertDevicePassedToMaskerPlayer(playingTrial);
    }

    TEST_F(RecognitionTestModelTests, playTrialFadesInMasker) {
        playTrial();
        assertTrue(maskerPlayerFadedIn());
    }

    TEST_F(RecognitionTestModelTests, playCalibrationPlaysTarget) {
        playCalibration();
        assertTargetPlayerPlayed();
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestQueriesTargetRmsAfterLoadingFile
    ) {
        assertTargetFileLoadedPriorToRmsQuery(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestQueriesTargetRmsAfterLoadingFile
    ) {
        assertTargetFileLoadedPriorToRmsQuery(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationQueriesTargetRmsAfterLoadingFile
    ) {
        assertTargetFileLoadedPriorToRmsQuery(playingCalibration);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseQueriesTargetRmsAfterLoadingFile
    ) {
        assertTargetFileLoadedPriorToRmsQuery(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponseQueriesTargetRmsAfterLoadingFile
    ) {
        assertTargetFileLoadedPriorToRmsQuery(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponseQueriesTargetRmsAfterLoadingFile
    ) {
        assertTargetFileLoadedPriorToRmsQuery(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitTypedResponseQueriesTargetRmsAfterLoadingFile
    ) {
        assertTargetFileLoadedPriorToRmsQuery(submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestPassesNextTargetToTargetPlayerAfterLoadingFromDirectory
    ) {
        run(initializingFixedLevelTest);
        assertEqual("loadFromDirectory next ", finiteTargetList.log());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestPassesNextTargetToTargetPlayer
    ) {
        assertNextTargetPassedToPlayer(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestPassesNextTargetToTargetPlayer
    ) {
        assertNextTargetPassedToPlayer(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseLoadsNextTargetForFixedLevelTest
    ) {
        assertNextTargetPassedToPlayer(
            initializingFixedLevelTest,
            submittingCoordinateResponse
        );
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseLoadsNextTargetForAdaptiveTest
    ) {
        assertNextTargetPassedToPlayer(
            initializingAdaptiveTest,
            submittingCoordinateResponse
        );
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
        initializeAdaptiveTestSelectsRandomListInRange
    ) {
        run(initializingAdaptiveTest);
        assertRandomizerPassedIntegerBounds(0, 2);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseSelectsRandomListInRangeAfterRemovingCompleteTracks
    ) {
        assertSelectsRandomListInRangeAfterRemovingCompleteTracks(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponseSelectsRandomListInRangeAfterRemovingCompleteTracks
    ) {
        assertSelectsRandomListInRangeAfterRemovingCompleteTracks(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponseSelectsRandomListInRangeAfterRemovingCompleteTracks
    ) {
        assertSelectsRandomListInRangeAfterRemovingCompleteTracks(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationPassesAudioFileToTargetPlayer
    ) {
        playingCalibration.setFilePath("a");
        playCalibration();
        assertTargetFilePathEquals("a");
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestPassesMaskerFilePathToMaskerPlayer
    ) {
        assertMaskerFilePathPassedToPlayer(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestPassesMaskerFilePathToMaskerPlayer
    ) {
        assertMaskerFilePathPassedToPlayer(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestSubscribesToTargetPlaybackCompletionNotification
    ) {
        assertTargetPlayerPlaybackCompletionSubscribed(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestSubscribesToTargetPlaybackCompletionNotification
    ) {
        assertTargetPlayerPlaybackCompletionSubscribed(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseSubscribesToTargetPlaybackCompletionNotification
    ) {
        assertTargetPlayerPlaybackCompletionSubscribed(submittingCoordinateResponse);
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
        initializeAdaptiveTestSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        assertSeeksToRandomMaskerPositionWithinTrialDuration(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        assertSeeksToRandomMaskerPositionWithinTrialDuration(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        assertSeeksToRandomMaskerPositionWithinTrialDuration(submittingCoordinateResponse);
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
        initializeAdaptiveTestSeeksToRandomMaskerPosition
    ) {
        assertMaskerPlayerSeekedToRandomTime(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestSeeksToRandomMaskerPosition
    ) {
        assertMaskerPlayerSeekedToRandomTime(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseSeeksToRandomMaskerPosition
    ) {
        assertMaskerPlayerSeekedToRandomTime(submittingCoordinateResponse);
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
        initializeAdaptiveTestSetsInitialMaskerPlayerLevel
    ) {
        assertMaskerPlayerLevelSet(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestSetsInitialMaskerPlayerLevel
    ) {
        assertMaskerPlayerLevelSet(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestSetsTargetPlayerLevel
    ) {
        assertSetsTargetLevel(initializingAdaptiveTest);
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
        submitCoordinateResponseSetsTargetPlayerLevelForAdaptiveTest
    ) {
        assertSetsTargetLevel(initializingAdaptiveTest, submittingCoordinateResponse);
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
        playCalibrationSetsTargetPlayerLevel
    ) {
        playingCalibration.setLevel_dB_SPL(1);
        playingCalibration.setFullScaleLevel_dB_SPL(2);
        setTargetPlayerRms(3);
        playCalibration();
        assertEqual(1 - 2 - dB(3), targetPlayerLevel_dB());
    }

    TEST_F(RecognitionTestModelTests, fadeInCompletePlaysTarget) {
        maskerPlayer.fadeInComplete();
        assertTargetPlayerPlayed();
    }

    TEST_F(RecognitionTestModelTests, targetPlaybackCompleteFadesOutMasker) {
        targetPlayer.playbackComplete();
        assertTrue(maskerPlayer.fadeOutCalled());
    }

    TEST_F(RecognitionTestModelTests, fadeOutCompleteNotifiesTrialComplete) {
        maskerPlayer.fadeOutComplete();
        assertTrue(listener.notified());
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
        submitFreeResponsePassesCurrentTargetToEvaluatorForFixedLevelTest
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
        submitCoordinateResponseWritesColorForAdaptiveTest
    ) {
        assertWritesSubjectColor(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesColorForFixedLevelTest
    ) {
        assertWritesSubjectColor(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesNumberForAdaptiveTest
    ) {
        assertWritesSubjectNumber(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesNumberForFixedLevelTest
    ) {
        assertWritesSubjectNumber(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesReversalsForAdaptiveTestAfterUpdatingTrack
    ) {
        initializingAdaptiveTest.selectList(1);
        run(initializingAdaptiveTest);
        initializingAdaptiveTest.snrTrack(1)->setReversalsWhenUpdated(3);
        initializingAdaptiveTest.selectList(2);
        run(submittingCoordinateResponse);
        assertEqual(3, writtenAdaptiveCoordinateResponseTrial().reversals);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesSnrBeforeUpdatingForAdaptiveTest
    ) {
        initializingAdaptiveTest.selectList(1);
        run(initializingAdaptiveTest);
        initializingAdaptiveTest.snrTrack(1)->setX(4);
        initializingAdaptiveTest.snrTrack(1)->setXWhenUpdated(3);
        initializingAdaptiveTest.selectList(2);
        submitCoordinateResponse();
        assertEqual(4, writtenAdaptiveCoordinateResponseTrial().SNR_dB);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesCorrectColorForAdaptiveTest
    ) {
        assertWritesCorrectColor(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesCorrectColorForFixedLevelTest
    ) {
        assertWritesCorrectColor(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesCorrectNumberForAdaptiveTest
    ) {
        assertWritesCorrectNumber(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesCorrectNumberForFixedLevelTest
    ) {
        assertWritesCorrectNumber(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesCorrectTrialForAdaptiveTest
    ) {
        assertWritesCorrectEvaluation(initializingAdaptiveTest);
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
        submitCoordinateResponseSavesOutputFileAfterWritingTrial
    ) {
        assertSavesOutputFileAfterWritingTrial(submittingCoordinateResponse);
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
        assertCoordinateResponsePassesCurrentTargetToEvaluatorForDeterminingResponseCorrectness(initializingAdaptiveTest);

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
        initializeAdaptiveTestThrowsRequestFailureIfFileFailsToOpen
    ) {
        assertThrowsRequestFailureWhenOutputFileThrows(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestThrowsRequestFailureIfFileFailsToOpen
    ) {
        assertThrowsRequestFailureWhenOutputFileThrows(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationThrowsRequestFailureWhenTargetPlayerThrowsInvalidAudioFile
    ) {
        playingCalibration.setFilePath("a");
        targetPlayer.throwInvalidAudioFileOnRms();
        assertCallThrowsRequestFailure(playingCalibration, "unable to read a");
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialWithInvalidAudioDeviceThrowsRequestFailure
    ) {
        assertThrowsRequestFailureWhenInvalidAudioDevice(playingTrial);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationWithInvalidAudioDeviceThrowsRequestFailure
    ) {
        assertThrowsRequestFailureWhenInvalidAudioDevice(playingCalibration);
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialDoesNotChangeAudioDeviceWhenTrialInProgress
    ) {
        playTrialWhenTrialAlreadyInProgressIgnoringFailure();
        assertFalse(maskerPlayer.setDeviceCalled());
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationDoesNotChangeAudioDeviceWhenTrialInProgress
    ) {
        playCalibrationWhenTrialAlreadyInProgressIgnoringFailure();
        assertFalse(targetPlayer.setDeviceCalled());
    }

    TEST_F(RecognitionTestModelTests, playTrialDoesNotPlayIfTrialInProgress) {
        playTrialWhenTrialAlreadyInProgressIgnoringFailure();
        assertMaskerPlayerNotPlayed();
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationDoesNotPlayIfTrialInProgress
    ) {
        playCalibrationWhenTrialAlreadyInProgressIgnoringFailure();
        assertTargetPlayerNotPlayed();
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestThrowsRequestFailureIfTrialInProgress
    ) {
        assertThrowsRequestFailureWhenTrialInProgress(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestThrowsRequestFailureIfTrialInProgress
    ) {
        assertThrowsRequestFailureWhenTrialInProgress(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialThrowsRequestFailureIfTrialInProgress
    ) {
        assertThrowsRequestFailureWhenTrialInProgress(playingTrial);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationThrowsRequestFailureIfTrialInProgress
    ) {
        assertThrowsRequestFailureWhenTrialInProgress(playingCalibration);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestDoesNotLoadMaskerIfTrialInProgress
    ) {
        assertMaskerFilePathNotPassedToPlayerWhenTrialInProgress(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestDoesNotLoadMaskerIfTrialInProgress
    ) {
        assertMaskerFilePathNotPassedToPlayerWhenTrialInProgress(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestDoesNotHideTargetPlayerWhenAuditoryOnlyButTrialInProgress
    ) {
        assertTargetVideoNotHiddenWhenAuditoryOnlyButTrialInProgress(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestDoesNotHideTargetPlayerWhenAuditoryOnlyButTrialInProgress
    ) {
        assertTargetVideoNotHiddenWhenAuditoryOnlyButTrialInProgress(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        audioDevicesReturnsOutputAudioDeviceDescriptions
    ) {
        setOutputAudioDeviceDescriptions({"a", "b", "c"});
        assertEqual({"a", "b", "c"}, model.audioDevices());
    }

    TEST_F(
        RecognitionTestModelTests,
        testCompleteWhenAllTracksComplete
    ) {
        run(initializingAdaptiveTest);
        initializingAdaptiveTest.setSnrTrackComplete(0);
        submitCoordinateResponse();
        assertTestIncomplete();
        initializingAdaptiveTest.setSnrTrackComplete(1);
        submitCoordinateResponse();
        assertTestIncomplete();
        initializingAdaptiveTest.setSnrTrackComplete(2);
        submitCoordinateResponse();
        assertTestComplete();
    }

    TEST_F(
        RecognitionTestModelTests,
        fixedLevelTestCompleteWhenListEmpty
    ) {
        initializeFixedLevelTest();
        submitCoordinateResponse();
        assertTestIncomplete();
        finiteTargetList.setEmpty();
        submitCoordinateResponse();
        assertTestComplete();
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseDoesNotLoadNextTargetWhenCompleteForFixedLevelTest
    ) {
        assertCoordinateResponseDoesNotLoadNextTargetWhenTestComplete(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseDoesNotLoadNextTargetWhenCompleteForAdaptiveTest
    ) {
        assertCoordinateResponseDoesNotLoadNextTargetWhenTestComplete(initializingAdaptiveTest);
    }
}

