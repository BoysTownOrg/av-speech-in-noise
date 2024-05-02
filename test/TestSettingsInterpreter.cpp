#include "AdaptiveMethodStub.hpp"
#include "FixedLevelMethodStub.hpp"
#include "RunningATestStub.hpp"
#include "TargetPlaylistSetReaderStub.hpp"
#include "TargetPlaylistStub.hpp"
#include "assert-utility.hpp"
#include "PuzzleStub.hpp"

#include <av-speech-in-noise/ui/TestSettingsInterpreter.hpp>
#include <av-speech-in-noise/ui/SessionController.hpp>

#include <gtest/gtest.h>

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
    void complete() override {}
    void notifyThatTrialHasStarted() override {}
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

void initializeTest(TestSettingsInterpreterImpl &interpreter,
    const std::vector<std::string> &v, int startingSnr = {},
    const TestIdentity &identity = {}) {
    interpreter.initializeTest(concatenate(v), identity, SNR{startingSnr});
}

void assertPassesSettingsWithExtraneousWhitespace(
    TestSettingsInterpreterImpl &interpreter, Method m,
    const FixedLevelTest &fixedLevelTest) {
    initializeTest(interpreter,
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

void initializeTest(TestSettingsInterpreterImpl &interpreter, Method m,
    const TestIdentity &identity = {}, int startingSnr = {}) {
    initializeTest(interpreter, {entryWithNewline(TestSetting::method, m)},
        startingSnr, identity);
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

void assertPassesTestIdentity(
    TestSettingsInterpreterImpl &interpreter, Method m, const TestIdentity &f) {
    TestIdentity testIdentity;
    setSubjectId(testIdentity, "a");
    setTesterId(testIdentity, "b");
    setSession(testIdentity, "c");
    initializeTest(interpreter, m, testIdentity);
    assertSubjectIdEquals("a", f);
    assertTesterIdEquals("b", f);
    assertSessionIdEquals("c", f);
}

void assertOverridesTestIdentity(
    TestSettingsInterpreterImpl &interpreter, Method m, const TestIdentity &f) {
    TestIdentity testIdentity;
    setSubjectId(testIdentity, "a");
    setTesterId(testIdentity, "b");
    setSession(testIdentity, "c");
    testIdentity.rmeSetting = "g";
    testIdentity.transducer = "h";
    testIdentity.meta = "k";
    testIdentity.relativeOutputUrl.path = "n";
    initializeTest(interpreter,
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
    Method m, const FixedLevelTest &f) {
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method, m),
            entryWithNewline(TestSetting::startingSnr, "6")},
        5);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(6, f.snr.dB);
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
    RunningATest::TestObserver eyeTracking;
    RunningATest::TestObserver audioRecording;
    TargetPlaylistSetReaderStub cyclicTargetsReader;
    TargetPlaylistSetReaderStub targetsWithReplacementReader;
    FiniteTargetPlaylistWithRepeatablesStub predeterminedTargets;
    FiniteTargetPlaylistWithRepeatablesStub everyTargetOnce;
    FiniteTargetPlaylistWithRepeatablesStub silentIntervalTargets;
    RepeatableFiniteTargetPlaylistStub eachTargetNTimes;
    TargetPlaylistStub targetsWithReplacement;
    SessionControllerStub sessionController;
    submitting_free_response::PuzzleStub puzzle;
    FreeResponseControllerStub freeResponseController;
    TaskPresenterStub coordinateResponseMeasurePresenter;
    TaskPresenterStub freeResponsePresenter;
    TaskPresenterStub chooseKeywordsPresenter;
    TaskPresenterStub syllablesPresenter;
    TaskPresenterStub correctKeywordsPresenter;
    TaskPresenterStub consonantPresenter;
    TaskPresenterStub passFailPresenter;
    TestSettingsInterpreterImpl interpreter{runningATest, adaptiveMethod,
        fixedLevelMethod, eyeTracking, audioRecording, cyclicTargetsReader,
        targetsWithReplacementReader, predeterminedTargets, everyTargetOnce,
        silentIntervalTargets, eachTargetNTimes, targetsWithReplacement, puzzle,
        freeResponseController, sessionController,
        coordinateResponseMeasurePresenter, freeResponsePresenter,
        chooseKeywordsPresenter, syllablesPresenter, correctKeywordsPresenter,
        consonantPresenter, passFailPresenter};
    TestIdentity testIdentity;
};

#define TEST_SETTINGS_INTERPRETER_TEST(a)                                      \
    TEST_F(TestSettingsInterpreterTests, a)

#define TEST_SETTINGS_INTREPRETER_TEST_EXPECT_INITIALIZES_FIXED_LEVEL_TEST_WITH_ALL_TARGETS( \
    m)                                                                                       \
    initializeTest(interpreter, m);                                                          \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(                                                         \
        fixedLevelMethod.targetList, &everyTargetOnce)

#define ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_METHOD(m)                       \
    initializeTest(interpreter, m);                                            \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.testMethod, &adaptiveMethod);

#define ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(m)                    \
    initializeTest(interpreter, m);                                            \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.testMethod, &fixedLevelMethod);

#define AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_SETTINGS(m)  \
    initializeTest(interpreter,                                                \
        {entryWithNewline(TestSetting::method, m),                             \
            entryWithNewline(TestSetting::targets, "a"),                       \
            entryWithNewline(TestSetting::masker, "b"),                        \
            entryWithNewline(TestSetting::maskerLevel, "65"),                  \
            entryWithNewline(TestSetting::thresholdReversals, "4"),            \
            entryWithNewline(TestSetting::condition, Condition::audioVisual),  \
            entryWithNewline(TestSetting::videoScaleNumerator, "7"),           \
            entryWithNewline(TestSetting::videoScaleDenominator, "9"),         \
            entryWithNewline(TestSetting::keepVideoShown, "true")},            \
        5);                                                                    \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(                                           \
        std::string{"a"}, adaptiveMethod.test.targetsUrl.path);                \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(                                           \
        std::string{"b"}, adaptiveMethod.test.maskerFileUrl.path);             \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(                                           \
        65, adaptiveMethod.test.maskerLevel.dB_SPL);                           \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(5, adaptiveMethod.test.startingSnr.dB);    \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(                                           \
        4, adaptiveMethod.test.thresholdReversals);                            \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(SessionControllerImpl::ceilingSnr.dB,      \
        adaptiveMethod.test.ceilingSnr.dB);                                    \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(                                           \
        SessionControllerImpl::floorSnr.dB, adaptiveMethod.test.floorSnr.dB);  \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(SessionControllerImpl::trackBumpLimit,     \
        adaptiveMethod.test.trackBumpLimit);                                   \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(                                           \
        SessionControllerImpl::fullScaleLevel.dB_SPL,                          \
        adaptiveMethod.test.fullScaleLevel.dB_SPL);                            \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(                                           \
        7, adaptiveMethod.test.videoScale.numerator);                          \
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(                                           \
        9, adaptiveMethod.test.videoScale.denominator);                        \
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(true, adaptiveMethod.test.keepVideoShown)

#define AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_SETTINGS( \
    m, test)                                                                   \
    initializeTest(interpreter,                                                \
        {entryWithNewline(TestSetting::method, m),                             \
            entryWithNewline(TestSetting::targets, "a"),                       \
            entryWithNewline(TestSetting::masker, "b"),                        \
            entryWithNewline(TestSetting::maskerLevel, "65")},                 \
        5);                                                                    \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, (test).targetsUrl.path); \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(                                           \
        std::string{"b"}, (test).maskerFileUrl.path);                          \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(65, (test).maskerLevel.dB_SPL);            \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(5, (test).snr.dB);                         \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(                                           \
        SessionControllerImpl::fullScaleLevel.dB_SPL,                          \
        (test).fullScaleLevel.dB_SPL)

#define AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(m)        \
    initializeTest(interpreter, m);                                            \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(name(m), runningATest.test.identity.method)

TEST_SETTINGS_INTERPRETER_TEST(initializeTestPasses_adaptivePassFail_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::adaptivePassFail);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_adaptivePassFailWithEyeTracking_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::adaptivePassFailWithEyeTracking);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_adaptiveCorrectKeywords_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::adaptiveCorrectKeywords);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_adaptiveCorrectKeywordsWithEyeTracking_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::adaptiveCorrectKeywordsWithEyeTracking);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_adaptiveCoordinateResponseMeasure_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::adaptiveCoordinateResponseMeasure);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_adaptiveCoordinateResponseMeasureWithSingleSpeaker_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_adaptiveCoordinateResponseMeasureWithDelayedMasker_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::adaptiveCoordinateResponseMeasureWithDelayedMasker);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_adaptiveCoordinateResponseMeasureWithEyeTracking_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::adaptiveCoordinateResponseMeasureWithEyeTracking);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_fixedLevelFreeResponseWithTargetReplacement_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::fixedLevelFreeResponseWithTargetReplacement);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_fixedLevelFreeResponseWithSilentIntervalTargets_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::fixedLevelFreeResponseWithSilentIntervalTargets);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_fixedLevelFreeResponseWithAllTargets_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::fixedLevelFreeResponseWithAllTargets);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_fixedLevelFreeResponseWithAllTargetsAndEyeTracking_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_fixedLevelFreeResponseWithAllTargetsAndAudioRecording_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::fixedLevelFreeResponseWithAllTargetsAndAudioRecording);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(Method::
            fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_fixedLevelFreeResponseWithPredeterminedTargets_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::fixedLevelFreeResponseWithPredeterminedTargets);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_fixedLevelCoordinateResponseMeasureWithTargetReplacement_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(Method::
            fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_fixedLevelConsonants_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::fixedLevelConsonants);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_fixedLevelChooseKeywordsWithAllTargets_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::fixedLevelChooseKeywordsWithAllTargets);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestPasses_fixedLevelSyllablesWithAllTargets_Method) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_TEST_METHOD(
        Method::fixedLevelSyllablesWithAllTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptivePassFail_PassesAdaptiveSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_SETTINGS(
        Method::adaptivePassFail);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptivePassFailWithEyeTracking_PassesAdaptiveSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_SETTINGS(
        Method::adaptivePassFailWithEyeTracking);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCorrectKeywords_PassesAdaptiveSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_SETTINGS(
        Method::adaptiveCorrectKeywords);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCorrectKeywordsWithEyeTracking_PassesAdaptiveSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_SETTINGS(
        Method::adaptiveCorrectKeywordsWithEyeTracking);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCoordinateResponseMeasure_PassesAdaptiveSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_SETTINGS(
        Method::adaptiveCoordinateResponseMeasure);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCoordinateResponseMeasureWithSingleSpeaker_PassesAdaptiveSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_SETTINGS(
        Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCoordinateResponseMeasureWithDelayedMasker_PassesAdaptiveSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_SETTINGS(
        Method::adaptiveCoordinateResponseMeasureWithDelayedMasker);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCoordinateResponseMeasureWithEyeTracking_PassesAdaptiveSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_SETTINGS(
        Method::adaptiveCoordinateResponseMeasureWithEyeTracking);
}

TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithSilentIntervalTargets_PassesFixedLevelSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_SETTINGS(
        Method::fixedLevelFreeResponseWithSilentIntervalTargets,
        fixedLevelMethod.test);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithAllTargets_PassesFixedLevelSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_SETTINGS(
        Method::fixedLevelFreeResponseWithAllTargets, fixedLevelMethod.test);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithAllTargetsAndEyeTracking_PassesFixedLevelSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_SETTINGS(
        Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking,
        fixedLevelMethod.test);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithAllTargetsAndAudioRecording_PassesFixedLevelSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_SETTINGS(
        Method::fixedLevelFreeResponseWithAllTargetsAndAudioRecording,
        fixedLevelMethod.test);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording_PassesFixedLevelSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_SETTINGS(
        Method::fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording,
        fixedLevelMethod.test);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking_PassesFixedLevelSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_SETTINGS(
        Method::fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking,
        fixedLevelMethod.test);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithPredeterminedTargets_PassesFixedLevelSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_SETTINGS(
        Method::fixedLevelFreeResponseWithPredeterminedTargets,
        fixedLevelMethod.test);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets_PassesFixedLevelSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_SETTINGS(
        Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets,
        fixedLevelMethod.test);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelConsonants_PassesFixedLevelSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_SETTINGS(
        Method::fixedLevelConsonants, fixedLevelMethod.test);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelChooseKeywordsWithAllTargets_PassesFixedLevelSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_SETTINGS(
        Method::fixedLevelChooseKeywordsWithAllTargets, fixedLevelMethod.test);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelSyllablesWithAllTargets_PassesFixedLevelSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_SETTINGS(
        Method::fixedLevelSyllablesWithAllTargets, fixedLevelMethod.test);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithTargetReplacement_PassesFixedLevelSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_SETTINGS(
        Method::fixedLevelFreeResponseWithTargetReplacement,
        fixedLevelMethod.fixedLevelFixedTrialsTest);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelCoordinateResponseMeasureWithTargetReplacement_PassesFixedLevelSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_SETTINGS(
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement,
        fixedLevelMethod.fixedLevelFixedTrialsTest);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking_PassesFixedLevelSettings) {
    AV_SPEECH_IN_NOISE_ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_SETTINGS(
        Method::
            fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking,
        fixedLevelMethod.fixedLevelFixedTrialsTest);
}

TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptivePassFail_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_METHOD(Method::adaptivePassFail);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptivePassFailWithEyeTracking_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_METHOD(
        Method::adaptivePassFailWithEyeTracking);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCorrectKeywords_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_METHOD(
        Method::adaptiveCorrectKeywords);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCorrectKeywordsWithEyeTracking_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_METHOD(
        Method::adaptiveCorrectKeywordsWithEyeTracking);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCoordinateResponseMeasure_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_METHOD(
        Method::adaptiveCoordinateResponseMeasure);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCoordinateResponseMeasureWithSingleSpeaker_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_METHOD(
        Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCoordinateResponseMeasureWithDelayedMasker_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_METHOD(
        Method::adaptiveCoordinateResponseMeasureWithDelayedMasker);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCoordinateResponseMeasureWithEyeTracking_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_METHOD(
        Method::adaptiveCoordinateResponseMeasureWithEyeTracking);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithTargetReplacement_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelFreeResponseWithTargetReplacement);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithSilentIntervalTargets_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelFreeResponseWithSilentIntervalTargets);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithAllTargets_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelFreeResponseWithAllTargets);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithAllTargetsAndEyeTracking_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithAllTargetsAndAudioRecording_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelFreeResponseWithAllTargetsAndAudioRecording);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(Method::
            fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithPredeterminedTargets_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelFreeResponseWithPredeterminedTargets);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelCoordinateResponseMeasureWithTargetReplacement_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(Method::
            fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelConsonants_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelConsonants);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelChooseKeywordsWithAllTargets_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelChooseKeywordsWithAllTargets);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelSyllablesWithAllTargets_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelSyllablesWithAllTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptivePassFail_PassesTaskPresenter) {
    initializeTest(interpreter, Method::adaptivePassFail);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &passFailPresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptivePassFailWithEyeTracking_PassesTaskPresenter) {
    initializeTest(interpreter, Method::adaptivePassFailWithEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &passFailPresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCorrectKeywords_PassesTaskPresenter) {
    initializeTest(interpreter, Method::adaptiveCorrectKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &correctKeywordsPresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCorrectKeywordsWithEyeTracking_PassesTaskPresenter) {
    initializeTest(interpreter, Method::adaptiveCorrectKeywordsWithEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &correctKeywordsPresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCoordinateResponseMeasure_PassesTaskPresenter) {
    initializeTest(interpreter, Method::adaptiveCoordinateResponseMeasure);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &coordinateResponseMeasurePresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCoordinateResponseMeasureWithSingleSpeaker_PassesTaskPresenter) {
    initializeTest(interpreter,
        Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &coordinateResponseMeasurePresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCoordinateResponseMeasureWithDelayedMasker_PassesTaskPresenter) {
    initializeTest(interpreter,
        Method::adaptiveCoordinateResponseMeasureWithDelayedMasker);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &coordinateResponseMeasurePresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCoordinateResponseMeasureWithEyeTracking_PassesTaskPresenter) {
    initializeTest(
        interpreter, Method::adaptiveCoordinateResponseMeasureWithEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &coordinateResponseMeasurePresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithTargetReplacement_PassesTaskPresenter) {
    initializeTest(
        interpreter, Method::fixedLevelFreeResponseWithTargetReplacement);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &freeResponsePresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithSilentIntervalTargets_PassesTaskPresenter) {
    initializeTest(
        interpreter, Method::fixedLevelFreeResponseWithSilentIntervalTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &freeResponsePresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithAllTargets_PassesTaskPresenter) {
    initializeTest(interpreter, Method::fixedLevelFreeResponseWithAllTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &freeResponsePresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithAllTargetsAndEyeTracking_PassesTaskPresenter) {
    initializeTest(interpreter,
        Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &freeResponsePresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithAllTargetsAndAudioRecording_PassesTaskPresenter) {
    initializeTest(interpreter,
        Method::fixedLevelFreeResponseWithAllTargetsAndAudioRecording);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &freeResponsePresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording_PassesTaskPresenter) {
    initializeTest(interpreter,
        Method::
            fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &freeResponsePresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking_PassesTaskPresenter) {
    initializeTest(interpreter,
        Method::fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &freeResponsePresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithPredeterminedTargets_PassesTaskPresenter) {
    initializeTest(
        interpreter, Method::fixedLevelFreeResponseWithPredeterminedTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &freeResponsePresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelCoordinateResponseMeasureWithTargetReplacement_PassesTaskPresenter) {
    initializeTest(interpreter,
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &coordinateResponseMeasurePresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking_PassesTaskPresenter) {
    initializeTest(interpreter,
        Method::
            fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &coordinateResponseMeasurePresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets_PassesTaskPresenter) {
    initializeTest(interpreter,
        Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &coordinateResponseMeasurePresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelConsonants_PassesTaskPresenter) {
    initializeTest(interpreter, Method::fixedLevelConsonants);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &consonantPresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelChooseKeywordsWithAllTargets_PassesTaskPresenter) {
    initializeTest(interpreter, Method::fixedLevelChooseKeywordsWithAllTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &chooseKeywordsPresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelSyllablesWithAllTargets_PassesTaskPresenter) {
    initializeTest(interpreter, Method::fixedLevelSyllablesWithAllTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &syllablesPresenter, sessionController.taskPresenter());
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsInitializesFixedLevelTest) {
    TEST_SETTINGS_INTREPRETER_TEST_EXPECT_INITIALIZES_FIXED_LEVEL_TEST_WITH_ALL_TARGETS(
        Method::fixedLevelFreeResponseWithAllTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelChooseKeywordsWithAllTargetsInitializesFixedLevelTest) {
    TEST_SETTINGS_INTREPRETER_TEST_EXPECT_INITIALIZES_FIXED_LEVEL_TEST_WITH_ALL_TARGETS(
        Method::fixedLevelChooseKeywordsWithAllTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelSyllablesWithAllTargetsInitializesFixedLevelTest) {
    TEST_SETTINGS_INTREPRETER_TEST_EXPECT_INITIALIZES_FIXED_LEVEL_TEST_WITH_ALL_TARGETS(
        Method::fixedLevelSyllablesWithAllTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsAndAudioRecordingSelectsAudioRecordingPeripheral) {
    initializeTest(interpreter,
        Method::fixedLevelFreeResponseWithAllTargetsAndAudioRecording);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &everyTargetOnce);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, &audioRecording);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecordingSelectsAudioRecordingPeripheral) {
    initializeTest(interpreter,
        Method::
            fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &predeterminedTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, &audioRecording);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptivePassFailInitializesAdaptiveTest) {
    initializeTest(interpreter, Method::adaptivePassFail);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        adaptiveMethod.targetListReader, &targetsWithReplacementReader);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, nullptr);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptivePassFailWithEyeTrackingInitializesAdaptiveTest) {
    initializeTest(interpreter, Method::adaptivePassFailWithEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        adaptiveMethod.targetListReader, &targetsWithReplacementReader);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, &eyeTracking);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptiveCorrectKeywordsInitializesAdaptiveTest) {
    initializeTest(interpreter, Method::adaptiveCorrectKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        adaptiveMethod.targetListReader, &cyclicTargetsReader);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, nullptr);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCorrectKeywordsWithEyeTrackingInitializesAdaptiveTest) {
    initializeTest(interpreter, Method::adaptiveCorrectKeywordsWithEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        adaptiveMethod.targetListReader, &cyclicTargetsReader);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, &eyeTracking);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureInitializesAdaptiveTest) {
    initializeTest(interpreter, Method::adaptiveCoordinateResponseMeasure);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        adaptiveMethod.targetListReader, &targetsWithReplacementReader);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, nullptr);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithEyeTrackingInitializesAdaptiveTest) {
    initializeTest(
        interpreter, Method::adaptiveCoordinateResponseMeasureWithEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        adaptiveMethod.targetListReader, &targetsWithReplacementReader);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, &eyeTracking);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithSilentIntervalTargetsInitializesFixedLevelTest) {
    initializeTest(interpreter,
        Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &silentIntervalTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, nullptr);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithTargetReplacementInitializesFixedLevelTest) {
    initializeTest(interpreter,
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &targetsWithReplacement);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, nullptr);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTrackingInitializesFixedLevelTest) {
    initializeTest(interpreter,
        Method::
            fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &targetsWithReplacement);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, &eyeTracking);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTrackingInitializesFixedLevelTest) {
    initializeTest(interpreter,
        Method::fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &predeterminedTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, &eyeTracking);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithPredeterminedTargetsInitializesFixedLevelTest) {
    initializeTest(
        interpreter, Method::fixedLevelFreeResponseWithPredeterminedTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &predeterminedTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, nullptr);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsAndEyeTrackingInitializesFixedLevelTest) {
    initializeTest(interpreter,
        Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &everyTargetOnce);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.observer, &eyeTracking);
}

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
        initializeTest(interpreter,
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
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            "f:\n", entryWithNewline(TestSetting::targets, "a")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, adaptiveMethod.test.targetsUrl.path);
}

TEST_SETTINGS_INTERPRETER_TEST(ignoresBadLine2) {
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail), "\n",
            entryWithNewline(TestSetting::targets, "a")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, adaptiveMethod.test.targetsUrl.path);
}

TEST_SETTINGS_INTERPRETER_TEST(ignoresBadLine3) {
    initializeTest(interpreter,
        {"\n", entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            "\n", entryWithNewline(TestSetting::targets, "a")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, adaptiveMethod.test.targetsUrl.path);
}

TEST_SETTINGS_INTERPRETER_TEST(badMaskerLevelResolvesToZero) {
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::maskerLevel, "a")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(0, adaptiveMethod.test.maskerLevel.dB_SPL);
}

TEST_SETTINGS_INTERPRETER_TEST(meta) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "a", interpreter.meta(entryWithNewline(TestSetting::meta, "a")));
}

TEST_SETTINGS_INTERPRETER_TEST(preparesTestAfterConfirmButtonIsClicked) {
    runningATest.testComplete_ = false;
    initializeTest(interpreter, Method::adaptivePassFail);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &passFailPresenter, sessionController.taskPresenter());
}

TEST_SETTINGS_INTERPRETER_TEST(
    doesNotPrepareTestAfterConfirmButtonIsClickedWhenTestWouldAlreadyBeComplete) {
    runningATest.testComplete_ = true;
    initializeTest(interpreter, Method::adaptivePassFail);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(sessionController.prepareCalled());
}

TEST_SETTINGS_INTERPRETER_TEST(initializesPuzzleWithPath) {
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method,
             Method::
                 fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording),
            entryWithNewline(TestSetting::puzzle, "/Users/user/puzzle.png")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "/Users/user/puzzle.png", puzzle.url().path);
}

TEST_SETTINGS_INTERPRETER_TEST(initializesFreeResponseControllerWithPuzzle) {
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method,
             Method::
                 fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording),
            entryWithNewline(TestSetting::puzzle, "/Users/user/puzzle.png")});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(freeResponseController.usingPuzzle());
}

TEST_SETTINGS_INTERPRETER_TEST(initializesFreeResponseControllerWithoutPuzzle) {
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method,
            Method::
                fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording)});
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(freeResponseController.usingPuzzle());
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecordingPassesMethod) {
    initializeTest(interpreter,
        Method::
            fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        name(Method::
                fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording),
        fixedLevelMethod.test.identity.method);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTrackingPassesMethod) {
    initializeTest(interpreter,
        Method::fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        name(Method::
                fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking),
        fixedLevelMethod.test.identity.method);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithPredeterminedTargetsPassesMethod) {
    initializeTest(
        interpreter, Method::fixedLevelFreeResponseWithPredeterminedTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        name(Method::fixedLevelFreeResponseWithPredeterminedTargets),
        fixedLevelMethod.test.identity.method);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptivePassFailOverridesTestIdentity) {
    assertOverridesTestIdentity(
        interpreter, Method::adaptivePassFail, adaptiveMethod.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsOverridesTestIdentity) {
    assertOverridesTestIdentity(interpreter,
        Method::fixedLevelFreeResponseWithAllTargets,
        runningATest.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelChooseKeywordsWithAllTargetsOverridesTestIdentity) {
    assertOverridesTestIdentity(interpreter,
        Method::fixedLevelChooseKeywordsWithAllTargets,
        runningATest.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptivePassFailPassesTestIdentity) {
    assertPassesTestIdentity(
        interpreter, Method::adaptivePassFail, adaptiveMethod.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptivePassFailWithEyeTrackingPassesTestIdentity) {
    assertPassesTestIdentity(interpreter,
        Method::adaptivePassFailWithEyeTracking, adaptiveMethod.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsPassesTestIdentity) {
    assertPassesTestIdentity(interpreter,
        Method::fixedLevelFreeResponseWithAllTargets,
        runningATest.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelChooseKeywordsWithAllTargetsPassesTestIdentity) {
    assertPassesTestIdentity(interpreter,
        Method::fixedLevelChooseKeywordsWithAllTargets,
        runningATest.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsAndEyeTrackingPassesTestIdentity) {
    assertPassesTestIdentity(interpreter,
        Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking,
        runningATest.test.identity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithDelayedMaskerInitializesAdaptiveTest) {
    initializeTest(interpreter,
        Method::adaptiveCoordinateResponseMeasureWithDelayedMasker);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(AudioChannelOption::delayedMasker,
        runningATest.test.audioChannelOption);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithSingleSpeakerInitializesAdaptiveTest) {
    initializeTest(interpreter,
        Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(AudioChannelOption::singleSpeaker,
        runningATest.test.audioChannelOption);
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelConsonantsInitializesFixedLevelTest) {
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method, Method::fixedLevelConsonants),
            entryWithNewline(TestSetting::targetRepetitions, "5")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(4, eachTargetNTimes.repeats());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &eachTargetNTimes);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithSilentIntervalTargetsInitializesFixedLevelTest) {
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method,
             Method::fixedLevelFreeResponseWithSilentIntervalTargets),
            entryWithNewline(TestSetting::subjectId, "abc")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "abc", runningATest.test.identity.subjectId);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithTargetReplacementInitializesFixedLevelTest) {
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method,
             Method::fixedLevelFreeResponseWithTargetReplacement),
            entryWithNewline(TestSetting::maskerLevel, "5")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(5, runningATest.test.maskerLevel.dB_SPL);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptivePassFailOverridesStartingSnr) {
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::startingSnr, "6")},
        5);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(6, adaptiveMethod.test.startingSnr.dB);
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelConsonantsOverridesStartingSnr) {
    assertOverridesStartingSnr(
        interpreter, Method::fixedLevelConsonants, fixedLevelMethod.test);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithTargetReplacementOverridesStartingSnr) {
    assertOverridesStartingSnr(interpreter,
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement,
        fixedLevelMethod.fixedLevelFixedTrialsTest);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptiveAudioVisual) {
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::condition, Condition::audioVisual)});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Condition::audioVisual, adaptiveMethod.test.condition);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptiveAuditoryOnly) {
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::condition, Condition::auditoryOnly)});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Condition::auditoryOnly, adaptiveMethod.test.condition);
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelAudioVisual) {
    initializeTest(interpreter,
        {entryWithNewline(
             TestSetting::method, Method::fixedLevelFreeResponseWithAllTargets),
            entryWithNewline(TestSetting::condition, Condition::audioVisual)});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Condition::audioVisual, runningATest.test.condition);
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelFixedTargetsAudioVisual) {
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method,
             Method::fixedLevelFreeResponseWithTargetReplacement),
            entryWithNewline(TestSetting::condition, Condition::audioVisual)});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Condition::audioVisual, runningATest.test.condition);
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelAuditoryOnly) {
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method,
             Method::fixedLevelFreeResponseWithTargetReplacement),
            entryWithNewline(TestSetting::condition, Condition::auditoryOnly)});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Condition::auditoryOnly, runningATest.test.condition);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsPassesSettingsWithExtraneousWhitespace) {
    assertPassesSettingsWithExtraneousWhitespace(interpreter,
        Method::fixedLevelFreeResponseWithAllTargets, fixedLevelMethod.test);
}

TEST_SETTINGS_INTERPRETER_TEST(oneSequence) {
    TrackingSequence sequence{};
    sequence.up = 1;
    sequence.down = 2;
    sequence.runCount = 3;
    sequence.stepSize = 4;
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::up, "1"),
            entryWithNewline(TestSetting::down, "2"),
            entryWithNewline(TestSetting::reversalsPerStepSize, "3"),
            entryWithNewline(TestSetting::stepSizes, "4")});
    assertEqual({sequence}, adaptiveMethod.test.trackingRule);
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
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::up, "1 2"),
            entryWithNewline(TestSetting::down, "3 4"),
            entryWithNewline(TestSetting::reversalsPerStepSize, "5 6"),
            entryWithNewline(TestSetting::stepSizes, "7 8")});
    assertEqual({first, second}, adaptiveMethod.test.trackingRule);
}

TEST_SETTINGS_INTERPRETER_TEST(consonantTestWithTargetRepetitions) {
    initializeTest(interpreter,
        {"\n",
            entryWithNewline(TestSetting::method, Method::fixedLevelConsonants),
            "\n", entryWithNewline(TestSetting::targetRepetitions, "2")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, eachTargetNTimes.repeats());
}

TEST_SETTINGS_INTERPRETER_TEST(
    consonantTestWithTargetRepetitionsDefaultsToOne) {
    initializeTest(interpreter,
        {"\n",
            entryWithNewline(
                TestSetting::method, Method::fixedLevelConsonants)});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(0, eachTargetNTimes.repeats());
}
}
}
