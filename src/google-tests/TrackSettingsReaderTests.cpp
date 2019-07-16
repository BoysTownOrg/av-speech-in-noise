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
            std::stringstream line_;
            std::string ignore;
            std::string line;
            int value;
            int value2;
            bool twoSequences{};
            TrackingSequence first;
            TrackingSequence second;
            std::getline(stream, line);
            line_ = std::stringstream{line};
            line_ >> ignore;
            line_ >> value;
            if (line_ >> value2) {
                second.up = value2;
                twoSequences = true;
            }
            first.up = value;
            std::getline(stream, line);
            line_ = std::stringstream{line};
            line_ >> ignore;
            line_ >> value;
            if (line_ >> value2)
                second.down = value2;
            first.down = value;
            std::getline(stream, line);
            line_ = std::stringstream{line};
            line_ >> ignore;
            line_ >> ignore;
            line_ >> ignore;
            line_ >> ignore;
            line_ >> value;
            if (line_ >> value2)
                second.runCount = value2;
            first.runCount = value;
            std::getline(stream, line);
            line_ = std::stringstream{line};
            line_ >> ignore;
            line_ >> ignore;
            line_ >> ignore;
            line_ >> value;
            if (line_ >> value2)
                second.stepSize = value2;
            first.stepSize = value;
            if (twoSequences)
                return {first, second};
            else
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
            "step sizes (dB): 4",
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
            "up: 1 2\n"
            "down: 3 4\n"
            "reversals per step size: 5 6\n"
            "step sizes (dB): 7 8",
            {first, second}
        );
    }
}
