#include "assert-utility.h"
#include "ModelStub.hpp"
#include <presentation/TestSettingsInterpreter.hpp>
#include <gtest/gtest.h>

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
    const std::vector<std::string> &v) {
    interpreter.apply(model, concatenate(v));
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
}

class TestSettingsInterpreterTests : public ::testing::Test {
  protected:
    ModelStub model;
    TestSettingsInterpreterImpl interpreter;
};

TEST_F(TestSettingsInterpreterTests, adaptivePassFailInitializesAdaptiveTest) {
    apply(interpreter, model,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail)});
    assertTrue(model.defaultAdaptiveTestInitialized());
}

TEST_F(TestSettingsInterpreterTests,
    adaptivePassFailPassesSimpleAdaptiveSettings) {
    assertPassesSimpleAdaptiveSettings(
        interpreter, model, Method::adaptivePassFail);
}

TEST_F(TestSettingsInterpreterTests,
    adaptiveCorrectKeywordsPassesSimpleAdaptiveSettings) {
    assertPassesSimpleAdaptiveSettings(
        interpreter, model, Method::adaptiveCorrectKeywords);
}

TEST_F(TestSettingsInterpreterTests,
    adaptiveCoordinateResponseMeasureWithDelayedMaskerPassesSimpleAdaptiveSettings) {
    assertPassesSimpleAdaptiveSettings(interpreter, model,
        Method::adaptiveCoordinateResponseMeasureWithDelayedMasker);
}

TEST_F(TestSettingsInterpreterTests, tbd2) {
    apply(interpreter, model,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::condition, Condition::audioVisual)});
    assertEqual(Condition::audioVisual, adaptiveTest(model).condition);
}

TEST_F(TestSettingsInterpreterTests, tbd3) {
    apply(interpreter, model,
        {entryWithNewline(
             TestSetting::method, Method::fixedLevelFreeResponseWithAllTargets),
            entryWithNewline(TestSetting::targets, "a"),
            entryWithNewline(TestSetting::masker, "b"),
            entryWithNewline(TestSetting::maskerLevel, "65"),
            entryWithNewline(TestSetting::condition, Condition::audioVisual)});
    assertEqual("a", fixedLevelTest(model).targetListDirectory);
    assertEqual("b", fixedLevelTest(model).maskerFilePath);
    assertEqual(65, fixedLevelTest(model).maskerLevel_dB_SPL);
    assertEqual(Condition::audioVisual, fixedLevelTest(model).condition);
}

TEST_F(TestSettingsInterpreterTests, oneSequence) {
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

TEST_F(TestSettingsInterpreterTests, twoSequences) {
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
