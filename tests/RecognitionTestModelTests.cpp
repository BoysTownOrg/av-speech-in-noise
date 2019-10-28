#include "LogString.h"
#include "MaskerPlayerStub.h"
#include "ModelEventListenerStub.h"
#include "OutputFileStub.h"
#include "RandomizerStub.h"
#include "ResponseEvaluatorStub.h"
#include "TargetPlayerStub.h"
#include "assert-utility.h"
#include <cmath>
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests::recognition_test {
namespace {
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
    auto submittedCorrectResponse() const { return submittedCorrectResponse_; }

    auto submittedIncorrectResponse() const {
        return submittedIncorrectResponse_;
    }

    void setComplete() { complete_ = true; }

    void setSnr_dB(int x) { snr_dB_ = x; }

    void setNextTarget(std::string s) { next_ = std::move(s); }

    bool complete() { return complete_; }

    std::string next() {
        log_.insert("next ");
        current_ = currentWhenNext_;
        return next_;
    }

    std::string current() { return current_; }

    void setCurrent(std::string s) { current_ = std::move(s); }

    void setCurrentWhenNext(std::string s) { currentWhenNext_ = std::move(s); }

    int snr_dB() { return snr_dB_; }

    void submitCorrectResponse() {
        log_.insert("submitCorrectResponse ");
        submittedCorrectResponse_ = true;
    }

    void submitIncorrectResponse() {
        log_.insert("submitIncorrectResponse ");
        submittedIncorrectResponse_ = true;
    }

    void submitResponse(const FreeResponse &) {}

    void writeTestingParameters(OutputFile *file) {
        file->writeTest(AdaptiveTest{});
    }

    void writeLastCoordinateResponse(OutputFile *) {
        log_.insert("writeLastCoordinateResponse ");
    }

    void writeLastCorrectResponse(OutputFile *) {
        log_.insert("writeLastCorrectResponse ");
    }

    void writeLastIncorrectResponse(OutputFile *) {
        log_.insert("writeLastIncorrectResponse ");
    }

    void submitResponse(const coordinate_response_measure::Response &) {
        log_.insert("submitResponse ");
    }

    auto &log() const { return log_; }
};

class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run(ModelImpl &) {}
    virtual void run(RecognitionTestModelImpl &) = 0;
};

class TargetWritingUseCase : public virtual UseCase {
  public:
    virtual ~TargetWritingUseCase() = default;
    virtual std::string writtenTarget(OutputFileStub &) = 0;
};

class SubmittingResponse : public virtual UseCase {};

class InitializingTest : public UseCase {
    TestIdentity information{};
    Test common{};
    TestMethod *method;

  public:
    explicit InitializingTest(TestMethod *method) : method{method} {}

    void run(RecognitionTestModelImpl &m) override {
        m.initialize(method, common, information);
    }

    auto &testIdentity() const { return information; }

    void setMaskerFilePath(std::string s) {
        common.maskerFilePath = std::move(s);
    }

    void setMaskerLevel_dB_SPL(int x) { common.maskerLevel_dB_SPL = x; }

    void setTestingFullScaleLevel_dB_SPL(int x) {
        common.fullScaleLevel_dB_SPL = x;
    }

    void setAudioVisual() { common.condition = Condition::audioVisual; }

    void setAuditoryOnly() { common.condition = Condition::auditoryOnly; }
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

class PlayingCalibration : public AudioDeviceUseCase, public ConditionUseCase {
    Calibration calibration{};

  public:
    void setAudioDevice(std::string s) override {
        calibration.audioSettings.audioDevice = std::move(s);
    }

    void run(RecognitionTestModelImpl &m) override {
        m.playCalibration(calibration);
    }

    void setFilePath(std::string s) { calibration.filePath = std::move(s); }

    void setLevel_dB_SPL(int x) { calibration.level_dB_SPL = x; }

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

    void run(RecognitionTestModelImpl &m) override { m.playTrial(trial); }
};

class SubmittingFreeResponse : public SubmittingResponse,
                               public TargetWritingUseCase {
    FreeResponse response_{};

  public:
    void run(RecognitionTestModelImpl &m) override {
        m.submitResponse(response_);
    }

    std::string writtenTarget(OutputFileStub &file) override {
        return file.writtenFreeResponseTrial().target;
    }

    void setResponse(std::string s) { response_.response = std::move(s); }

    void setFlagged() { response_.flagged = true; }
};

class SubmittingCoordinateResponse : public SubmittingResponse {
    coordinate_response_measure::Response response_{};

  public:
    void run(RecognitionTestModelImpl &m) override {
        m.submitResponse(response_);
    }

    void setNumber(int n) { response_.number = n; }

    void setColor(coordinate_response_measure::Color c) { response_.color = c; }

    auto &response() const { return response_; }
};

class SubmittingCorrectResponse : public TargetWritingUseCase {
  public:
    void run(RecognitionTestModelImpl &m) override {
        m.submitCorrectResponse();
    }

    std::string writtenTarget(OutputFileStub &file) override {
        return file.writtenOpenSetAdaptiveTrial().target;
    }
};

class SubmittingIncorrectResponse : public TargetWritingUseCase {
  public:
    void run(RecognitionTestModelImpl &m) override {
        m.submitIncorrectResponse();
    }

    std::string writtenTarget(OutputFileStub &file) override {
        return file.writtenOpenSetAdaptiveTrial().target;
    }
};

class RecognitionTestModelTests : public ::testing::Test {
  protected:
    ModelEventListenerStub listener;
    TargetPlayerStub targetPlayer{};
    MaskerPlayerStub maskerPlayer{};
    ResponseEvaluatorStub evaluator{};
    OutputFileStub outputFile{};
    RandomizerStub randomizer{};
    RecognitionTestModelImpl model{
        &targetPlayer, &maskerPlayer, &evaluator, &outputFile, &randomizer};
    TestMethodStub testMethod;
    PlayingCalibration playingCalibration{};
    InitializingTest initializingTest{&testMethod};
    PlayingTrial playingTrial;
    SubmittingCoordinateResponse submittingCoordinateResponse;
    SubmittingCorrectResponse submittingCorrectResponse;
    SubmittingIncorrectResponse submittingIncorrectResponse;
    SubmittingFreeResponse submittingFreeResponse;

    RecognitionTestModelTests() { model.subscribe(&listener); }

    void run(UseCase &useCase) { useCase.run(model); }

    void assertTargetVideoOnlyHidden() {
        assertTrue(targetPlayerVideoHidden());
        assertTargetVideoNotShown();
    }

    bool targetPlayerVideoHidden() { return targetPlayer.videoHidden(); }

    void assertTargetVideoNotShown() { assertFalse(targetPlayerVideoShown()); }

    bool targetPlayerVideoShown() { return targetPlayer.videoShown(); }

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

    auto &outputFileLog() { return outputFile.log(); }

    void assertOutputFileLog(std::string s) {
        assertEqual(std::move(s), outputFileLog());
    }

    template <typename T>
    void assertDevicePassedToPlayer(
        const T &player, AudioDeviceUseCase &useCase) {
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

    bool maskerPlayerFadedIn() { return maskerPlayer.fadeInCalled(); }

    void assertTargetPlayerPlayed() { assertTrue(targetPlayerPlayed()); }

    bool targetPlayerPlayed() { return targetPlayer.played(); }

    auto targetFilePath() { return targetPlayer.filePath(); }

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

    void assertSeeksToRandomMaskerPositionWithinTrialDuration(
        UseCase &useCase) {
        targetPlayer.setDurationSeconds(1);
        maskerPlayer.setFadeTimeSeconds(2);
        maskerPlayer.setDurationSeconds(3);
        run(useCase);
        assertEqual(0., randomizer.lowerFloatBound());
        assertEqual(3. - 2 - 1 - 2, randomizer.upperFloatBound());
    }

    auto maskerPlayerSecondsSeeked() { return maskerPlayer.secondsSeeked(); }

    void assertMaskerPlayerSeekedToRandomTime(UseCase &useCase) {
        randomizer.setRandomFloat(1);
        run(useCase);
        assertEqual(1., maskerPlayerSecondsSeeked());
    }

    double dB(double x) { return 20 * std::log10(x); }

    auto targetPlayerLevel_dB() { return targetPlayer.level_dB(); }

    void assertTargetPlayerLevelEquals_dB(double x) {
        assertEqual(x, targetPlayerLevel_dB());
    }

    void setMaskerLevel_dB_SPL(int x) {
        initializingTest.setMaskerLevel_dB_SPL(x);
    }

    void setTestingFullScaleLevel_dB_SPL(int x) {
        initializingTest.setTestingFullScaleLevel_dB_SPL(x);
    }

    void setMaskerRms(double x) { maskerPlayer.setRms(x); }

    void setSnr_dB(int x) { testMethod.setSnr_dB(x); }

    void maskerFadeOutComplete() { maskerPlayer.fadeOutComplete(); }

    void assertSavesOutputFileAfterWritingTrial(UseCase &useCase) {
        run(useCase);
        assertTrue(outputFileLog().endsWith("save "));
    }

    void assertCallThrowsRequestFailure(UseCase &useCase, std::string what) {
        try {
            run(useCase);
            FAIL() << "Expected recognition_test::"
                      "ModelImpl::"
                      "RequestFailure";
        } catch (const ModelImpl::RequestFailure &e) {
            assertEqual(std::move(what), e.what());
        }
    }

    void setMaskerFilePath(std::string s) {
        initializingTest.setMaskerFilePath(std::move(s));
    }

    void assertThrowsRequestFailureWhenInvalidAudioDevice(
        AudioDeviceUseCase &useCase) {
        throwInvalidAudioDeviceWhenSet();
        useCase.setAudioDevice("a");
        assertCallThrowsRequestFailure(
            useCase, "'a' is not a valid audio device.");
    }

    void throwInvalidAudioDeviceWhenSet() {
        maskerPlayer.throwInvalidAudioDeviceWhenDeviceSet();
        targetPlayer.throwInvalidAudioDeviceWhenDeviceSet();
    }

    void runIgnoringFailureWithTrialInProgress(UseCase &useCase) {
        setTrialInProgress();
        runIgnoringFailure(useCase);
    }

    void setTrialInProgress() { maskerPlayer.setPlaying(); }

    void runIgnoringFailure(UseCase &useCase) {
        try {
            run(useCase);
        } catch (const ModelImpl::RequestFailure &) {
        }
    }

    void assertMaskerPlayerNotPlayed() { assertFalse(maskerPlayerFadedIn()); }

    void assertTargetPlayerNotPlayed() { assertFalse(targetPlayerPlayed()); }

    void assertThrowsRequestFailureWhenTrialInProgress(UseCase &useCase) {
        setTrialInProgress();
        assertCallThrowsRequestFailure(useCase, "Trial in progress.");
    }

    void assertTestIncomplete() { assertFalse(testComplete()); }

    bool testComplete() { return model.testComplete(); }

    void assertTestComplete() { assertTrue(testComplete()); }

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

    void assertPassesCurrentTargetToEvaluatorBeforeAdvancingTarget(
        UseCase &useCase) {
        run(initializingTest);
        testMethod.setCurrent("a");
        testMethod.setCurrentWhenNext("b");
        run(useCase);
        assertEqual("a", evaluator.filePathForFileName());
    }

    void assertTestMethodLogContains(
        UseCase &useCase, const std::string &what) {
        run(initializingTest);
        run(useCase);
        assertTrue(testMethod.log().contains(what));
    }

    void assertTrialNumber(int n) { assertEqual(n, model.trialNumber()); }

    void assertYieldsTrialNumber(UseCase &useCase, int n) {
        run(useCase);
        assertTrialNumber(n);
    }
};

TEST_F(RecognitionTestModelTests, subscribesToPlayerEvents) {
    assertEqual(static_cast<TargetPlayer::EventListener *>(&model),
        targetPlayer.listener());
    assertEqual(static_cast<MaskerPlayer::EventListener *>(&model),
        maskerPlayer.listener());
}

TEST_F(RecognitionTestModelTests,
    playCalibrationHidesTargetVideoWhenAuditoryOnly) {
    assertTargetVideoHiddenWhenAuditoryOnly(playingCalibration);
}

TEST_F(
    RecognitionTestModelTests, playCalibrationShowsTargetVideoWhenAudioVisual) {
    assertTargetVideoShownWhenAudioVisual(playingCalibration);
}

TEST_F(RecognitionTestModelTests,
    initializeTestClosesOutputFileOpensAndWritesTestInOrder) {
    assertClosesOutputFileOpensAndWritesTestInOrder(initializingTest);
}

TEST_F(RecognitionTestModelTests,
    initializeTestOpensNewOutputFilePassingTestInformation) {
    run(initializingTest);
    assertEqual(
        outputFile.openNewFileParameters(), &initializingTest.testIdentity());
}

TEST_F(RecognitionTestModelTests, playTrialPassesAudioDeviceToTargetPlayer) {
    assertDevicePassedToTargetPlayer(playingTrial);
}

TEST_F(
    RecognitionTestModelTests, playCalibrationPassesAudioDeviceToTargetPlayer) {
    assertDevicePassedToTargetPlayer(playingCalibration);
}

TEST_F(RecognitionTestModelTests, playTrialPassesAudioDeviceToMaskerPlayer) {
    assertDevicePassedToMaskerPlayer(playingTrial);
}

TEST_F(RecognitionTestModelTests, playTrialFadesInMasker) {
    run(playingTrial);
    assertTrue(maskerPlayerFadedIn());
}

TEST_F(RecognitionTestModelTests, playCalibrationPlaysTarget) {
    run(playingCalibration);
    assertTargetPlayerPlayed();
}

TEST_F(RecognitionTestModelTests, fadeInCompletePlaysTarget) {
    maskerPlayer.fadeInComplete();
    assertTargetPlayerPlayed();
}

TEST_F(
    RecognitionTestModelTests, initializeTestPassesNextTargetToTargetPlayer) {
    assertPassesNextTargetToPlayer(initializingTest);
}

TEST_F(RecognitionTestModelTests, initializingTestResetsTrialNumber) {
    assertYieldsTrialNumber(initializingTest, 1);
}

TEST_F(RecognitionTestModelTests,
    submittingCoordinateResponseIncrementsTrialNumber) {
    run(initializingTest);
    assertYieldsTrialNumber(submittingCoordinateResponse, 2);
}

TEST_F(
    RecognitionTestModelTests, submittingCorrectResponseIncrementsTrialNumber) {
    run(initializingTest);
    assertYieldsTrialNumber(submittingCorrectResponse, 2);
}

TEST_F(RecognitionTestModelTests,
    submittingIncorrectResponseIncrementsTrialNumber) {
    run(initializingTest);
    assertYieldsTrialNumber(submittingIncorrectResponse, 2);
}

TEST_F(RecognitionTestModelTests, submittingFreeResponseIncrementsTrialNumber) {
    run(initializingTest);
    assertYieldsTrialNumber(submittingFreeResponse, 2);
}

TEST_F(RecognitionTestModelTests,
    submittingCoordinateResponsePassesNextTargetToTargetPlayer) {
    run(initializingTest);
    assertPassesNextTargetToPlayer(submittingCoordinateResponse);
}

TEST_F(RecognitionTestModelTests,
    submittingCorrectResponsePassesNextTargetToTargetPlayer) {
    run(initializingTest);
    assertPassesNextTargetToPlayer(submittingCorrectResponse);
}

TEST_F(RecognitionTestModelTests,
    submittingIncorrectResponsePassesNextTargetToTargetPlayer) {
    run(initializingTest);
    assertPassesNextTargetToPlayer(submittingIncorrectResponse);
}

TEST_F(RecognitionTestModelTests,
    submittingFreeResponsePassesNextTargetToTargetPlayer) {
    run(initializingTest);
    assertPassesNextTargetToPlayer(submittingFreeResponse);
}

TEST_F(
    RecognitionTestModelTests, playCalibrationPassesAudioFileToTargetPlayer) {
    playingCalibration.setFilePath("a");
    run(playingCalibration);
    assertTargetFilePathEquals("a");
}

TEST_F(RecognitionTestModelTests,
    initializeTestPassesMaskerFilePathToMaskerPlayer) {
    setMaskerFilePath("a");
    run(initializingTest);
    assertEqual("a", maskerPlayer.filePath());
}

TEST_F(RecognitionTestModelTests,
    initializeTestSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(initializingTest);
}

TEST_F(RecognitionTestModelTests,
    submitCoordinateResponseSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(
        submittingCoordinateResponse);
}

TEST_F(RecognitionTestModelTests,
    submitFreeResponseSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(submittingFreeResponse);
}

TEST_F(RecognitionTestModelTests,
    submitCorrectResponseSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(submittingCorrectResponse);
}

TEST_F(RecognitionTestModelTests,
    submitIncorrectResponseSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(submittingIncorrectResponse);
}

TEST_F(RecognitionTestModelTests,
    initializeTestSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(initializingTest);
}

TEST_F(RecognitionTestModelTests,
    submitCoordinateResponseSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(
        submittingCoordinateResponse);
}

TEST_F(RecognitionTestModelTests,
    submitCorrectResponseSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(
        submittingCorrectResponse);
}

TEST_F(RecognitionTestModelTests,
    submitIncorrectResponseSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(
        submittingIncorrectResponse);
}

TEST_F(RecognitionTestModelTests,
    submitFreeResponseSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(
        submittingFreeResponse);
}

TEST_F(RecognitionTestModelTests, initializeTestSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(initializingTest);
}

TEST_F(RecognitionTestModelTests,
    submitCoordinateResponseSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(submittingCoordinateResponse);
}

TEST_F(RecognitionTestModelTests,
    submitCorrectResponseSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(submittingCorrectResponse);
}

TEST_F(RecognitionTestModelTests,
    submitIncorrectResponseSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(submittingIncorrectResponse);
}

TEST_F(
    RecognitionTestModelTests, submitFreeResponseSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(submittingFreeResponse);
}

TEST_F(RecognitionTestModelTests, initializeTestSetsInitialMaskerPlayerLevel) {
    setMaskerLevel_dB_SPL(1);
    setTestingFullScaleLevel_dB_SPL(2);
    setMaskerRms(3);
    run(initializingTest);
    assertEqual(1 - 2 - dB(3), maskerPlayer.level_dB());
}

TEST_F(RecognitionTestModelTests, initializeTestSetsTargetPlayerLevel) {
    setSnr_dB(2);
    setMaskerLevel_dB_SPL(3);
    setTestingFullScaleLevel_dB_SPL(4);
    setMaskerRms(5);
    run(initializingTest);
    assertTargetPlayerLevelEquals_dB(2 + 3 - 4 - dB(5));
}

TEST_F(
    RecognitionTestModelTests, submitCoordinateResponseSetsTargetPlayerLevel) {
    assertSetsTargetLevel(submittingCoordinateResponse);
}

TEST_F(RecognitionTestModelTests, submitFreeResponseSetsTargetPlayerLevel) {
    assertSetsTargetLevel(submittingFreeResponse);
}

TEST_F(RecognitionTestModelTests, submitCorrectResponseSetsTargetPlayerLevel) {
    assertSetsTargetLevel(submittingCorrectResponse);
}

TEST_F(
    RecognitionTestModelTests, submitIncorrectResponseSetsTargetPlayerLevel) {
    assertSetsTargetLevel(submittingIncorrectResponse);
}

TEST_F(RecognitionTestModelTests, playCalibrationSetsTargetPlayerLevel) {
    playingCalibration.setLevel_dB_SPL(1);
    playingCalibration.setFullScaleLevel_dB_SPL(2);
    targetPlayer.setRms(3);
    run(playingCalibration);
    assertTargetPlayerLevelEquals_dB(1 - 2 - dB(3));
}

TEST_F(RecognitionTestModelTests, startTrialShowsTargetPlayerWhenAudioVisual) {
    initializingTest.setAudioVisual();
    run(initializingTest);
    run(playingTrial);
    assertTrue(targetPlayerVideoShown());
}

TEST_F(RecognitionTestModelTests, maskerFadeOutCompleteHidesTargetPlayer) {
    maskerFadeOutComplete();
    assertTargetVideoOnlyHidden();
}

TEST_F(RecognitionTestModelTests,
    startTrialDoesNotShowTargetPlayerWhenAuditoryOnly) {
    initializingTest.setAuditoryOnly();
    run(initializingTest);
    run(playingTrial);
    assertTargetVideoNotShown();
}

TEST_F(RecognitionTestModelTests, initializeTestHidesTargetPlayer) {
    run(initializingTest);
    assertTargetVideoOnlyHidden();
}

TEST_F(RecognitionTestModelTests, targetPlaybackCompleteFadesOutMasker) {
    targetPlayer.playbackComplete();
    assertTrue(maskerPlayer.fadeOutCalled());
}

TEST_F(RecognitionTestModelTests, fadeOutCompleteNotifiesTrialComplete) {
    maskerFadeOutComplete();
    assertTrue(listener.notified());
}

TEST_F(RecognitionTestModelTests,
    submitCoordinateResponseSavesOutputFileAfterWritingTrial) {
    assertSavesOutputFileAfterWritingTrial(submittingCoordinateResponse);
}

TEST_F(RecognitionTestModelTests,
    submitFreeResponseSavesOutputFileAfterWritingTrial) {
    assertSavesOutputFileAfterWritingTrial(submittingFreeResponse);
}

TEST_F(RecognitionTestModelTests,
    submitCorrectResponseSavesOutputFileAfterWritingTrial) {
    assertSavesOutputFileAfterWritingTrial(submittingCorrectResponse);
}

TEST_F(RecognitionTestModelTests,
    submitIncorrectResponseSavesOutputFileAfterWritingTrial) {
    assertSavesOutputFileAfterWritingTrial(submittingIncorrectResponse);
}

TEST_F(RecognitionTestModelTests,
    initializeTestThrowsRequestFailureIfFileFailsToOpen) {
    outputFile.throwOnOpen();
    assertCallThrowsRequestFailure(
        initializingTest, "Unable to open output file.");
}

TEST_F(RecognitionTestModelTests,
    playCalibrationThrowsRequestFailureWhenTargetPlayerThrowsInvalidAudioFile) {
    playingCalibration.setFilePath("a");
    targetPlayer.throwInvalidAudioFileOnRms();
    assertCallThrowsRequestFailure(playingCalibration, "unable to read a");
}

TEST_F(RecognitionTestModelTests,
    initializeTestThrowsRequestFailureWhenMaskerPlayerThrowsInvalidAudioFile) {
    setMaskerFilePath("a");
    maskerPlayer.throwInvalidAudioFileOnLoad();
    assertCallThrowsRequestFailure(initializingTest, "unable to read a");
}

TEST_F(RecognitionTestModelTests,
    playTrialWithInvalidAudioDeviceThrowsRequestFailure) {
    assertThrowsRequestFailureWhenInvalidAudioDevice(playingTrial);
}

TEST_F(RecognitionTestModelTests,
    playCalibrationWithInvalidAudioDeviceThrowsRequestFailure) {
    assertThrowsRequestFailureWhenInvalidAudioDevice(playingCalibration);
}

TEST_F(RecognitionTestModelTests,
    playTrialDoesNotChangeAudioDeviceWhenTrialInProgress) {
    runIgnoringFailureWithTrialInProgress(playingTrial);
    assertFalse(maskerPlayer.setDeviceCalled());
}

TEST_F(RecognitionTestModelTests,
    playCalibrationDoesNotChangeAudioDeviceWhenTrialInProgress) {
    runIgnoringFailureWithTrialInProgress(playingCalibration);
    assertFalse(targetPlayer.setDeviceCalled());
}

TEST_F(RecognitionTestModelTests, playTrialDoesNotPlayIfTrialInProgress) {
    runIgnoringFailureWithTrialInProgress(playingTrial);
    assertMaskerPlayerNotPlayed();
}

TEST_F(RecognitionTestModelTests, playCalibrationDoesNotPlayIfTrialInProgress) {
    runIgnoringFailureWithTrialInProgress(playingCalibration);
    assertTargetPlayerNotPlayed();
}

TEST_F(RecognitionTestModelTests,
    initializeTestThrowsRequestFailureIfTrialInProgress) {
    assertThrowsRequestFailureWhenTrialInProgress(initializingTest);
}

TEST_F(
    RecognitionTestModelTests, playTrialThrowsRequestFailureIfTrialInProgress) {
    assertThrowsRequestFailureWhenTrialInProgress(playingTrial);
}

TEST_F(RecognitionTestModelTests,
    playCalibrationThrowsRequestFailureIfTrialInProgress) {
    assertThrowsRequestFailureWhenTrialInProgress(playingCalibration);
}

TEST_F(RecognitionTestModelTests,
    initializeTestDoesNotLoadMaskerIfTrialInProgress) {
    setMaskerFilePath("a");
    runIgnoringFailureWithTrialInProgress(initializingTest);
    assertEqual("", maskerPlayer.filePath());
}

TEST_F(RecognitionTestModelTests,
    initializeTestDoesNotHideTargetPlayerWhenAuditoryOnlyButTrialInProgress) {
    initializingTest.setAuditoryOnly();
    runIgnoringFailureWithTrialInProgress(initializingTest);
    assertTargetVideoNotHidden();
}

TEST_F(RecognitionTestModelTests,
    audioDevicesReturnsOutputAudioDeviceDescriptions) {
    maskerPlayer.setOutputAudioDeviceDescriptions({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, model.audioDevices());
}

TEST_F(RecognitionTestModelTests, testCompleteWhenComplete) {
    run(initializingTest);
    assertTestIncomplete();
    testMethod.setComplete();
    assertTestComplete();
}

TEST_F(RecognitionTestModelTests,
    submitCoordinateResponseDoesNotLoadNextTargetWhenComplete) {
    assertResponseDoesNotLoadNextTargetWhenComplete(
        submittingCoordinateResponse);
}

TEST_F(RecognitionTestModelTests,
    submitFreeResponseDoesNotLoadNextTargetWhenComplete) {
    assertResponseDoesNotLoadNextTargetWhenComplete(submittingFreeResponse);
}

TEST_F(RecognitionTestModelTests,
    submitCorrectResponseDoesNotLoadNextTargetWhenComplete) {
    assertResponseDoesNotLoadNextTargetWhenComplete(submittingCorrectResponse);
}

TEST_F(RecognitionTestModelTests,
    submitIncorrectResponseDoesNotLoadNextTargetWhenComplete) {
    assertResponseDoesNotLoadNextTargetWhenComplete(
        submittingIncorrectResponse);
}

TEST_F(RecognitionTestModelTests,
    initializeTestDoesNotLoadNextTargetWhenComplete) {
    testMethod.setNextTarget("a");
    testMethod.setComplete();
    run(initializingTest);
    assertTargetFilePathEquals("");
}

TEST_F(RecognitionTestModelTests, submitFreeResponseWritesResponse) {
    submittingFreeResponse.setResponse("a");
    run(submittingFreeResponse);
    assertEqual("a", writtenFreeResponseTrial().response);
}

TEST_F(RecognitionTestModelTests, submitFreeResponseWritesFlagged) {
    submittingFreeResponse.setFlagged();
    run(submittingFreeResponse);
    assertTrue(writtenFreeResponseTrial().flagged);
}

TEST_F(RecognitionTestModelTests, submitFreeResponseWritesWithoutFlag) {
    run(submittingFreeResponse);
    assertFalse(writtenFreeResponseTrial().flagged);
}

TEST_F(RecognitionTestModelTests, submitFreeResponseWritesTarget) {
    assertWritesTarget(submittingFreeResponse);
}

TEST_F(RecognitionTestModelTests,
    submitFreeResponsePassesCurrentTargetToEvaluatorBeforeAdvancingTarget) {
    assertPassesCurrentTargetToEvaluatorBeforeAdvancingTarget(
        submittingFreeResponse);
}

TEST_F(RecognitionTestModelTests,
    submitCoordinateResponseWritesTrialAfterSubmittingResponse) {
    assertTestMethodLogContains(submittingCoordinateResponse,
        "submitResponse writeLastCoordinateResponse ");
}

TEST_F(RecognitionTestModelTests,
    submitCorrectResponseWritesTrialAfterSubmittingResponse) {
    assertTestMethodLogContains(submittingCorrectResponse,
        "submitCorrectResponse writeLastCorrectResponse ");
}

TEST_F(RecognitionTestModelTests,
    submitIncorrectResponseWritesTrialAfterSubmittingResponse) {
    assertTestMethodLogContains(submittingIncorrectResponse,
        "submitIncorrectResponse writeLastIncorrectResponse ");
}

TEST_F(RecognitionTestModelTests,
    submitCoordinateResponseQueriesNextTargetAfterWritingResponse) {
    assertTestMethodLogContains(
        submittingCoordinateResponse, "writeLastCoordinateResponse next ");
}

TEST_F(RecognitionTestModelTests,
    submitIncorrectResponseQueriesNextTargetAfterWritingResponse) {
    assertTestMethodLogContains(
        submittingIncorrectResponse, "writeLastIncorrectResponse next ");
}

TEST_F(RecognitionTestModelTests,
    submitCorrectResponseQueriesNextTargetAfterWritingResponse) {
    assertTestMethodLogContains(
        submittingCorrectResponse, "writeLastCorrectResponse next ");
}

TEST_F(RecognitionTestModelTests, submitCorrectResponseSubmitsCorrectResponse) {
    run(initializingTest);
    run(submittingCorrectResponse);
    assertTrue(testMethod.submittedCorrectResponse());
}

TEST_F(RecognitionTestModelTests,
    submitIncorrectResponseSubmitsIncorrectResponse) {
    run(initializingTest);
    run(submittingIncorrectResponse);
    assertTrue(testMethod.submittedIncorrectResponse());
}
}
}
