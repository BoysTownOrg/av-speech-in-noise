#include "assert-utility.h"
#include "ModelStub.hpp"
#include <presentation/TestSettingsInterpreter.hpp>
#include <gtest/gtest.h>
#include <functional>

namespace av_speech_in_noise {
namespace {
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

auto adaptiveTest(ModelStub &m) -> AdaptiveTest { return m.adaptiveTest(); }

auto fixedLevelTest(ModelStub &m) -> FixedLevelTest {
    return m.fixedLevelTest();
}

void initialize(TestSettingsInterpreterImpl &interpreter, Model &model,
    const std::vector<std::string> &v, const TestIdentity &identity = {}) {
    interpreter.initialize(model, concatenate(v), identity, {});
}

void assertPassesSimpleAdaptiveSettings(
    TestSettingsInterpreterImpl &interpreter, ModelStub &model, Method m) {
    initialize(interpreter, model,
        {entryWithNewline(TestSetting::method, m),
            entryWithNewline(TestSetting::targets, "a"),
            entryWithNewline(TestSetting::masker, "b"),
            entryWithNewline(TestSetting::maskerLevel, "65"),
            entryWithNewline(TestSetting::startingSnr, "5"),
            entryWithNewline(TestSetting::thresholdReversals, "4"),
            entryWithNewline(TestSetting::condition, Condition::audioVisual)});
    assertEqual("a", adaptiveTest(model).targetListDirectory);
    assertEqual("b", adaptiveTest(model).maskerFilePath);
    assertEqual(65, adaptiveTest(model).maskerLevel_dB_SPL);
    assertEqual(5, adaptiveTest(model).startingSnr_dB);
    assertEqual(4, adaptiveTest(model).thresholdReversals);
    assertEqual(Presenter::ceilingSnr_dB, adaptiveTest(model).ceilingSnr_dB);
    assertEqual(Presenter::floorSnr_dB, adaptiveTest(model).floorSnr_dB);
    assertEqual(Presenter::trackBumpLimit, adaptiveTest(model).trackBumpLimit);
    assertEqual(Presenter::fullScaleLevel_dB_SPL,
        adaptiveTest(model).fullScaleLevel_dB_SPL);
}

void assertPassesSimpleFixedLevelSettings(
    TestSettingsInterpreterImpl &interpreter, ModelStub &model, Method m) {
    initialize(interpreter, model,
        {entryWithNewline(TestSetting::method, m),
            entryWithNewline(TestSetting::targets, "a"),
            entryWithNewline(TestSetting::masker, "b"),
            entryWithNewline(TestSetting::maskerLevel, "65"),
            entryWithNewline(TestSetting::startingSnr, "5")});
    assertEqual("a", fixedLevelTest(model).targetListDirectory);
    assertEqual("b", fixedLevelTest(model).maskerFilePath);
    assertEqual(65, fixedLevelTest(model).maskerLevel_dB_SPL);
    assertEqual(5, fixedLevelTest(model).snr_dB);
    assertEqual(Presenter::fullScaleLevel_dB_SPL,
        fixedLevelTest(model).fullScaleLevel_dB_SPL);
}

void initialize(TestSettingsInterpreterImpl &interpreter, Model &model,
    Method m, const TestIdentity &identity = {}) {
    initialize(interpreter, model, {entryWithNewline(TestSetting::method, m)},
        identity);
}

auto method(TestSettingsInterpreterImpl &interpreter,
    const std::vector<std::string> &v) -> Method {
    return interpreter.method(concatenate(v));
}

auto method(TestSettingsInterpreterImpl &interpreter, Method m) -> Method {
    return method(interpreter, {entryWithNewline(TestSetting::method, m)});
}

void assertDefaultAdaptiveTestInitialized(ModelStub &model) {
    assertTrue(model.defaultAdaptiveTestInitialized());
}

void assertFixedLevelTestWithSilentIntervalTargetsInitialized(
    ModelStub &model) {
    assertTrue(model.fixedLevelTestWithSilentIntervalTargetsInitialized());
}

void assertDefaultFixedLevelTestInitialized(ModelStub &model) {
    assertTrue(model.defaultFixedLevelTestInitialized());
}

auto adaptiveTestIdentity(ModelStub &model) -> TestIdentity {
    return adaptiveTest(model).identity;
}

auto fixedLevelTestIdentity(ModelStub &model) -> TestIdentity {
    return fixedLevelTest(model).identity;
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
    assertEqual(s, subjectId(identity));
}

void assertTesterIdEquals(const std::string &s, const TestIdentity &identity) {
    assertEqual(s, testerId(identity));
}

void assertSessionIdEquals(const std::string &s, const TestIdentity &identity) {
    assertEqual(s, session(identity));
}

void assertTestMethodEquals(
    const std::string &s, const TestIdentity &identity) {
    assertEqual(s, identity.method);
}

void assertPassesTestIdentity(TestSettingsInterpreterImpl &interpreter,
    ModelStub &model, Method m,
    const std::function<TestIdentity(ModelStub &)> &f) {
    TestIdentity testIdentity;
    setSubjectId(testIdentity, "a");
    setTesterId(testIdentity, "b");
    setSession(testIdentity, "c");
    initialize(interpreter, model, m, testIdentity);
    assertSubjectIdEquals("a", f(model));
    assertTesterIdEquals("b", f(model));
    assertSessionIdEquals("c", f(model));
}

void assertPassesTestMethod(TestSettingsInterpreterImpl &interpreter,
    ModelStub &model, Method m,
    const std::function<TestIdentity(ModelStub &)> &f) {
    initialize(interpreter, model, m);
    assertTestMethodEquals(name(m), f(model));
}

void assertMethod(TestSettingsInterpreterImpl &interpreter, Method m) {
    assertEqual(m, method(interpreter, m));
}

class TestSettingsInterpreterTests : public ::testing::Test {
  protected:
    ModelStub model;
    TestSettingsInterpreterImpl interpreter;
    TestIdentity testIdentity;
};

#define TEST_SETTINGS_INTERPRETER_TEST(a)                                      \
    TEST_F(TestSettingsInterpreterTests, a)

TEST_SETTINGS_INTERPRETER_TEST(usesMaskerForCalibration) {
    auto calibration{interpreter.calibration(
        concatenate({entryWithNewline(TestSetting::masker, "a"),
            entryWithNewline(TestSetting::maskerLevel, "1")}))};
    assertEqual("a", calibration.filePath);
    assertEqual(1, calibration.level_dB_SPL);
    assertEqual(
        Presenter::fullScaleLevel_dB_SPL, calibration.fullScaleLevel_dB_SPL);
}

TEST_SETTINGS_INTERPRETER_TEST(ignoresBadLine) {
    initialize(interpreter, model,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            "f:\n", entryWithNewline(TestSetting::targets, "a")});
    assertEqual("a", adaptiveTest(model).targetListDirectory);
}

TEST_SETTINGS_INTERPRETER_TEST(ignoresBadLine2) {
    initialize(interpreter, model,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail), "\n",
            entryWithNewline(TestSetting::targets, "a")});
    assertEqual("a", adaptiveTest(model).targetListDirectory);
}

TEST_SETTINGS_INTERPRETER_TEST(ignoresBadLine3) {
    initialize(interpreter, model,
        {"\n", entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            "\n", entryWithNewline(TestSetting::targets, "a")});
    assertEqual("a", adaptiveTest(model).targetListDirectory);
}

TEST_SETTINGS_INTERPRETER_TEST(badStartingSnrResolvesToZero) {
    initialize(interpreter, model,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::startingSnr, "a")});
    assertEqual(0, adaptiveTest(model).startingSnr_dB);
}

TEST_SETTINGS_INTERPRETER_TEST(badFixedSnrResolvesToZero) {
    initialize(interpreter, model,
        {entryWithNewline(TestSetting::method,
             Method::fixedLevelFreeResponseWithTargetReplacement),
            entryWithNewline(TestSetting::startingSnr, "a")});
    assertEqual(0, fixedLevelTest(model).snr_dB);
}

TEST_SETTINGS_INTERPRETER_TEST(badMaskerLevelResolvesToZero) {
    initialize(interpreter, model,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::maskerLevel, "a")});
    assertEqual(0, adaptiveTest(model).maskerLevel_dB_SPL);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptivePassFailReturnsMethod) {
    assertMethod(interpreter, Method::adaptivePassFail);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptiveCorrectKeywordsReturnsMethod) {
    assertMethod(interpreter, Method::adaptiveCorrectKeywords);
}

TEST_SETTINGS_INTERPRETER_TEST(
    defaultAdaptiveCoordinateResponseMeasureReturnsMethod) {
    assertMethod(interpreter, Method::defaultAdaptiveCoordinateResponseMeasure);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithSingleSpeakerReturnsMethod) {
    assertMethod(interpreter,
        Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithDelayedMaskerReturnsMethod) {
    assertMethod(interpreter,
        Method::adaptiveCoordinateResponseMeasureWithDelayedMasker);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithTargetReplacementReturnsMethod) {
    assertMethod(
        interpreter, Method::fixedLevelFreeResponseWithTargetReplacement);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithTargetReplacementReturnsMethod) {
    assertMethod(interpreter,
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithSilentIntervalTargetsReturnsMethod) {
    assertMethod(
        interpreter, Method::fixedLevelFreeResponseWithSilentIntervalTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsReturnsMethod) {
    assertMethod(interpreter, Method::fixedLevelFreeResponseWithAllTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithSilentIntervalTargetsReturnsMethod) {
    assertMethod(interpreter,
        Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptivePassFailPassesMethod) {
    assertPassesTestMethod(
        interpreter, model, Method::adaptivePassFail, adaptiveTestIdentity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsPassesMethod) {
    assertPassesTestMethod(interpreter, model,
        Method::fixedLevelFreeResponseWithAllTargets, fixedLevelTestIdentity);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptivePassFailPassesTestIdentity) {
    assertPassesTestIdentity(
        interpreter, model, Method::adaptivePassFail, adaptiveTestIdentity);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsPassesTestIdentity) {
    assertPassesTestIdentity(interpreter, model,
        Method::fixedLevelFreeResponseWithAllTargets, fixedLevelTestIdentity);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptivePassFailInitializesAdaptiveTest) {
    initialize(interpreter, model, Method::adaptivePassFail);
    assertDefaultAdaptiveTestInitialized(model);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptiveCorrectKeywordsInitializesAdaptiveTest) {
    initialize(interpreter, model, Method::adaptiveCorrectKeywords);
    assertTrue(model.initializedWithCyclicTargets());
}

TEST_SETTINGS_INTERPRETER_TEST(
    defaultAdaptiveCoordinateResponseMeasureInitializesAdaptiveTest) {
    initialize(
        interpreter, model, Method::defaultAdaptiveCoordinateResponseMeasure);
    assertDefaultAdaptiveTestInitialized(model);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithDelayedMaskerInitializesAdaptiveTest) {
    initialize(interpreter, model,
        Method::adaptiveCoordinateResponseMeasureWithDelayedMasker);
    assertTrue(model.initializedWithDelayedMasker());
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithSingleSpeakerInitializesAdaptiveTest) {
    initialize(interpreter, model,
        Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker);
    assertTrue(model.initializedWithSingleSpeaker());
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithSilentIntervalTargetsInitializesFixedLevelTest) {
    initialize(interpreter, model,
        Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets);
    assertFixedLevelTestWithSilentIntervalTargetsInitialized(model);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithTargetReplacementInitializesFixedLevelTest) {
    initialize(interpreter, model,
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
    assertDefaultFixedLevelTestInitialized(model);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsInitializesFixedLevelTest) {
    initialize(
        interpreter, model, Method::fixedLevelFreeResponseWithAllTargets);
    assertTrue(model.fixedLevelTestWithAllTargetsInitialized());
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithSilentIntervalTargetsInitializesFixedLevelTest) {
    initialize(interpreter, model,
        Method::fixedLevelFreeResponseWithSilentIntervalTargets);
    assertFixedLevelTestWithSilentIntervalTargetsInitialized(model);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithTargetReplacementInitializesFixedLevelTest) {
    initialize(interpreter, model,
        Method::fixedLevelFreeResponseWithTargetReplacement);
    assertDefaultFixedLevelTestInitialized(model);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptivePassFailPassesSimpleAdaptiveSettings) {
    assertPassesSimpleAdaptiveSettings(
        interpreter, model, Method::adaptivePassFail);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCorrectKeywordsPassesSimpleAdaptiveSettings) {
    assertPassesSimpleAdaptiveSettings(
        interpreter, model, Method::adaptiveCorrectKeywords);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithDelayedMaskerPassesSimpleAdaptiveSettings) {
    assertPassesSimpleAdaptiveSettings(interpreter, model,
        Method::adaptiveCoordinateResponseMeasureWithDelayedMasker);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithSingleSpeakerPassesSimpleAdaptiveSettings) {
    assertPassesSimpleAdaptiveSettings(interpreter, model,
        Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker);
}

TEST_SETTINGS_INTERPRETER_TEST(
    defaultAdaptiveCoordinateResponseMeasurePassesSimpleAdaptiveSettings) {
    assertPassesSimpleAdaptiveSettings(
        interpreter, model, Method::defaultAdaptiveCoordinateResponseMeasure);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptiveAudioVisual) {
    initialize(interpreter, model,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::condition, Condition::audioVisual)});
    assertEqual(Condition::audioVisual, adaptiveTest(model).condition);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptiveAuditoryOnly) {
    initialize(interpreter, model,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::condition, Condition::auditoryOnly)});
    assertEqual(Condition::auditoryOnly, adaptiveTest(model).condition);
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelAudioVisual) {
    initialize(interpreter, model,
        {entryWithNewline(TestSetting::method,
             Method::fixedLevelFreeResponseWithTargetReplacement),
            entryWithNewline(TestSetting::condition, Condition::audioVisual)});
    assertEqual(Condition::audioVisual, fixedLevelTest(model).condition);
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelAuditoryOnly) {
    initialize(interpreter, model,
        {entryWithNewline(TestSetting::method,
             Method::fixedLevelFreeResponseWithTargetReplacement),
            entryWithNewline(TestSetting::condition, Condition::auditoryOnly)});
    assertEqual(Condition::auditoryOnly, fixedLevelTest(model).condition);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsPassesSimpleFixedLevelSettings) {
    assertPassesSimpleFixedLevelSettings(
        interpreter, model, Method::fixedLevelFreeResponseWithAllTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithSilentIntervalTargetsPassesSimpleFixedLevelSettings) {
    assertPassesSimpleFixedLevelSettings(interpreter, model,
        Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithTargetReplacementPassesSimpleFixedLevelSettings) {
    assertPassesSimpleFixedLevelSettings(interpreter, model,
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithSilentIntervalTargetsPassesSimpleFixedLevelSettings) {
    assertPassesSimpleFixedLevelSettings(interpreter, model,
        Method::fixedLevelFreeResponseWithSilentIntervalTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithTargetReplacementPassesSimpleFixedLevelSettings) {
    assertPassesSimpleFixedLevelSettings(interpreter, model,
        Method::fixedLevelFreeResponseWithTargetReplacement);
}

TEST_SETTINGS_INTERPRETER_TEST(oneSequence) {
    TrackingSequence sequence{};
    sequence.up = 1;
    sequence.down = 2;
    sequence.runCount = 3;
    sequence.stepSize = 4;
    initialize(interpreter, model,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::up, "1"),
            entryWithNewline(TestSetting::down, "2"),
            entryWithNewline(TestSetting::reversalsPerStepSize, "3"),
            entryWithNewline(TestSetting::stepSizes, "4")});
    assertEqual({sequence}, adaptiveTest(model).trackingRule);
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
    initialize(interpreter, model,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::up, "1 2"),
            entryWithNewline(TestSetting::down, "3 4"),
            entryWithNewline(TestSetting::reversalsPerStepSize, "5 6"),
            entryWithNewline(TestSetting::stepSizes, "7 8")});
    assertEqual({first, second}, adaptiveTest(model).trackingRule);
}
}
}
