#include <av-speech-in-noise/Model.h>
#include <string>

class TrackSettingsReader {
    std::string contents;
public:
    explicit TrackSettingsReader(std::string s) : contents{std::move(s)} {}
    
    av_speech_in_noise::TrackingRule trackingRule() {
        av_speech_in_noise::TrackingSequence first;
        first.up = 1;
        first.down = 2;
        first.runCount = 3;
        first.stepSize = 4;
        av_speech_in_noise::TrackingRule rule {first};
        return rule;
    }
};

#include "assert-utility.h"
#include <gtest/gtest.h>

namespace {
    class TrackSettingsReaderTests : public ::testing::Test {
    protected:
        void assertFileContentsYield(
            std::string s,
            const av_speech_in_noise::TrackingRule &expected
        ) {
            TrackSettingsReader reader{std::move(s)};
            assertEqual(expected, reader.trackingRule());
        }
    };
    
    TEST_F(TrackSettingsReaderTests, tbd) {
        av_speech_in_noise::TrackingSequence first;
        first.up = 1;
        first.down = 2;
        first.runCount = 3;
        first.stepSize = 4;
        av_speech_in_noise::TrackingRule rule {first};
        assertFileContentsYield(
            "up: 1\n"
            "down: 2\n"
            "reversals per step size: 3\n"
            "step sizes (dB): 4\n",
            rule
        );
    }
}
