#include "LogString.h"
#include "MaskerPlayerStub.h"
#include "ModelEventListenerStub.h"
#include "OutputFileStub.h"
#include "RandomizerStub.h"
#include "ResponseEvaluatorStub.h"
#include "TargetPlayerStub.h"
#include "assert-utility.h"
#include "recognition-test/RecognitionTestModel.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <functional>

namespace av_speech_in_noise::tests {
namespace {
void insert(LogString &log, const std::string &s) { log.insert(s); }

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

    auto nextTarget() -> std::string override {
        insert(log_, "next ");
        current_ = currentWhenNext_;
        return next_;
    }

    auto currentTarget() -> std::string override { return current_; }

    void setCurrent(std::string s) { current_ = std::move(s); }

    void setCurrentWhenNext(std::string s) { currentWhenNext_ = std::move(s); }

    auto snr_dB() -> int override { return snr_dB_; }

    void submitCorrectResponse() override {
        insert(log_, "submitCorrectResponse ");
        submittedCorrectResponse_ = true;
    }

    void submitIncorrectResponse() override {
        insert(log_, "submitIncorrectResponse ");
        submittedIncorrectResponse_ = true;
    }

    void submitResponse(const open_set::FreeResponse &) override {}

    void writeTestingParameters(OutputFile *file) override {
        file->writeTest(AdaptiveTest{});
    }

    void writeLastCoordinateResponse(OutputFile *) override {
        insert(log_, "writeLastCoordinateResponse ");
    }

    void writeLastCorrectResponse(OutputFile *) override {
        insert(log_, "writeLastCorrectResponse ");
    }

    void writeLastIncorrectResponse(OutputFile *) override {
        insert(log_, "writeLastIncorrectResponse ");
    }

    void submitResponse(
        const coordinate_response_measure::Response &) override {
        insert(log_, "submitResponse ");
    }

    auto log() const -> auto & { return log_; }
};

class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run(RecognitionTestModelImpl &) = 0;
};

class TargetWritingUseCase : public virtual UseCase {
  public:
    virtual auto writtenTarget(OutputFileStub &) -> std::string = 0;
};

class SubmittingResponse : public virtual UseCase {};

auto identity(const Test &test) -> auto & { return test.identity; }

void setMaskerFilePath(Test &test, std::string s) {
    test.maskerFilePath = std::move(s);
}

void setMaskerLevel_dB_SPL(Test &test, int x) { test.maskerLevel_dB_SPL = x; }

void setTestingFullScaleLevel_dB_SPL(Test &test, int x) {
    test.fullScaleLevel_dB_SPL = x;
}

void setCondition(Test &test, Condition c) { test.condition = c; }

void setAudioVisual(Test &test) { setCondition(test, Condition::audioVisual); }

void setAuditoryOnly(Test &test) {
    setCondition(test, Condition::auditoryOnly);
}

class InitializingTest : public virtual UseCase {
  public:
    virtual void run(RecognitionTestModelImpl &, const Test &) = 0;
};

class InitializingDefaultTest : public InitializingTest {
    TestMethod *method;

  public:
    explicit InitializingDefaultTest(TestMethod *method) : method{method} {}

    void run(RecognitionTestModelImpl &model) override {
        model.initialize(method, {});
    }

    void run(RecognitionTestModelImpl &model, const Test &test) override {
        model.initialize(method, test);
    }
};

class InitializingTestWithSingleSpeaker : public UseCase {
    TestMethod *method;

  public:
    explicit InitializingTestWithSingleSpeaker(TestMethod *method)
        : method{method} {}

    void run(RecognitionTestModelImpl &model) override {
        model.initializeWithSingleSpeaker(method, {});
    }
};

class InitializingTestWithDelayedMasker : public UseCase {
    TestMethod *method;

  public:
    explicit InitializingTestWithDelayedMasker(TestMethod *method)
        : method{method} {}

    void run(RecognitionTestModelImpl &model) override {
        model.initializeWithDelayedMasker(method, {});
    }
};

class InitializingTestWithEyeTracking : public InitializingTest {
    TestMethod *method;

  public:
    explicit InitializingTestWithEyeTracking(TestMethod *method)
        : method{method} {}

    void run(RecognitionTestModelImpl &model) override {
        model.initializeWithEyeTracking(method, {});
    }

    void run(RecognitionTestModelImpl &model, const Test &test) override {
        model.initializeWithEyeTracking(method, test);
    }
};

class AudioDeviceUseCase : public virtual UseCase {
  public:
    virtual void setAudioDevice(std::string) = 0;
};

class PlayingCalibration : public AudioDeviceUseCase {
    Calibration calibration{};

  public:
    void setAudioDevice(std::string s) override {
        calibration.audioSettings.audioDevice = std::move(s);
    }

    void run(RecognitionTestModelImpl &model) override {
        model.playCalibration(calibration);
    }

    void setFilePath(std::string s) { calibration.filePath = std::move(s); }

    void setLevel_dB_SPL(int x) { calibration.level_dB_SPL = x; }

    void setFullScaleLevel_dB_SPL(int x) {
        calibration.fullScaleLevel_dB_SPL = x;
    }

    void setAudioVisual() { calibration.condition = Condition::audioVisual; }

    void setAuditoryOnly() { calibration.condition = Condition::auditoryOnly; }
};

class PlayingTrial : public AudioDeviceUseCase {
    AudioSettings trial;

  public:
    void setAudioDevice(std::string s) override {
        trial.audioDevice = std::move(s);
    }

    void run(RecognitionTestModelImpl &model) override {
        model.playTrial(trial);
    }
};

class SubmittingFreeResponse : public SubmittingResponse,
                               public TargetWritingUseCase {
    open_set::FreeResponse response_{};

  public:
    void run(RecognitionTestModelImpl &model) override {
        model.submitResponse(response_);
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
    void run(RecognitionTestModelImpl &model) override {
        model.submitResponse(response_);
    }

    void setNumber(int n) { response_.number = n; }

    void setColor(coordinate_response_measure::Color c) { response_.color = c; }

    [[nodiscard]] auto response() const -> auto & { return response_; }
};

class SubmittingCorrectResponse : public TargetWritingUseCase {
  public:
    void run(RecognitionTestModelImpl &model) override {
        model.submitCorrectResponse();
    }

    auto writtenTarget(OutputFileStub &file) -> std::string override {
        return file.writtenOpenSetAdaptiveTrial().target;
    }
};

class SubmittingIncorrectResponse : public TargetWritingUseCase {
  public:
    void run(RecognitionTestModelImpl &model) override {
        model.submitIncorrectResponse();
    }

    auto writtenTarget(OutputFileStub &file) -> std::string override {
        return file.writtenOpenSetAdaptiveTrial().target;
    }
};

class EyeTrackerStub : public EyeTracker {
  public:
    auto recordingTimeAllocatedSeconds() -> double {
        return recordingTimeAllocatedSeconds_;
    }

    auto started() -> bool { return started_; }

    auto stopped() -> bool { return stopped_; }

    auto log() const -> auto & { return log_; }

    void allocateRecordingTimeSeconds(double x) override {
        insert(log_, "allocateRecordingTimeSeconds ");
        recordingTimeAllocatedSeconds_ = x;
        recordingTimeAllocated_ = true;
    }

    void start() override {
        insert(log_, "start ");
        started_ = true;
    }

    void stop() override {
        insert(log_, "stop ");
        stopped_ = true;
    }

    auto recordingTimeAllocated() -> bool { return recordingTimeAllocated_; }

  private:
    LogString log_{};
    double recordingTimeAllocatedSeconds_{};
    bool recordingTimeAllocated_{};
    bool started_{};
    bool stopped_{};
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
    EyeTrackerStub eyeTracker{};
    RecognitionTestModelImpl model{&targetPlayer, &maskerPlayer, &evaluator,
        &outputFile, &randomizer, &eyeTracker};
    TestMethodStub testMethod;
    PlayingCalibration playingCalibration{};
    InitializingDefaultTest initializingDefaultTest{&testMethod};
    InitializingTestWithSingleSpeaker initializingTestWithSingleSpeaker{
        &testMethod};
    InitializingTestWithDelayedMasker initializingTestWithDelayedMasker{
        &testMethod};
    InitializingTestWithEyeTracking initializingTestWithEyeTracking{
        &testMethod};
    PlayingTrial playingTrial;
    SubmittingCoordinateResponse submittingCoordinateResponse;
    SubmittingCorrectResponse submittingCorrectResponse;
    SubmittingIncorrectResponse submittingIncorrectResponse;
    SubmittingFreeResponse submittingFreeResponse;
    av_speech_in_noise::Test test;
    AudioSampleTime fadeInCompleteTime{};

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

    void assertTargetVideoHiddenWhenAuditoryOnly(PlayingCalibration &useCase) {
        useCase.setAuditoryOnly();
        run(useCase);
        assertTargetVideoOnlyHidden();
    }

    void assertTargetVideoShownWhenAudioVisual(PlayingCalibration &useCase) {
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

    void assertClosesOutputFile_OpensAndWritesTestInOrder(UseCase &useCase) {
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
        setTargetPlayerDurationSeconds(1);
        setMaskerPlayerFadeTimeSeconds(2);
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

    void setMaskerLevel_dB_SPL(int x) { tests::setMaskerLevel_dB_SPL(test, x); }

    void setTestingFullScaleLevel_dB_SPL(int x) {
        tests::setTestingFullScaleLevel_dB_SPL(test, x);
    }

    void setMaskerRms(double x) { maskerPlayer.setRms(x); }

    void setSnr_dB(int x) { testMethod.setSnr_dB(x); }

    void maskerFadeOutComplete() { maskerPlayer.fadeOutComplete(); }

    void assertSavesOutputFileAfterWritingTrial(UseCase &useCase) {
        run(useCase);
        assertTrue(outputFileLog().endsWith("save "));
    }

    static void assertCallThrowsRequestFailure(
        const std::function<void()> &f, const std::string &what) {
        try {
            f();
            FAIL() << "Expected recognition_test::"
                      "ModelImpl::"
                      "RequestFailure";
        } catch (const ModelImpl::RequestFailure &e) {
            assertEqual(what, e.what());
        }
    }

    void assertCallThrowsRequestFailure(
        UseCase &useCase, const std::string &what) {
        assertCallThrowsRequestFailure([&]() { run(useCase); }, what);
    }

    void assertCallThrowsRequestFailure(
        InitializingTest &useCase, const std::string &what) {
        assertCallThrowsRequestFailure([&]() { run(useCase); }, what);
    }

    void setMaskerFilePath(std::string s) {
        tests::setMaskerFilePath(test, std::move(s));
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

    void run(InitializingTest &useCase) { useCase.run(model, test); }

    void assertSetsTargetLevel(UseCase &useCase) {
        setMaskerLevel_dB_SPL(3);
        setTestingFullScaleLevel_dB_SPL(4);
        run(initializingDefaultTest);
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
        run(initializingDefaultTest);
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
        run(initializingDefaultTest);
        testMethod.setCurrent("a");
        testMethod.setCurrentWhenNext("b");
        run(useCase);
        assertEqual("a", evaluator.filePathForFileName());
    }

    void assertTestMethodLogContains(
        UseCase &useCase, const std::string &what) {
        run(initializingDefaultTest);
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

    void assertUsingAllTargetPlayerChannels() {
        assertTrue(targetPlayer.usingAllChannels());
    }

    void assertUsesAllTargetPlayerChannels(UseCase &useCase) {
        run(useCase);
        assertUsingAllTargetPlayerChannels();
    }

    void assertUsesAllMaskerPlayerChannels(UseCase &useCase) {
        run(useCase);
        assertUsingAllMaskerPlayerChannels();
    }

    void assertMaskerPlayerChannelDelaysCleared(UseCase &useCase) {
        run(useCase);
        assertMaskerPlayerChannelDelaysCleared();
    }

    void assertPassesTestIdentityToOutputFile(InitializingTest &useCase) {
        run(useCase);
        assertEqual(outputFile.openNewFileParameters(), &identity(test));
    }

    void assertPassesMaskerFilePathToMaskerPlayer(InitializingTest &useCase) {
        setMaskerFilePath("a");
        run(useCase);
        assertEqual("a", maskerPlayer.filePath());
    }

    void setTargetPlayerDurationSeconds(double x) {
        targetPlayer.setDurationSeconds(x);
    }

    void setMaskerPlayerFadeTimeSeconds(double x) {
        maskerPlayer.setFadeTimeSeconds(x);
    }

    void assertAllocatesTrialDurationForEyeTracking(
        InitializingTest &initializing, UseCase &useCase) {
        run(initializing);
        setTargetPlayerDurationSeconds(3);
        setMaskerPlayerFadeTimeSeconds(4);
        run(useCase);
        assertEqual(3 + 2 * 4., eyeTracker.recordingTimeAllocatedSeconds());
    }

    void assertPlayTrialDoesNotAllocateRecordingTime(UseCase &useCase) {
        run(useCase);
        run(playingTrial);
        assertFalse(eyeTracker.recordingTimeAllocated());
    }

    void
    assertPlayTrialDoesNotAllocateRecordingTimeForEyeTrackingAfterTestWithEyeTracking(
        UseCase &useCase) {
        run(initializingTestWithEyeTracking);
        assertPlayTrialDoesNotAllocateRecordingTime(useCase);
    }

    auto eyeTrackerStarted() -> bool { return eyeTracker.started(); }

    auto eyeTrackerStopped() -> bool { return eyeTracker.stopped(); }

    void setMaskerPlayerFadeInCompleteAudioSampleSystemTime(system_time t) {
        fadeInCompleteTime.systemTime = t;
    }

    void setMaskerPlayerFadeInCompleteAudioSampleOffsetTime(gsl::index t) {
        fadeInCompleteTime.systemTimeSampleOffset = t;
    }

    void setMaskerPlayerSampleRateHz(double x) {
        maskerPlayer.setSampleRateHz(x);
    }

    void fadeInComplete() {
        maskerPlayer.fadeInComplete(fadeInCompleteTime);
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
    initializeDefaultTestClosesOutputFile_OpensAndWritesTestInOrder) {
    assertClosesOutputFile_OpensAndWritesTestInOrder(initializingDefaultTest);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithSingleSpeakerClosesOutputFile_OpensAndWritesTestInOrder) {
    assertClosesOutputFile_OpensAndWritesTestInOrder(
        initializingTestWithSingleSpeaker);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithDelayedMaskerClosesOutputFile_OpensAndWritesTestInOrder) {
    assertClosesOutputFile_OpensAndWritesTestInOrder(
        initializingTestWithDelayedMasker);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithEyeTrackingClosesOutputFile_OpensAndWritesTestInOrder) {
    assertClosesOutputFile_OpensAndWritesTestInOrder(
        initializingTestWithEyeTracking);
}

RECOGNITION_TEST_MODEL_TEST(initializeDefaultTestUsesAllTargetPlayerChannels) {
    assertUsesAllTargetPlayerChannels(initializingDefaultTest);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithEyeTrackingUsesAllTargetPlayerChannels) {
    assertUsesAllTargetPlayerChannels(initializingTestWithEyeTracking);
}

RECOGNITION_TEST_MODEL_TEST(initializeDefaultTestUsesAllMaskerPlayerChannels) {
    assertUsesAllMaskerPlayerChannels(initializingDefaultTest);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithEyeTrackingUsesAllMaskerPlayerChannels) {
    assertUsesAllMaskerPlayerChannels(initializingTestWithEyeTracking);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestClearsAllMaskerPlayerChannelDelays) {
    assertMaskerPlayerChannelDelaysCleared(initializingDefaultTest);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithEyeTrackingClearsAllMaskerPlayerChannelDelays) {
    assertMaskerPlayerChannelDelaysCleared(initializingTestWithEyeTracking);
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
    initializeTestWithDelayedMaskerSetsFirstChannelMaskerDelay) {
    run(initializingTestWithDelayedMasker);
    assertEqual(gsl::index{0}, maskerPlayer.channelDelayed());
    assertEqual(RecognitionTestModelImpl::maskerChannelDelaySeconds,
        maskerPlayer.channelDelaySeconds());
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestOpensNewOutputFilePassingTestInformation) {
    assertPassesTestIdentityToOutputFile(initializingDefaultTest);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithEyeTrackingOpensNewOutputFilePassingTestInformation) {
    assertPassesTestIdentityToOutputFile(initializingTestWithEyeTracking);
}

RECOGNITION_TEST_MODEL_TEST(
    playTrialForTestWithEyeTrackingAllocatesTrialDurationsWorthRecordingTimeForEyeTracking) {
    assertAllocatesTrialDurationForEyeTracking(
        initializingTestWithEyeTracking, playingTrial);
}

RECOGNITION_TEST_MODEL_TEST(playTrialForTestWithEyeTrackingStartsEyeTracking) {
    run(initializingTestWithEyeTracking);
    run(playingTrial);
    assertTrue(eyeTrackerStarted());
}

RECOGNITION_TEST_MODEL_TEST(playTrialForDefaultTestDoesNotStartEyeTracking) {
    run(initializingDefaultTest);
    run(playingTrial);
    assertFalse(eyeTrackerStarted());
}

RECOGNITION_TEST_MODEL_TEST(
    playTrialForTestWithEyeTrackingStartsEyeTrackingAfterAllocatingRecordingTime) {
    run(initializingTestWithEyeTracking);
    run(playingTrial);
    assertEqual("allocateRecordingTimeSeconds start ", eyeTracker.log());
}

RECOGNITION_TEST_MODEL_TEST(
    fadeOutCompleteForTestWithEyeTrackingStopsEyeTracking) {
    run(initializingTestWithEyeTracking);
    maskerFadeOutComplete();
    assertTrue(eyeTrackerStopped());
}

RECOGNITION_TEST_MODEL_TEST(
    fadeOutCompleteForDefaultTestDoesNotStopEyeTracking) {
    run(initializingDefaultTest);
    maskerFadeOutComplete();
    assertFalse(eyeTrackerStopped());
}

RECOGNITION_TEST_MODEL_TEST(
    playTrialForDefaultTestDoesNotAllocateRecordingTimeForEyeTracking) {
    assertPlayTrialDoesNotAllocateRecordingTime(initializingDefaultTest);
}

RECOGNITION_TEST_MODEL_TEST(
    playTrialForTestWithDelayedMaskerDoesNotAllocateRecordingTimeForEyeTracking) {
    assertPlayTrialDoesNotAllocateRecordingTime(
        initializingTestWithDelayedMasker);
}

RECOGNITION_TEST_MODEL_TEST(
    playTrialForTestWithSingleSpeakerDoesNotAllocateRecordingTimeForEyeTracking) {
    assertPlayTrialDoesNotAllocateRecordingTime(
        initializingTestWithSingleSpeaker);
}

RECOGNITION_TEST_MODEL_TEST(
    playTrialForDefaultTestFollowingTestWithEyeTrackingDoesNotAllocateRecordingTimeForEyeTracking) {
    assertPlayTrialDoesNotAllocateRecordingTimeForEyeTrackingAfterTestWithEyeTracking(
        initializingDefaultTest);
}

RECOGNITION_TEST_MODEL_TEST(
    playTrialForTestWithSingleSpeakerFollowingTestWithEyeTrackingDoesNotAllocateRecordingTimeForEyeTracking) {
    assertPlayTrialDoesNotAllocateRecordingTimeForEyeTrackingAfterTestWithEyeTracking(
        initializingTestWithSingleSpeaker);
}

RECOGNITION_TEST_MODEL_TEST(
    playTrialForTestWithDelayedMaskerFollowingTestWithEyeTrackingDoesNotAllocateRecordingTimeForEyeTracking) {
    assertPlayTrialDoesNotAllocateRecordingTimeForEyeTrackingAfterTestWithEyeTracking(
        initializingTestWithDelayedMasker);
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

RECOGNITION_TEST_MODEL_TEST(fadeInCompletePlaysTargetAt) {
    setMaskerPlayerFadeInCompleteAudioSampleSystemTime(1);
    setMaskerPlayerFadeInCompleteAudioSampleOffsetTime(2);
    setMaskerPlayerSampleRateHz(3);
    fadeInComplete();
    assertEqual(system_time{1}, targetPlayer.baseSystemTimePlayedAt());
    assertEqual(2 / 3. + 0.5, targetPlayer.secondsDelayedPlayedAt());
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestPassesNextTargetToTargetPlayer) {
    assertPassesNextTargetToPlayer(initializingDefaultTest);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithEyeTrackingPassesNextTargetToTargetPlayer) {
    assertPassesNextTargetToPlayer(initializingTestWithEyeTracking);
}

RECOGNITION_TEST_MODEL_TEST(initializeDefaultTestResetsTrialNumber) {
    assertYieldsTrialNumber(initializingDefaultTest, 1);
}

RECOGNITION_TEST_MODEL_TEST(initializeTestWithEyeTrackingResetsTrialNumber) {
    assertYieldsTrialNumber(initializingTestWithEyeTracking, 1);
}

RECOGNITION_TEST_MODEL_TEST(submittingCoordinateResponseIncrementsTrialNumber) {
    run(initializingDefaultTest);
    assertYieldsTrialNumber(submittingCoordinateResponse, 2);
}

RECOGNITION_TEST_MODEL_TEST(submittingCorrectResponseIncrementsTrialNumber) {
    run(initializingDefaultTest);
    assertYieldsTrialNumber(submittingCorrectResponse, 2);
}

RECOGNITION_TEST_MODEL_TEST(submittingIncorrectResponseIncrementsTrialNumber) {
    run(initializingDefaultTest);
    assertYieldsTrialNumber(submittingIncorrectResponse, 2);
}

RECOGNITION_TEST_MODEL_TEST(submittingFreeResponseIncrementsTrialNumber) {
    run(initializingDefaultTest);
    assertYieldsTrialNumber(submittingFreeResponse, 2);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingCoordinateResponsePassesNextTargetToTargetPlayer) {
    run(initializingDefaultTest);
    assertPassesNextTargetToPlayer(submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingCorrectResponsePassesNextTargetToTargetPlayer) {
    run(initializingDefaultTest);
    assertPassesNextTargetToPlayer(submittingCorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingIncorrectResponsePassesNextTargetToTargetPlayer) {
    run(initializingDefaultTest);
    assertPassesNextTargetToPlayer(submittingIncorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingFreeResponsePassesNextTargetToTargetPlayer) {
    run(initializingDefaultTest);
    assertPassesNextTargetToPlayer(submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationPassesAudioFileToTargetPlayer) {
    playingCalibration.setFilePath("a");
    run(playingCalibration);
    assertTargetFilePathEquals("a");
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestPassesMaskerFilePathToMaskerPlayer) {
    assertPassesMaskerFilePathToMaskerPlayer(initializingDefaultTest);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithEyeTrackingPassesMaskerFilePathToMaskerPlayer) {
    assertPassesMaskerFilePathToMaskerPlayer(initializingTestWithEyeTracking);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(initializingDefaultTest);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithEyeTrackingSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(
        initializingTestWithEyeTracking);
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
    initializeDefaultTestSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(
        initializingDefaultTest);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithEyeTrackingSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(
        initializingTestWithEyeTracking);
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

RECOGNITION_TEST_MODEL_TEST(initializeDefaultTestSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(initializingDefaultTest);
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

RECOGNITION_TEST_MODEL_TEST(initializeDefaultTestSetsInitialMaskerPlayerLevel) {
    setMaskerLevel_dB_SPL(1);
    setTestingFullScaleLevel_dB_SPL(2);
    setMaskerRms(3);
    run(initializingDefaultTest);
    assertEqual(1 - 2 - dB(3), maskerPlayer.level_dB());
}

RECOGNITION_TEST_MODEL_TEST(initializeDefaultTestSetsTargetPlayerLevel) {
    setSnr_dB(2);
    setMaskerLevel_dB_SPL(3);
    setTestingFullScaleLevel_dB_SPL(4);
    setMaskerRms(5);
    run(initializingDefaultTest);
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
    setAudioVisual(test);
    run(initializingDefaultTest);
    run(playingTrial);
    assertTrue(targetPlayerVideoShown());
}

RECOGNITION_TEST_MODEL_TEST(maskerFadeOutCompleteHidesTargetPlayer) {
    maskerFadeOutComplete();
    assertTargetVideoOnlyHidden();
}

RECOGNITION_TEST_MODEL_TEST(startTrialDoesNotShowTargetPlayerWhenAuditoryOnly) {
    setAuditoryOnly(test);
    run(initializingDefaultTest);
    run(playingTrial);
    assertTargetVideoNotShown();
}

RECOGNITION_TEST_MODEL_TEST(initializeDefaultTestHidesTargetPlayer) {
    run(initializingDefaultTest);
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
    initializeDefaultTestThrowsRequestFailureIfFileFailsToOpen) {
    outputFile.throwOnOpen();
    assertCallThrowsRequestFailure(
        initializingDefaultTest, "Unable to open output file.");
}

RECOGNITION_TEST_MODEL_TEST(
    playCalibrationThrowsRequestFailureWhenTargetPlayerThrowsInvalidAudioFile) {
    playingCalibration.setFilePath("a");
    targetPlayer.throwInvalidAudioFileOnRms();
    assertCallThrowsRequestFailure(playingCalibration, "unable to read a");
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestThrowsRequestFailureWhenMaskerPlayerThrowsInvalidAudioFile) {
    setMaskerFilePath("a");
    maskerPlayer.throwInvalidAudioFileOnLoad();
    assertCallThrowsRequestFailure(initializingDefaultTest, "unable to read a");
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
    initializeDefaultTestThrowsRequestFailureIfTrialInProgress) {
    assertThrowsRequestFailureWhenTrialInProgress(initializingDefaultTest);
}

RECOGNITION_TEST_MODEL_TEST(playTrialThrowsRequestFailureIfTrialInProgress) {
    assertThrowsRequestFailureWhenTrialInProgress(playingTrial);
}

RECOGNITION_TEST_MODEL_TEST(
    playCalibrationThrowsRequestFailureIfTrialInProgress) {
    assertThrowsRequestFailureWhenTrialInProgress(playingCalibration);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestDoesNotLoadMaskerIfTrialInProgress) {
    setMaskerFilePath("a");
    runIgnoringFailureWithTrialInProgress(initializingDefaultTest);
    assertEqual("", maskerPlayer.filePath());
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestDoesNotHideTargetPlayerWhenAuditoryOnlyButTrialInProgress) {
    setAuditoryOnly(test);
    runIgnoringFailureWithTrialInProgress(initializingDefaultTest);
    assertTargetVideoNotHidden();
}

RECOGNITION_TEST_MODEL_TEST(audioDevicesReturnsOutputAudioDeviceDescriptions) {
    maskerPlayer.setOutputAudioDeviceDescriptions({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, model.audioDevices());
}

RECOGNITION_TEST_MODEL_TEST(testCompleteWhenComplete) {
    run(initializingDefaultTest);
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

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestDoesNotLoadNextTargetWhenComplete) {
    testMethod.setNextTarget("a");
    testMethod.setComplete();
    run(initializingDefaultTest);
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
    run(initializingDefaultTest);
    run(submittingCorrectResponse);
    assertTrue(testMethod.submittedCorrectResponse());
}

RECOGNITION_TEST_MODEL_TEST(submitIncorrectResponseSubmitsIncorrectResponse) {
    run(initializingDefaultTest);
    run(submittingIncorrectResponse);
    assertTrue(testMethod.submittedIncorrectResponse());
}
}
}
