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

class TestSettingsInterpreterTests : public ::testing::Test {
  protected:
    ModelStub model;
    TestSettingsInterpreterImpl interpreter;

    void apply(const std::vector<std::string> &v) {
        interpreter.apply(model, concatenate(v));
    }
};

TEST_F(TestSettingsInterpreterTests, adaptivePassFailInitializesAdaptiveTest) {
    apply({entryWithNewline(TestSetting::method, Method::adaptivePassFail)});
    assertTrue(model.defaultAdaptiveTestInitialized());
}

TEST_F(TestSettingsInterpreterTests, tbd) {
    apply({entryWithNewline(TestSetting::method, Method::adaptivePassFail),
        entryWithNewline(TestSetting::targets, "a"),
        entryWithNewline(TestSetting::masker, "b"),
        entryWithNewline(TestSetting::maskerLevel, "65"),
        entryWithNewline(TestSetting::condition, Condition::audioVisual)});
    assertEqual("a", model.adaptiveTest().targetListDirectory);
    assertEqual("b", model.adaptiveTest().maskerFilePath);
    assertEqual(65, model.adaptiveTest().maskerLevel_dB_SPL);
    assertEqual(Condition::audioVisual, model.adaptiveTest().condition);
}
/*
TEST_F(TestSettingsInterpreterTests, oneSequence) {
    TrackingSequence sequence{};
    sequence.up = 1;
    sequence.down = 2;
    sequence.runCount = 3;
    sequence.stepSize = 4;
    apply({entryWithNewline(TestSetting::up, "1"),
        entryWithNewline(TestSetting::down, "2"),
        entryWithNewline(TestSetting::reversalsPerStepSize, "3"),
        entryWithNewline(TestSetting::stepSizes, "4")});
    assertEqual({sequence}, model.adaptiveTest().);
}
*/
}
}
