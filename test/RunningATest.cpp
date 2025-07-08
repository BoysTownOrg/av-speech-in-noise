#include "LogString.hpp"
#include "MaskerPlayerStub.hpp"
#include "ModelObserverStub.hpp"
#include "OutputFileStub.hpp"
#include "RandomizerStub.hpp"
#include "ResponseEvaluatorStub.hpp"
#include "TargetPlayerStub.hpp"
#include "assert-utility.hpp"

#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/core/RunningATest.hpp>

#include <gtest/gtest.h>

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace av_speech_in_noise {
constexpr auto operator==(const EyeGaze &a, const EyeGaze &b) -> bool {
    return a.x == b.x && a.y == b.y;
}

namespace {
class TestMethodStub : public TestMethod {
  public:
    void setSnr_dB(int x) { snr_dB_ = x; }

    auto snr() -> FloatSNR override {
        FloatSNR snr{};
        snr.dB = snr_dB_;
        return snr;
    }

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
};

class UseCase {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(UseCase);
    virtual void run(RunningATestImpl &) = 0;
};

class InitializingTest : public UseCase {
  public:
    explicit InitializingTest(TestMethod *method, const Test &test,
        std::vector<std::reference_wrapper<RunningATest::TestObserver>>
            observer)
        : test{test}, method{method}, observer{std::move(observer)} {}

    void run(RunningATestImpl &m) override {
        m.initialize(method, test, observer);
    }

  private:
    const Test &test{};
    TestMethod *method;
    std::vector<std::reference_wrapper<RunningATest::TestObserver>> observer;
};

class InitializingTestWithSingleSpeaker : public UseCase {
  public:
    explicit InitializingTestWithSingleSpeaker(TestMethod *method)
        : method{method} {}

    void run(RunningATestImpl &m) override {
        Test test;
        test.audioChannelOption = AudioChannelOption::singleSpeaker;
        m.initialize(method, test, {});
    }

  private:
    TestMethod *method;
};

class InitializingTestWithDelayedMasker : public UseCase {
  public:
    explicit InitializingTestWithDelayedMasker(TestMethod *method)
        : method{method} {}

    void run(RunningATestImpl &m) override {
        Test test;
        test.audioChannelOption = AudioChannelOption::delayedMasker;
        m.initialize(method, test, {});
    }

  private:
    TestMethod *method;
};

class AudioDeviceUseCase : public virtual UseCase {
  public:
    virtual void setAudioDevice(std::string) = 0;
};

class PlayerLevelUseCase : public virtual UseCase {
  public:
    virtual void set(DigitalLevel) = 0;
    virtual auto levelAmplification() -> LevelAmplification = 0;
};

class PlayingCalibration : public AudioDeviceUseCase,
                           public PlayerLevelUseCase {
  public:
    explicit PlayingCalibration(
        Calibration &calibration, TargetPlayerStub &player)
        : calibration{calibration}, player{player} {}

    void setAudioDevice(std::string s) override {
        calibration.audioDevice = std::move(s);
    }

    void run(RunningATestImpl &model) override {
        model.playCalibration(calibration);
    }

    void set(DigitalLevel x) override { player.setDigitalLevel(x); }

    auto levelAmplification() -> LevelAmplification override {
        return player.levelAmplification();
    }

  private:
    Calibration &calibration;
    TargetPlayerStub &player;
};

class PlayingLeftSpeakerCalibration : public AudioDeviceUseCase,
                                      public PlayerLevelUseCase {
  public:
    explicit PlayingLeftSpeakerCalibration(
        Calibration &calibration, MaskerPlayerStub &player)
        : calibration{calibration}, player{player} {}

    void setAudioDevice(std::string s) override {
        calibration.audioDevice = std::move(s);
    }

    void run(RunningATestImpl &model) override {
        model.playLeftSpeakerCalibration(calibration);
    }

    void set(DigitalLevel x) override { player.setDigitalLevel(x); }

    auto levelAmplification() -> LevelAmplification override {
        return player.levelAmplification();
    }

  private:
    Calibration &calibration;
    MaskerPlayerStub &player;
};

class PlayingRightSpeakerCalibration : public AudioDeviceUseCase,
                                       public PlayerLevelUseCase {
  public:
    explicit PlayingRightSpeakerCalibration(
        Calibration &calibration, MaskerPlayerStub &player)
        : calibration{calibration}, player{player} {}

    void setAudioDevice(std::string s) override {
        calibration.audioDevice = std::move(s);
    }

    void run(RunningATestImpl &model) override {
        model.playRightSpeakerCalibration(calibration);
    }

    void set(DigitalLevel x) override { player.setDigitalLevel(x); }

    auto levelAmplification() -> LevelAmplification override {
        return player.levelAmplification();
    }

  private:
    Calibration &calibration;
    MaskerPlayerStub &player;
};

class PlayingTrial : public AudioDeviceUseCase {
  public:
    void setAudioDevice(std::string s) override {
        trial.audioDevice = std::move(s);
    }

    void run(RunningATestImpl &m) override { m.playTrial(trial); }

  private:
    AudioSettings trial;
};

class PreparingNextTrialIfNeeded : public UseCase {
  public:
    void run(RunningATestImpl &m) override { m.prepareNextTrialIfNeeded(); }
};

class SubmittingCoordinateResponse : public UseCase {
  public:
    void run(RunningATestImpl &m) override {
        m.submit(coordinate_response_measure::Response{});
    }
};

class ClockStub : public Clock {
  public:
    [[nodiscard]] auto timeQueried() const -> bool { return timeQueried_; }

    auto time() -> std::string override {
        timeQueried_ = true;
        return time_;
    }

  private:
    std::string time_;
    bool timeQueried_{};
};

class RunningATestObserverStub : public RunningATest::TestObserver {
  public:
    void notifyThatNewTestIsReady(std::string_view session) override {
        this->session = session;
    }

    void notifyThatTrialWillBegin(int trialNumber) override {
        this->trialNumber = trialNumber;
    }

    void notifyThatTargetWillPlayAt(
        const PlayerTimeWithDelay &playerTimeWithDelay) override {
        this->playerTimeWithDelay = playerTimeWithDelay;
    }

    void notifyThatStimulusHasEnded() override {
        notifiedThatStimulusHasEnded = true;
    }

    void notifyThatSubjectHasResponded() override {
        notifiedThatSubjectHasResponded = true;
    }

    PlayerTimeWithDelay playerTimeWithDelay;
    std::string session;
    int trialNumber{};
    bool notifiedThatStimulusHasEnded{};
    bool notifiedThatSubjectHasResponded{};
};

void setMaskerLevel_dB_SPL(Test &test, int x) { test.maskerLevel.dB_SPL = x; }

void setCurrentTarget(TestMethodStub &m, std::string s) {
    m.setCurrentTarget(std::move(s));
}

auto targetFileName(RunningATestImpl &m) -> std::string {
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
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(shown(targetPlayer));
}

void assertNotHidden(TargetPlayerStub &targetPlayer) {
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(hidden(targetPlayer));
}

void assertOnlyHidden(TargetPlayerStub &targetPlayer) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(hidden(targetPlayer));
    assertNotShown(targetPlayer);
}

void assertOnlyShown(TargetPlayerStub &targetPlayer) {
    assertNotHidden(targetPlayer);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(shown(targetPlayer));
}

auto log(OutputFileStub &file) -> const std::stringstream & {
    return file.log();
}

void run(UseCase &useCase, RunningATestImpl &model) { useCase.run(model); }

auto fadedIn(MaskerPlayerStub &maskerPlayer) -> bool {
    return maskerPlayer.fadeInCalled();
}

auto played(TargetPlayerStub &player) -> bool { return player.played(); }

void assertPlayed(TargetPlayerStub &player) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(played(player));
}

void assertPlayed(MaskerPlayerStub &player) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(player.played());
}

auto filePath(TargetPlayerStub &player) -> std::string {
    return player.filePath();
}

void assertFilePathEquals(TargetPlayerStub &player, const std::string &what) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(what, filePath(player));
}

void assertFilePathEquals(MaskerPlayerStub &player, const std::string &what) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(what, player.filePath());
}

auto secondsSeeked(MaskerPlayerStub &player) -> double {
    return player.secondsSeeked();
}

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

void assertLevelEquals_dB(TargetPlayerStub &player, double x) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(x, player.level_dB());
}

auto testComplete(RunningATestImpl &model) -> bool {
    return model.testComplete();
}

void assertOnlyUsingFirstChannel(TargetPlayerStub &player) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(player.usingFirstChannelOnly());
}

void assertOnlyUsingFirstChannel(MaskerPlayerStub &player) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(player.usingFirstChannelOnly());
}

void assertOnlyUsingSecondChannel(MaskerPlayerStub &player) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(player.usingSecondChannelOnly());
}

void assertUsingAllChannels(MaskerPlayerStub &player) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(player.usingAllChannels());
}

void assertChannelDelaysCleared(MaskerPlayerStub &player) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(player.channelDelaysCleared());
}

auto targetPlayerObserver(const RunningATestImpl &model)
    -> const TargetPlayer::Observer * {
    return &model;
}

auto maskerPlayerObserver(const RunningATestImpl &model)
    -> const MaskerPlayer::Observer * {
    return &model;
}

void runIgnoringFailure(UseCase &useCase, RunningATestImpl &model) {
    try {
        run(useCase, model);
    } catch (const RunningATest::RequestFailure &) {
        // ignoring
        0;
    }
}

void setDurationSeconds(TargetPlayerStub &player, double x) {
    player.setDurationSeconds(x);
}

void setFadeTimeSeconds(MaskerPlayerStub &player, double x) {
    player.setFadeTimeSeconds(x);
}

void setSystemTime(AudioSampleTimeWithOffset &time, player_system_time_type s) {
    time.playerTime.system = s;
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

class RunningATestTests : public ::testing::Test {
  protected:
    ModelObserverStub listener;
    TargetPlayerStub targetPlayer;
    MaskerPlayerStub maskerPlayer;
    ResponseEvaluatorStub evaluator;
    OutputFileStub outputFile;
    RandomizerStub randomizer;
    ClockStub clock;
    RunningATestImpl model{
        targetPlayer, maskerPlayer, evaluator, outputFile, randomizer, clock};
    TestMethodStub testMethod;
    Calibration calibration{};
    PlayingCalibration playingCalibration{calibration, targetPlayer};
    PlayingLeftSpeakerCalibration playingLeftSpeakerCalibration{
        calibration, maskerPlayer};
    PlayingRightSpeakerCalibration playingRightSpeakerCalibration{
        calibration, maskerPlayer};
    av_speech_in_noise::Test test{};
    RunningATestObserverStub observer;
    RunningATestObserverStub secondObserver;
    InitializingTest initializingTest{
        &testMethod, test, {std::ref(observer), std::ref(secondObserver)}};
    InitializingTestWithSingleSpeaker initializingTestWithSingleSpeaker{
        &testMethod};
    InitializingTestWithDelayedMasker initializingTestWithDelayedMasker{
        &testMethod};
    PlayingTrial playingTrial;
    SubmittingCoordinateResponse submittingCoordinateResponse;
    FreeResponse freeResponse{};
    ThreeKeywordsResponse threeKeywords;
    SyllableResponse syllableResponse;
    AudioSampleTimeWithOffset fadeInCompleteTime{};
    PreparingNextTrialIfNeeded preparingNextTrialIfNeeded;

    RunningATestTests() { model.attach(&listener); }

    void assertClosesOutputFileOpensAndWritesTestInOrder(UseCase &useCase) {
        run(useCase, model);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"close openNewFile writeTest "},
            string(log(outputFile)));
    }

    template <typename T>
    void assertDevicePassedToPlayer(
        const T &player, AudioDeviceUseCase &useCase) {
        useCase.setAudioDevice("a");
        run(useCase, model);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, player.device());
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

    void assertSeeksToRandomMaskerPositionWithinTrialDuration(
        UseCase &useCase) {
        setDurationSeconds(targetPlayer, 1);
        setFadeTimeSeconds(maskerPlayer, 2);
        maskerPlayer.setDurationSeconds(10);
        run(useCase, model);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(0., randomizer.lowerFloatBound());
        ::assertEqual(10. - 2 - 1 - 2 -
                RunningATestImpl::targetOnsetFringeDuration.seconds -
                RunningATestImpl::targetOffsetFringeDuration.seconds,
            randomizer.upperFloatBound(), 1e-15);
    }

    void assertMaskerPlayerSeekedToRandomTime(UseCase &useCase) {
        randomizer.setRandomFloat(1);
        run(useCase, model);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1., secondsSeeked(maskerPlayer));
    }

    void assertSavesOutputFileAfterWritingTrial(UseCase &useCase) {
        run(useCase, model);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(endsWith(log(outputFile), "save "));
    }

    void assertCallThrowsRequestFailure(
        UseCase &useCase, const std::string &what) {
        try {
            run(useCase, model);
            FAIL() << "Expected Model::RequestFailure";
        } catch (const RunningATest::RequestFailure &e) {
            AV_SPEECH_IN_NOISE_EXPECT_EQUAL(what, e.what());
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
        model.playTrial({});
        runIgnoringFailure(useCase, model);
    }

    void assertThrowsRequestFailureWhenTrialInProgress(UseCase &useCase) {
        model.playTrial({});
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

    void assertTestMethodLogContains(
        UseCase &useCase, const std::string &what) {
        run(initializingTest, model);
        run(useCase, model);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(contains(testMethod.log(), what));
    }

    void assertYieldsTrialNumber(UseCase &useCase, int n) {
        run(useCase, model);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(n, model.trialNumber());
    }

    void assertUsingAllTargetPlayerChannels() {
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(targetPlayer.usingAllChannels());
    }

    void assertUsingAllMaskerPlayerChannels() {
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(maskerPlayer.usingAllChannels());
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
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            outputFile.openNewFileParameters(), &std::as_const(test.identity));
    }

    void assertPassesMaskerFilePathToMaskerPlayer(UseCase &useCase) {
        setMaskerFilePath(test, "a");
        run(useCase, model);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"a"}, maskerPlayer.filePath());
    }
};

#define RECOGNITION_TEST_MODEL_TEST(a) TEST_F(RunningATestTests, a)

RECOGNITION_TEST_MODEL_TEST(subscribesToPlayerEvents) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        targetPlayerObserver(model), targetPlayer.listener());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        maskerPlayerObserver(model), maskerPlayer.listener());
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
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(targetPlayer.usingAllChannels());
}

RECOGNITION_TEST_MODEL_TEST(playingCalibrationUsesAllTargetPlayerChannels) {
    run(playingCalibration, model);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(targetPlayer.usingAllChannels());
}

RECOGNITION_TEST_MODEL_TEST(initializeTestUsesAllMaskerPlayerChannels) {
    run(initializingTest, model);
    assertUsingAllChannels(maskerPlayer);
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
    playingLeftSpeakerCalibrationUsesFirstChannelOnlyOfMaskerPlayer) {
    run(playingLeftSpeakerCalibration, model);
    assertOnlyUsingFirstChannel(maskerPlayer);
}

RECOGNITION_TEST_MODEL_TEST(
    playingRightSpeakerCalibrationUsesSecondChannelOnlyOfTargetPlayer) {
    run(playingRightSpeakerCalibration, model);
    assertOnlyUsingSecondChannel(maskerPlayer);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithSingleSpeakerClearsAllMaskerPlayerChannelDelays) {
    run(initializingTestWithSingleSpeaker, model);
    assertChannelDelaysCleared(maskerPlayer);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithSingleSpeakerUsesFirstChannelOnlyOfMaskerPlayer) {
    run(initializingTestWithSingleSpeaker, model);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(maskerPlayer.usingFirstChannelOnly());
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
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        gsl::index{0}, maskerPlayer.channelDelayed());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        RunningATestImpl::maskerChannelDelay.seconds,
        maskerPlayer.channelDelaySeconds());
}

RECOGNITION_TEST_MODEL_TEST(initializeTestEnablesVibrotactileStimulus) {
    test.enableVibrotactileStimulus = true;
    run(initializingTest, model);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(maskerPlayer.vibrotactileStimulusEnabled);
}

RECOGNITION_TEST_MODEL_TEST(initializeTestDisablesVibrotactileStimulus) {
    test.enableVibrotactileStimulus = false;
    run(initializingTest, model);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(maskerPlayer.vibrotactileStimulusDisabled);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestOpensNewOutputFilePassingTestInformation) {
    assertPassesTestIdentityToOutputFile(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(initializeTestNotifiesObserverOfNewTest) {
    test.identity.session = "smile";
    run(initializingTest, model);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("smile", observer.session);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("smile", secondObserver.session);
}

RECOGNITION_TEST_MODEL_TEST(playTrialNotifiesObserverOfTrialAboutToBegin) {
    run(initializingTest, model);
    run(playingTrial, model);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, observer.trialNumber);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, secondObserver.trialNumber);
}

RECOGNITION_TEST_MODEL_TEST(fadeOutCompleteNotifiesThatStimulusHasEnded) {
    run(initializingTest, model);
    fadeOutComplete(maskerPlayer);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(observer.notifiedThatStimulusHasEnded);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(secondObserver.notifiedThatStimulusHasEnded);
}

RECOGNITION_TEST_MODEL_TEST(playTrialCapturesTimeStampForEventualReporting) {
    run(initializingTest, model);
    run(playingTrial, model);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(clock.timeQueried());
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestOpensNewOutputFilePassingTestIdentity) {
    run(initializingTest, model);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        outputFile.openNewFileParameters(), &std::as_const(test.identity));
}

RECOGNITION_TEST_MODEL_TEST(playTrialPassesAudioDeviceToTargetPlayer) {
    assertDevicePassedToTargetPlayer(playingTrial);
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationPassesAudioDeviceToTargetPlayer) {
    assertDevicePassedToTargetPlayer(playingCalibration);
}

RECOGNITION_TEST_MODEL_TEST(
    playLeftSpeakerCalibrationPassesAudioDeviceToMaskerPlayer) {
    assertDevicePassedToMaskerPlayer(playingLeftSpeakerCalibration);
}

RECOGNITION_TEST_MODEL_TEST(
    playRightSpeakerCalibrationPassesAudioDeviceToMaskerPlayer) {
    assertDevicePassedToMaskerPlayer(playingRightSpeakerCalibration);
}

RECOGNITION_TEST_MODEL_TEST(playTrialPassesAudioDeviceToMaskerPlayer) {
    assertDevicePassedToMaskerPlayer(playingTrial);
}

RECOGNITION_TEST_MODEL_TEST(targetPrerollCompleteFadesInMasker) {
    targetPlayer.preRollComplete();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(fadedIn(maskerPlayer));
}

RECOGNITION_TEST_MODEL_TEST(playTrialPrerollsTarget) {
    run(playingTrial, model);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(targetPlayer.preRolling());
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationPlaysTarget) {
    run(playingCalibration, model);
    assertPlayed(targetPlayer);
}

RECOGNITION_TEST_MODEL_TEST(playLeftSpeakerCalibrationPlaysMasker) {
    run(playingLeftSpeakerCalibration, model);
    assertPlayed(maskerPlayer);
}

RECOGNITION_TEST_MODEL_TEST(playRightSpeakerCalibrationPlaysMasker) {
    run(playingRightSpeakerCalibration, model);
    assertPlayed(maskerPlayer);
}

RECOGNITION_TEST_MODEL_TEST(fadeInCompletePlaysTargetAt) {
    run(initializingTest, model);
    setSystemTime(fadeInCompleteTime, 1);
    setSampleOffset(fadeInCompleteTime, 2);
    setSampleRateHz(maskerPlayer, 3);
    fadeInComplete(maskerPlayer, fadeInCompleteTime);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(player_system_time_type{1},
        targetPlayer.timePlayedAt().playerTime.system);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        2 / 3. + RunningATestImpl::targetOnsetFringeDuration.seconds,
        targetPlayer.timePlayedAt().delay.seconds);
}

RECOGNITION_TEST_MODEL_TEST(
    fadeInCompleteNotifiesObserverThatTargetWillPlayAt) {
    run(initializingTest, model);
    setSystemTime(fadeInCompleteTime, 1);
    setSampleOffset(fadeInCompleteTime, 2);
    setSampleRateHz(maskerPlayer, 3);
    fadeInComplete(maskerPlayer, fadeInCompleteTime);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(player_system_time_type{1},
        observer.playerTimeWithDelay.playerTime.system);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        2 / 3. + RunningATestImpl::targetOnsetFringeDuration.seconds,
        observer.playerTimeWithDelay.delay.seconds);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestPassesNextTargetToTargetPlayer) {
    assertPassesNextTargetToPlayer(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestPassesVideoScaleToTargetPlayer) {
    test.videoScale = RationalNumber{3, 4};
    run(initializingTest, model);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(3, targetPlayer.videoScale().numerator);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(4, targetPlayer.videoScale().denominator);
}

RECOGNITION_TEST_MODEL_TEST(initializeDefaultTestResetsTrialNumber) {
    assertYieldsTrialNumber(initializingTest, 1);
}

RECOGNITION_TEST_MODEL_TEST(returnsTargetFileName) {
    evaluator.setFileName("a");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, targetFileName(model));
}

RECOGNITION_TEST_MODEL_TEST(
    passesCurrentTargetToEvaluatorWhenReturningTargetFileName) {
    run(initializingTest, model);
    setCurrentTarget(testMethod, "a");
    targetFileName(model);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, filePathForFileName(evaluator));
}

RECOGNITION_TEST_MODEL_TEST(initializingTestResetsTrialNumber) {
    assertYieldsTrialNumber(initializingTest, 1);
}

RECOGNITION_TEST_MODEL_TEST(submittingCoordinateResponseIncrementsTrialNumber) {
    run(initializingTest, model);
    assertYieldsTrialNumber(submittingCoordinateResponse, 2);
}

RECOGNITION_TEST_MODEL_TEST(preparingNextTrialIfNeededIncrementsTrialNumber) {
    run(initializingTest, model);
    assertYieldsTrialNumber(preparingNextTrialIfNeeded, 2);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingCoordinateResponsePassesNextTargetToTargetPlayer) {
    run(initializingTest, model);
    assertPassesNextTargetToPlayer(submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingCoordinateResponsePassesVideoScaleToTargetPlayer) {
    test.videoScale = RationalNumber{3, 4};
    run(initializingTest, model);
    run(submittingCoordinateResponse, model);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(3, targetPlayer.videoScale().numerator);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(4, targetPlayer.videoScale().denominator);
}

RECOGNITION_TEST_MODEL_TEST(
    preparingNextTrialIfNeededPassesNextTargetToTargetPlayer) {
    run(initializingTest, model);
    assertPassesNextTargetToPlayer(preparingNextTrialIfNeeded);
}

RECOGNITION_TEST_MODEL_TEST(
    preparingNextTrialIfNeededPassesVideoScaleToTargetPlayer) {
    test.videoScale = RationalNumber{3, 4};
    run(initializingTest, model);
    run(preparingNextTrialIfNeeded, model);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(3, targetPlayer.videoScale().numerator);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(4, targetPlayer.videoScale().denominator);
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationPassesAudioFileToTargetPlayer) {
    calibration.fileUrl.path = "a";
    run(playingCalibration, model);
    assertFilePathEquals(targetPlayer, "a");
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationKeepsOriginalVideoScaling) {
    run(playingCalibration, model);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, targetPlayer.videoScale().numerator);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, targetPlayer.videoScale().denominator);
}

RECOGNITION_TEST_MODEL_TEST(
    playLeftSpeakerCalibrationPassesAudioFileToMaskerPlayer) {
    calibration.fileUrl.path = "a";
    run(playingLeftSpeakerCalibration, model);
    assertFilePathEquals(maskerPlayer, "a");
}

RECOGNITION_TEST_MODEL_TEST(
    playRightSpeakerCalibrationPassesAudioFileToMaskerPlayer) {
    calibration.fileUrl.path = "a";
    run(playingRightSpeakerCalibration, model);
    assertFilePathEquals(maskerPlayer, "a");
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestPassesMaskerFilePathToMaskerPlayer) {
    assertPassesMaskerFilePathToMaskerPlayer(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(initializeTestPassesMaskerFilePathToMaskerPlayer) {
    setMaskerFilePath(test, "a");
    run(initializingTest, model);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, maskerPlayer.filePath());
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestSetsMaskerSteadyLevelDuration) {
    setDurationSeconds(targetPlayer, 1);
    run(initializingTest, model);
    ::assertEqual(RunningATestImpl::targetOnsetFringeDuration.seconds +
            RunningATestImpl::targetOffsetFringeDuration.seconds + 1,
        maskerPlayer.steadyLevelDuration().seconds, 1e-15);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(
        submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    preparingNextTrialIfNeededSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(
        preparingNextTrialIfNeeded);
}

RECOGNITION_TEST_MODEL_TEST(initializeDefaultTestSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    preparingNextTrialIfNeededSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(preparingNextTrialIfNeeded);
}

RECOGNITION_TEST_MODEL_TEST(initializeDefaultTestSetsInitialMaskerPlayerLevel) {
    setMaskerLevel_dB_SPL(test, 1);
    setFullScaleLevel_dB_SPL(test, 2);
    setDigitalLevel(maskerPlayer, DigitalLevel{3});
    run(initializingTest, model);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1 - 2 - 3., maskerPlayer.level_dB());
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

RECOGNITION_TEST_MODEL_TEST(preparingNextTrialIfNeededSetsTargetPlayerLevel) {
    assertSetsTargetLevel(preparingNextTrialIfNeeded);
}

void assertLevelSet(Calibration &calibration, PlayerLevelUseCase &useCase,
    RunningATestImpl &model) {
    calibration.level.dB_SPL = 1;
    calibration.fullScaleLevel.dB_SPL = 2;
    useCase.set(DigitalLevel{3});
    run(useCase, model);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1 - 2 - 3, useCase.levelAmplification().dB);
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationSetsTargetPlayerLevel) {
    assertLevelSet(calibration, playingCalibration, model);
}

RECOGNITION_TEST_MODEL_TEST(playLeftSpeakerCalibrationSetsTargetPlayerLevel) {
    assertLevelSet(calibration, playingLeftSpeakerCalibration, model);
}

RECOGNITION_TEST_MODEL_TEST(playRightSpeakerCalibrationSetsTargetPlayerLevel) {
    assertLevelSet(calibration, playingRightSpeakerCalibration, model);
}

RECOGNITION_TEST_MODEL_TEST(startTrialShowsTargetPlayerWhenAudioVisual) {
    test.condition = Condition::audioVisual;
    run(initializingTest, model);
    run(playingTrial, model);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(shown(targetPlayer));
}

RECOGNITION_TEST_MODEL_TEST(maskerFadeOutCompleteHidesTargetPlayer) {
    fadeOutComplete(maskerPlayer);
    assertOnlyHidden(targetPlayer);
}

RECOGNITION_TEST_MODEL_TEST(
    maskerFadeOutCompleteDoesNotHideTargetPlayerWhenKeepingVideoShown) {
    test.keepVideoShown = true;
    run(initializingTest, model);
    targetPlayer.clearHidden();
    fadeOutComplete(maskerPlayer);
    assertNotHidden(targetPlayer);
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

RECOGNITION_TEST_MODEL_TEST(initializeTestStopsTargetPlayer) {
    run(initializingTest, model);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(maskerPlayer.stopped());
}

RECOGNITION_TEST_MODEL_TEST(fadeOutCompleteNotifiesTrialComplete) {
    fadeOutComplete(maskerPlayer);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(listener.notified());
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseSavesOutputFileAfterWritingTrial) {
    assertSavesOutputFileAfterWritingTrial(submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    preparingNextTrialStopsNotifiesThatSubjectHasResponded) {
    run(initializingTest, model);
    model.prepareNextTrialIfNeeded();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(observer.notifiedThatSubjectHasResponded);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        secondObserver.notifiedThatSubjectHasResponded);
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
    targetPlayer.throwInvalidAudioFileOnDigitalLevel();
    assertCallThrowsRequestFailure(playingCalibration, "unable to read a");
}

RECOGNITION_TEST_MODEL_TEST(
    playingLeftSpeakerCalibrationThrowsRequestFailureWhenMaskerPlayerThrowsInvalidAudioFile) {
    calibration.fileUrl.path = "a";
    maskerPlayer.throwInvalidAudioFileOnLoad();
    assertCallThrowsRequestFailure(
        playingLeftSpeakerCalibration, "unable to read a");
}

RECOGNITION_TEST_MODEL_TEST(
    playingRightSpeakerCalibrationThrowsRequestFailureWhenMaskerPlayerThrowsInvalidAudioFile) {
    calibration.fileUrl.path = "a";
    maskerPlayer.throwInvalidAudioFileOnLoad();
    assertCallThrowsRequestFailure(
        playingRightSpeakerCalibration, "unable to read a");
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
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(maskerPlayer.setDeviceCalled());
}

RECOGNITION_TEST_MODEL_TEST(
    playCalibrationDoesNotChangeAudioDeviceWhenTrialInProgress) {
    runIgnoringFailureWithTrialInProgress(playingCalibration);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, targetPlayer.timesSetDeviceCalled());
}

RECOGNITION_TEST_MODEL_TEST(playTrialDoesNotPreRollTargetIfTrialInProgress) {
    runIgnoringFailureWithTrialInProgress(playingTrial);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, targetPlayer.timesPreRolled());
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationDoesNotPlayIfTrialInProgress) {
    runIgnoringFailureWithTrialInProgress(playingCalibration);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(played(targetPlayer));
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
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{""}, maskerPlayer.filePath());
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestDoesNotHideTargetPlayerWhenAuditoryOnlyButTrialInProgress) {
    test.condition = Condition::auditoryOnly;
    runIgnoringFailureWithTrialInProgress(initializingTest);
    assertNotHidden(targetPlayer);
}

RECOGNITION_TEST_MODEL_TEST(audioDevicesReturnsOutputAudioDeviceDescriptions) {
    maskerPlayer.setOutputAudioDeviceDescriptions({"a", "b", "c"});
    ::assertEqual({"a", "b", "c"}, model.audioDevices());
}

RECOGNITION_TEST_MODEL_TEST(testCompleteWhenComplete) {
    run(initializingTest, model);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(testComplete(model));
    testMethod.setComplete();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testComplete(model));
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseDoesNotLoadNextTargetWhenComplete) {
    assertResponseDoesNotLoadNextTargetWhenComplete(
        submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    preparingNextTrialIfNeededDoesNotLoadNextTargetWhenComplete) {
    assertResponseDoesNotLoadNextTargetWhenComplete(preparingNextTrialIfNeeded);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeDefaultTestDoesNotLoadNextTargetWhenComplete) {
    testMethod.setNextTarget("a");
    testMethod.setComplete();
    run(initializingTest, model);
    assertFilePathEquals(targetPlayer, "");
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseWritesTrialAfterSubmittingResponse) {
    assertTestMethodLogContains(submittingCoordinateResponse,
        "submitCoordinateResponse writeLastCoordinateResponse ");
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseQueriesNextTargetAfterWritingResponse) {
    assertTestMethodLogContains(
        submittingCoordinateResponse, "writeLastCoordinateResponse next ");
}
}
}
