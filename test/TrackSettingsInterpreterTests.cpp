#include "assert-utility.hpp"
#include <recognition-test/TrackSettingsInterpreter.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise {
namespace {
using Property = TrackSettingsInterpreterImpl::Property;

class TrackSettingsInterpreterTests : public ::testing::Test {
  protected:
    TrackSettingsInterpreterImpl interpreter;

    void assertFileContentsYield(
        const std::vector<std::string> &v, const TrackingRule &expected) {
        assertEqual(expected, *interpreter.trackingRule(concatenate(v)));
    }

    static auto concatenate(const std::vector<std::string> &v) -> std::string {
        std::string result;
        for (const auto &v_ : v)
            result.append(v_);
        return result;
    }

    static auto propertyEntry(Property p, std::string s) -> std::string {
        return std::string{TrackSettingsInterpreterImpl::propertyName(p)} +
            ": " + std::move(s);
    }

    static auto propertyEntryWithNewline(Property p, std::string s)
        -> std::string {
        return withNewLine(propertyEntry(p, std::move(s)));
    }

    static auto withNewLine(std::string s) -> std::string {
        return std::move(s) + '\n';
    }
};

TEST_F(TrackSettingsInterpreterTests, oneSequence) {
    TrackingSequence sequence{};
    sequence.up = 1;
    sequence.down = 2;
    sequence.runCount = 3;
    sequence.stepSize = 4;
    assertFileContentsYield(
        {propertyEntryWithNewline(Property::up, "1"),
            propertyEntryWithNewline(Property::down, "2"),
            propertyEntryWithNewline(Property::reversalsPerStepSize, "3"),
            propertyEntry(Property::stepSizes, "4")},
        {sequence});
}

TEST_F(TrackSettingsInterpreterTests, twoSequences) {
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
    assertFileContentsYield(
        {propertyEntryWithNewline(Property::up, "1 2"),
            propertyEntryWithNewline(Property::down, "3 4"),
            propertyEntryWithNewline(Property::reversalsPerStepSize, "5 6"),
            propertyEntry(Property::stepSizes, "7 8")},
        {first, second});
}

TEST_F(TrackSettingsInterpreterTests, differentPropertyOrder) {
    TrackingSequence sequence{};
    sequence.up = 1;
    sequence.down = 2;
    sequence.runCount = 3;
    sequence.stepSize = 4;
    assertFileContentsYield(
        {propertyEntryWithNewline(Property::down, "2"),
            propertyEntryWithNewline(Property::up, "1"),
            propertyEntryWithNewline(Property::reversalsPerStepSize, "3"),
            propertyEntry(Property::stepSizes, "4")},
        {sequence});
}

TEST_F(TrackSettingsInterpreterTests, ignoresAnyUninterpretableEntries) {
    TrackingSequence sequence{};
    sequence.up = 1;
    sequence.down = 2;
    sequence.runCount = 3;
    sequence.stepSize = 4;
    assertFileContentsYield(
        {withNewLine(" "), propertyEntryWithNewline(Property::down, "2"),
            propertyEntryWithNewline(Property::up, "1"),
            withNewLine("not: real"),
            propertyEntryWithNewline(Property::reversalsPerStepSize, "3"),
            propertyEntryWithNewline(Property::stepSizes, "4"),
            withNewLine(" ")},
        {sequence});
}

TEST_F(TrackSettingsInterpreterTests, canBeUsedToInterpretMoreThanOnce) {
    TrackingSequence firstCallFirst{};
    firstCallFirst.up = 1;
    firstCallFirst.down = 2;
    firstCallFirst.runCount = 3;
    firstCallFirst.stepSize = 4;
    TrackingSequence firstCallSecond{};
    firstCallSecond.up = 5;
    firstCallSecond.down = 6;
    firstCallSecond.runCount = 7;
    firstCallSecond.stepSize = 8;
    assertFileContentsYield(
        {propertyEntryWithNewline(Property::up, "1 5"),
            propertyEntryWithNewline(Property::down, "2 6"),
            propertyEntryWithNewline(Property::reversalsPerStepSize, "3 7"),
            propertyEntry(Property::stepSizes, "4 8")},
        {firstCallFirst, firstCallSecond});
    TrackingSequence secondCall{};
    secondCall.up = 9;
    secondCall.down = 10;
    secondCall.runCount = 11;
    secondCall.stepSize = 12;
    assertFileContentsYield(
        {propertyEntryWithNewline(Property::up, "9"),
            propertyEntryWithNewline(Property::down, "10"),
            propertyEntryWithNewline(Property::reversalsPerStepSize, "11"),
            propertyEntry(Property::stepSizes, "12")},
        {secondCall});
}
}
}
