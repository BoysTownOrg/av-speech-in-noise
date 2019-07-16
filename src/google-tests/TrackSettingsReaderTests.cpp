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
            std::string line;
            std::getline(stream, line);
            auto line_ = std::stringstream{line};
            std::string ignore;
            line_ >> ignore;
            int value;
            line_ >> value;
            TrackingRule rule;
            TrackingSequence first;
            first.up = value;
            while (line_ >> value) {
                rule.push_back({});
                rule.back().up = value;
            }
            std::getline(stream, line);
            line_ = std::stringstream{line};
            line_ >> ignore;
            line_ >> value;
            first.down = value;
            for (size_t i = 0; i < rule.size(); ++i) {
                line_ >> value;
                rule.at(i).down = value;
            }
            std::getline(stream, line);
            line_ = std::stringstream{line};
            for (int i = 0; i < 4; ++i)
                line_ >> ignore;
            line_ >> value;
            first.runCount = value;
            for (size_t i = 0; i < rule.size(); ++i) {
                line_ >> value;
                rule.at(i).runCount = value;
            }
            std::getline(stream, line);
            line_ = std::stringstream{line};
            for (int i = 0; i < 3; ++i)
                line_ >> ignore;
            line_ >> value;
            first.stepSize = value;
            for (size_t i = 0; i < rule.size(); ++i) {
                line_ >> value;
                rule.at(i).stepSize = value;
            }
            if (rule.size())
                return {first, rule.at(0)};
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
