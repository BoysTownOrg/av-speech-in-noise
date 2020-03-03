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
    return entryWithNewline(p, methodName(m));
}

auto entryWithNewline(TestSetting p, Condition c) -> std::string {
    return entryWithNewline(p, conditionName(c));
}

auto adaptiveTest(ModelStub &m) -> AdaptiveTest { return m.adaptiveTest(); }

auto fixedLevelTest(ModelStub &m) -> FixedLevelTest {
    return m.fixedLevelTest();
}

void apply(TestSettingsInterpreterImpl &interpreter, Model &model,
    const std::vector<std::string> &v, const TestIdentity &identity = {}) {
    interpreter.apply(model, concatenate(v), identity);
}

void assertPassesSimpleAdaptiveSettings(
    TestSettingsInterpreterImpl &interpreter, ModelStub &model, Method m) {
    apply(interpreter, model,
        {entryWithNewline(TestSetting::method, m),
            entryWithNewline(TestSetting::targets, "a"),
            entryWithNewline(TestSetting::masker, "b"),
            entryWithNewline(TestSetting::maskerLevel, "65"),
            entryWithNewline(TestSetting::startingSnr, "5"),
            entryWithNewline(TestSetting::condition, Condition::audioVisual)});
    assertEqual("a", adaptiveTest(model).targetListDirectory);
    assertEqual("b", adaptiveTest(model).maskerFilePath);
    assertEqual(65, adaptiveTest(model).maskerLevel_dB_SPL);
    assertEqual(5, adaptiveTest(model).startingSnr_dB);
    assertEqual(Presenter::ceilingSnr_dB, adaptiveTest(model).ceilingSnr_dB);
    assertEqual(Presenter::floorSnr_dB, adaptiveTest(model).floorSnr_dB);
    assertEqual(Presenter::trackBumpLimit, adaptiveTest(model).trackBumpLimit);
    assertEqual(Presenter::fullScaleLevel_dB_SPL,
        adaptiveTest(model).fullScaleLevel_dB_SPL);
}

void assertPassesSimpleFixedLevelSettings(
    TestSettingsInterpreterImpl &interpreter, ModelStub &model, Method m) {
    apply(interpreter, model,
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

void apply(TestSettingsInterpreterImpl &interpreter, Model &model, Method m,
    const TestIdentity &identity = {}) {
    apply(interpreter, model, {entryWithNewline(TestSetting::method, m)},
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
    apply(interpreter, model, m, testIdentity);
    assertSubjectIdEquals("a", f(model));
    assertTesterIdEquals("b", f(model));
    assertSessionIdEquals("c", f(model));
}

void assertPassesTestMethod(TestSettingsInterpreterImpl &interpreter,
    ModelStub &model, Method m,
    const std::function<TestIdentity(ModelStub &)> &f) {
    apply(interpreter, model, m);
    assertTestMethodEquals(methodName(m), f(model));
}

class TestSettingsInterpreterTests : public ::testing::Test {
  protected:
    ModelStub model;
    TestSettingsInterpreterImpl interpreter;
    TestIdentity testIdentity;
};

#define TEST_SETTINGS_INTERPRETER_TEST(a)                                      \
    TEST_F(TestSettingsInterpreterTests, a)

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsReturnsMethod) {
    assertEqual(Method::fixedLevelFreeResponseWithAllTargets,
        method(interpreter, Method::fixedLevelFreeResponseWithAllTargets));
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
    apply(interpreter, model, Method::adaptivePassFail);
    assertDefaultAdaptiveTestInitialized(model);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptiveCorrectKeywordsInitializesAdaptiveTest) {
    apply(interpreter, model, Method::adaptiveCorrectKeywords);
    assertDefaultAdaptiveTestInitialized(model);
}

TEST_SETTINGS_INTERPRETER_TEST(
    defaultAdaptiveCoordinateResponseMeasureInitializesAdaptiveTest) {
    apply(interpreter, model, Method::defaultAdaptiveCoordinateResponseMeasure);
    assertDefaultAdaptiveTestInitialized(model);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithDelayedMaskerInitializesAdaptiveTest) {
    apply(interpreter, model,
        Method::adaptiveCoordinateResponseMeasureWithDelayedMasker);
    assertTrue(model.initializedWithDelayedMasker());
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithSingleSpeakerInitializesAdaptiveTest) {
    apply(interpreter, model,
        Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker);
    assertTrue(model.initializedWithSingleSpeaker());
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithSilentIntervalTargetsInitializesFixedLevelTest) {
    apply(interpreter, model,
        Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets);
    assertFixedLevelTestWithSilentIntervalTargetsInitialized(model);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithTargetReplacementInitializesFixedLevelTest) {
    apply(interpreter, model,
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
    assertDefaultFixedLevelTestInitialized(model);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsInitializesFixedLevelTest) {
    apply(interpreter, model, Method::fixedLevelFreeResponseWithAllTargets);
    assertTrue(model.fixedLevelTestWithAllTargetsInitialized());
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithSilentIntervalTargetsInitializesFixedLevelTest) {
    apply(interpreter, model,
        Method::fixedLevelFreeResponseWithSilentIntervalTargets);
    assertFixedLevelTestWithSilentIntervalTargetsInitialized(model);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithTargetReplacementInitializesFixedLevelTest) {
    apply(interpreter, model,
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
    apply(interpreter, model,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::condition, Condition::audioVisual)});
    assertEqual(Condition::audioVisual, adaptiveTest(model).condition);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptiveAuditoryOnly) {
    apply(interpreter, model,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::condition, Condition::auditoryOnly)});
    assertEqual(Condition::auditoryOnly, adaptiveTest(model).condition);
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelAudioVisual) {
    apply(interpreter, model,
        {entryWithNewline(TestSetting::method,
             Method::fixedLevelFreeResponseWithTargetReplacement),
            entryWithNewline(TestSetting::condition, Condition::audioVisual)});
    assertEqual(Condition::audioVisual, fixedLevelTest(model).condition);
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelAuditoryOnly) {
    apply(interpreter, model,
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
    apply(interpreter, model,
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
    apply(interpreter, model,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::up, "1 2"),
            entryWithNewline(TestSetting::down, "3 4"),
            entryWithNewline(TestSetting::reversalsPerStepSize, "5 6"),
            entryWithNewline(TestSetting::stepSizes, "7 8")});
    assertEqual({first, second}, adaptiveTest(model).trackingRule);
}
}
}
