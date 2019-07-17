#include "assert-utility.h"
#include <recognition-test/TrackSettingsInterpreter.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests {
    using Property = TrackSettingsInterpreter::Property;
    
    class TrackSettingsInterpreterTests : public ::testing::Test {
    protected:
        void assertFileContentsYield(
            std::vector<std::string> v,
            const TrackingRule &expected
        ) {
            TrackSettingsInterpreter reader{concatenate(v)};
            assertEqual(expected, *reader.trackingRule(concatenate(std::move(v))));
        }
        
        std::string concatenate(std::vector<std::string> v) {
            std::string result;
            for (auto v_ : std::move(v))
                result.append(v_);
            return result;
        }
        
        std::string propertyEntry(Property p, std::string s) {
            return
                std::string{TrackSettingsInterpreter::propertyName(p)} +
                ": " +
                std::move(s);
        }
        
        std::string propertyEntryWithNewline(Property p, std::string s) {
            return withNewLine(propertyEntry(p, std::move(s)));
        }
        
        std::string withNewLine(std::string s) {
            return std::move(s) + '\n';
        }
    };
    
    TEST_F(TrackSettingsInterpreterTests, oneSequence) {
        TrackingSequence sequence;
        sequence.up = 1;
        sequence.down = 2;
        sequence.runCount = 3;
        sequence.stepSize = 4;
        assertFileContentsYield(
            {
                propertyEntryWithNewline(Property::up, "1"),
                propertyEntryWithNewline(Property::down, "2"),
                propertyEntryWithNewline(Property::reversalsPerStepSize, "3"),
                propertyEntry(Property::stepSizes, "4")
            },
            {sequence}
        );
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
            {
                propertyEntryWithNewline(Property::up, "1 2"),
                propertyEntryWithNewline(Property::down, "3 4"),
                propertyEntryWithNewline(Property::reversalsPerStepSize, "5 6"),
                propertyEntry(Property::stepSizes, "7 8")
            },
            {first, second}
        );
    }
    
    TEST_F(TrackSettingsInterpreterTests, differentPropertyOrder) {
        TrackingSequence sequence;
        sequence.up = 1;
        sequence.down = 2;
        sequence.runCount = 3;
        sequence.stepSize = 4;
        assertFileContentsYield(
            {
                propertyEntryWithNewline(Property::down, "2"),
                propertyEntryWithNewline(Property::up, "1"),
                propertyEntryWithNewline(Property::reversalsPerStepSize, "3"),
                propertyEntry(Property::stepSizes, "4")
            },
            {sequence}
        );
    }
    
    TEST_F(TrackSettingsInterpreterTests, ignoresAnyUninterpretableEntries) {
        TrackingSequence sequence;
        sequence.up = 1;
        sequence.down = 2;
        sequence.runCount = 3;
        sequence.stepSize = 4;
        assertFileContentsYield(
            {
                withNewLine(" "),
                propertyEntryWithNewline(Property::down, "2"),
                propertyEntryWithNewline(Property::up, "1"),
                withNewLine("not: real"),
                propertyEntryWithNewline(Property::reversalsPerStepSize, "3"),
                propertyEntryWithNewline(Property::stepSizes, "4"),
                withNewLine(" ")
            },
            {sequence}
        );
    }
}
