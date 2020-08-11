#include "LogString.hpp"
#include "MaskerPlayerStub.hpp"
#include "ModelEventListenerStub.hpp"
#include "OutputFileStub.hpp"
#include "RandomizerStub.hpp"
#include "ResponseEvaluatorStub.hpp"
#include "TargetPlayerStub.hpp"
#include "assert-utility.hpp"
#include <recognition-test/RecognitionTestModel.hpp>
#include <gtest/gtest.h>
#include <cmath>

namespace av_speech_in_noise {
constexpr auto operator==(const EyeGaze &a, const EyeGaze &b) -> bool {
    return a.x == b.x && a.y == b.y;
}

constexpr auto operator==(
    const BinocularGazeSample &a, const BinocularGazeSample &b) -> bool {
    return a.systemTime.microseconds == b.systemTime.microseconds &&
        a.left == b.left && a.right == b.right;
}

namespace {
class TestMethodStub : public TestMethod {
  public:
    auto submittedCorrectResponse() const -> bool {
        return submittedCorrectResponse_;
    }

    auto submittedIncorrectResponse() const -> bool {
        return submittedIncorrectResponse_;
    }

    auto submittedFreeResponse() const -> bool {
        return submittedFreeResponse_;
    }

    auto submittedCorrectKeywords() const -> bool {
        return submittedCorrectKeywords_;
    }

    void setSnr_dB(int x) { snr_dB_ = x; }

    auto snr() -> SNR override { return SNR{snr_dB_}; }

    void setNextTarget(std::string s) { nextTarget_ = std::move(s); }

    auto nextTarget() -> LocalUrl override {
        insert(log_, "next ");
        currentTarget_ = currentTargetWhenNextTarget_;
        return {nextTarget_};
    }

    void setComplete() { complete_ = true; }

    auto complete() -> bool override { return complete_; }

    auto currentTarget() -> LocalUrl override { return {currentTarget_}; }

    void setCurrentTarget(std::string s) { currentTarget_ = std::move(s); }

    void setCurrentTargetWhenNextTarget(std::string s) {
        currentTargetWhenNextTarget_ = std::move(s);
    }

    void submitCorrectResponse() override {
        insert(log_, "submitCorrectResponse ");
        submittedCorrectResponse_ = true;
    }

    void submitIncorrectResponse() override {
        insert(log_, "submitIncorrectResponse ");
        submittedIncorrectResponse_ = true;
    }

    void submit(const FreeResponse &) override {
        insert(log_, "submitFreeResponse ");
        submittedFreeResponse_ = true;
    }

    void submit(const CorrectKeywords &) override {
        insert(log_, "submitCorrectKeywords ");
        submittedCorrectKeywords_ = true;
    }

    void submit(const coordinate_response_measure::Response &) override {
        insert(log_, "submitCoordinateResponse ");
    }

    void writeTestingParameters(OutputFile &file) override {
        insert(log_, "writeTestingParameters ");
        file.write(AdaptiveTest{});
    }

    void writeLastCoordinateResponse(OutputFile &) override {
        insert(log_, "writeLastCoordinateResponse ");
    }

    void writeLastCorrectResponse(OutputFile &) override {
        insert(log_, "writeLastCorrectResponse ");
    }

    void writeLastIncorrectResponse(OutputFile &) override {
        insert(log_, "writeLastIncorrectResponse ");
    }

    void writeLastCorrectKeywords(OutputFile &) override {
        insert(log_, "writeLastCorrectKeywords ");
    }

    void writeTestResult(OutputFile &) override {
        insert(log_, "writeTestResult ");
    }

    auto log() const -> const std::stringstream & { return log_; }

  private:
    std::stringstream log_{};
    std::string currentTarget_{};
    std::string currentTargetWhenNextTarget_{};
    std::string nextTarget_{};
    int snr_dB_{};
    bool complete_{};
    bool submittedCorrectResponse_{};
    bool submittedIncorrectResponse_{};
    bool submittedFreeResponse_{};
    bool submittedCorrectKeywords_{};
};

class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run(RecognitionTestModelImpl &) = 0;
};

class TargetWritingUseCase : public virtual UseCase {
  public:
    virtual auto target(OutputFileStub &) -> std::string = 0;
};

class InitializingTest : public UseCase {
  public:
    explicit InitializingTest(TestMethod *method, const Test &test)
        : test{test}, method{method} {}

    void run(RecognitionTestModelImpl &m) override {
        m.initialize(method, test);
    }

  private:
    const Test &test{};
    TestMethod *method;
};

class InitializingTestWithSingleSpeaker : public UseCase {
  public:
    explicit InitializingTestWithSingleSpeaker(TestMethod *method)
        : method{method} {}

    void run(RecognitionTestModelImpl &m) override {
        m.initializeWithSingleSpeaker(method, {});
    }

  private:
    TestMethod *method;
};

class InitializingTestWithDelayedMasker : public UseCase {
  public:
    explicit InitializingTestWithDelayedMasker(TestMethod *method)
        : method{method} {}

    void run(RecognitionTestModelImpl &m) override {
        m.initializeWithDelayedMasker(method, {});
    }

  private:
    TestMethod *method;
};

class InitializingTestWithEyeTracking : public UseCase {
    TestMethod *method;
    const Test &test;

  public:
    explicit InitializingTestWithEyeTracking(
        TestMethod *method, const Test &test)
        : method{method}, test{test} {}

    void run(RecognitionTestModelImpl &model) override {
        model.initializeWithEyeTracking(method, test);
    }
};

class AudioDeviceUseCase : public virtual UseCase {
  public:
    virtual void setAudioDevice(std::string) = 0;
};

class PlayingCalibration : public AudioDeviceUseCase {
  public:
    explicit PlayingCalibration(Calibration &calibration)
        : calibration{calibration} {}

    void setAudioDevice(std::string s) override {
        calibration.audioDevice = std::move(s);
    }

    void run(RecognitionTestModelImpl &model) override {
        model.playCalibration(calibration);
    }

  private:
    Calibration &calibration;
};

class PlayingTrial : public AudioDeviceUseCase {
  public:
    void setAudioDevice(std::string s) override {
        trial.audioDevice = std::move(s);
    }

    void run(RecognitionTestModelImpl &m) override { m.playTrial(trial); }

  private:
    AudioSettings trial;
};

class SubmittingFreeResponse : public TargetWritingUseCase {
  public:
    explicit SubmittingFreeResponse(const FreeResponse &response = {})
        : response{response} {}

    void run(RecognitionTestModelImpl &m) override { m.submit(response); }

    auto target(OutputFileStub &file) -> std::string override {
        return file.freeResponseTrial().target;
    }

  private:
    const FreeResponse &response;
};

class SubmittingCorrectKeywords : public TargetWritingUseCase {
  public:
    void run(RecognitionTestModelImpl &m) override {
        m.submit(CorrectKeywords{});
    }

    auto target(OutputFileStub &file) -> std::string override {
        return file.correctKeywordsTrial().target;
    }
};

class SubmittingConsonant : public TargetWritingUseCase {
  public:
    void run(RecognitionTestModelImpl &m) override {
        m.submit(ConsonantResponse{});
    }

    auto target(OutputFileStub &file) -> std::string override {
        return file.consonantTrial().target;
    }
};

class SubmittingCoordinateResponse : public UseCase {
  public:
    void run(RecognitionTestModelImpl &m) override {
        m.submit(coordinate_response_measure::Response{});
    }
};

auto openSetAdaptiveTarget(OutputFileStub &file) -> std::string {
    return file.openSetAdaptiveTrial().target;
}

class SubmittingCorrectResponse : public TargetWritingUseCase {
  public:
    void run(RecognitionTestModelImpl &model) override {
        model.submitCorrectResponse();
    }

    auto target(OutputFileStub &file) -> std::string override {
        return openSetAdaptiveTarget(file);
    }
};

class SubmittingIncorrectResponse : public TargetWritingUseCase {
  public:
    void run(RecognitionTestModelImpl &model) override {
        model.submitIncorrectResponse();
    }

    auto target(OutputFileStub &file) -> std::string override {
        return openSetAdaptiveTarget(file);
    }
};

class EyeTrackerStub : public EyeTracker {
  public:
    auto recordingTimeAllocatedSeconds() const -> double {
        return recordingTimeAllocatedSeconds_;
    }

    auto started() const -> bool { return started_; }

    auto stopped() const -> bool { return stopped_; }

    auto log() const -> const std::stringstream & { return log_; }

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

    auto recordingTimeAllocated() const -> bool {
        return recordingTimeAllocated_;
    }

    auto gazeSamples() -> BinocularGazeSamples override {
        insert(log_, "gazeSamples ");
        return gazeSamples_;
    }

    void setGazes(BinocularGazeSamples g) { gazeSamples_ = std::move(g); }

    auto currentSystemTime() -> EyeTrackerSystemTime override {
        return currentSystemTime_;
    }

    void setCurrentSystemTime(EyeTrackerSystemTime t) {
        currentSystemTime_ = t;
    }

  private:
    BinocularGazeSamples gazeSamples_;
    std::stringstream log_{};
    EyeTrackerSystemTime currentSystemTime_{};
    double recordingTimeAllocatedSeconds_{};
    bool recordingTimeAllocated_{};
    bool started_{};
    bool stopped_{};
};

void setMaskerLevel_dB_SPL(Test &test, int x) { test.maskerLevel.dB_SPL = x; }

void setCurrentTarget(TestMethodStub &m, std::string s) {
    m.setCurrentTarget(std::move(s));
}

auto targetFileName(RecognitionTestModelImpl &m) -> std::string {
    return m.targetFileName();
}

auto filePathForFileName(ResponseEvaluatorStub &r) -> std::string {
    return r.filePathForFileName();
}

auto hidden(TargetPlayerStub &targetPlayer) -> bool {
    return targetPlayer.videoHidden();
}

auto shown(TargetPlayerStub &targetPlayer) -> bool {
    return targetPlayer.videoShown();
}

void assertNotShown(TargetPlayerStub &targetPlayer) {
    assertFalse(shown(targetPlayer));
}

void assertNotHidden(TargetPlayerStub &targetPlayer) {
    assertFalse(hidden(targetPlayer));
}

void assertOnlyHidden(TargetPlayerStub &targetPlayer) {
    assertTrue(hidden(targetPlayer));
    assertNotShown(targetPlayer);
}

void assertOnlyShown(TargetPlayerStub &targetPlayer) {
    assertNotHidden(targetPlayer);
    assertTrue(shown(targetPlayer));
}

auto log(OutputFileStub &file) -> const std::stringstream & {
    return file.log();
}

void run(UseCase &useCase, RecognitionTestModelImpl &model) {
    useCase.run(model);
}

auto fadedIn(MaskerPlayerStub &maskerPlayer) -> bool {
    return maskerPlayer.fadeInCalled();
}

auto played(TargetPlayerStub &player) -> bool { return player.played(); }

void assertPlayed(TargetPlayerStub &player) { assertTrue(played(player)); }

auto filePath(TargetPlayerStub &player) { return player.filePath(); }

void assertFilePathEquals(TargetPlayerStub &player, const std::string &what) {
    assertEqual(what, filePath(player));
}

auto playbackCompletionSubscribed(TargetPlayerStub &player) -> bool {
    return player.playbackCompletionSubscribedTo();
}

auto secondsSeeked(MaskerPlayerStub &player) { return player.secondsSeeked(); }

void setFullScaleLevel_dB_SPL(Test &test, int x) {
    test.fullScaleLevel.dB_SPL = x;
}

void setMaskerFilePath(Test &test, std::string s) {
    test.maskerFileUrl.path = std::move(s);
}

void setDigitalLevel(MaskerPlayerStub &player, DigitalLevel x) {
    player.setDigitalLevel(x);
}

void setSnr_dB(TestMethodStub &method, int x) { method.setSnr_dB(x); }

void fadeOutComplete(MaskerPlayerStub &player) { player.fadeOutComplete(); }

void setTrialInProgress(MaskerPlayerStub &player) { player.setPlaying(); }

void assertLevelEquals_dB(TargetPlayerStub &player, double x) {
    assertEqual(x, player.level_dB());
}

auto testComplete(RecognitionTestModelImpl &model) -> bool {
    return model.testComplete();
}

auto freeResponseTrial(OutputFileStub &file) {
    return file.freeResponseTrial();
}

void assertOnlyUsingFirstChannel(TargetPlayerStub &player) {
    assertTrue(player.usingFirstChannelOnly());
}

void assertUsingAllChannels(MaskerPlayerStub &player) {
    assertTrue(player.usingAllChannels());
}

void assertChannelDelaysCleared(MaskerPlayerStub &player) {
    assertTrue(player.channelDelaysCleared());
}

auto targetPlayerEventListener(const RecognitionTestModelImpl &model)
    -> const TargetPlayer::EventListener * {
    return &model;
}

auto maskerPlayerEventListener(const RecognitionTestModelImpl &model)
    -> const MaskerPlayer::EventListener * {
    return &model;
}

void setEyeGazes(EyeTrackerStub &eyeTracker, BinocularGazeSamples g) {
    eyeTracker.setGazes(std::move(g));
}

void runIgnoringFailure(UseCase &useCase, RecognitionTestModelImpl &model) {
    try {
        run(useCase, model);
    } catch (const ModelImpl::RequestFailure &) {
    }
}

void setDurationSeconds(TargetPlayerStub &player, double x) {
    player.setDurationSeconds(x);
}

void setFadeTimeSeconds(MaskerPlayerStub &player, double x) {
    player.setFadeTimeSeconds(x);
}

auto started(EyeTrackerStub &eyeTracker) -> bool {
    return eyeTracker.started();
}

auto stopped(EyeTrackerStub &eyeTracker) -> bool {
    return eyeTracker.stopped();
}

void setSystemTime(AudioSampleTimeWithOffset &time, player_system_time_type s) {
    time.playerTime.system = s;
}

void setNanosecondsFromPlayerTime(MaskerPlayerStub &player, std::uintmax_t t) {
    player.setNanosecondsFromPlayerTime(t);
}

void setCurrentSystemTimeMicroseconds(
    EyeTrackerStub &eyeTracker, std::int_least64_t t) {
    eyeTracker.setCurrentSystemTime({t});
}

void setSampleOffset(AudioSampleTimeWithOffset &time, gsl::index n) {
    time.sampleOffset = n;
}

void setSampleRateHz(MaskerPlayerStub &player, double x) {
    player.setSampleRateHz(x);
}

void fadeInComplete(
    MaskerPlayerStub &player, const AudioSampleTimeWithOffset &t) {
    player.fadeInComplete(t);
}

class RecognitionTestModelTests : public ::testing::Test {
  protected:
    ModelEventListenerStub listener;
    TargetPlayerStub targetPlayer;
    MaskerPlayerStub maskerPlayer;
    ResponseEvaluatorStub evaluator;
    OutputFileStub outputFile;
    RandomizerStub randomizer;
    EyeTrackerStub eyeTracker;
    RecognitionTestModelImpl model{targetPlayer, maskerPlayer, evaluator,
        outputFile, randomizer, eyeTracker};
    TestMethodStub testMethod;
    Calibration calibration{};
    PlayingCalibration playingCalibration{calibration};
    av_speech_in_noise::Test test{};
    InitializingTest initializingTest{&testMethod, test};
    InitializingTestWithSingleSpeaker initializingTestWithSingleSpeaker{
        &testMethod};
    InitializingTestWithDelayedMasker initializingTestWithDelayedMasker{
        &testMethod};
    InitializingTestWithEyeTracking initializingTestWithEyeTracking{
        &testMethod, test};
    PlayingTrial playingTrial;
    SubmittingCoordinateResponse submittingCoordinateResponse;
    SubmittingCorrectResponse submittingCorrectResponse;
    SubmittingIncorrectResponse submittingIncorrectResponse;
    FreeResponse freeResponse{};
    SubmittingFreeResponse submittingFreeResponse{freeResponse};
    AudioSampleTimeWithOffset fadeInCompleteTime{};
    SubmittingCorrectKeywords submittingCorrectKeywords;
    SubmittingConsonant submittingConsonant;

    RecognitionTestModelTests() { model.subscribe(&listener); }

    void assertClosesOutputFileOpensAndWritesTestInOrder(UseCase &useCase) {
        run(useCase, model);
        assertEqual("close openNewFile write ", string(log(outputFile)));
    }

    template <typename T>
    void assertDevicePassedToPlayer(
        const T &player, AudioDeviceUseCase &useCase) {
        useCase.setAudioDevice("a");
        run(useCase, model);
        assertEqual("a", player.device());
    }

    void assertDevicePassedToTargetPlayer(AudioDeviceUseCase &useCase) {
        assertDevicePassedToPlayer(targetPlayer, useCase);
    }

    void assertDevicePassedToMaskerPlayer(AudioDeviceUseCase &useCase) {
        assertDevicePassedToPlayer(maskerPlayer, useCase);
    }

    void assertPassesNextTargetToPlayer(UseCase &useCase) {
        testMethod.setNextTarget("a");
        run(useCase, model);
        assertFilePathEquals(targetPlayer, "a");
    }

    void assertTargetPlayerPlaybackCompletionSubscribed(UseCase &useCase) {
        run(useCase, model);
        assertTrue(playbackCompletionSubscribed(targetPlayer));
    }

    void assertSeeksToRandomMaskerPositionWithinTrialDuration(
        UseCase &useCase) {
        setDurationSeconds(targetPlayer, 1);
        setFadeTimeSeconds(maskerPlayer, 2);
        maskerPlayer.setDurationSeconds(3);
        run(useCase, model);
        assertEqual(0., randomizer.lowerFloatBound());
        assertEqual(3. - 2 - 1 - 2, randomizer.upperFloatBound());
    }

    void assertMaskerPlayerSeekedToRandomTime(UseCase &useCase) {
        randomizer.setRandomFloat(1);
        run(useCase, model);
        assertEqual(1., secondsSeeked(maskerPlayer));
    }

    void assertSavesOutputFileAfterWritingTrial(UseCase &useCase) {
        run(useCase, model);
        assertTrue(endsWith(log(outputFile), "save "));
    }

    void assertCallThrowsRequestFailure(
        UseCase &useCase, const std::string &what) {
        try {
            run(useCase, model);
            FAIL() << "Expected recognition_test::"
                      "ModelImpl::"
                      "RequestFailure";
        } catch (const ModelImpl::RequestFailure &e) {
            assertEqual(what, e.what());
        }
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
        setTrialInProgress(maskerPlayer);
        runIgnoringFailure(useCase, model);
    }

    void assertThrowsRequestFailureWhenTrialInProgress(UseCase &useCase) {
        setTrialInProgress(maskerPlayer);
        assertCallThrowsRequestFailure(useCase, "Trial in progress.");
    }

    void assertSetsTargetLevel(UseCase &useCase) {
        setMaskerLevel_dB_SPL(test, 3);
        setFullScaleLevel_dB_SPL(test, 4);
        run(initializingTest, model);
        setDigitalLevel(maskerPlayer, DigitalLevel{5});
        setSnr_dB(testMethod, 2);
        run(useCase, model);
        assertLevelEquals_dB(targetPlayer, 2 + 3 - 4 - 5);
    }

    void assertResponseDoesNotLoadNextTargetWhenComplete(UseCase &useCase) {
        testMethod.setNextTarget("a");
        run(initializingTest, model);
        testMethod.setComplete();
        testMethod.setNextTarget("b");
        run(useCase, model);
        assertFilePathEquals(targetPlayer, "a");
    }

    void assertWritesTarget(TargetWritingUseCase &useCase) {
        evaluator.setFileName("a");
        run(useCase, model);
        assertEqual("a", useCase.target(outputFile));
    }

    void assertPassesCurrentTargetToEvaluatorBeforeAdvancingTarget(
        UseCase &useCase) {
        run(initializingTest, model);
        setCurrentTarget(testMethod, "a");
        testMethod.setCurrentTargetWhenNextTarget("b");
        run(useCase, model);
        assertEqual("a", filePathForFileName(evaluator));
    }

    void assertTestMethodLogContains(
        UseCase &useCase, const std::string &what) {
        run(initializingTest, model);
        run(useCase, model);
        assertTrue(contains(testMethod.log(), what));
    }

    void assertYieldsTrialNumber(UseCase &useCase, int n) {
        run(useCase, model);
        assertEqual(n, model.trialNumber());
    }

    void assertUsingAllTargetPlayerChannels() {
        assertTrue(targetPlayer.usingAllChannels());
    }

    void assertUsingAllMaskerPlayerChannels() {
        assertTrue(maskerPlayer.usingAllChannels());
    }

    void assertUsesAllTargetPlayerChannels(UseCase &useCase) {
        run(useCase, model);
        assertUsingAllTargetPlayerChannels();
    }

    void assertUsesAllMaskerPlayerChannels(UseCase &useCase) {
        run(useCase, model);
        assertUsingAllMaskerPlayerChannels();
    }

    void assertMaskerPlayerChannelDelaysCleared(UseCase &useCase) {
        run(useCase, model);
        assertChannelDelaysCleared(maskerPlayer);
    }

    void assertPassesTestIdentityToOutputFile(UseCase &useCase) {
        run(useCase, model);
        assertEqual(
            outputFile.openNewFileParameters(), &std::as_const(test.identity));
    }

    void assertPassesMaskerFilePathToMaskerPlayer(UseCase &useCase) {
        setMaskerFilePath(test, "a");
        run(useCase, model);
        assertEqual("a", maskerPlayer.filePath());
    }

    void assertAllocatesTrialDurationForEyeTracking(
        UseCase &initializing, UseCase &useCase) {
        run(initializing, model);
        setDurationSeconds(targetPlayer, 3);
        setFadeTimeSeconds(maskerPlayer, 4);
        run(useCase, model);
        assertEqual(3 + 2 * 4. +
                RecognitionTestModelImpl::additionalTargetDelay.seconds,
            eyeTracker.recordingTimeAllocatedSeconds());
    }

    void assertPlayTrialDoesNotAllocateRecordingTime(UseCase &useCase) {
        run(useCase, model);
        run(playingTrial, model);
        assertFalse(eyeTracker.recordingTimeAllocated());
    }

    void
    assertPlayTrialDoesNotAllocateRecordingTimeForEyeTrackingAfterTestWithEyeTracking(
        UseCase &useCase) {
        run(initializingTestWithEyeTracking, model);
        assertPlayTrialDoesNotAllocateRecordingTime(useCase);
    }
};

#define RECOGNITION_TEST_MODEL_TEST(a) TEST_F(RecognitionTestModelTests, a)

RECOGNITION_TEST_MODEL_TEST(subscribesToPlayerEvents) {
    assertEqual(targetPlayerEventListener(model), targetPlayer.listener());
    assertEqual(maskerPlayerEventListener(model), maskerPlayer.listener());
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationShowsTargetPlayer) {
    run(playingCalibration, model);
    assertOnlyShown(targetPlayer);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestClosesOutputFile_OpensAndWritesTestInOrder) {
    assertClosesOutputFileOpensAndWritesTestInOrder(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithSingleSpeakerClosesOutputFile_OpensAndWritesTestInOrder) {
    assertClosesOutputFileOpensAndWritesTestInOrder(
        initializingTestWithSingleSpeaker);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithDelayedMaskerClosesOutputFile_OpensAndWritesTestInOrder) {
    assertClosesOutputFileOpensAndWritesTestInOrder(
        initializingTestWithDelayedMasker);
}

RECOGNITION_TEST_MODEL_TEST(initializeTestUsesAllTargetPlayerChannels) {
    run(initializingTest, model);
    assertTrue(targetPlayer.usingAllChannels());
}

RECOGNITION_TEST_MODEL_TEST(initializeTestUsesAllMaskerPlayerChannels) {
    run(initializingTest, model);
    assertUsingAllChannels(maskerPlayer);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithEyeTrackingClosesOutputFile_OpensAndWritesTestInOrder) {
    assertClosesOutputFileOpensAndWritesTestInOrder(
        initializingTestWithEyeTracking);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithEyeTrackingUsesAllMaskerPlayerChannels) {
    assertUsesAllMaskerPlayerChannels(initializingTestWithEyeTracking);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithEyeTrackingUsesAllTargetPlayerChannels) {
    assertUsesAllTargetPlayerChannels(initializingTestWithEyeTracking);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithEyeTrackingClearsAllMaskerPlayerChannelDelays) {
    assertMaskerPlayerChannelDelaysCleared(initializingTestWithEyeTracking);
}

RECOGNITION_TEST_MODEL_TEST(initializeTestClearsAllMaskerPlayerChannelDelays) {
    run(initializingTest, model);
    assertChannelDelaysCleared(maskerPlayer);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithSingleSpeakerUsesFirstChannelOnlyOfTargetPlayer) {
    run(initializingTestWithSingleSpeaker, model);
    assertOnlyUsingFirstChannel(targetPlayer);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithSingleSpeakerClearsAllMaskerPlayerChannelDelays) {
    run(initializingTestWithSingleSpeaker, model);
    assertChannelDelaysCleared(maskerPlayer);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithSingleSpeakerUsesFirstChannelOnlyOfMaskerPlayer) {
    run(initializingTestWithSingleSpeaker, model);
    assertTrue(maskerPlayer.usingFirstChannelOnly());
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithDelayedMaskerUsesFirstChannelOnlyOfTargetPlayer) {
    run(initializingTestWithDelayedMasker, model);
    assertOnlyUsingFirstChannel(targetPlayer);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithDelayedMaskerUsesAllMaskerPlayerChannels) {
    run(initializingTestWithDelayedMasker, model);
    assertUsingAllChannels(maskerPlayer);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithDelayedMaskerSetsFirstChannelMaskerDelay) {
    run(initializingTestWithDelayedMasker, model);
    assertEqual(gsl::index{0}, maskerPlayer.channelDelayed());
    assertEqual(RecognitionTestModelImpl::maskerChannelDelay.seconds,
        maskerPlayer.channelDelaySeconds());
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestOpensNewOutputFilePassingTestInformation) {
    assertPassesTestIdentityToOutputFile(initializingTest);
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
    run(initializingTestWithEyeTracking, model);
    run(playingTrial, model);
    assertTrue(started(eyeTracker));
}

RECOGNITION_TEST_MODEL_TEST(playTrialForDefaultTestDoesNotStartEyeTracking) {
    run(initializingTest, model);
    run(playingTrial, model);
    assertFalse(started(eyeTracker));
}

RECOGNITION_TEST_MODEL_TEST(
    playTrialForTestWithEyeTrackingStartsEyeTrackingAfterAllocatingRecordingTime) {
    run(initializingTestWithEyeTracking, model);
    run(playingTrial, model);
    assertEqual(
        "allocateRecordingTimeSeconds start ", string(eyeTracker.log()));
}

RECOGNITION_TEST_MODEL_TEST(
    fadeOutCompleteForTestWithEyeTrackingStopsEyeTracking) {
    run(initializingTestWithEyeTracking, model);
    fadeOutComplete(maskerPlayer);
    assertTrue(stopped(eyeTracker));
}

RECOGNITION_TEST_MODEL_TEST(
    fadeOutCompleteForDefaultTestDoesNotStopEyeTracking) {
    run(initializingTest, model);
    fadeOutComplete(maskerPlayer);
    assertFalse(stopped(eyeTracker));
}

RECOGNITION_TEST_MODEL_TEST(
    playTrialForDefaultTestDoesNotAllocateRecordingTimeForEyeTracking) {
    assertPlayTrialDoesNotAllocateRecordingTime(initializingTest);
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
        initializingTest);
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
RECOGNITION_TEST_MODEL_TEST(
    initializeTestOpensNewOutputFilePassingTestIdentity) {
    run(initializingTest, model);
    assertEqual(
        outputFile.openNewFileParameters(), &std::as_const(test.identity));
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
    run(playingTrial, model);
    assertTrue(fadedIn(maskerPlayer));
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationPlaysTarget) {
    run(playingCalibration, model);
    assertPlayed(targetPlayer);
}

RECOGNITION_TEST_MODEL_TEST(fadeInCompletePlaysTargetAtWhenEyeTracking) {
    run(initializingTestWithEyeTracking, model);
    setSystemTime(fadeInCompleteTime, 1);
    setSampleOffset(fadeInCompleteTime, 2);
    setSampleRateHz(maskerPlayer, 3);
    fadeInComplete(maskerPlayer, fadeInCompleteTime);
    assertEqual(player_system_time_type{1},
        targetPlayer.timePlayedAt().playerTime.system);
    assertEqual(
        2 / 3. + RecognitionTestModelImpl::additionalTargetDelay.seconds,
        targetPlayer.timePlayedAt().delay.seconds);
}

RECOGNITION_TEST_MODEL_TEST(
    fadeInCompletePassesTargetStartSystemTimeForConversionWhenEyeTracking) {
    run(initializingTestWithEyeTracking, model);
    setSystemTime(fadeInCompleteTime, 1);
    fadeInComplete(maskerPlayer, fadeInCompleteTime);
    assertEqual(player_system_time_type{1},
        maskerPlayer.toNanosecondsSystemTime().at(0));
}

RECOGNITION_TEST_MODEL_TEST(fadeOutCompleteStopsEyeTracker) {
    run(initializingTestWithEyeTracking, model);
    fadeOutComplete(maskerPlayer);
    assertEqual("stop ", string(eyeTracker.log()));
}

RECOGNITION_TEST_MODEL_TEST(submittingCoordinateResponseWritesEyeGazes) {
    run(initializingTestWithEyeTracking, model);
    setEyeGazes(eyeTracker, {{{1}, {2, 3}, {4, 5}}, {{6}, {7, 8}, {9, 10}}});
    run(submittingCoordinateResponse, model);
    assertEqual(
        {{{1}, {2, 3}, {4, 5}}, {{6}, {7, 8}, {9, 10}}}, outputFile.eyeGazes());
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseWritesTargetStartTimeWhenEyeTracking) {
    run(initializingTestWithEyeTracking, model);
    setNanosecondsFromPlayerTime(maskerPlayer, 1);
    setSampleOffset(fadeInCompleteTime, 2);
    setSampleRateHz(maskerPlayer, 3);
    fadeInComplete(maskerPlayer, fadeInCompleteTime);
    fadeOutComplete(maskerPlayer);
    run(submittingCoordinateResponse, model);
    assertEqual(1 +
            gsl::narrow_cast<std::uintmax_t>(
                (2 / 3. +
                    RecognitionTestModelImpl::additionalTargetDelay.seconds) *
                1e9),
        outputFile.targetStartTime().nanoseconds);
}

auto eyeTrackerTargetPlayerSynchronization(OutputFileStub &file)
    -> EyeTrackerTargetPlayerSynchronization {
    return file.eyeTrackerTargetPlayerSynchronization();
}

RECOGNITION_TEST_MODEL_TEST(submitCoordinateResponseWritesSyncTimes) {
    run(initializingTestWithEyeTracking, model);
    setNanosecondsFromPlayerTime(maskerPlayer, 1);
    setCurrentSystemTimeMicroseconds(eyeTracker, 2);
    fadeInComplete(maskerPlayer, fadeInCompleteTime);
    fadeOutComplete(maskerPlayer);
    run(submittingCoordinateResponse, model);
    assertEqual(std::uintmax_t{1},
        eyeTrackerTargetPlayerSynchronization(outputFile)
            .targetPlayerSystemTime.nanoseconds);
    assertEqual(std::int_least64_t{2},
        eyeTrackerTargetPlayerSynchronization(outputFile)
            .eyeTrackerSystemTime.microseconds);
}

RECOGNITION_TEST_MODEL_TEST(passesCurrentMaskerTimeForNanosecondConversion) {
    run(initializingTestWithEyeTracking, model);
    av_speech_in_noise::PlayerTime t{};
    t.system = 1;
    maskerPlayer.setCurrentSystemTime(t);
    fadeInComplete(maskerPlayer, fadeInCompleteTime);
    assertEqual(player_system_time_type{1},
        maskerPlayer.toNanosecondsSystemTime().at(1));
}

RECOGNITION_TEST_MODEL_TEST(fadeInCompletePlaysTargetWhenDefaultTest) {
    run(initializingTest, model);
    maskerPlayer.fadeInComplete();
    assertPlayed(targetPlayer);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestPassesNextTargetToTargetPlayer) {
    assertPassesNextTargetToPlayer(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithEyeTrackingPassesNextTargetToTargetPlayer) {
    assertPassesNextTargetToPlayer(initializingTestWithEyeTracking);
}

RECOGNITION_TEST_MODEL_TEST(initializeDefaultTestResetsTrialNumber) {
    assertYieldsTrialNumber(initializingTest, 1);
}

RECOGNITION_TEST_MODEL_TEST(initializeTestWithEyeTrackingResetsTrialNumber) {
    assertYieldsTrialNumber(initializingTestWithEyeTracking, 1);
}

RECOGNITION_TEST_MODEL_TEST(returnsTargetFileName) {
    evaluator.setFileName("a");
    assertEqual("a", targetFileName(model));
}

RECOGNITION_TEST_MODEL_TEST(
    passesCurrentTargetToEvaluatorWhenReturningTargetFileName) {
    run(initializingTest, model);
    setCurrentTarget(testMethod, "a");
    targetFileName(model);
    assertEqual("a", filePathForFileName(evaluator));
}

RECOGNITION_TEST_MODEL_TEST(initializingTestResetsTrialNumber) {
    assertYieldsTrialNumber(initializingTest, 1);
}

RECOGNITION_TEST_MODEL_TEST(submittingCoordinateResponseIncrementsTrialNumber) {
    run(initializingTest, model);
    assertYieldsTrialNumber(submittingCoordinateResponse, 2);
}

RECOGNITION_TEST_MODEL_TEST(submittingCorrectResponseIncrementsTrialNumber) {
    run(initializingTest, model);
    assertYieldsTrialNumber(submittingCorrectResponse, 2);
}

RECOGNITION_TEST_MODEL_TEST(submittingIncorrectResponseIncrementsTrialNumber) {
    run(initializingTest, model);
    assertYieldsTrialNumber(submittingIncorrectResponse, 2);
}

RECOGNITION_TEST_MODEL_TEST(submittingFreeResponseIncrementsTrialNumber) {
    run(initializingTest, model);
    assertYieldsTrialNumber(submittingFreeResponse, 2);
}

RECOGNITION_TEST_MODEL_TEST(submittingCorrectKeywordsIncrementsTrialNumber) {
    run(initializingTest, model);
    assertYieldsTrialNumber(submittingCorrectKeywords, 2);
}

RECOGNITION_TEST_MODEL_TEST(submittingConsonantIncrementsTrialNumber) {
    run(initializingTest, model);
    assertYieldsTrialNumber(submittingConsonant, 2);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingCorrectKeywordsWritesTestResultWhenComplete) {
    run(initializingTest, model);
    testMethod.setComplete();
    run(submittingCorrectKeywords, model);
    assertTrue(endsWith(testMethod.log(), "writeTestResult "));
    assertTrue(endsWith(log(outputFile), "save "));
}

RECOGNITION_TEST_MODEL_TEST(
    submittingCoordinateResponsePassesNextTargetToTargetPlayer) {
    run(initializingTest, model);
    assertPassesNextTargetToPlayer(submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(submittingConsonantPassesNextTargetToTargetPlayer) {
    run(initializingTest, model);
    assertPassesNextTargetToPlayer(submittingConsonant);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingCorrectResponsePassesNextTargetToTargetPlayer) {
    run(initializingTest, model);
    assertPassesNextTargetToPlayer(submittingCorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingIncorrectResponsePassesNextTargetToTargetPlayer) {
    run(initializingTest, model);
    assertPassesNextTargetToPlayer(submittingIncorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingFreeResponsePassesNextTargetToTargetPlayer) {
    run(initializingTest, model);
    assertPassesNextTargetToPlayer(submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingCorrectKeywordsPassesNextTargetToTargetPlayer) {
    run(initializingTest, model);
    assertPassesNextTargetToPlayer(submittingCorrectKeywords);
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationPassesAudioFileToTargetPlayer) {
    calibration.fileUrl.path = "a";
    run(playingCalibration, model);
    assertFilePathEquals(targetPlayer, "a");
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestPassesMaskerFilePathToMaskerPlayer) {
    assertPassesMaskerFilePathToMaskerPlayer(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithEyeTrackingPassesMaskerFilePathToMaskerPlayer) {
    assertPassesMaskerFilePathToMaskerPlayer(initializingTestWithEyeTracking);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(initializingTest);
}
RECOGNITION_TEST_MODEL_TEST(initializeTestPassesMaskerFilePathToMaskerPlayer) {
    setMaskerFilePath(test, "a");
    run(initializingTest, model);
    assertEqual("a", maskerPlayer.filePath());
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
    submitConsonantSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(submittingConsonant);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectKeywordsSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(submittingCorrectKeywords);
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
    assertSeeksToRandomMaskerPositionWithinTrialDuration(initializingTest);
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
    submitConsonantSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(submittingConsonant);
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

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectKeywordsSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(
        submittingCorrectKeywords);
}

RECOGNITION_TEST_MODEL_TEST(initializeDefaultTestSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(submitConsonantSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(submittingConsonant);
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

RECOGNITION_TEST_MODEL_TEST(submitCorrectKeywordsSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(submittingCorrectKeywords);
}

RECOGNITION_TEST_MODEL_TEST(initializeDefaultTestSetsInitialMaskerPlayerLevel) {
    setMaskerLevel_dB_SPL(test, 1);
    setFullScaleLevel_dB_SPL(test, 2);
    setDigitalLevel(maskerPlayer, DigitalLevel{3});
    run(initializingTest, model);
    assertEqual(1 - 2 - 3., maskerPlayer.level_dB());
}

RECOGNITION_TEST_MODEL_TEST(initializeDefaultTestSetsTargetPlayerLevel) {
    setSnr_dB(testMethod, 2);
    setMaskerLevel_dB_SPL(test, 3);
    setFullScaleLevel_dB_SPL(test, 4);
    setDigitalLevel(maskerPlayer, DigitalLevel{5});
    run(initializingTest, model);
    assertLevelEquals_dB(targetPlayer, 2 + 3 - 4 - 5);
}

RECOGNITION_TEST_MODEL_TEST(submitCoordinateResponseSetsTargetPlayerLevel) {
    assertSetsTargetLevel(submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(submitFreeResponseSetsTargetPlayerLevel) {
    assertSetsTargetLevel(submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(submitCorrectKeywordsSetsTargetPlayerLevel) {
    assertSetsTargetLevel(submittingCorrectKeywords);
}

RECOGNITION_TEST_MODEL_TEST(submitCorrectResponseSetsTargetPlayerLevel) {
    assertSetsTargetLevel(submittingCorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(submitIncorrectResponseSetsTargetPlayerLevel) {
    assertSetsTargetLevel(submittingIncorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationSetsTargetPlayerLevel) {
    calibration.level.dB_SPL = 1;
    calibration.fullScaleLevel.dB_SPL = 2;
    targetPlayer.setDigitalLevel(DigitalLevel{3});
    run(playingCalibration, model);
    assertLevelEquals_dB(targetPlayer, 1 - 2 - 3);
}

RECOGNITION_TEST_MODEL_TEST(startTrialShowsTargetPlayerWhenAudioVisual) {
    test.condition = Condition::audioVisual;
    run(initializingTest, model);
    run(playingTrial, model);
    assertTrue(shown(targetPlayer));
}

RECOGNITION_TEST_MODEL_TEST(maskerFadeOutCompleteHidesTargetPlayer) {
    fadeOutComplete(maskerPlayer);
    assertOnlyHidden(targetPlayer);
}

RECOGNITION_TEST_MODEL_TEST(startTrialDoesNotShowTargetPlayerWhenAuditoryOnly) {
    test.condition = Condition::auditoryOnly;
    run(initializingTest, model);
    run(playingTrial, model);
    assertNotShown(targetPlayer);
}

RECOGNITION_TEST_MODEL_TEST(initializeTestHidesTargetPlayer) {
    run(initializingTest, model);
    assertOnlyHidden(targetPlayer);
}

RECOGNITION_TEST_MODEL_TEST(targetPlaybackCompleteFadesOutMasker) {
    targetPlayer.playbackComplete();
    assertTrue(maskerPlayer.fadeOutCalled());
}

RECOGNITION_TEST_MODEL_TEST(fadeOutCompleteNotifiesTrialComplete) {
    fadeOutComplete(maskerPlayer);
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
    submitCorrectKeywordsSavesOutputFileAfterWritingTrial) {
    assertSavesOutputFileAfterWritingTrial(submittingCorrectKeywords);
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
        initializingTest, "Unable to open output file.");
}

RECOGNITION_TEST_MODEL_TEST(
    playCalibrationThrowsRequestFailureWhenTargetPlayerThrowsInvalidAudioFile) {
    calibration.fileUrl.path = "a";
    targetPlayer.throwInvalidAudioFileOnRms();
    assertCallThrowsRequestFailure(playingCalibration, "unable to read a");
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestThrowsRequestFailureWhenMaskerPlayerThrowsInvalidAudioFile) {
    setMaskerFilePath(test, "a");
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
    assertFalse(fadedIn(maskerPlayer));
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationDoesNotPlayIfTrialInProgress) {
    runIgnoringFailureWithTrialInProgress(playingCalibration);
    assertFalse(played(targetPlayer));
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestThrowsRequestFailureIfTrialInProgress) {
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
    setMaskerFilePath(test, "a");
    runIgnoringFailureWithTrialInProgress(initializingTest);
    assertEqual("", maskerPlayer.filePath());
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestDoesNotHideTargetPlayerWhenAuditoryOnlyButTrialInProgress) {
    test.condition = Condition::auditoryOnly;
    runIgnoringFailureWithTrialInProgress(initializingTest);
    assertNotHidden(targetPlayer);
}

RECOGNITION_TEST_MODEL_TEST(audioDevicesReturnsOutputAudioDeviceDescriptions) {
    maskerPlayer.setOutputAudioDeviceDescriptions({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, model.audioDevices());
}

RECOGNITION_TEST_MODEL_TEST(testCompleteWhenComplete) {
    run(initializingTest, model);
    assertFalse(testComplete(model));
    testMethod.setComplete();
    assertTrue(testComplete(model));
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
    submitCorrectKeywordsDoesNotLoadNextTargetWhenComplete) {
    assertResponseDoesNotLoadNextTargetWhenComplete(submittingCorrectKeywords);
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
    run(initializingTest, model);
    assertFilePathEquals(targetPlayer, "");
}

RECOGNITION_TEST_MODEL_TEST(submitFreeResponseWritesResponse) {
    freeResponse.response = "a";
    run(submittingFreeResponse, model);
    assertEqual("a", freeResponseTrial(outputFile).response);
}

RECOGNITION_TEST_MODEL_TEST(submitFreeResponseWritesFlagged) {
    freeResponse.flagged = true;
    run(submittingFreeResponse, model);
    assertTrue(freeResponseTrial(outputFile).flagged);
}

RECOGNITION_TEST_MODEL_TEST(submitFreeResponseWritesWithoutFlag) {
    run(submittingFreeResponse, model);
    assertFalse(freeResponseTrial(outputFile).flagged);
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
        "submitCoordinateResponse writeLastCoordinateResponse ");
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
    submitCorrectKeywordsWritesTrialAfterSubmittingResponse) {
    assertTestMethodLogContains(submittingCorrectKeywords,
        "submitCorrectKeywords writeLastCorrectKeywords ");
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
    run(initializingTest, model);
    run(submittingCorrectResponse, model);
    assertTrue(testMethod.submittedCorrectResponse());
}

RECOGNITION_TEST_MODEL_TEST(submitIncorrectResponseSubmitsIncorrectResponse) {
    run(initializingTest, model);
    run(submittingIncorrectResponse, model);
    assertTrue(testMethod.submittedIncorrectResponse());
}

RECOGNITION_TEST_MODEL_TEST(submitFreeResponseSubmitsResponse) {
    run(initializingTest, model);
    run(submittingFreeResponse, model);
    assertTrue(testMethod.submittedFreeResponse());
}

RECOGNITION_TEST_MODEL_TEST(submitCorrectKeywordsSubmitsResponse) {
    run(initializingTest, model);
    run(submittingCorrectKeywords, model);
    assertTrue(testMethod.submittedCorrectKeywords());
}
}
}
