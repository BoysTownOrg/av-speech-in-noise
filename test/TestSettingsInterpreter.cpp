#include "AdaptiveMethodStub.hpp"
#include "FixedLevelMethodStub.hpp"
#include "RunningATestStub.hpp"
#include "TargetPlaylistSetReaderStub.hpp"
#include "TargetPlaylistStub.hpp"
#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "PuzzleStub.hpp"

#include <av-speech-in-noise/ui/TestSettingsInterpreter.hpp>

#include <gtest/gtest.h>

#include <functional>
#include <stdexcept>

namespace av_speech_in_noise {
constexpr auto operator==(const TrackingSequence &a, const TrackingSequence &b)
    -> bool {
    return a.down == b.down && a.up == b.up && a.runCount == b.runCount &&
        a.stepSize == b.stepSize;
}

namespace {
class SessionControllerStub : public SessionController {
  public:
    void notifyThatTestIsComplete() override {}

    void prepare(TaskPresenter &p) override {
        taskPresenter_ = &p;
        prepareCalled_ = true;
    }

    auto taskPresenter() -> const TaskPresenter * { return taskPresenter_; }

    [[nodiscard]] auto prepareCalled() const -> bool { return prepareCalled_; }

  private:
    const TaskPresenter *taskPresenter_{};
    bool prepareCalled_{};
};

class TaskPresenterStub : public TaskPresenter {
  public:
    void showResponseSubmission() override {}
    void hideResponseSubmission() override {}
    void start() override {}
    void stop() override {}
};

auto concatenate(const std::vector<std::string> &v) -> std::string {
    std::string result;
    for (const auto &v_ : v)
        result.append(v_);
    return result;
}

auto entry(TestSetting p, std::string s) -> std::string {
    return std::string{name(p)} + ": " + std::move(s);
}

auto withNewLine(std::string s) -> std::string { return std::move(s) + '\n'; }

auto entryWithNewline(TestSetting p, std::string s) -> std::string {
    return withNewLine(entry(p, std::move(s)));
}

auto entryWithNewline(TestSetting p, Method m) -> std::string {
    return entryWithNewline(p, name(m));
}

auto entryWithNewline(TestSetting p, Condition c) -> std::string {
    return entryWithNewline(p, name(c));
}

auto fixedLevelTest(ModelStub &m) -> FixedLevelTest {
    return m.fixedLevelTest();
}

auto fixedLevelTestWithEachTargetNTimes(ModelStub &m)
    -> const FixedLevelTestWithEachTargetNTimes & {
    return m.fixedLevelTestWithEachTargetNTimes();
}

void initialize(TestSettingsInterpreterImpl &interpreter,
    RunningATestFacade &model, SessionController &sessionController,
    const std::vector<std::string> &v, int startingSnr = {},
    const TestIdentity &identity = {}) {
    interpreter.initialize(
        model, sessionController, concatenate(v), identity, SNR{startingSnr});
}

void assertPassesSimpleAdaptiveSettings(
    TestSettingsInterpreterImpl &interpreter, ModelStub &model,
    SessionController &sessionController, Method m, const AdaptiveTest &test) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method, m),
            entryWithNewline(TestSetting::targets, "a"),
            entryWithNewline(TestSetting::masker, "b"),
            entryWithNewline(TestSetting::maskerLevel, "65"),
            entryWithNewline(TestSetting::thresholdReversals, "4"),
            entryWithNewline(TestSetting::condition, Condition::audioVisual),
            entryWithNewline(TestSetting::keepVideoShown, "true")},
        5);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, test.targetsUrl.path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"b"}, test.maskerFileUrl.path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(65, test.maskerLevel.dB_SPL);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(5, test.startingSnr.dB);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(4, test.thresholdReversals);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        SessionControllerImpl::ceilingSnr.dB, test.ceilingSnr.dB);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        SessionControllerImpl::floorSnr.dB, test.floorSnr.dB);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        SessionControllerImpl::trackBumpLimit, test.trackBumpLimit);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        SessionControllerImpl::fullScaleLevel.dB_SPL,
        test.fullScaleLevel.dB_SPL);
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(true, test.keepVideoShown);
}

void assertPassesSimpleFixedLevelSettings(
    TestSettingsInterpreterImpl &interpreter, ModelStub &model,
    SessionController &sessionController, Method m,
    const FixedLevelTest &fixedLevelTest) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method, m),
            entryWithNewline(TestSetting::targets, "a"),
            entryWithNewline(TestSetting::masker, "b"),
            entryWithNewline(TestSetting::maskerLevel, "65")},
        5);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, fixedLevelTest.targetsUrl.path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"b"}, fixedLevelTest.maskerFileUrl.path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(65, fixedLevelTest.maskerLevel.dB_SPL);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(5, fixedLevelTest.snr.dB);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        SessionControllerImpl::fullScaleLevel.dB_SPL,
        fixedLevelTest.fullScaleLevel.dB_SPL);
}

void assertPassesSettingsWithExtraneousWhitespace(
    TestSettingsInterpreterImpl &interpreter, ModelStub &model,
    SessionController &sessionController, Method m,
    const FixedLevelTest &fixedLevelTest) {
    initialize(interpreter, model, sessionController,
        {withNewLine(std::string{"  "} + name(TestSetting::method) +
             std::string{" :  "} + name(m) + "  "),
            withNewLine(name(TestSetting::targets) + std::string{" :a "}),
            entryWithNewline(TestSetting::masker, "b"),
            entryWithNewline(TestSetting::maskerLevel, "65")},
        5);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, fixedLevelTest.targetsUrl.path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"b"}, fixedLevelTest.maskerFileUrl.path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(65, fixedLevelTest.maskerLevel.dB_SPL);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(5, fixedLevelTest.snr.dB);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        SessionControllerImpl::fullScaleLevel.dB_SPL,
        fixedLevelTest.fullScaleLevel.dB_SPL);
}

void initialize(TestSettingsInterpreterImpl &interpreter,
    RunningATestFacade &model, SessionController &sessionController, Method m,
    const TestIdentity &identity = {}, int startingSnr = {}) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method, m)}, startingSnr, identity);
}

void setSubjectId(TestIdentity &identity, std::string s) {
    identity.subjectId = std::move(s);
}

void setTesterId(TestIdentity &identity, std::string s) {
    identity.testerId = std::move(s);
}

void setSession(TestIdentity &identity, std::string s) {
    identity.session = std::move(s);
}

auto subjectId(const TestIdentity &identity) -> std::string {
    return identity.subjectId;
}

auto testerId(const TestIdentity &identity) -> std::string {
    return identity.testerId;
}

auto session(const TestIdentity &identity) -> std::string {
    return identity.session;
}

void assertSubjectIdEquals(const std::string &s, const TestIdentity &identity) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(s, subjectId(identity));
}

void assertTesterIdEquals(const std::string &s, const TestIdentity &identity) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(s, testerId(identity));
}

void assertSessionIdEquals(const std::string &s, const TestIdentity &identity) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(s, session(identity));
}

void assertTestMethodEquals(
    const std::string &s, const TestIdentity &identity) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(s, identity.method);
}

void assertPassesTestIdentity(TestSettingsInterpreterImpl &interpreter,
    ModelStub &model, SessionController &sessionController, Method m,
    const TestIdentity &f) {
    TestIdentity testIdentity;
    setSubjectId(testIdentity, "a");
    setTesterId(testIdentity, "b");
    setSession(testIdentity, "c");
    initialize(interpreter, model, sessionController, m, testIdentity);
    assertSubjectIdEquals("a", f);
    assertTesterIdEquals("b", f);
    assertSessionIdEquals("c", f);
}

void assertOverridesTestIdentity(TestSettingsInterpreterImpl &interpreter,
    ModelStub &model, SessionController &sessionController, Method m,
    const TestIdentity &f) {
    TestIdentity testIdentity;
    setSubjectId(testIdentity, "a");
    setTesterId(testIdentity, "b");
    setSession(testIdentity, "c");
    testIdentity.rmeSetting = "g";
    testIdentity.transducer = "h";
    testIdentity.meta = "k";
    testIdentity.relativeOutputUrl.path = "n";
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method, m),
            entryWithNewline(TestSetting::subjectId, "d"),
            entryWithNewline(TestSetting::testerId, "e"),
            entryWithNewline(TestSetting::session, "f"),
            entryWithNewline(TestSetting::rmeSetting, "i"),
            entryWithNewline(TestSetting::transducer, "j"),
            entryWithNewline(TestSetting::meta, "m"),
            entryWithNewline(TestSetting::relativeOutputPath, "p")},
        0, testIdentity);
    assertSubjectIdEquals("d", f);
    assertTesterIdEquals("e", f);
    assertSessionIdEquals("f", f);
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(std::string{"i"}, f.rmeSetting);
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(std::string{"j"}, f.transducer);
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(std::string{"m"}, f.meta);
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(std::string{"p"}, f.relativeOutputUrl.path);
}

void assertOverridesStartingSnr(TestSettingsInterpreterImpl &interpreter,
    ModelStub &model, SessionController &sessionController, Method m,
    const FixedLevelTest &f) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method, m),
            entryWithNewline(TestSetting::startingSnr, "6")},
        5);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(6, f.snr.dB);
}

void assertPassesTestMethod(TestSettingsInterpreterImpl &interpreter,
    ModelStub &model, SessionController &sessionController, Method m,
    const TestIdentity &f) {
    initialize(interpreter, model, sessionController, m);
    assertTestMethodEquals(name(m), f);
}

class FreeResponseControllerStub : public FreeResponseController {
  public:
    void initialize(bool usingPuzzle) override { usingPuzzle_ = usingPuzzle; }

    [[nodiscard]] auto usingPuzzle() const -> bool { return usingPuzzle_; }

  private:
    bool usingPuzzle_{};
};

class TestSettingsInterpreterTests : public ::testing::Test {
  protected:
    RunningATestStub runningATest;
    AdaptiveMethodStub adaptiveMethod;
    FixedLevelMethodStub fixedLevelMethod;
    RunningATest::Observer eyeTracking;
    RunningATest::Observer audioRecording;
    TargetPlaylistSetReaderStub cyclicTargetsReader;
    TargetPlaylistSetReaderStub targetsWithReplacementReader;
    FiniteTargetPlaylistWithRepeatablesStub predeterminedTargets;
    FiniteTargetPlaylistWithRepeatablesStub everyTargetOnce;
    FiniteTargetPlaylistWithRepeatablesStub silentIntervalTargets;
    RepeatableFiniteTargetPlaylistStub eachTargetNTimes;
    TargetPlaylistStub targetsWithReplacement;
    ModelStub model;
    SessionControllerStub sessionController;
    TaskPresenterStub consonantPresenter;
    TaskPresenterStub passFailPresenter;
    submitting_free_response::PuzzleStub puzzle;
    FreeResponseControllerStub freeResponseController;
    TestSettingsInterpreterImpl interpreter{
        {
            {Method::fixedLevelConsonants, consonantPresenter},
            {Method::adaptivePassFail, passFailPresenter},
        },
        runningATest, adaptiveMethod, fixedLevelMethod, eyeTracking,
        audioRecording, cyclicTargetsReader, targetsWithReplacementReader,
        predeterminedTargets, everyTargetOnce, silentIntervalTargets,
        eachTargetNTimes, targetsWithReplacement, puzzle,
        freeResponseController};
    TestIdentity testIdentity;
};

#define TEST_SETTINGS_INTERPRETER_TEST(a)                                      \
    TEST_F(TestSettingsInterpreterTests, a)

#define TEST_SETTINGS_INTREPRETER_TEST_EXPECT_INITIALIZES_FIXED_LEVEL_TEST_WITH_ALL_TARGETS( \
    interpreter, model, sessionController, method)                                           \
    initialize(interpreter, model, sessionController, method);                               \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(                                                         \
        fixedLevelMethod.targetList_, &everyTargetOnce)

TEST_SETTINGS_INTERPRETER_TEST(usesMaskerForCalibration) {
    auto calibration{interpreter.calibration(
        concatenate({entryWithNewline(TestSetting::masker, "a"),
            entryWithNewline(TestSetting::maskerLevel, "1")}))};
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, calibration.fileUrl.path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, calibration.level.dB_SPL);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        SessionControllerImpl::fullScaleLevel.dB_SPL,
        calibration.fullScaleLevel.dB_SPL);
}

TEST_SETTINGS_INTERPRETER_TEST(throwsRuntimeErrorIfMethodUnknown) {
    try {
        initialize(interpreter, model, sessionController,
            {
                entryWithNewline(
                    TestSetting::method, "this is not a real test method"),
            });
        FAIL() << "Expected std::runtime_error";
    } catch (const std::runtime_error &e) {
        AV_SPEECH_IN_NOISE_ASSERT_EQUAL(
            std::string{
                "Test method not recognized: this is not a real test method"},
            e.what());
    }
}

TEST_SETTINGS_INTERPRETER_TEST(ignoresBadLine) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            "f:\n", entryWithNewline(TestSetting::targets, "a")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, adaptiveMethod.test_.targetsUrl.path);
}

TEST_SETTINGS_INTERPRETER_TEST(ignoresBadLine2) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail), "\n",
            entryWithNewline(TestSetting::targets, "a")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, adaptiveMethod.test_.targetsUrl.path);
}

TEST_SETTINGS_INTERPRETER_TEST(ignoresBadLine3) {
    initialize(interpreter, model, sessionController,
        {"\n", entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            "\n", entryWithNewline(TestSetting::targets, "a")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, adaptiveMethod.test_.targetsUrl.path);
}

TEST_SETTINGS_INTERPRETER_TEST(badMaskerLevelResolvesToZero) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::maskerLevel, "a")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(0, adaptiveMethod.test_.maskerLevel.dB_SPL);
}

TEST_SETTINGS_INTERPRETER_TEST(meta) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "a", interpreter.meta(entryWithNewline(TestSetting::meta, "a")));
}

TEST_SETTINGS_INTERPRETER_TEST(preparesTestAfterConfirmButtonIsClicked) {
    initialize(interpreter, model, sessionController, Method::adaptivePassFail);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &passFailPresenter, sessionController.taskPresenter());
}

TEST_SETTINGS_INTERPRETER_TEST(
    doesNotPrepareTestAfterConfirmButtonIsClickedWhenTestWouldAlreadyBeComplete) {
    runningATest.testComplete_ = true;
    initialize(interpreter, model, sessionController, Method::adaptivePassFail);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(sessionController.prepareCalled());
}

TEST_SETTINGS_INTERPRETER_TEST(initializesPuzzleWithPath) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method,
             Method::
                 fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording),
            entryWithNewline(TestSetting::puzzle, "/Users/user/puzzle.png")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "/Users/user/puzzle.png", puzzle.url().path);
}

TEST_SETTINGS_INTERPRETER_TEST(initializesFreeResponseControllerWithPuzzle) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method,
             Method::
                 fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording),
            entryWithNewline(TestSetting::puzzle, "/Users/user/puzzle.png")});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(freeResponseController.usingPuzzle());
}

TEST_SETTINGS_INTERPRETER_TEST(initializesFreeResponseControllerWithoutPuzzle) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method,
            Method::
                fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording)});
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(freeResponseController.usingPuzzle());
}

TEST_SETTINGS_INTERPRETER_TEST(adaptivePassFailPassesMethod) {
    assertPassesTestMethod(interpreter, model, sessionController,
        Method::adaptivePassFail, runningATest.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptivePassFailWithEyeTrackingPassesMethod) {
    assertPassesTestMethod(interpreter, model, sessionController,
        Method::adaptivePassFailWithEyeTracking, runningATest.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelSyllablesWithAllTargetsPassesMethod) {
    assertPassesTestMethod(interpreter, model, sessionController,
        Method::fixedLevelSyllablesWithAllTargets, runningATest.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelChooseKeywordsWithAllTargetsPassesMethod) {
    assertPassesTestMethod(interpreter, model, sessionController,
        Method::fixedLevelChooseKeywordsWithAllTargets,
        runningATest.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsPassesMethod) {
    assertPassesTestMethod(interpreter, model, sessionController,
        Method::fixedLevelFreeResponseWithAllTargets,
        runningATest.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelConsonantsPassesMethod) {
    assertPassesTestMethod(interpreter, model, sessionController,
        Method::fixedLevelConsonants, runningATest.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsAndEyeTrackingPassesMethod) {
    assertPassesTestMethod(interpreter, model, sessionController,
        Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking,
        runningATest.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsAndAudioRecordingPassesMethod) {
    assertPassesTestMethod(interpreter, model, sessionController,
        Method::fixedLevelFreeResponseWithAllTargetsAndAudioRecording,
        runningATest.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsAndAudioRecordingSelectsAudioRecordingPeripheral) {
    initialize(interpreter, model, sessionController,
        Method::fixedLevelFreeResponseWithAllTargetsAndAudioRecording);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, &audioRecording);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.testMethod, &fixedLevelMethod);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList_, &everyTargetOnce);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecordingPassesMethod) {
    initialize(interpreter, model, sessionController,
        Method::
            fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        name(Method::
                fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording),
        fixedLevelMethod.fixedLevelTest.identity.method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        name(Method::
                fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording),
        runningATest.test.identity.method);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecordingSelectsAudioRecordingPeripheral) {
    initialize(interpreter, model, sessionController,
        Method::
            fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, &audioRecording);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.testMethod, &fixedLevelMethod);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList_, &predeterminedTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTrackingPassesMethod) {
    initialize(interpreter, model, sessionController,
        Method::fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        name(Method::
                fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking),
        fixedLevelMethod.fixedLevelTest.identity.method);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        name(Method::
                fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking),
        runningATest.test.identity.method);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptivePassFailOverridesTestIdentity) {
    assertOverridesTestIdentity(interpreter, model, sessionController,
        Method::adaptivePassFail, adaptiveMethod.test_.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsOverridesTestIdentity) {
    assertOverridesTestIdentity(interpreter, model, sessionController,
        Method::fixedLevelFreeResponseWithAllTargets,
        runningATest.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelChooseKeywordsWithAllTargetsOverridesTestIdentity) {
    assertOverridesTestIdentity(interpreter, model, sessionController,
        Method::fixedLevelChooseKeywordsWithAllTargets,
        runningATest.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptivePassFailPassesTestIdentity) {
    assertPassesTestIdentity(interpreter, model, sessionController,
        Method::adaptivePassFail, adaptiveMethod.test_.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptivePassFailWithEyeTrackingPassesTestIdentity) {
    assertPassesTestIdentity(interpreter, model, sessionController,
        Method::adaptivePassFailWithEyeTracking, adaptiveMethod.test_.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsPassesTestIdentity) {
    assertPassesTestIdentity(interpreter, model, sessionController,
        Method::fixedLevelFreeResponseWithAllTargets,
        runningATest.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelChooseKeywordsWithAllTargetsPassesTestIdentity) {
    assertPassesTestIdentity(interpreter, model, sessionController,
        Method::fixedLevelChooseKeywordsWithAllTargets,
        runningATest.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsAndEyeTrackingPassesTestIdentity) {
    assertPassesTestIdentity(interpreter, model, sessionController,
        Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking,
        runningATest.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptivePassFailInitializesAdaptiveTest) {
    initialize(interpreter, model, sessionController, Method::adaptivePassFail);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.testMethod, &adaptiveMethod);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptivePassFailWithEyeTrackingInitializesAdaptiveTest) {
    initialize(interpreter, model, sessionController,
        Method::adaptivePassFailWithEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.testMethod, &adaptiveMethod);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, &eyeTracking);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptiveCorrectKeywordsInitializesAdaptiveTest) {
    initialize(
        interpreter, model, sessionController, Method::adaptiveCorrectKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        adaptiveMethod.targetListReader_, &cyclicTargetsReader);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.testMethod, &adaptiveMethod);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCorrectKeywordsWithEyeTrackingInitializesAdaptiveTest) {
    initialize(interpreter, model, sessionController,
        Method::adaptiveCorrectKeywordsWithEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        adaptiveMethod.targetListReader_, &cyclicTargetsReader);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.testMethod, &adaptiveMethod);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, &eyeTracking);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureInitializesAdaptiveTest) {
    initialize(interpreter, model, sessionController,
        Method::adaptiveCoordinateResponseMeasure);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.testMethod, &adaptiveMethod);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithDelayedMaskerInitializesAdaptiveTest) {
    initialize(interpreter, model, sessionController,
        Method::adaptiveCoordinateResponseMeasureWithDelayedMasker);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(AudioChannelOption::delayedMasker,
        runningATest.test.audioChannelOption);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithSingleSpeakerInitializesAdaptiveTest) {
    initialize(interpreter, model, sessionController,
        Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(AudioChannelOption::singleSpeaker,
        runningATest.test.audioChannelOption);
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelConsonantsInitializesFixedLevelTest) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method, Method::fixedLevelConsonants),
            entryWithNewline(TestSetting::targetRepetitions, "5")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(4, eachTargetNTimes.repeats());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList_, &eachTargetNTimes);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.testMethod, &fixedLevelMethod);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithEyeTrackingInitializesAdaptiveTest) {
    initialize(interpreter, model, sessionController,
        Method::adaptiveCoordinateResponseMeasureWithEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, &eyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.testMethod, &adaptiveMethod);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithSilentIntervalTargetsInitializesFixedLevelTest) {
    initialize(interpreter, model, sessionController,
        Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList_, &silentIntervalTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.testMethod, &fixedLevelMethod);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithTargetReplacementInitializesFixedLevelTest) {
    initialize(interpreter, model, sessionController,
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList_, &targetsWithReplacement);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.testMethod, &fixedLevelMethod);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTrackingInitializesFixedLevelTest) {
    initialize(interpreter, model, sessionController,
        Method::
            fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, &eyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList_, &targetsWithReplacement);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.testMethod, &fixedLevelMethod);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsInitializesFixedLevelTest) {
    TEST_SETTINGS_INTREPRETER_TEST_EXPECT_INITIALIZES_FIXED_LEVEL_TEST_WITH_ALL_TARGETS(
        interpreter, model, sessionController,
        Method::fixedLevelFreeResponseWithAllTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelChooseKeywordsWithAllTargetsInitializesFixedLevelTest) {
    TEST_SETTINGS_INTREPRETER_TEST_EXPECT_INITIALIZES_FIXED_LEVEL_TEST_WITH_ALL_TARGETS(
        interpreter, model, sessionController,
        Method::fixedLevelChooseKeywordsWithAllTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelSyllablesWithAllTargetsInitializesFixedLevelTest) {
    TEST_SETTINGS_INTREPRETER_TEST_EXPECT_INITIALIZES_FIXED_LEVEL_TEST_WITH_ALL_TARGETS(
        interpreter, model, sessionController,
        Method::fixedLevelSyllablesWithAllTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTrackingInitializesFixedLevelTest) {
    initialize(interpreter, model, sessionController,
        Method::fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, &eyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList_, &predeterminedTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.testMethod, &fixedLevelMethod);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsAndEyeTrackingInitializesFixedLevelTest) {
    initialize(interpreter, model, sessionController,
        Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, &eyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList_, &everyTargetOnce);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.testMethod, &fixedLevelMethod);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithSilentIntervalTargetsInitializesFixedLevelTest) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method,
             Method::fixedLevelFreeResponseWithSilentIntervalTargets),
            entryWithNewline(TestSetting::subjectId, "abc")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "abc", runningATest.test.identity.subjectId);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithTargetReplacementInitializesFixedLevelTest) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method,
             Method::fixedLevelFreeResponseWithTargetReplacement),
            entryWithNewline(TestSetting::maskerLevel, "5")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(5, runningATest.test.maskerLevel.dB_SPL);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptivePassFailOverridesStartingSnr) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::startingSnr, "6")},
        5);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(6, adaptiveMethod.test_.startingSnr.dB);
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelConsonantsOverridesStartingSnr) {
    assertOverridesStartingSnr(interpreter, model, sessionController,
        Method::fixedLevelConsonants, fixedLevelMethod.fixedLevelTest);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithTargetReplacementOverridesStartingSnr) {
    assertOverridesStartingSnr(interpreter, model, sessionController,
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement,
        fixedLevelMethod.fixedLevelFixedTrialsTest);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptivePassFailPassesSimpleAdaptiveSettings) {
    assertPassesSimpleAdaptiveSettings(interpreter, model, sessionController,
        Method::adaptivePassFail, adaptiveMethod.test_);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptivePassFailWithEyeTrackingPassesSimpleAdaptiveSettings) {
    assertPassesSimpleAdaptiveSettings(interpreter, model, sessionController,
        Method::adaptivePassFailWithEyeTracking, adaptiveMethod.test_);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCorrectKeywordsPassesSimpleAdaptiveSettings) {
    assertPassesSimpleAdaptiveSettings(interpreter, model, sessionController,
        Method::adaptiveCorrectKeywords, adaptiveMethod.test_);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCorrectKeywordsWithEyeTrackingPassesSimpleAdaptiveSettings) {
    assertPassesSimpleAdaptiveSettings(interpreter, model, sessionController,
        Method::adaptiveCorrectKeywordsWithEyeTracking, adaptiveMethod.test_);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithDelayedMaskerPassesSimpleAdaptiveSettings) {
    assertPassesSimpleAdaptiveSettings(interpreter, model, sessionController,
        Method::adaptiveCoordinateResponseMeasureWithDelayedMasker,
        adaptiveMethod.test_);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithSingleSpeakerPassesSimpleAdaptiveSettings) {
    assertPassesSimpleAdaptiveSettings(interpreter, model, sessionController,
        Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker,
        adaptiveMethod.test_);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithEyeTrackingPassesSimpleAdaptiveSettings) {
    assertPassesSimpleAdaptiveSettings(interpreter, model, sessionController,
        Method::adaptiveCoordinateResponseMeasureWithEyeTracking,
        adaptiveMethod.test_);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasurePassesSimpleAdaptiveSettings) {
    assertPassesSimpleAdaptiveSettings(interpreter, model, sessionController,
        Method::adaptiveCoordinateResponseMeasure, adaptiveMethod.test_);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptiveAudioVisual) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::condition, Condition::audioVisual)});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Condition::audioVisual, adaptiveMethod.test_.condition);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptiveAuditoryOnly) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::condition, Condition::auditoryOnly)});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Condition::auditoryOnly, adaptiveMethod.test_.condition);
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelAudioVisual) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(
             TestSetting::method, Method::fixedLevelFreeResponseWithAllTargets),
            entryWithNewline(TestSetting::condition, Condition::audioVisual)});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Condition::audioVisual, runningATest.test.condition);
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelFixedTargetsAudioVisual) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method,
             Method::fixedLevelFreeResponseWithTargetReplacement),
            entryWithNewline(TestSetting::condition, Condition::audioVisual)});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Condition::audioVisual, runningATest.test.condition);
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelAuditoryOnly) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method,
             Method::fixedLevelFreeResponseWithTargetReplacement),
            entryWithNewline(TestSetting::condition, Condition::auditoryOnly)});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Condition::auditoryOnly, fixedLevelTest(model).condition);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsPassesSettingsWithExtraneousWhitespace) {
    assertPassesSettingsWithExtraneousWhitespace(interpreter, model,
        sessionController, Method::fixedLevelFreeResponseWithAllTargets,
        fixedLevelMethod.fixedLevelTest);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsPassesSimpleFixedLevelSettings) {
    assertPassesSimpleFixedLevelSettings(interpreter, model, sessionController,
        Method::fixedLevelFreeResponseWithAllTargets,
        fixedLevelMethod.fixedLevelTest);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelChooseKeywordsWithAllTargetsPassesSimpleFixedLevelSettings) {
    assertPassesSimpleFixedLevelSettings(interpreter, model, sessionController,
        Method::fixedLevelChooseKeywordsWithAllTargets,
        fixedLevelMethod.fixedLevelTest);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelConsonantsPassesSimpleFixedLevelSettings) {
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method, Method::fixedLevelConsonants),
            entryWithNewline(TestSetting::targets, "a"),
            entryWithNewline(TestSetting::masker, "b"),
            entryWithNewline(TestSetting::maskerLevel, "65")},
        5);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, runningATest.test.targetsUrl.path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"b"}, runningATest.test.maskerFileUrl.path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(65, runningATest.test.maskerLevel.dB_SPL);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(5, fixedLevelMethod.fixedLevelTest.snr.dB);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        SessionControllerImpl::fullScaleLevel.dB_SPL,
        runningATest.test.fullScaleLevel.dB_SPL);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsAndEyeTrackingPassesSimpleFixedLevelSettings) {
    assertPassesSimpleFixedLevelSettings(interpreter, model, sessionController,
        Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking,
        fixedLevelMethod.fixedLevelTest);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithSilentIntervalTargetsPassesSimpleFixedLevelSettings) {
    assertPassesSimpleFixedLevelSettings(interpreter, model, sessionController,
        Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets,
        fixedLevelMethod.fixedLevelTest);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithTargetReplacementPassesSimpleFixedLevelSettings) {
    assertPassesSimpleFixedLevelSettings(interpreter, model, sessionController,
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement,
        fixedLevelMethod.fixedLevelFixedTrialsTest);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTrackingPassesSimpleFixedLevelSettings) {
    assertPassesSimpleFixedLevelSettings(interpreter, model, sessionController,
        Method::
            fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking,
        fixedLevelMethod.fixedLevelFixedTrialsTest);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithSilentIntervalTargetsPassesSimpleFixedLevelSettings) {
    assertPassesSimpleFixedLevelSettings(interpreter, model, sessionController,
        Method::fixedLevelFreeResponseWithSilentIntervalTargets,
        fixedLevelMethod.fixedLevelTest);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithTargetReplacementPassesSimpleFixedLevelSettings) {
    assertPassesSimpleFixedLevelSettings(interpreter, model, sessionController,
        Method::fixedLevelFreeResponseWithTargetReplacement,
        fixedLevelMethod.fixedLevelFixedTrialsTest);
}

TEST_SETTINGS_INTERPRETER_TEST(oneSequence) {
    TrackingSequence sequence{};
    sequence.up = 1;
    sequence.down = 2;
    sequence.runCount = 3;
    sequence.stepSize = 4;
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::up, "1"),
            entryWithNewline(TestSetting::down, "2"),
            entryWithNewline(TestSetting::reversalsPerStepSize, "3"),
            entryWithNewline(TestSetting::stepSizes, "4")});
    assertEqual({sequence}, adaptiveMethod.test_.trackingRule);
}

TEST_SETTINGS_INTERPRETER_TEST(twoSequences) {
    TrackingSequence first{};
    first.up = 1;
    first.down = 3;
    first.runCount = 5;
    first.stepSize = 7;
    TrackingSequence second{};
    second.up = 2;
    second.down = 4;
    second.runCount = 6;
    second.stepSize = 8;
    initialize(interpreter, model, sessionController,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::up, "1 2"),
            entryWithNewline(TestSetting::down, "3 4"),
            entryWithNewline(TestSetting::reversalsPerStepSize, "5 6"),
            entryWithNewline(TestSetting::stepSizes, "7 8")});
    assertEqual({first, second}, adaptiveMethod.test_.trackingRule);
}

TEST_SETTINGS_INTERPRETER_TEST(consonantTestWithTargetRepetitions) {
    initialize(interpreter, model, sessionController,
        {"\n",
            entryWithNewline(TestSetting::method, Method::fixedLevelConsonants),
            "\n", entryWithNewline(TestSetting::targetRepetitions, "2")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, eachTargetNTimes.repeats());
}

TEST_SETTINGS_INTERPRETER_TEST(
    consonantTestWithTargetRepetitionsDefaultsToOne) {
    initialize(interpreter, model, sessionController,
        {"\n",
            entryWithNewline(
                TestSetting::method, Method::fixedLevelConsonants)});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        1, fixedLevelTestWithEachTargetNTimes(model).timesEachTargetIsPlayed);
}
}
}
