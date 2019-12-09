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

namespace av_speech_in_noise::tests {
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

    auto complete() -> bool override { return complete_; }

    auto next() -> std::string override {
        log_.insert("next ");
        current_ = currentWhenNext_;
        return next_;
    }

    auto current() -> std::string override { return current_; }

    void setCurrent(std::string s) { current_ = std::move(s); }

    void setCurrentWhenNext(std::string s) { currentWhenNext_ = std::move(s); }

    auto snr_dB() -> int override { return snr_dB_; }

    void submitCorrectResponse() override {
        log_.insert("submitCorrectResponse ");
        submittedCorrectResponse_ = true;
    }

    void submitIncorrectResponse() override {
        log_.insert("submitIncorrectResponse ");
        submittedIncorrectResponse_ = true;
    }

    void submitResponse(const open_set::FreeResponse &) override {}

    void writeTestingParameters(OutputFile *file) override {
        file->writeTest(AdaptiveTest{});
    }

    void writeLastCoordinateResponse(OutputFile *) override {
        log_.insert("writeLastCoordinateResponse ");
    }

    void writeLastCorrectResponse(OutputFile *) override {
        log_.insert("writeLastCorrectResponse ");
    }

    void writeLastIncorrectResponse(OutputFile *) override {
        log_.insert("writeLastIncorrectResponse ");
    }

    void submitResponse(
        const coordinate_response_measure::Response &) override {
        log_.insert("submitResponse ");
    }

    auto log() const -> auto & { return log_; }
};

class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run(ModelImpl &) {}
    virtual void run(RecognitionTestModelImpl &) = 0;
};

class TargetWritingUseCase : public virtual UseCase {
  public:
    virtual auto writtenTarget(OutputFileStub &) -> std::string = 0;
};

class SubmittingResponse : public virtual UseCase {};

class InitializingTest : public UseCase {
    TestIdentity information{};
    Test test{};
    TestMethod *method;

  public:
    explicit InitializingTest(TestMethod *method) : method{method} {}

    void run(RecognitionTestModelImpl &m) override {
        m.initialize(method, test);
    }

    [[nodiscard]] auto testIdentity() const -> auto & { return test.identity; }

    void setMaskerFilePath(std::string s) {
        test.maskerFilePath = std::move(s);
    }

    void setMaskerLevel_dB_SPL(int x) { test.maskerLevel_dB_SPL = x; }

    void setTestingFullScaleLevel_dB_SPL(int x) {
        test.fullScaleLevel_dB_SPL = x;
    }

    void setAudioVisual() { test.condition = Condition::audioVisual; }

    void setAuditoryOnly() { test.condition = Condition::auditoryOnly; }
};

class InitializingTestWithSingleSpeaker : public UseCase {
    TestIdentity information{};
    Test test{};
    TestMethod *method;

  public:
    explicit InitializingTestWithSingleSpeaker(TestMethod *method) : method{method} {}

    void run(RecognitionTestModelImpl &m) override {
        m.initializeWithSingleSpeaker(method, test);
    }

    [[nodiscard]] auto testIdentity() const -> auto & { return test.identity; }

    void setMaskerFilePath(std::string s) {
        test.maskerFilePath = std::move(s);
    }

    void setMaskerLevel_dB_SPL(int x) { test.maskerLevel_dB_SPL = x; }

    void setTestingFullScaleLevel_dB_SPL(int x) {
        test.fullScaleLevel_dB_SPL = x;
    }

    void setAudioVisual() { test.condition = Condition::audioVisual; }

    void setAuditoryOnly() { test.condition = Condition::auditoryOnly; }
};

class InitializingTestWithDelayedMasker : public UseCase {
    TestIdentity information{};
    Test test{};
    TestMethod *method;

  public:
    explicit InitializingTestWithDelayedMasker(TestMethod *method) : method{method} {}

    void run(RecognitionTestModelImpl &m) override {
        m.initializeWithDelayedMasker(method, test);
    }

    [[nodiscard]] auto testIdentity() const -> auto & { return test.identity; }

    void setMaskerFilePath(std::string s) {
        test.maskerFilePath = std::move(s);
    }

    void setMaskerLevel_dB_SPL(int x) { test.maskerLevel_dB_SPL = x; }

    void setTestingFullScaleLevel_dB_SPL(int x) {
        test.fullScaleLevel_dB_SPL = x;
    }

    void setAudioVisual() { test.condition = Condition::audioVisual; }

    void setAuditoryOnly() { test.condition = Condition::auditoryOnly; }
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
    open_set::FreeResponse response_{};

  public:
    void run(RecognitionTestModelImpl &m) override {
        m.submitResponse(response_);
    }

    auto writtenTarget(OutputFileStub &file) -> std::string override {
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

    [[nodiscard]] auto response() const -> auto & { return response_; }
};

class SubmittingCorrectResponse : public TargetWritingUseCase {
  public:
    void run(RecognitionTestModelImpl &m) override {
        m.submitCorrectResponse();
    }

    auto writtenTarget(OutputFileStub &file) -> std::string override {
        return file.writtenOpenSetAdaptiveTrial().target;
    }
};

class SubmittingIncorrectResponse : public TargetWritingUseCase {
  public:
    void run(RecognitionTestModelImpl &m) override {
        m.submitIncorrectResponse();
    }

    auto writtenTarget(OutputFileStub &file) -> std::string override {
        return file.writtenOpenSetAdaptiveTrial().target;
    }
};

auto dB(double x) -> double { return 20 * std::log10(x); }

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
    InitializingTestWithSingleSpeaker initializingTestWithSingleSpeaker{&testMethod};
    InitializingTestWithDelayedMasker initializingTestWithDelayedMasker{&testMethod};
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

    auto targetPlayerVideoHidden() -> bool {
        return targetPlayer.videoHidden();
    }

    void assertTargetVideoNotShown() { assertFalse(targetPlayerVideoShown()); }

    auto targetPlayerVideoShown() -> bool { return targetPlayer.videoShown(); }

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

    auto outputFileLog() -> auto & { return outputFile.log(); }

    void assertOutputFileLog(const std::string &s) {
        assertEqual(s, outputFileLog());
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

    auto maskerPlayerFadedIn() -> bool { return maskerPlayer.fadeInCalled(); }

    void assertTargetPlayerPlayed() { assertTrue(targetPlayerPlayed()); }

    auto targetPlayerPlayed() -> bool { return targetPlayer.played(); }

    auto targetFilePath() { return targetPlayer.filePath(); }

    void assertTargetFilePathEquals(const std::string &what) {
        assertEqual(what, targetFilePath());
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

    auto targetPlayerPlaybackCompletionSubscribed() -> bool {
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

    void assertCallThrowsRequestFailure(
        UseCase &useCase, const std::string &what) {
        try {
            run(useCase);
            FAIL() << "Expected recognition_test::"
                      "ModelImpl::"
                      "RequestFailure";
        } catch (const ModelImpl::RequestFailure &e) {
            assertEqual(what, e.what());
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

    auto testComplete() -> bool { return model.testComplete(); }

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

    void assertOnlyUsingFirstChannelOfTargetPlayer() {
        assertTrue(targetPlayer.usingFirstChannelOnly());
    }

    void assertUsingAllMaskerPlayerChannels() {
        assertTrue(maskerPlayer.usingAllChannels());
    }

    void assertMaskerPlayerChannelDelaysCleared() {
        assertTrue(maskerPlayer.channelDelaysCleared());
    }
};

#define RECOGNITION_TEST_MODEL_TEST(a) TEST_F(RecognitionTestModelTests, a)

RECOGNITION_TEST_MODEL_TEST(subscribesToPlayerEvents) {
    assertEqual(static_cast<TargetPlayer::EventListener *>(&model),
        targetPlayer.listener());
    assertEqual(static_cast<MaskerPlayer::EventListener *>(&model),
        maskerPlayer.listener());
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationHidesTargetVideoWhenAuditoryOnly) {
    assertTargetVideoHiddenWhenAuditoryOnly(playingCalibration);
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationShowsTargetVideoWhenAudioVisual) {
    assertTargetVideoShownWhenAudioVisual(playingCalibration);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestClosesOutputFileOpensAndWritesTestInOrder) {
    assertClosesOutputFileOpensAndWritesTestInOrder(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithSingleSpeakerClosesOutputFileOpensAndWritesTestInOrder) {
    assertClosesOutputFileOpensAndWritesTestInOrder(initializingTestWithSingleSpeaker);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithDelayedMaskerClosesOutputFileOpensAndWritesTestInOrder) {
    assertClosesOutputFileOpensAndWritesTestInOrder(initializingTestWithDelayedMasker);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestUsesAllTargetPlayerChannels) {
    run(initializingTest);
    assertTrue(targetPlayer.usingAllChannels());
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestUsesAllMaskerPlayerChannels) {
    run(initializingTest);
    assertUsingAllMaskerPlayerChannels();
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestClearsAllMaskerPlayerChannelDelays) {
    run(initializingTest);
    assertMaskerPlayerChannelDelaysCleared();
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithSingleSpeakerUsesFirstChannelOnlyOfTargetPlayer) {
    run(initializingTestWithSingleSpeaker);
    assertOnlyUsingFirstChannelOfTargetPlayer();
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithSingleSpeakerClearsAllMaskerPlayerChannelDelays) {
    run(initializingTestWithSingleSpeaker);
    assertMaskerPlayerChannelDelaysCleared();
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithSingleSpeakerUsesFirstChannelOnlyOfMaskerPlayer) {
    run(initializingTestWithSingleSpeaker);
    assertTrue(maskerPlayer.usingFirstChannelOnly());
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithDelayedMaskerUsesFirstChannelOnlyOfTargetPlayer) {
    run(initializingTestWithDelayedMasker);
    assertOnlyUsingFirstChannelOfTargetPlayer();
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithDelayedMaskerUsesAllMaskerPlayerChannels) {
    run(initializingTestWithDelayedMasker);
    assertUsingAllMaskerPlayerChannels();
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestOpensNewOutputFilePassingTestInformation) {
    run(initializingTest);
    assertEqual(
        outputFile.openNewFileParameters(), &initializingTest.testIdentity());
}

RECOGNITION_TEST_MODEL_TEST(playTrialPassesAudioDeviceToTargetPlayer) {
    assertDevicePassedToTargetPlayer(playingTrial);
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationPassesAudioDeviceToTargetPlayer) {
    assertDevicePassedToTargetPlayer(playingCalibration);
}

RECOGNITION_TEST_MODEL_TEST(playTrialPassesAudioDeviceToMaskerPlayer) {
    assertDevicePassedToMaskerPlayer(playingTrial);
}

RECOGNITION_TEST_MODEL_TEST(playTrialFadesInMasker) {
    run(playingTrial);
    assertTrue(maskerPlayerFadedIn());
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationPlaysTarget) {
    run(playingCalibration);
    assertTargetPlayerPlayed();
}

RECOGNITION_TEST_MODEL_TEST(fadeInCompletePlaysTarget) {
    maskerPlayer.fadeInComplete();
    assertTargetPlayerPlayed();
}

RECOGNITION_TEST_MODEL_TEST(initializeTestPassesNextTargetToTargetPlayer) {
    assertPassesNextTargetToPlayer(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(initializingTestResetsTrialNumber) {
    assertYieldsTrialNumber(initializingTest, 1);
}

RECOGNITION_TEST_MODEL_TEST(submittingCoordinateResponseIncrementsTrialNumber) {
    run(initializingTest);
    assertYieldsTrialNumber(submittingCoordinateResponse, 2);
}

RECOGNITION_TEST_MODEL_TEST(submittingCorrectResponseIncrementsTrialNumber) {
    run(initializingTest);
    assertYieldsTrialNumber(submittingCorrectResponse, 2);
}

RECOGNITION_TEST_MODEL_TEST(submittingIncorrectResponseIncrementsTrialNumber) {
    run(initializingTest);
    assertYieldsTrialNumber(submittingIncorrectResponse, 2);
}

RECOGNITION_TEST_MODEL_TEST(submittingFreeResponseIncrementsTrialNumber) {
    run(initializingTest);
    assertYieldsTrialNumber(submittingFreeResponse, 2);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingCoordinateResponsePassesNextTargetToTargetPlayer) {
    run(initializingTest);
    assertPassesNextTargetToPlayer(submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingCorrectResponsePassesNextTargetToTargetPlayer) {
    run(initializingTest);
    assertPassesNextTargetToPlayer(submittingCorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingIncorrectResponsePassesNextTargetToTargetPlayer) {
    run(initializingTest);
    assertPassesNextTargetToPlayer(submittingIncorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingFreeResponsePassesNextTargetToTargetPlayer) {
    run(initializingTest);
    assertPassesNextTargetToPlayer(submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationPassesAudioFileToTargetPlayer) {
    playingCalibration.setFilePath("a");
    run(playingCalibration);
    assertTargetFilePathEquals("a");
}

RECOGNITION_TEST_MODEL_TEST(initializeTestPassesMaskerFilePathToMaskerPlayer) {
    setMaskerFilePath("a");
    run(initializingTest);
    assertEqual("a", maskerPlayer.filePath());
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(
        submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitFreeResponseSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectResponseSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(submittingCorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitIncorrectResponseSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(submittingIncorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(
        submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectResponseSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(
        submittingCorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitIncorrectResponseSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(
        submittingIncorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitFreeResponseSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(
        submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(initializeTestSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(submitCorrectResponseSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(submittingCorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitIncorrectResponseSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(submittingIncorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(submitFreeResponseSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(initializeTestSetsInitialMaskerPlayerLevel) {
    setMaskerLevel_dB_SPL(1);
    setTestingFullScaleLevel_dB_SPL(2);
    setMaskerRms(3);
    run(initializingTest);
    assertEqual(1 - 2 - dB(3), maskerPlayer.level_dB());
}

RECOGNITION_TEST_MODEL_TEST(initializeTestSetsTargetPlayerLevel) {
    setSnr_dB(2);
    setMaskerLevel_dB_SPL(3);
    setTestingFullScaleLevel_dB_SPL(4);
    setMaskerRms(5);
    run(initializingTest);
    assertTargetPlayerLevelEquals_dB(2 + 3 - 4 - dB(5));
}

RECOGNITION_TEST_MODEL_TEST(submitCoordinateResponseSetsTargetPlayerLevel) {
    assertSetsTargetLevel(submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(submitFreeResponseSetsTargetPlayerLevel) {
    assertSetsTargetLevel(submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(submitCorrectResponseSetsTargetPlayerLevel) {
    assertSetsTargetLevel(submittingCorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(submitIncorrectResponseSetsTargetPlayerLevel) {
    assertSetsTargetLevel(submittingIncorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationSetsTargetPlayerLevel) {
    playingCalibration.setLevel_dB_SPL(1);
    playingCalibration.setFullScaleLevel_dB_SPL(2);
    targetPlayer.setRms(3);
    run(playingCalibration);
    assertTargetPlayerLevelEquals_dB(1 - 2 - dB(3));
}

RECOGNITION_TEST_MODEL_TEST(startTrialShowsTargetPlayerWhenAudioVisual) {
    initializingTest.setAudioVisual();
    run(initializingTest);
    run(playingTrial);
    assertTrue(targetPlayerVideoShown());
}

RECOGNITION_TEST_MODEL_TEST(maskerFadeOutCompleteHidesTargetPlayer) {
    maskerFadeOutComplete();
    assertTargetVideoOnlyHidden();
}

RECOGNITION_TEST_MODEL_TEST(startTrialDoesNotShowTargetPlayerWhenAuditoryOnly) {
    initializingTest.setAuditoryOnly();
    run(initializingTest);
    run(playingTrial);
    assertTargetVideoNotShown();
}

RECOGNITION_TEST_MODEL_TEST(initializeTestHidesTargetPlayer) {
    run(initializingTest);
    assertTargetVideoOnlyHidden();
}

RECOGNITION_TEST_MODEL_TEST(targetPlaybackCompleteFadesOutMasker) {
    targetPlayer.playbackComplete();
    assertTrue(maskerPlayer.fadeOutCalled());
}

RECOGNITION_TEST_MODEL_TEST(fadeOutCompleteNotifiesTrialComplete) {
    maskerFadeOutComplete();
    assertTrue(listener.notified());
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseSavesOutputFileAfterWritingTrial) {
    assertSavesOutputFileAfterWritingTrial(submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitFreeResponseSavesOutputFileAfterWritingTrial) {
    assertSavesOutputFileAfterWritingTrial(submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectResponseSavesOutputFileAfterWritingTrial) {
    assertSavesOutputFileAfterWritingTrial(submittingCorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitIncorrectResponseSavesOutputFileAfterWritingTrial) {
    assertSavesOutputFileAfterWritingTrial(submittingIncorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestThrowsRequestFailureIfFileFailsToOpen) {
    outputFile.throwOnOpen();
    assertCallThrowsRequestFailure(
        initializingTest, "Unable to open output file.");
}

RECOGNITION_TEST_MODEL_TEST(
    playCalibrationThrowsRequestFailureWhenTargetPlayerThrowsInvalidAudioFile) {
    playingCalibration.setFilePath("a");
    targetPlayer.throwInvalidAudioFileOnRms();
    assertCallThrowsRequestFailure(playingCalibration, "unable to read a");
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestThrowsRequestFailureWhenMaskerPlayerThrowsInvalidAudioFile) {
    setMaskerFilePath("a");
    maskerPlayer.throwInvalidAudioFileOnLoad();
    assertCallThrowsRequestFailure(initializingTest, "unable to read a");
}

RECOGNITION_TEST_MODEL_TEST(
    playTrialWithInvalidAudioDeviceThrowsRequestFailure) {
    assertThrowsRequestFailureWhenInvalidAudioDevice(playingTrial);
}

RECOGNITION_TEST_MODEL_TEST(
    playCalibrationWithInvalidAudioDeviceThrowsRequestFailure) {
    assertThrowsRequestFailureWhenInvalidAudioDevice(playingCalibration);
}

RECOGNITION_TEST_MODEL_TEST(
    playTrialDoesNotChangeAudioDeviceWhenTrialInProgress) {
    runIgnoringFailureWithTrialInProgress(playingTrial);
    assertFalse(maskerPlayer.setDeviceCalled());
}

RECOGNITION_TEST_MODEL_TEST(
    playCalibrationDoesNotChangeAudioDeviceWhenTrialInProgress) {
    runIgnoringFailureWithTrialInProgress(playingCalibration);
    assertFalse(targetPlayer.setDeviceCalled());
}

RECOGNITION_TEST_MODEL_TEST(playTrialDoesNotPlayIfTrialInProgress) {
    runIgnoringFailureWithTrialInProgress(playingTrial);
    assertMaskerPlayerNotPlayed();
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationDoesNotPlayIfTrialInProgress) {
    runIgnoringFailureWithTrialInProgress(playingCalibration);
    assertTargetPlayerNotPlayed();
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestThrowsRequestFailureIfTrialInProgress) {
    assertThrowsRequestFailureWhenTrialInProgress(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(playTrialThrowsRequestFailureIfTrialInProgress) {
    assertThrowsRequestFailureWhenTrialInProgress(playingTrial);
}

RECOGNITION_TEST_MODEL_TEST(
    playCalibrationThrowsRequestFailureIfTrialInProgress) {
    assertThrowsRequestFailureWhenTrialInProgress(playingCalibration);
}

RECOGNITION_TEST_MODEL_TEST(initializeTestDoesNotLoadMaskerIfTrialInProgress) {
    setMaskerFilePath("a");
    runIgnoringFailureWithTrialInProgress(initializingTest);
    assertEqual("", maskerPlayer.filePath());
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestDoesNotHideTargetPlayerWhenAuditoryOnlyButTrialInProgress) {
    initializingTest.setAuditoryOnly();
    runIgnoringFailureWithTrialInProgress(initializingTest);
    assertTargetVideoNotHidden();
}

RECOGNITION_TEST_MODEL_TEST(audioDevicesReturnsOutputAudioDeviceDescriptions) {
    maskerPlayer.setOutputAudioDeviceDescriptions({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, model.audioDevices());
}

RECOGNITION_TEST_MODEL_TEST(testCompleteWhenComplete) {
    run(initializingTest);
    assertTestIncomplete();
    testMethod.setComplete();
    assertTestComplete();
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseDoesNotLoadNextTargetWhenComplete) {
    assertResponseDoesNotLoadNextTargetWhenComplete(
        submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitFreeResponseDoesNotLoadNextTargetWhenComplete) {
    assertResponseDoesNotLoadNextTargetWhenComplete(submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectResponseDoesNotLoadNextTargetWhenComplete) {
    assertResponseDoesNotLoadNextTargetWhenComplete(submittingCorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitIncorrectResponseDoesNotLoadNextTargetWhenComplete) {
    assertResponseDoesNotLoadNextTargetWhenComplete(
        submittingIncorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(initializeTestDoesNotLoadNextTargetWhenComplete) {
    testMethod.setNextTarget("a");
    testMethod.setComplete();
    run(initializingTest);
    assertTargetFilePathEquals("");
}

RECOGNITION_TEST_MODEL_TEST(submitFreeResponseWritesResponse) {
    submittingFreeResponse.setResponse("a");
    run(submittingFreeResponse);
    assertEqual("a", writtenFreeResponseTrial().response);
}

RECOGNITION_TEST_MODEL_TEST(submitFreeResponseWritesFlagged) {
    submittingFreeResponse.setFlagged();
    run(submittingFreeResponse);
    assertTrue(writtenFreeResponseTrial().flagged);
}

RECOGNITION_TEST_MODEL_TEST(submitFreeResponseWritesWithoutFlag) {
    run(submittingFreeResponse);
    assertFalse(writtenFreeResponseTrial().flagged);
}

RECOGNITION_TEST_MODEL_TEST(submitFreeResponseWritesTarget) {
    assertWritesTarget(submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitFreeResponsePassesCurrentTargetToEvaluatorBeforeAdvancingTarget) {
    assertPassesCurrentTargetToEvaluatorBeforeAdvancingTarget(
        submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseWritesTrialAfterSubmittingResponse) {
    assertTestMethodLogContains(submittingCoordinateResponse,
        "submitResponse writeLastCoordinateResponse ");
}

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectResponseWritesTrialAfterSubmittingResponse) {
    assertTestMethodLogContains(submittingCorrectResponse,
        "submitCorrectResponse writeLastCorrectResponse ");
}

RECOGNITION_TEST_MODEL_TEST(
    submitIncorrectResponseWritesTrialAfterSubmittingResponse) {
    assertTestMethodLogContains(submittingIncorrectResponse,
        "submitIncorrectResponse writeLastIncorrectResponse ");
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseQueriesNextTargetAfterWritingResponse) {
    assertTestMethodLogContains(
        submittingCoordinateResponse, "writeLastCoordinateResponse next ");
}

RECOGNITION_TEST_MODEL_TEST(
    submitIncorrectResponseQueriesNextTargetAfterWritingResponse) {
    assertTestMethodLogContains(
        submittingIncorrectResponse, "writeLastIncorrectResponse next ");
}

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectResponseQueriesNextTargetAfterWritingResponse) {
    assertTestMethodLogContains(
        submittingCorrectResponse, "writeLastCorrectResponse next ");
}

RECOGNITION_TEST_MODEL_TEST(submitCorrectResponseSubmitsCorrectResponse) {
    run(initializingTest);
    run(submittingCorrectResponse);
    assertTrue(testMethod.submittedCorrectResponse());
}

RECOGNITION_TEST_MODEL_TEST(submitIncorrectResponseSubmitsIncorrectResponse) {
    run(initializingTest);
    run(submittingIncorrectResponse);
    assertTrue(testMethod.submittedIncorrectResponse());
}
}
}
