#include "assert-utility.h"
#include <recognition-test/TrackSettingsReader.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests {
    class TrackSettingsReaderTests : public ::testing::Test {
    protected:
        void assertFileContentsYield(
            std::vector<std::string> v,
            const TrackingRule &expected
        ) {
            TrackSettingsReader reader{concatenate(std::move(v))};
            assertEqual(expected, reader.trackingRule());
        }
        
        std::string concatenate(std::vector<std::string> v) {
            std::string result;
            for (auto v_ : std::move(v))
                result.append(v_);
            return result;
        }
        
        std::string propertyEntry(TrackSettingsReader::Property p, std::string s) {
            return std::string{TrackSettingsReader::propertyName(p)} + ": " + std::move(s);
        }
        
        std::string propertyEntryWithNewline(TrackSettingsReader::Property p, std::string s) {
            return propertyEntry(p, std::move(s)) + '\n';
        }
    };
    
    TEST_F(TrackSettingsReaderTests, oneSequence) {
        TrackingSequence first;
        first.up = 1;
        first.down = 2;
        first.runCount = 3;
        first.stepSize = 4;
        assertFileContentsYield(
            {
                propertyEntryWithNewline(TrackSettingsReader::Property::up, "1"),
                propertyEntryWithNewline(TrackSettingsReader::Property::down, "2"),
                propertyEntryWithNewline(TrackSettingsReader::Property::reversalsPerStepSize, "3"),
                propertyEntry(TrackSettingsReader::Property::stepSizes, "4")
            },
            {first}
        );
    }
    
    TEST_F(TrackSettingsReaderTests, twoSequences) {
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
                propertyEntryWithNewline(TrackSettingsReader::Property::up, "1 2"),
                propertyEntryWithNewline(TrackSettingsReader::Property::down, "3 4"),
                propertyEntryWithNewline(TrackSettingsReader::Property::reversalsPerStepSize, "5 6"),
                propertyEntry(TrackSettingsReader::Property::stepSizes, "7 8")
            },
            {first, second}
        );
    }
    
    TEST_F(TrackSettingsReaderTests, differentPropertyOrder) {
        TrackingSequence first;
        first.up = 1;
        first.down = 2;
        first.runCount = 3;
        first.stepSize = 4;
        assertFileContentsYield(
            {
                propertyEntryWithNewline(TrackSettingsReader::Property::down, "2"),
                propertyEntryWithNewline(TrackSettingsReader::Property::up, "1"),
                propertyEntryWithNewline(TrackSettingsReader::Property::reversalsPerStepSize, "3"),
                propertyEntry(TrackSettingsReader::Property::stepSizes, "4")
            },
            {first}
        );
    }
}
