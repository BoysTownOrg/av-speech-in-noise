#include "RecognitionTestModelTests.hpp"
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests::recognition_test {
    class TestMethodStub : public TestMethod {
        std::string next_{};
        int snr_dB_{};
    public:
        void setSnr_dB(int x) {
            snr_dB_ = x;
        }
        
        void setNextTarget(std::string s) {
            next_ = std::move(s);
        }
        
        bool complete()  {return {};}
        
        std::string next() {
            return next_;
        }
        
        std::string current()  {return {};}
        
        int snr_dB() {
            return snr_dB_;
        }
        
        void submitCorrectResponse()  {}
        void submitIncorrectResponse()  {}
        void submitResponse(const FreeResponse &)  {}
        void writeTestingParameters(OutputFile *file) {
            file->writeTest(AdaptiveTest{});
        }
        void writeLastCoordinateResponse(OutputFile *)  {}
        void submitResponse(const coordinate_response_measure::SubjectResponse &)  {}
    };
    
    class InitializingTest : public UseCase {
        TestInformation information{};
        CommonTest common{};
        TestMethod *method;
    public:
        explicit InitializingTest(TestMethod *method) :
            method{method} {}
        
        void run(RecognitionTestModel &) override {
        
        }
        
        void run(RecognitionTestModel_Internal &m) override {
            m.initialize(method, common, information);
        }
        
        auto &testInformation() const {
            return information;
        }
        
        void setMaskerFilePath(std::string s) {
            common.maskerFilePath = std::move(s);
        }
        
        void setMaskerLevel_dB_SPL(int x) {
            common.maskerLevel_dB_SPL = x;
        }
        
        void setTestingFullScaleLevel_dB_SPL(int x) {
            common.fullScaleLevel_dB_SPL = x;
        }
        
        void setAudioVisual() {
            common.condition = Condition::audioVisual;
        }
        
        void setAuditoryOnly() {
            common.condition = Condition::auditoryOnly;
        }
    };
    
    class RecognitionTestModelTests3 : public ::testing::Test {
    protected:
        ModelEventListenerStub listener;
        TargetPlayerStub targetPlayer{};
        MaskerPlayerStub maskerPlayer{};
        ResponseEvaluatorStub evaluator{};
        OutputFileStub outputFile{};
        RandomizerStub randomizer{};
        RecognitionTestModel_Internal model{
            &targetPlayer,
            &maskerPlayer,
            &evaluator,
            &outputFile,
            &randomizer
        };
        TestMethodStub testMethod;
        PlayingCalibration playingCalibration{};
        InitializingTest initializingTest{&testMethod};
        PlayingTrial playingTrial;
        SubmittingCoordinateResponse submittingCoordinateResponse;
        
        RecognitionTestModelTests3() {
            model.subscribe(&listener);
        }
        
        void run(UseCase &useCase) {
            useCase.run(model);
        }
        
        void assertTargetVideoOnlyHidden() {
            assertTrue(targetPlayerVideoHidden());
            assertTargetVideoNotShown();
        }
        
        bool targetPlayerVideoHidden() {
            return targetPlayer.videoHidden();
        }
        
        void assertTargetVideoNotShown() {
            assertFalse(targetPlayerVideoShown());
        }
        
        bool targetPlayerVideoShown() {
            return targetPlayer.videoShown();
        }
        
        void assertTargetVideoHiddenWhenAuditoryOnly(ConditionUseCase &useCase) {
            useCase.setAuditoryOnly();
            run(useCase);
            assertTargetVideoOnlyHidden();
        }
        
        void assertTargetVideoShownWhenAudioVisual(ConditionUseCase &useCase) {
            useCase.setAudioVisual();
            run(useCase);
            assertTargetVideoOnlyShown();
        }
        
        void assertTargetVideoOnlyShown() {
            assertTargetVideoNotHidden();
            assertTrue(targetPlayerVideoShown());
        }
        
        void assertTargetVideoNotHidden() {
            assertFalse(targetPlayerVideoHidden());
        }
        
        void assertClosesOutputFileOpensAndWritesTestInOrder(UseCase &useCase) {
            run(useCase);
            assertOutputFileLog("close openNewFile writeTest ");
        }
        
        auto &outputFileLog() {
            return outputFile.log();
        }
        
        void assertOutputFileLog(std::string s) {
            assertEqual(std::move(s), outputFileLog());
        }
        
        template<typename T>
        void assertDevicePassedToPlayer(
            const T &player,
            AudioDeviceUseCase &useCase
        ) {
            useCase.setAudioDevice("a");
            run(useCase);
            assertEqual("a", player.device());
        }
        
        void assertDevicePassedToTargetPlayer(AudioDeviceUseCase &useCase) {
            assertDevicePassedToPlayer(targetPlayer, useCase);
        }
        
        void assertDevicePassedToMaskerPlayer(AudioDeviceUseCase &useCase) {
            assertDevicePassedToPlayer(maskerPlayer, useCase);
        }
        
        bool maskerPlayerFadedIn() {
            return maskerPlayer.fadeInCalled();
        }
        
        void assertTargetPlayerPlayed() {
            assertTrue(targetPlayerPlayed());
        }
        
        bool targetPlayerPlayed() {
            return targetPlayer.played();
        }
        
        auto targetFilePath() {
            return targetPlayer.filePath();
        }
        
        void assertTargetFilePathEquals(std::string what) {
            assertEqual(std::move(what), targetFilePath());
        }
        
        void assertPassesNextTargetToPlayer(UseCase &useCase) {
            testMethod.setNextTarget("a");
            run(useCase);
            assertTargetFilePathEquals("a");
        }
        
        void assertTargetPlayerPlaybackCompletionSubscribed(UseCase &useCase) {
            run(useCase);
            assertTrue(targetPlayerPlaybackCompletionSubscribed());
        }
        
        bool targetPlayerPlaybackCompletionSubscribed() {
            return targetPlayer.playbackCompletionSubscribedTo();
        }
        
        void assertSeeksToRandomMaskerPositionWithinTrialDuration(UseCase &useCase) {
            targetPlayer.setDurationSeconds(1);
            maskerPlayer.setFadeTimeSeconds(2);
            maskerPlayer.setDurationSeconds(3);
            run(useCase);
            assertEqual(0., randomizer.lowerFloatBound());
            assertEqual(3. - 2 - 1 - 2, randomizer.upperFloatBound());
        }
        
        auto maskerPlayerSecondsSeeked() {
            return maskerPlayer.secondsSeeked();
        }
        
        void assertMaskerPlayerSeekedToRandomTime(UseCase &useCase) {
            randomizer.setRandomFloat(1);
            run(useCase);
            assertEqual(1., maskerPlayerSecondsSeeked());
        }
    
        double dB(double x) {
            return 20 * std::log10(x);
        }
        
        auto targetPlayerLevel_dB() {
            return targetPlayer.level_dB();
        }
        
        void assertTargetPlayerLevelEquals_dB(double x) {
            assertEqual(x, targetPlayerLevel_dB());
        }
        
        void setMaskerLevel_dB_SPL(int x) {
            initializingTest.setMaskerLevel_dB_SPL(x);
        }
        
        void setTestingFullScaleLevel_dB_SPL(int x) {
            initializingTest.setTestingFullScaleLevel_dB_SPL(x);
        }
        
        void setMaskerRms(double x) {
            maskerPlayer.setRms(x);
        }
        
        void setSnr_dB(int x) {
            testMethod.setSnr_dB(x);
        }
        
        void maskerFadeOutComplete() {
            maskerPlayer.fadeOutComplete();
        }
        
        void assertSavesOutputFileAfterWritingTrial(UseCase &useCase) {
            run(useCase);
            assertTrue(outputFileLog().endsWith("save "));
        }
        
        void assertCallThrowsRequestFailure(
            UseCase &useCase,
            std::string what
        ) {
            try {
                run(useCase);
                FAIL() <<
                    "Expected recognition_test::"
                    "RecognitionTestModel::"
                    "RequestFailure";
            } catch (const RecognitionTestModel::RequestFailure &e) {
                assertEqual(std::move(what), e.what());
            }
        }
        
        void setMaskerFilePath(std::string s) {
            initializingTest.setMaskerFilePath(std::move(s));
        }
        
        void assertThrowsRequestFailureWhenInvalidAudioDevice(
            AudioDeviceUseCase &useCase
        ) {
            throwInvalidAudioDeviceWhenSet();
            useCase.setAudioDevice("a");
            assertCallThrowsRequestFailure(
                useCase,
                "'a' is not a valid audio device."
            );
        }
        
        void throwInvalidAudioDeviceWhenSet() {
            maskerPlayer.throwInvalidAudioDeviceWhenDeviceSet();
            targetPlayer.throwInvalidAudioDeviceWhenDeviceSet();
        }
    };
    
    TEST_F(RecognitionTestModelTests3, subscribesToPlayerEvents) {
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
        RecognitionTestModelTests3,
        playCalibrationHidesTargetVideoWhenAuditoryOnly
    ) {
        assertTargetVideoHiddenWhenAuditoryOnly(playingCalibration);
    }
    
    TEST_F(
        RecognitionTestModelTests3,
        playCalibrationShowsTargetVideoWhenAudioVisual
    ) {
        assertTargetVideoShownWhenAudioVisual(playingCalibration);
    }

    TEST_F(
        RecognitionTestModelTests3,
        initializeTestClosesOutputFileOpensAndWritesTestInOrder
    ) {
        assertClosesOutputFileOpensAndWritesTestInOrder(initializingTest);
    }

    TEST_F(
        RecognitionTestModelTests3,
        initializeTestOpensNewOutputFilePassingTestInformation
    ) {
        run(initializingTest);
        assertEqual(outputFile.openNewFileParameters(), &initializingTest.testInformation());
    }

    TEST_F(
        RecognitionTestModelTests3,
        playTrialPassesAudioDeviceToTargetPlayer
    ) {
        assertDevicePassedToTargetPlayer(playingTrial);
    }

    TEST_F(
        RecognitionTestModelTests3,
        playCalibrationPassesAudioDeviceToTargetPlayer
    ) {
        assertDevicePassedToTargetPlayer(playingCalibration);
    }

    TEST_F(RecognitionTestModelTests3, playTrialPassesAudioDeviceToMaskerPlayer) {
        assertDevicePassedToMaskerPlayer(playingTrial);
    }

    TEST_F(RecognitionTestModelTests3, playTrialFadesInMasker) {
        run(playingTrial);
        assertTrue(maskerPlayerFadedIn());
    }

    TEST_F(RecognitionTestModelTests3, playCalibrationPlaysTarget) {
        run(playingCalibration);
        assertTargetPlayerPlayed();
    }

    TEST_F(RecognitionTestModelTests3, fadeInCompletePlaysTarget) {
        maskerPlayer.fadeInComplete();
        assertTargetPlayerPlayed();
    }

    TEST_F(
        RecognitionTestModelTests3,
        initializeTestPassesNextTargetToTargetPlayer
    ) {
        assertPassesNextTargetToPlayer(initializingTest);
    }

    TEST_F(
        RecognitionTestModelTests3,
        submittingCoordinateResponsePassesNextTargetToTargetPlayer
    ) {
        run(initializingTest);
        assertPassesNextTargetToPlayer(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests3,
        playCalibrationPassesAudioFileToTargetPlayer
    ) {
        playingCalibration.setFilePath("a");
        run(playingCalibration);
        assertTargetFilePathEquals("a");
    }

    TEST_F(
        RecognitionTestModelTests3,
        initializeTestPassesMaskerFilePathToMaskerPlayer
    ) {
        setMaskerFilePath("a");
        run(initializingTest);
        assertEqual("a", maskerPlayer.filePath());
    }

    TEST_F(
        RecognitionTestModelTests3,
        initializeTestSubscribesToTargetPlaybackCompletionNotification
    ) {
        assertTargetPlayerPlaybackCompletionSubscribed(initializingTest);
    }

    TEST_F(
        RecognitionTestModelTests3,
        submitCoordinateResponseSubscribesToTargetPlaybackCompletionNotification
    ) {
        assertTargetPlayerPlaybackCompletionSubscribed(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests3,
        initializeTestSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        assertSeeksToRandomMaskerPositionWithinTrialDuration(initializingTest);
    }

    TEST_F(
        RecognitionTestModelTests3,
        submitCoordinateResponseSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        assertSeeksToRandomMaskerPositionWithinTrialDuration(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests3,
        initializeTestSeeksToRandomMaskerPosition
    ) {
        assertMaskerPlayerSeekedToRandomTime(initializingTest);
    }

    TEST_F(
        RecognitionTestModelTests3,
        submitCoordinateResponseSeeksToRandomMaskerPosition
    ) {
        assertMaskerPlayerSeekedToRandomTime(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests3,
        initializeTestSetsInitialMaskerPlayerLevel
    ) {
        setMaskerLevel_dB_SPL(1);
        setTestingFullScaleLevel_dB_SPL(2);
        setMaskerRms(3);
        run(initializingTest);
        assertEqual(1 - 2 - dB(3), maskerPlayer.level_dB());
    }

    TEST_F(
        RecognitionTestModelTests3,
        initializeTestSetsTargetPlayerLevel
    ) {
        setSnr_dB(2);
        setMaskerLevel_dB_SPL(3);
        setTestingFullScaleLevel_dB_SPL(4);
        setMaskerRms(5);
        run(initializingTest);
        assertTargetPlayerLevelEquals_dB(2 + 3 - 4 - dB(5));
    }

    TEST_F(
        RecognitionTestModelTests3,
        submitCoordinateResponseSetsTargetPlayerLevel
    ) {
        setMaskerLevel_dB_SPL(3);
        setTestingFullScaleLevel_dB_SPL(4);
        run(initializingTest);
        setMaskerRms(5);
        setSnr_dB(2);
        run(submittingCoordinateResponse);
        assertTargetPlayerLevelEquals_dB(2 + 3 - 4 - dB(5));
    }

    TEST_F(
        RecognitionTestModelTests3,
        playCalibrationSetsTargetPlayerLevel
    ) {
        playingCalibration.setLevel_dB_SPL(1);
        playingCalibration.setFullScaleLevel_dB_SPL(2);
        targetPlayer.setRms(3);
        run(playingCalibration);
        assertTargetPlayerLevelEquals_dB(1 - 2 - dB(3));
    }

    TEST_F(RecognitionTestModelTests3, startTrialShowsTargetPlayerWhenAudioVisual) {
        initializingTest.setAudioVisual();
        run(initializingTest);
        run(playingTrial);
        assertTrue(targetPlayerVideoShown());
    }

    TEST_F(RecognitionTestModelTests3, maskerFadeOutCompleteHidesTargetPlayer) {
        maskerFadeOutComplete();
        assertTargetVideoOnlyHidden();
    }

    TEST_F(RecognitionTestModelTests3, startTrialDoesNotShowTargetPlayerWhenAuditoryOnly) {
        initializingTest.setAuditoryOnly();
        run(initializingTest);
        run(playingTrial);
        assertTargetVideoNotShown();
    }

    TEST_F(RecognitionTestModelTests3, initializeTestHidesTargetPlayer) {
        run(initializingTest);
        assertTargetVideoOnlyHidden();
    }

    TEST_F(RecognitionTestModelTests3, targetPlaybackCompleteFadesOutMasker) {
        targetPlayer.playbackComplete();
        assertTrue(maskerPlayer.fadeOutCalled());
    }

    TEST_F(RecognitionTestModelTests3, fadeOutCompleteNotifiesTrialComplete) {
        maskerFadeOutComplete();
        assertTrue(listener.notified());
    }

    TEST_F(
        RecognitionTestModelTests3,
        submitCoordinateResponseSavesOutputFileAfterWritingTrial
    ) {
        assertSavesOutputFileAfterWritingTrial(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests3,
        initializeTestThrowsRequestFailureIfFileFailsToOpen
    ) {
        outputFile.throwOnOpen();
        assertCallThrowsRequestFailure(initializingTest, "Unable to open output file.");
    }

    TEST_F(
        RecognitionTestModelTests3,
        playCalibrationThrowsRequestFailureWhenTargetPlayerThrowsInvalidAudioFile
    ) {
        playingCalibration.setFilePath("a");
        targetPlayer.throwInvalidAudioFileOnRms();
        assertCallThrowsRequestFailure(playingCalibration, "unable to read a");
    }

    TEST_F(
        RecognitionTestModelTests3,
        initializeTestThrowsRequestFailureWhenMaskerPlayerThrowsInvalidAudioFile
    ) {
        setMaskerFilePath("a");
        maskerPlayer.throwInvalidAudioFileOnLoad();
        assertCallThrowsRequestFailure(initializingTest, "unable to read a");
    }

    TEST_F(
        RecognitionTestModelTests3,
        playTrialWithInvalidAudioDeviceThrowsRequestFailure
    ) {
        assertThrowsRequestFailureWhenInvalidAudioDevice(playingTrial);
    }

    TEST_F(
        RecognitionTestModelTests3,
        playCalibrationWithInvalidAudioDeviceThrowsRequestFailure
    ) {
        assertThrowsRequestFailureWhenInvalidAudioDevice(playingCalibration);
    }
}
