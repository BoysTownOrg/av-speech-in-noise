#include <recognition-test/AdaptiveTrack.hpp>
#include <gtest/gtest.h>

namespace {
    class AdaptiveTrackTests : public ::testing::Test {
    protected:
        recognition_test::AdaptiveTrack::Parameters parameters{};
        recognition_test::AdaptiveTrack::Parameters::RunSequence sequence{};
        recognition_test::AdaptiveTrack track{};
        
        void reset() {
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
        sequence.runCount = 3;
        sequence.stepSize = 4;
        sequence.up = 1;
        sequence.down = 2;
        parameters.runSequences.push_back(sequence);
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
        sequence.runCount = 3;
        sequence.stepSize = 4;
        sequence.up = 1;
        sequence.down = 2;
        parameters.runSequences.push_back(sequence);
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
        sequence.runCount = 8;
        sequence.stepSize = 1;
        sequence.down = 1;
        sequence.up = 1;
        parameters.runSequences.push_back(sequence);
        reset();
        push("dduuuudduuuddddduuudduu");
        EXPECT_EQ(1, track.x());
    }

    // see https://doi.org/10.1121/1.1912375
    TEST_F(AdaptiveTrackTests, LevittFigure5) {
        parameters.startingX = 0;
        sequence.runCount = 5;
        sequence.stepSize = 1;
        sequence.down = 2;
        sequence.up = 1;
        parameters.runSequences.push_back(sequence);
        reset();
        push("dddduduududdddduuuddddd");
        EXPECT_EQ(1, track.x());
    }

    TEST_F(AdaptiveTrackTests, twoSequences) {
        parameters.startingX = 65;
        sequence.runCount = 2;
        sequence.stepSize = 8;
        sequence.down = 2;
        sequence.up = 1;
        parameters.runSequences.push_back(sequence);
        sequence.runCount = 1;
        sequence.stepSize = 4;
        sequence.down = 2;
        sequence.up = 1;
        parameters.runSequences.push_back(sequence);
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
        sequence.runCount = 1;
        sequence.stepSize = 10;
        sequence.down = 3;
        sequence.up = 1;
        parameters.runSequences.push_back(sequence);
        sequence.runCount = 1;
        sequence.stepSize = 5;
        sequence.down = 3;
        sequence.up = 1;
        parameters.runSequences.push_back(sequence);
        sequence.runCount = 6;
        sequence.stepSize = 2;
        sequence.down = 3;
        sequence.up = 1;
        parameters.runSequences.push_back(sequence);
        reset();
        push("ddudddudddddudddddduddd");
        EXPECT_EQ(3, track.x());
    }

    TEST_F(AdaptiveTrackTests, varyingDownUpRule) {
        parameters.startingX = 65;
        sequence.runCount = 2;
        sequence.stepSize = 8;
        sequence.up = 1;
        sequence.down = 2;
        parameters.runSequences.push_back(sequence);
        sequence.runCount = 1;
        sequence.stepSize = 4;
        sequence.up = 2;
        sequence.down = 1;
        parameters.runSequences.push_back(sequence);
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
        sequence.runCount = 1000;
        sequence.down = 2;
        sequence.up = 1;
        parameters.runSequences.push_back(sequence);
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
