#include "ModelEventListenerStub.h"
#include "RandomizerStub.h"
#include "ResponseEvaluatorStub.h"
#include "OutputFileStub.h"
#include "MaskerPlayerStub.h"
#include "TargetPlayerStub.h"
#include "LogString.h"
#include "assert-utility.h"
#include <gtest/gtest.h>
#include <cmath>

namespace av_speech_in_noise::tests::recognition_test {
class TestMethodStub : public TestMethod {
    LogString log_{};
    std::string current_{};
    std::string currentWhenNext_{};
    std::string next_{};
    int snr_dB_{};
    bool complete_{};
    bool submittedCorrectResponse_{};
    bool submittedIncorrectResponse_{};
public:
    auto submittedCorrectResponse() const {
        return submittedCorrectResponse_;
    }

    auto submittedIncorrectResponse() const {
        return submittedIncorrectResponse_;
    }

    void setComplete() {
        complete_ = true;
    }

    void setSnr_dB(int x) {
        snr_dB_ = x;
    }

    void setNextTarget(std::string s) {
        next_ = std::move(s);
    }

    bool complete()  { return complete_; }

    std::string next() {
        log_.insert("next ");
        current_ = currentWhenNext_;
        return next_;
    }

    std::string current() { return current_; }

    void setCurrent(std::string s) {
        current_ = std::move(s);
    }

    void setCurrentWhenNext(std::string s) {
        currentWhenNext_ = std::move(s);
    }

    int snr_dB() {
        return snr_dB_;
    }

    void submitCorrectResponse() {
        log_.insert("submitCorrectResponse ");
        submittedCorrectResponse_ = true;
    }

    void submitIncorrectResponse() {
        log_.insert("submitIncorrectResponse ");
        submittedIncorrectResponse_ = true;
    }

    void submitResponse(const FreeResponse &)  {}

    void writeTestingParameters(OutputFile *file) {
        file->writeTest(AdaptiveTest{});
    }

    void writeLastCoordinateResponse(OutputFile *) {
        log_.insert("writeLastCoordinateResponse ");
    }

    void submitResponse(const coordinate_response_measure::SubjectResponse &) {
        log_.insert("submitResponse ");
    }

    auto &log() const {
        return log_;
    }
};

class UseCase {
public:
    virtual ~UseCase() = default;
    virtual void run(RecognitionTestModel &) {}
    virtual void run(RecognitionTestModel_Internal &) = 0;
};

class TargetWritingUseCase : public virtual UseCase {
public:
    virtual ~TargetWritingUseCase() = default;
    virtual std::string writtenTarget(OutputFileStub &) = 0;
};

class SubmittingResponse : public virtual UseCase {
};

class InitializingTest : public UseCase {
    TestInformation information{};
    CommonTest common{};
    TestMethod *method;
public:
    explicit InitializingTest(TestMethod *method) :
        method{method} {}

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

class AudioDeviceUseCase : public virtual UseCase {
public:
    virtual void setAudioDevice(std::string) = 0;
};

class ConditionUseCase : public virtual UseCase {
public:
    virtual void setAuditoryOnly() = 0;
    virtual void setAudioVisual() = 0;
};

class PlayingCalibration :
    public AudioDeviceUseCase,
    public ConditionUseCase
{
    Calibration calibration{};
public:
    void setAudioDevice(std::string s) override {
        calibration.audioSettings.audioDevice = std::move(s);
    }
    
    void run(RecognitionTestModel_Internal &m) override {
        m.playCalibration(calibration);
    }
    
    void setFilePath(std::string s) {
        calibration.filePath = std::move(s);
    }
    
    void setLevel_dB_SPL(int x) {
        calibration.level_dB_SPL = x;
    }
    
    void setFullScaleLevel_dB_SPL(int x) {
        calibration.fullScaleLevel_dB_SPL = x;
    }
    
    void setAudioVisual() override {
        calibration.condition = Condition::audioVisual;
    }
    
    void setAuditoryOnly() override {
        calibration.condition = Condition::auditoryOnly;
    }
};

class PlayingTrial : public AudioDeviceUseCase {
    AudioSettings trial;
public:
    void setAudioDevice(std::string s) override {
        trial.audioDevice = std::move(s);
    }
    
    void run(RecognitionTestModel_Internal &m) override {
        m.playTrial(trial);
    }
};

class SubmittingFreeResponse :
    public SubmittingResponse,
    public TargetWritingUseCase
{
    FreeResponse response_;
public:
    void run(RecognitionTestModel_Internal &m) override {
        m.submitResponse(response_);
    }

    std::string writtenTarget(OutputFileStub &file) override {
        return file.writtenFreeResponseTrial().target;
    }

    void setResponse(std::string s) {
        response_.response = std::move(s);
    }
};

class SubmittingCoordinateResponse : public SubmittingResponse {
    coordinate_response_measure::SubjectResponse response_{};
public:
    void run(RecognitionTestModel_Internal &m) override {
        m.submitResponse(response_);
    }
    
    void setNumber(int n) {
        response_.number = n;
    }
    
    void setColor(coordinate_response_measure::Color c) {
        response_.color = c;
    }
    
    auto &response() const {
        return response_;
    }
};

class SubmittingCorrectResponse : public TargetWritingUseCase {
public:
    void run(RecognitionTestModel_Internal &m) override {
        m.submitCorrectResponse();
    }

    std::string writtenTarget(OutputFileStub &file) override {
        return file.writtenOpenSetAdaptiveTrial().target;
    }
};

class SubmittingIncorrectResponse : public TargetWritingUseCase {
public:
    void run(RecognitionTestModel_Internal &m) override {
        m.submitIncorrectResponse();
    }

    std::string writtenTarget(OutputFileStub &file) override {
        return file.writtenOpenSetAdaptiveTrial().target;
    }
};

class RecognitionTestModel_InternalTests : public ::testing::Test {
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
    SubmittingCorrectResponse submittingCorrectResponse;
    SubmittingIncorrectResponse submittingIncorrectResponse;
    SubmittingFreeResponse submittingFreeResponse;
    
    RecognitionTestModel_InternalTests() {
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
    
    void runIgnoringFailureWithTrialInProgress(UseCase &useCase) {
        setTrialInProgress();
        runIgnoringFailure(useCase);
    }
    
    void setTrialInProgress() {
        maskerPlayer.setPlaying();
    }
    
    void runIgnoringFailure(UseCase &useCase) {
        try {
            run(useCase);
        } catch (const RecognitionTestModel::RequestFailure &) {
        }
    }
    
    void assertMaskerPlayerNotPlayed() {
        assertFalse(maskerPlayerFadedIn());
    }
    
    void assertTargetPlayerNotPlayed() {
        assertFalse(targetPlayerPlayed());
    }
    
    void assertThrowsRequestFailureWhenTrialInProgress(UseCase &useCase) {
        setTrialInProgress();
        assertCallThrowsRequestFailure(useCase, "Trial in progress.");
    }
    
    void assertTestIncomplete() {
        assertFalse(testComplete());
    }
    
    bool testComplete() {
        return model.testComplete();
    }
    
    void assertTestComplete() {
        assertTrue(testComplete());
    }
    
    void assertSetsTargetLevel(UseCase &useCase) {
        setMaskerLevel_dB_SPL(3);
        setTestingFullScaleLevel_dB_SPL(4);
        run(initializingTest);
        setMaskerRms(5);
        setSnr_dB(2);
        run(useCase);
        assertTargetPlayerLevelEquals_dB(2 + 3 - 4 - dB(5));
    }
    
    auto writtenFreeResponseTrial() {
        return outputFile.writtenFreeResponseTrial();
    }

    auto writtenOpenSetAdaptiveTrial() {
        return outputFile.writtenOpenSetAdaptiveTrial();
    }

    void assertResponseDoesNotLoadNextTargetWhenComplete(UseCase &useCase) {
        testMethod.setNextTarget("a");
        run(initializingTest);
        testMethod.setComplete();
        testMethod.setNextTarget("b");
        run(useCase);
        assertTargetFilePathEquals("a");
    }

    void assertWritesTarget(TargetWritingUseCase &useCase) {
        evaluator.setFileName("a");
        run(useCase);
        assertEqual("a", useCase.writtenTarget(outputFile));
    }

    void assertPassesCurrentTargetToEvaluatorBeforeAdvancingTarget(UseCase &useCase) {
        run(initializingTest);
        testMethod.setCurrent("a");
        testMethod.setCurrentWhenNext("b");
        run(useCase);
        assertEqual("a", evaluator.filePathForFileName());
    }
};

TEST_F(RecognitionTestModel_InternalTests, subscribesToPlayerEvents) {
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
    RecognitionTestModel_InternalTests,
    playCalibrationHidesTargetVideoWhenAuditoryOnly
) {
    assertTargetVideoHiddenWhenAuditoryOnly(playingCalibration);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    playCalibrationShowsTargetVideoWhenAudioVisual
) {
    assertTargetVideoShownWhenAudioVisual(playingCalibration);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    initializeTestClosesOutputFileOpensAndWritesTestInOrder
) {
    assertClosesOutputFileOpensAndWritesTestInOrder(initializingTest);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    initializeTestOpensNewOutputFilePassingTestInformation
) {
    run(initializingTest);
    assertEqual(outputFile.openNewFileParameters(), &initializingTest.testInformation());
}

TEST_F(
    RecognitionTestModel_InternalTests,
    playTrialPassesAudioDeviceToTargetPlayer
) {
    assertDevicePassedToTargetPlayer(playingTrial);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    playCalibrationPassesAudioDeviceToTargetPlayer
) {
    assertDevicePassedToTargetPlayer(playingCalibration);
}

TEST_F(RecognitionTestModel_InternalTests, playTrialPassesAudioDeviceToMaskerPlayer) {
    assertDevicePassedToMaskerPlayer(playingTrial);
}

TEST_F(RecognitionTestModel_InternalTests, playTrialFadesInMasker) {
    run(playingTrial);
    assertTrue(maskerPlayerFadedIn());
}

TEST_F(RecognitionTestModel_InternalTests, playCalibrationPlaysTarget) {
    run(playingCalibration);
    assertTargetPlayerPlayed();
}

TEST_F(RecognitionTestModel_InternalTests, fadeInCompletePlaysTarget) {
    maskerPlayer.fadeInComplete();
    assertTargetPlayerPlayed();
}

TEST_F(
    RecognitionTestModel_InternalTests,
    initializeTestPassesNextTargetToTargetPlayer
) {
    assertPassesNextTargetToPlayer(initializingTest);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submittingCoordinateResponsePassesNextTargetToTargetPlayer
) {
    run(initializingTest);
    assertPassesNextTargetToPlayer(submittingCoordinateResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submittingCorrectResponsePassesNextTargetToTargetPlayer
) {
    run(initializingTest);
    assertPassesNextTargetToPlayer(submittingCorrectResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submittingIncorrectResponsePassesNextTargetToTargetPlayer
) {
    run(initializingTest);
    assertPassesNextTargetToPlayer(submittingIncorrectResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submittingFreeResponsePassesNextTargetToTargetPlayer
) {
    run(initializingTest);
    assertPassesNextTargetToPlayer(submittingFreeResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    playCalibrationPassesAudioFileToTargetPlayer
) {
    playingCalibration.setFilePath("a");
    run(playingCalibration);
    assertTargetFilePathEquals("a");
}

TEST_F(
    RecognitionTestModel_InternalTests,
    initializeTestPassesMaskerFilePathToMaskerPlayer
) {
    setMaskerFilePath("a");
    run(initializingTest);
    assertEqual("a", maskerPlayer.filePath());
}

TEST_F(
    RecognitionTestModel_InternalTests,
    initializeTestSubscribesToTargetPlaybackCompletionNotification
) {
    assertTargetPlayerPlaybackCompletionSubscribed(initializingTest);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitCoordinateResponseSubscribesToTargetPlaybackCompletionNotification
) {
    assertTargetPlayerPlaybackCompletionSubscribed(submittingCoordinateResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitFreeResponseSubscribesToTargetPlaybackCompletionNotification
) {
    assertTargetPlayerPlaybackCompletionSubscribed(submittingFreeResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitCorrectResponseSubscribesToTargetPlaybackCompletionNotification
) {
    assertTargetPlayerPlaybackCompletionSubscribed(submittingCorrectResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitIncorrectResponseSubscribesToTargetPlaybackCompletionNotification
) {
    assertTargetPlayerPlaybackCompletionSubscribed(submittingIncorrectResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    initializeTestSeeksToRandomMaskerPositionWithinTrialDuration
) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(initializingTest);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitCoordinateResponseSeeksToRandomMaskerPositionWithinTrialDuration
) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(submittingCoordinateResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitCorrectResponseSeeksToRandomMaskerPositionWithinTrialDuration
) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(submittingCorrectResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitIncorrectResponseSeeksToRandomMaskerPositionWithinTrialDuration
) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(submittingIncorrectResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitFreeResponseSeeksToRandomMaskerPositionWithinTrialDuration
) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(submittingFreeResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    initializeTestSeeksToRandomMaskerPosition
) {
    assertMaskerPlayerSeekedToRandomTime(initializingTest);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitCoordinateResponseSeeksToRandomMaskerPosition
) {
    assertMaskerPlayerSeekedToRandomTime(submittingCoordinateResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitCorrectResponseSeeksToRandomMaskerPosition
) {
    assertMaskerPlayerSeekedToRandomTime(submittingCorrectResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitIncorrectResponseSeeksToRandomMaskerPosition
) {
    assertMaskerPlayerSeekedToRandomTime(submittingIncorrectResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitFreeResponseSeeksToRandomMaskerPosition
) {
    assertMaskerPlayerSeekedToRandomTime(submittingFreeResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    initializeTestSetsInitialMaskerPlayerLevel
) {
    setMaskerLevel_dB_SPL(1);
    setTestingFullScaleLevel_dB_SPL(2);
    setMaskerRms(3);
    run(initializingTest);
    assertEqual(1 - 2 - dB(3), maskerPlayer.level_dB());
}

TEST_F(
    RecognitionTestModel_InternalTests,
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
    RecognitionTestModel_InternalTests,
    submitCoordinateResponseSetsTargetPlayerLevel
) {
    assertSetsTargetLevel(submittingCoordinateResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitFreeResponseSetsTargetPlayerLevel
) {
    assertSetsTargetLevel(submittingFreeResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitCorrectResponseSetsTargetPlayerLevel
) {
    assertSetsTargetLevel(submittingCorrectResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitIncorrectResponseSetsTargetPlayerLevel
) {
    assertSetsTargetLevel(submittingIncorrectResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    playCalibrationSetsTargetPlayerLevel
) {
    playingCalibration.setLevel_dB_SPL(1);
    playingCalibration.setFullScaleLevel_dB_SPL(2);
    targetPlayer.setRms(3);
    run(playingCalibration);
    assertTargetPlayerLevelEquals_dB(1 - 2 - dB(3));
}

TEST_F(RecognitionTestModel_InternalTests, startTrialShowsTargetPlayerWhenAudioVisual) {
    initializingTest.setAudioVisual();
    run(initializingTest);
    run(playingTrial);
    assertTrue(targetPlayerVideoShown());
}

TEST_F(RecognitionTestModel_InternalTests, maskerFadeOutCompleteHidesTargetPlayer) {
    maskerFadeOutComplete();
    assertTargetVideoOnlyHidden();
}

TEST_F(RecognitionTestModel_InternalTests, startTrialDoesNotShowTargetPlayerWhenAuditoryOnly) {
    initializingTest.setAuditoryOnly();
    run(initializingTest);
    run(playingTrial);
    assertTargetVideoNotShown();
}

TEST_F(RecognitionTestModel_InternalTests, initializeTestHidesTargetPlayer) {
    run(initializingTest);
    assertTargetVideoOnlyHidden();
}

TEST_F(RecognitionTestModel_InternalTests, targetPlaybackCompleteFadesOutMasker) {
    targetPlayer.playbackComplete();
    assertTrue(maskerPlayer.fadeOutCalled());
}

TEST_F(RecognitionTestModel_InternalTests, fadeOutCompleteNotifiesTrialComplete) {
    maskerFadeOutComplete();
    assertTrue(listener.notified());
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitCoordinateResponseSavesOutputFileAfterWritingTrial
) {
    assertSavesOutputFileAfterWritingTrial(submittingCoordinateResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitFreeResponseSavesOutputFileAfterWritingTrial
) {
    assertSavesOutputFileAfterWritingTrial(submittingFreeResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitCorrectResponseSavesOutputFileAfterWritingTrial
) {
    assertSavesOutputFileAfterWritingTrial(submittingCorrectResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitIncorrectResponseSavesOutputFileAfterWritingTrial
) {
    assertSavesOutputFileAfterWritingTrial(submittingIncorrectResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    initializeTestThrowsRequestFailureIfFileFailsToOpen
) {
    outputFile.throwOnOpen();
    assertCallThrowsRequestFailure(initializingTest, "Unable to open output file.");
}

TEST_F(
    RecognitionTestModel_InternalTests,
    playCalibrationThrowsRequestFailureWhenTargetPlayerThrowsInvalidAudioFile
) {
    playingCalibration.setFilePath("a");
    targetPlayer.throwInvalidAudioFileOnRms();
    assertCallThrowsRequestFailure(playingCalibration, "unable to read a");
}

TEST_F(
    RecognitionTestModel_InternalTests,
    initializeTestThrowsRequestFailureWhenMaskerPlayerThrowsInvalidAudioFile
) {
    setMaskerFilePath("a");
    maskerPlayer.throwInvalidAudioFileOnLoad();
    assertCallThrowsRequestFailure(initializingTest, "unable to read a");
}

TEST_F(
    RecognitionTestModel_InternalTests,
    playTrialWithInvalidAudioDeviceThrowsRequestFailure
) {
    assertThrowsRequestFailureWhenInvalidAudioDevice(playingTrial);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    playCalibrationWithInvalidAudioDeviceThrowsRequestFailure
) {
    assertThrowsRequestFailureWhenInvalidAudioDevice(playingCalibration);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    playTrialDoesNotChangeAudioDeviceWhenTrialInProgress
) {
    runIgnoringFailureWithTrialInProgress(playingTrial);
    assertFalse(maskerPlayer.setDeviceCalled());
}

TEST_F(
    RecognitionTestModel_InternalTests,
    playCalibrationDoesNotChangeAudioDeviceWhenTrialInProgress
) {
    runIgnoringFailureWithTrialInProgress(playingCalibration);
    assertFalse(targetPlayer.setDeviceCalled());
}

TEST_F(RecognitionTestModel_InternalTests, playTrialDoesNotPlayIfTrialInProgress) {
    runIgnoringFailureWithTrialInProgress(playingTrial);
    assertMaskerPlayerNotPlayed();
}

TEST_F(
    RecognitionTestModel_InternalTests,
    playCalibrationDoesNotPlayIfTrialInProgress
) {
    runIgnoringFailureWithTrialInProgress(playingCalibration);
    assertTargetPlayerNotPlayed();
}

TEST_F(
    RecognitionTestModel_InternalTests,
    initializeTestThrowsRequestFailureIfTrialInProgress
) {
    assertThrowsRequestFailureWhenTrialInProgress(initializingTest);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    playTrialThrowsRequestFailureIfTrialInProgress
) {
    assertThrowsRequestFailureWhenTrialInProgress(playingTrial);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    playCalibrationThrowsRequestFailureIfTrialInProgress
) {
    assertThrowsRequestFailureWhenTrialInProgress(playingCalibration);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    initializeTestDoesNotLoadMaskerIfTrialInProgress
) {
    setMaskerFilePath("a");
    runIgnoringFailureWithTrialInProgress(initializingTest);
    assertEqual("", maskerPlayer.filePath());
}

TEST_F(
    RecognitionTestModel_InternalTests,
    initializeTestDoesNotHideTargetPlayerWhenAuditoryOnlyButTrialInProgress
) {
    initializingTest.setAuditoryOnly();
    runIgnoringFailureWithTrialInProgress(initializingTest);
    assertTargetVideoNotHidden();
}

TEST_F(
    RecognitionTestModel_InternalTests,
    audioDevicesReturnsOutputAudioDeviceDescriptions
) {
    maskerPlayer.setOutputAudioDeviceDescriptions({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, model.audioDevices());
}

TEST_F(
    RecognitionTestModel_InternalTests,
    testCompleteWhenComplete
) {
    run(initializingTest);
    assertTestIncomplete();
    testMethod.setComplete();
    assertTestComplete();
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitCoordinateResponseDoesNotLoadNextTargetWhenComplete
) {
    assertResponseDoesNotLoadNextTargetWhenComplete(submittingCoordinateResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitFreeResponseDoesNotLoadNextTargetWhenComplete
) {
    assertResponseDoesNotLoadNextTargetWhenComplete(submittingFreeResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitCorrectResponseDoesNotLoadNextTargetWhenComplete
) {
    assertResponseDoesNotLoadNextTargetWhenComplete(submittingCorrectResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitIncorrectResponseDoesNotLoadNextTargetWhenComplete
) {
    assertResponseDoesNotLoadNextTargetWhenComplete(submittingIncorrectResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    initializeTestDoesNotLoadNextTargetWhenComplete
) {
    testMethod.setNextTarget("a");
    testMethod.setComplete();
    run(initializingTest);
    assertTargetFilePathEquals("");
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitFreeResponseWritesResponse
) {
    submittingFreeResponse.setResponse("a");
    run(submittingFreeResponse);
    assertEqual("a", writtenFreeResponseTrial().response);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitFreeResponseWritesTarget
) {
    assertWritesTarget(submittingFreeResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitCorrectResponseWritesTarget
) {
    assertWritesTarget(submittingCorrectResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitIncorrectResponseWritesTarget
) {
    assertWritesTarget(submittingIncorrectResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitFreeResponsePassesCurrentTargetToEvaluatorBeforeAdvancingTarget
) {
    assertPassesCurrentTargetToEvaluatorBeforeAdvancingTarget(submittingFreeResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitCorrectResponsePassesCurrentTargetToEvaluatorBeforeAdvancingTarget
) {
    assertPassesCurrentTargetToEvaluatorBeforeAdvancingTarget(submittingCorrectResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitIncorrectResponsePassesCurrentTargetToEvaluatorBeforeAdvancingTarget
) {
    assertPassesCurrentTargetToEvaluatorBeforeAdvancingTarget(submittingIncorrectResponse);
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitCoordinateResponseWritesTrialAfterSubmittingResponse
) {
    run(initializingTest);
    run(submittingCoordinateResponse);
    assertTrue(testMethod.log().contains("submitResponse writeLastCoordinateResponse "));
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitCoordinateResponseQueriesNextTargetAfterWritingResponse
) {
    run(initializingTest);
    run(submittingCoordinateResponse);
    assertTrue(testMethod.log().contains("writeLastCoordinateResponse next "));
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitIncorrectResponseQueriesNextTargetAfterSubmittingResponse
) {
    run(initializingTest);
    run(submittingIncorrectResponse);
    assertTrue(testMethod.log().contains("submitIncorrectResponse next "));
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitCorrectResponseQueriesNextTargetAfterSubmittingResponse
) {
    run(initializingTest);
    run(submittingCorrectResponse);
    assertTrue(testMethod.log().contains("submitCorrectResponse next "));
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitCorrectResponseSubmitsCorrectResponse
) {
    run(initializingTest);
    run(submittingCorrectResponse);
    assertTrue(testMethod.submittedCorrectResponse());
}

TEST_F(
    RecognitionTestModel_InternalTests,
    submitIncorrectResponseSubmitsIncorrectResponse
) {
    run(initializingTest);
    run(submittingIncorrectResponse);
    assertTrue(testMethod.submittedIncorrectResponse());
}
}
