#include <av-speech-in-noise/Model.h>
#include <string>
#include <sstream>

namespace av_speech_in_noise {
    class TrackSettingsReader {
        std::string contents;
    public:
        explicit TrackSettingsReader(std::string s) : contents{std::move(s)} {}
        
        TrackingRule trackingRule() {
            std::stringstream stream{contents};
            std::string ignore;
            int value;
            stream >> ignore;
            stream >> value;
            TrackingSequence first;
            first.up = value;
            stream >> ignore;
            stream >> value;
            first.down = value;
            stream >> ignore;
            stream >> ignore;
            stream >> ignore;
            stream >> ignore;
            stream >> value;
            first.runCount = value;
            stream >> ignore;
            stream >> ignore;
            stream >> ignore;
            stream >> value;
            first.stepSize = value;
            return {first};
        }
    };
}

#include "assert-utility.h"
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests {
    class TrackSettingsReaderTests : public ::testing::Test {
    protected:
        void assertFileContentsYield(
            std::string s,
            const TrackingRule &expected
        ) {
            TrackSettingsReader reader{std::move(s)};
            assertEqual(expected, reader.trackingRule());
        }
    };
    
    TEST_F(TrackSettingsReaderTests, oneSequence) {
        TrackingSequence first;
        first.up = 1;
        first.down = 2;
        first.runCount = 3;
        first.stepSize = 4;
        assertFileContentsYield(
            "up: 1\n"
            "down: 2\n"
            "reversals per step size: 3\n"
            "step sizes (dB): 4\n",
            {first}
        );
    }
}
