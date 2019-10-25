#include "assert-utility.h"
#include <gtest/gtest.h>
#include <recognition-test/TrackSettingsInterpreter.hpp>

namespace av_speech_in_noise::tests {
namespace {
using Property = TrackSettingsInterpreter::Property;

class TrackSettingsInterpreterTests : public ::testing::Test {
protected:
  TrackSettingsInterpreter interpreter;

  void assertFileContentsYield(std::vector<std::string> v,
                               const TrackingRule &expected) {
    assertEqual(expected, *interpreter.trackingRule(concatenate(std::move(v))));
  }

  std::string concatenate(std::vector<std::string> v) {
    std::string result;
    for (auto v_ : std::move(v))
      result.append(v_);
    return result;
  }

  std::string propertyEntry(Property p, std::string s) {
    return std::string{TrackSettingsInterpreter::propertyName(p)} + ": " +
           std::move(s);
  }

  std::string propertyEntryWithNewline(Property p, std::string s) {
    return withNewLine(propertyEntry(p, std::move(s)));
  }

  std::string withNewLine(std::string s) { return std::move(s) + '\n'; }
};

TEST_F(TrackSettingsInterpreterTests, oneSequence) {
  TrackingSequence sequence;
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
  TrackingSequence first;
  first.up = 1;
  first.down = 3;
  first.runCount = 5;
  first.stepSize = 7;
  TrackingSequence second;
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
  TrackingSequence sequence;
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
  TrackingSequence sequence;
  sequence.up = 1;
  sequence.down = 2;
  sequence.runCount = 3;
  sequence.stepSize = 4;
  assertFileContentsYield(
      {withNewLine(" "), propertyEntryWithNewline(Property::down, "2"),
       propertyEntryWithNewline(Property::up, "1"), withNewLine("not: real"),
       propertyEntryWithNewline(Property::reversalsPerStepSize, "3"),
       propertyEntryWithNewline(Property::stepSizes, "4"), withNewLine(" ")},
      {sequence});
}

TEST_F(TrackSettingsInterpreterTests, canBeUsedToInterpretMoreThanOnce) {
  TrackingSequence firstCallFirst;
  firstCallFirst.up = 1;
  firstCallFirst.down = 2;
  firstCallFirst.runCount = 3;
  firstCallFirst.stepSize = 4;
  TrackingSequence firstCallSecond;
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
  TrackingSequence secondCall;
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
} // namespace
} // namespace av_speech_in_noise::tests
