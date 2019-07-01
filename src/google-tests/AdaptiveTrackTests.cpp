#include <adaptive-track/AdaptiveTrack.hpp>
#include <gtest/gtest.h>

namespace {
    class AdaptiveTrackFacade {
        adaptive_track::AdaptiveTrack track;
    public:
        explicit AdaptiveTrackFacade(const av_speech_in_noise::Track::Settings &s) :
            track{s} {}
        
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
        
        void assertComplete() {
            EXPECT_TRUE(complete());
        }
        
        void assertIncomplete() {
            EXPECT_FALSE(complete());
        }
        
        void assertReversalsEquals(int expected) {
            EXPECT_EQ(expected, reversals());
        }
    };
    
    class AdaptiveTrackTests : public ::testing::Test {
    protected:
        adaptive_track::AdaptiveTrack::Settings settings{};
        av_speech_in_noise::TrackingRule rule;
        
        AdaptiveTrackTests() {
            rule.resize(3);
            for (auto &sequence : rule)
                sequence.runCount = 0;
        }
        
        auto &firstSequence() {
            return rule.at(0);
        }
        
        auto &secondSequence() {
            return rule.at(1);
        }
        
        auto &thirdSequence() {
            return rule.at(2);
        }
        
        AdaptiveTrackFacade reset() {
            settings.rule = &rule;
            return AdaptiveTrackFacade{settings};
        }
        
        template<typename T>
        void setStartingX(T x) {
            settings.startingX = x;
        }
        
        void setCeiling(int x) {
            settings.ceiling = x;
        }
    };

    TEST_F(AdaptiveTrackTests, xEqualToStartingX) {
        setStartingX(1);
        auto track = reset();
        track.assertXEquals(1);
    }

    TEST_F(AdaptiveTrackTests, noRunSequencesMeansNoChanges) {
        setStartingX(5);
        auto track = reset();
        track.pushDown();
        track.assertXEquals(5);
        track.pushDown();
        track.assertXEquals(5);
        track.pushUp();
        track.assertXEquals(5);
    }

    TEST_F(AdaptiveTrackTests, exhaustedRunSequencesMeansNoMoreStepChanges) {
        setStartingX(5);
        firstSequence().runCount = 3;
        firstSequence().stepSize = 4;
        firstSequence().up = 1;
        firstSequence().down = 2;
        auto track = reset();
        track.pushDown();
        track.assertXEquals(5);
        track.pushDown();
        track.assertXEquals(5 - 4);
        track.pushUp();
        track.assertXEquals(5 - 4 + 4);
        track.pushDown();
        track.assertXEquals(5 - 4 + 4);
        track.pushDown();
        track.assertXEquals(5 - 4 + 4 - 4);
        track.pushUp();
        track.assertXEquals(5 - 4 + 4 - 4);
        track.pushDown();
        track.assertXEquals(5 - 4 + 4 - 4);
        track.pushDown();
        track.assertXEquals(5 - 4 + 4 - 4);
    }

    TEST_F(AdaptiveTrackTests, upperLimitActsAsCeiling) {
        setStartingX(5);
        setCeiling(10);
        firstSequence().runCount = 3;
        firstSequence().stepSize = 4;
        firstSequence().up = 1;
        firstSequence().down = 2;
        auto track = reset();
        track.assertXEquals(5);
        track.pushUp();
        track.assertXEquals(5 + 4);
        track.pushUp();
        track.assertXEquals(10);
    }

    TEST_F(AdaptiveTrackTests, completeWhenExhausted) {
        setStartingX(5);
        firstSequence().runCount = 3;
        firstSequence().stepSize = 4;
        firstSequence().up = 1;
        firstSequence().down = 2;
        auto track = reset();
        track.assertIncomplete();
        track.pushDown();
        track.assertIncomplete();
        track.pushDown();
        track.assertIncomplete();
        track.pushUp();
        track.assertIncomplete();
        track.pushDown();
        track.assertIncomplete();
        track.pushDown();
        track.assertIncomplete();
        track.pushUp();
        track.assertComplete();
    }

    // see https://doi.org/10.1121/1.1912375
    TEST_F(AdaptiveTrackTests, LevittFigure4) {
        setStartingX(0);
        firstSequence().runCount = 8;
        firstSequence().stepSize = 1;
        firstSequence().down = 1;
        firstSequence().up = 1;
        auto track = reset();
        track.push("dduuuudduuuddddduuudduu");
        track.assertXEquals(1);
    }

    // see https://doi.org/10.1121/1.1912375
    TEST_F(AdaptiveTrackTests, LevittFigure5) {
        setStartingX(0);
        firstSequence().runCount = 5;
        firstSequence().stepSize = 1;
        firstSequence().down = 2;
        firstSequence().up = 1;
        auto track = reset();
        track.push("dddduduududdddduuuddddd");
        track.assertXEquals(1);
    }

    TEST_F(AdaptiveTrackTests, twoSequences) {
        setStartingX(65);
        firstSequence().runCount = 2;
        firstSequence().stepSize = 8;
        firstSequence().down = 2;
        firstSequence().up = 1;
        secondSequence().runCount = 1;
        secondSequence().stepSize = 4;
        secondSequence().down = 2;
        secondSequence().up = 1;
        auto track = reset();
        track.pushDown();
        track.assertXEquals(65);
        track.pushDown();
        track.assertXEquals(65 - 8);
        track.pushDown();
        track.assertXEquals(65 - 8);
        track.pushDown();
        track.assertXEquals(65 - 8 - 8);
        track.pushUp();
        track.assertXEquals(65 - 8 - 8 + 8);
        track.pushUp();
        track.assertXEquals(65 - 8 - 8 + 8 + 8);
        track.pushDown();
        track.assertXEquals(65 - 8 - 8 + 8 + 8);
        track.pushDown();
        track.assertXEquals(65 - 8 - 8 + 8 + 8 - 4);
        track.pushDown();
        track.assertXEquals(65 - 8 - 8 + 8 + 8 - 4);
        track.pushDown();
        track.assertXEquals(65 - 8 - 8 + 8 + 8 - 4 - 4);
        track.pushUp();
        track.assertXEquals(65 - 8 - 8 + 8 + 8 - 4 - 4);
    }

    TEST_F(AdaptiveTrackTests, threeSequences) {
        setStartingX(0);
        firstSequence().runCount = 1;
        firstSequence().stepSize = 10;
        firstSequence().down = 3;
        firstSequence().up = 1;
        secondSequence().runCount = 1;
        secondSequence().stepSize = 5;
        secondSequence().down = 3;
        secondSequence().up = 1;
        thirdSequence().runCount = 6;
        thirdSequence().stepSize = 2;
        thirdSequence().down = 3;
        thirdSequence().up = 1;
        auto track = reset();
        track.push("ddudddudddddudddddduddd");
        track.assertXEquals(3);
    }

    TEST_F(AdaptiveTrackTests, varyingDownUpRule) {
        setStartingX(65);
        firstSequence().runCount = 2;
        firstSequence().stepSize = 8;
        firstSequence().up = 1;
        firstSequence().down = 2;
        secondSequence().runCount = 1;
        secondSequence().stepSize = 4;
        secondSequence().up = 2;
        secondSequence().down = 1;
        auto track = reset();
        track.pushDown();
        track.assertXEquals(65);
        track.pushDown();
        track.assertXEquals(65 - 8);
        track.pushDown();
        track.assertXEquals(65 - 8);
        track.pushDown();
        track.assertXEquals(65 - 8 - 8);
        track.pushUp();
        track.assertXEquals(65 - 8 - 8 + 8);
        track.pushUp();
        track.assertXEquals(65 - 8 - 8 + 8 + 8);
        track.pushDown();
        track.assertXEquals(65 - 8 - 8 + 8 + 8);
        track.pushDown();
        track.assertXEquals(65 - 8 - 8 + 8 + 8 - 4);
        track.pushDown();
        track.assertXEquals(65 - 8 - 8 + 8 + 8 - 4 - 4);
        track.pushUp();
        track.assertXEquals(65 - 8 - 8 + 8 + 8 - 4 - 4);
        track.pushUp();
        track.assertXEquals(65 - 8 - 8 + 8 + 8 - 4 - 4);
    }
    
    TEST_F(AdaptiveTrackTests, reversals) {
        setStartingX(0);
        firstSequence().runCount = 1000;
        firstSequence().down = 2;
        firstSequence().up = 1;
        auto track = reset();
        track.assertReversalsEquals(0);
        track.pushUp();
        track.assertReversalsEquals(0);
        track.pushDown();
        track.assertReversalsEquals(0);
        track.pushDown();
        track.assertReversalsEquals(1);
        track.pushUp();
        track.assertReversalsEquals(2);
        track.pushDown();
        track.assertReversalsEquals(2);
        track.pushDown();
        track.assertReversalsEquals(3);
    }
}
