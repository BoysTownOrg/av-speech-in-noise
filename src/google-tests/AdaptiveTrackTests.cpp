#include <recognition-test/AdaptiveTrack.hpp>
#include <gtest/gtest.h>

namespace {
    class AdaptiveTrackTests : public ::testing::Test {
    protected:
        av_coordinate_response_measure::AdaptiveTrack::Settings settings{};
        av_coordinate_response_measure::TrackingRule rule;
        av_coordinate_response_measure::AdaptiveTrack track{};
        
        AdaptiveTrackTests() {
            rule.resize(3);
            for (auto &sequence : rule)
                sequence.runCount = 0;
        }
        
        void reset() {
            settings.rule = &rule;
            track.reset(settings);
        }
        
        void push(std::string directions) {
            for (const auto &c : directions)
                if (c == 'd')
                    pushDown();
                else if (c == 'u')
                    pushUp();
        }
        
        void pushDown() {
            track.pushDown();
        }
        
        void pushUp() {
            track.pushUp();
        }
        
        auto x() {
            return track.x();
        }
        
        auto complete() {
            return track.complete();
        }
        
        auto reversals() {
            return track.reversals();
        }
        
        template<typename T>
        void assertXEquals(T expected) {
            EXPECT_EQ(expected, x());
        }
    };

    TEST_F(AdaptiveTrackTests, xEqualToStartingX) {
        settings.startingX = 1;
        reset();
        assertXEquals(1);
    }

    TEST_F(AdaptiveTrackTests, noRunSequencesMeansNoStepChanges) {
        settings.startingX = 5;
        reset();
        pushDown();
        assertXEquals(5);
        pushDown();
        assertXEquals(5);
        pushUp();
        assertXEquals(5);
    }

    TEST_F(AdaptiveTrackTests, exhaustedRunSequencesMeansNoMoreStepChanges) {
        settings.startingX = 5;
        rule.at(0).runCount = 3;
        rule.at(0).stepSize = 4;
        rule.at(0).up = 1;
        rule.at(0).down = 2;
        reset();
        pushDown();
        assertXEquals(5);
        pushDown();
        assertXEquals(5 - 4);
        pushUp();
        assertXEquals(5 - 4 + 4);
        pushDown();
        assertXEquals(5 - 4 + 4);
        pushDown();
        assertXEquals(5 - 4 + 4 - 4);
        pushUp();
        assertXEquals(5 - 4 + 4 - 4);
        pushDown();
        assertXEquals(5 - 4 + 4 - 4);
        pushDown();
        assertXEquals(5 - 4 + 4 - 4);
    }

    TEST_F(AdaptiveTrackTests, completeWhenExhausted) {
        settings.startingX = 5;
        rule.at(0).runCount = 3;
        rule.at(0).stepSize = 4;
        rule.at(0).up = 1;
        rule.at(0).down = 2;
        reset();
        EXPECT_FALSE(complete());
        pushDown();
        EXPECT_FALSE(complete());
        pushDown();
        EXPECT_FALSE(complete());
        pushUp();
        EXPECT_FALSE(complete());
        pushDown();
        EXPECT_FALSE(complete());
        pushDown();
        EXPECT_FALSE(complete());
        pushUp();
        EXPECT_TRUE(complete());
    }

    // see https://doi.org/10.1121/1.1912375
    TEST_F(AdaptiveTrackTests, LevittFigure4) {
        settings.startingX = 0;
        rule.at(0).runCount = 8;
        rule.at(0).stepSize = 1;
        rule.at(0).down = 1;
        rule.at(0).up = 1;
        reset();
        push("dduuuudduuuddddduuudduu");
        assertXEquals(1);
    }

    // see https://doi.org/10.1121/1.1912375
    TEST_F(AdaptiveTrackTests, LevittFigure5) {
        settings.startingX = 0;
        rule.at(0).runCount = 5;
        rule.at(0).stepSize = 1;
        rule.at(0).down = 2;
        rule.at(0).up = 1;
        reset();
        push("dddduduududdddduuuddddd");
        assertXEquals(1);
    }

    TEST_F(AdaptiveTrackTests, twoSequences) {
        settings.startingX = 65;
        rule.at(0).runCount = 2;
        rule.at(0).stepSize = 8;
        rule.at(0).down = 2;
        rule.at(0).up = 1;
        rule.at(1).runCount = 1;
        rule.at(1).stepSize = 4;
        rule.at(1).down = 2;
        rule.at(1).up = 1;
        reset();
        pushDown();
        assertXEquals(65);
        pushDown();
        assertXEquals(65 - 8);
        pushDown();
        assertXEquals(65 - 8);
        pushDown();
        assertXEquals(65 - 8 - 8);
        pushUp();
        assertXEquals(65 - 8 - 8 + 8);
        pushUp();
        assertXEquals(65 - 8 - 8 + 8 + 8);
        pushDown();
        assertXEquals(65 - 8 - 8 + 8 + 8);
        pushDown();
        assertXEquals(65 - 8 - 8 + 8 + 8 - 4);
        pushDown();
        assertXEquals(65 - 8 - 8 + 8 + 8 - 4);
        pushDown();
        assertXEquals(65 - 8 - 8 + 8 + 8 - 4 - 4);
        pushUp();
        assertXEquals(65 - 8 - 8 + 8 + 8 - 4 - 4);
    }

    TEST_F(AdaptiveTrackTests, threeSequences) {
        settings.startingX = 0;
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
        assertXEquals(3);
    }

    TEST_F(AdaptiveTrackTests, varyingDownUpRule) {
        settings.startingX = 65;
        rule.at(0).runCount = 2;
        rule.at(0).stepSize = 8;
        rule.at(0).up = 1;
        rule.at(0).down = 2;
        rule.at(1).runCount = 1;
        rule.at(1).stepSize = 4;
        rule.at(1).up = 2;
        rule.at(1).down = 1;
        reset();
        pushDown();
        assertXEquals(65);
        pushDown();
        assertXEquals(65 - 8);
        pushDown();
        assertXEquals(65 - 8);
        pushDown();
        assertXEquals(65 - 8 - 8);
        pushUp();
        assertXEquals(65 - 8 - 8 + 8);
        pushUp();
        assertXEquals(65 - 8 - 8 + 8 + 8);
        pushDown();
        assertXEquals(65 - 8 - 8 + 8 + 8);
        pushDown();
        assertXEquals(65 - 8 - 8 + 8 + 8 - 4);
        pushDown();
        assertXEquals(65 - 8 - 8 + 8 + 8 - 4 - 4);
        pushUp();
        assertXEquals(65 - 8 - 8 + 8 + 8 - 4 - 4);
        pushUp();
        assertXEquals(65 - 8 - 8 + 8 + 8 - 4 - 4);
    }
    
    TEST_F(AdaptiveTrackTests, reversals) {
        settings.startingX = 0;
        rule.at(0).runCount = 1000;
        rule.at(0).down = 2;
        rule.at(0).up = 1;
        reset();
        EXPECT_EQ(int{0}, reversals());
        pushUp();
        EXPECT_EQ(int{0}, reversals());
        pushDown();
        EXPECT_EQ(int{0}, reversals());
        pushDown();
        EXPECT_EQ(1, reversals());
        pushUp();
        EXPECT_EQ(2, reversals());
        pushDown();
        EXPECT_EQ(2, reversals());
        pushDown();
        EXPECT_EQ(3, reversals());
    }
}
