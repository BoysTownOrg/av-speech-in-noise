#include <recognition-test/AdaptiveTrack.hpp>
#include <gtest/gtest.h>

namespace {
    class AdaptiveTrackTests : public ::testing::Test {
    protected:
        av_coordinate_response_measure::AdaptiveTrack::Settings parameters{};
        av_coordinate_response_measure::TrackingRule rule;
        av_coordinate_response_measure::AdaptiveTrack track{};
        
        AdaptiveTrackTests() {
            rule.resize(3);
            for (auto &sequence : rule)
                sequence.runCount = 0;
        }
        
        void reset() {
            parameters.rule = &rule;
            track.reset(parameters);
        }
        
        void push(std::string directions) {
            for (const auto &c : directions)
                if (c == 'd')
                    track.pushDown();
                else if (c == 'u')
                    track.pushUp();
        }
    };

    TEST_F(AdaptiveTrackTests, xEqualToStartingX) {
        parameters.startingX = 1;
        reset();
        EXPECT_EQ(1, track.x());
    }

    TEST_F(AdaptiveTrackTests, noRunSequencesMeansNoStepChanges) {
        parameters.startingX = 5;
        reset();
        track.pushDown();
        EXPECT_EQ(5, track.x());
        track.pushDown();
        EXPECT_EQ(5, track.x());
        track.pushUp();
        EXPECT_EQ(5, track.x());
    }

    TEST_F(AdaptiveTrackTests, exhaustedRunSequencesMeansNoMoreStepChanges) {
        parameters.startingX = 5;
        rule.at(0).runCount = 3;
        rule.at(0).stepSize = 4;
        rule.at(0).up = 1;
        rule.at(0).down = 2;
        reset();
        track.pushDown();
        EXPECT_EQ(5, track.x());
        track.pushDown();
        EXPECT_EQ(5 - 4, track.x());
        track.pushUp();
        EXPECT_EQ(5 - 4 + 4, track.x());
        track.pushDown();
        EXPECT_EQ(5 - 4 + 4, track.x());
        track.pushDown();
        EXPECT_EQ(5 - 4 + 4 - 4, track.x());
        track.pushUp();
        EXPECT_EQ(5 - 4 + 4 - 4, track.x());
        track.pushDown();
        EXPECT_EQ(5 - 4 + 4 - 4, track.x());
        track.pushDown();
        EXPECT_EQ(5 - 4 + 4 - 4, track.x());
    }

    TEST_F(AdaptiveTrackTests, completeWhenExhausted) {
        parameters.startingX = 5;
        rule.at(0).runCount = 3;
        rule.at(0).stepSize = 4;
        rule.at(0).up = 1;
        rule.at(0).down = 2;
        reset();
        EXPECT_FALSE(track.complete());
        track.pushDown();
        EXPECT_FALSE(track.complete());
        track.pushDown();
        EXPECT_FALSE(track.complete());
        track.pushUp();
        EXPECT_FALSE(track.complete());
        track.pushDown();
        EXPECT_FALSE(track.complete());
        track.pushDown();
        EXPECT_FALSE(track.complete());
        track.pushUp();
        EXPECT_TRUE(track.complete());
    }

    // see https://doi.org/10.1121/1.1912375
    TEST_F(AdaptiveTrackTests, LevittFigure4) {
        parameters.startingX = 0;
        rule.at(0).runCount = 8;
        rule.at(0).stepSize = 1;
        rule.at(0).down = 1;
        rule.at(0).up = 1;
        reset();
        push("dduuuudduuuddddduuudduu");
        EXPECT_EQ(1, track.x());
    }

    // see https://doi.org/10.1121/1.1912375
    TEST_F(AdaptiveTrackTests, LevittFigure5) {
        parameters.startingX = 0;
        rule.at(0).runCount = 5;
        rule.at(0).stepSize = 1;
        rule.at(0).down = 2;
        rule.at(0).up = 1;
        reset();
        push("dddduduududdddduuuddddd");
        EXPECT_EQ(1, track.x());
    }

    TEST_F(AdaptiveTrackTests, twoSequences) {
        parameters.startingX = 65;
        rule.at(0).runCount = 2;
        rule.at(0).stepSize = 8;
        rule.at(0).down = 2;
        rule.at(0).up = 1;
        rule.at(1).runCount = 1;
        rule.at(1).stepSize = 4;
        rule.at(1).down = 2;
        rule.at(1).up = 1;
        reset();
        track.pushDown();
        EXPECT_EQ(65, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8 - 8, track.x());
        track.pushUp();
        EXPECT_EQ(65 - 8 - 8 + 8, track.x());
        track.pushUp();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8 - 4, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8 - 4, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8 - 4 - 4, track.x());
        track.pushUp();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8 - 4 - 4, track.x());
    }

    TEST_F(AdaptiveTrackTests, threeSequences) {
        parameters.startingX = 0;
        rule.at(0).runCount = 1;
        rule.at(0).stepSize = 10;
        rule.at(0).down = 3;
        rule.at(0).up = 1;
        rule.at(1).runCount = 1;
        rule.at(1).stepSize = 5;
        rule.at(1).down = 3;
        rule.at(1).up = 1;
        rule.at(2).runCount = 6;
        rule.at(2).stepSize = 2;
        rule.at(2).down = 3;
        rule.at(2).up = 1;
        reset();
        push("ddudddudddddudddddduddd");
        EXPECT_EQ(3, track.x());
    }

    TEST_F(AdaptiveTrackTests, varyingDownUpRule) {
        parameters.startingX = 65;
        rule.at(0).runCount = 2;
        rule.at(0).stepSize = 8;
        rule.at(0).up = 1;
        rule.at(0).down = 2;
        rule.at(1).runCount = 1;
        rule.at(1).stepSize = 4;
        rule.at(1).up = 2;
        rule.at(1).down = 1;
        reset();
        track.pushDown();
        EXPECT_EQ(65, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8 - 8, track.x());
        track.pushUp();
        EXPECT_EQ(65 - 8 - 8 + 8, track.x());
        track.pushUp();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8 - 4, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8 - 4 - 4, track.x());
        track.pushUp();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8 - 4 - 4, track.x());
        track.pushUp();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8 - 4 - 4, track.x());
    }
    
    TEST_F(AdaptiveTrackTests, reversals) {
        parameters.startingX = 0;
        rule.at(0).runCount = 1000;
        rule.at(0).down = 2;
        rule.at(0).up = 1;
        reset();
        EXPECT_EQ(int{0}, track.reversals());
        track.pushUp();
        EXPECT_EQ(int{0}, track.reversals());
        track.pushDown();
        EXPECT_EQ(int{0}, track.reversals());
        track.pushDown();
        EXPECT_EQ(1, track.reversals());
        track.pushUp();
        EXPECT_EQ(2, track.reversals());
        track.pushDown();
        EXPECT_EQ(2, track.reversals());
        track.pushDown();
        EXPECT_EQ(3, track.reversals());
    }
}
