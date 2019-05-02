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
        
        AdaptiveTrackFacade reset() {
            settings.rule = &rule;
            return AdaptiveTrackFacade{settings};
        }
        
        template<typename T>
        void setStartingX(T x) {
            settings.startingX = x;
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
        rule.at(0).runCount = 3;
        rule.at(0).stepSize = 4;
        rule.at(0).up = 1;
        rule.at(0).down = 2;
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

    TEST_F(AdaptiveTrackTests, completeWhenExhausted) {
        setStartingX(5);
        rule.at(0).runCount = 3;
        rule.at(0).stepSize = 4;
        rule.at(0).up = 1;
        rule.at(0).down = 2;
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
        rule.at(0).runCount = 8;
        rule.at(0).stepSize = 1;
        rule.at(0).down = 1;
        rule.at(0).up = 1;
        auto track = reset();
        track.push("dduuuudduuuddddduuudduu");
        track.assertXEquals(1);
    }

    // see https://doi.org/10.1121/1.1912375
    TEST_F(AdaptiveTrackTests, LevittFigure5) {
        setStartingX(0);
        rule.at(0).runCount = 5;
        rule.at(0).stepSize = 1;
        rule.at(0).down = 2;
        rule.at(0).up = 1;
        auto track = reset();
        track.push("dddduduududdddduuuddddd");
        track.assertXEquals(1);
    }

    TEST_F(AdaptiveTrackTests, twoSequences) {
        setStartingX(65);
        rule.at(0).runCount = 2;
        rule.at(0).stepSize = 8;
        rule.at(0).down = 2;
        rule.at(0).up = 1;
        rule.at(1).runCount = 1;
        rule.at(1).stepSize = 4;
        rule.at(1).down = 2;
        rule.at(1).up = 1;
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
        auto track = reset();
        track.push("ddudddudddddudddddduddd");
        track.assertXEquals(3);
    }

    TEST_F(AdaptiveTrackTests, varyingDownUpRule) {
        setStartingX(65);
        rule.at(0).runCount = 2;
        rule.at(0).stepSize = 8;
        rule.at(0).up = 1;
        rule.at(0).down = 2;
        rule.at(1).runCount = 1;
        rule.at(1).stepSize = 4;
        rule.at(1).up = 2;
        rule.at(1).down = 1;
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
        rule.at(0).runCount = 1000;
        rule.at(0).down = 2;
        rule.at(0).up = 1;
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
