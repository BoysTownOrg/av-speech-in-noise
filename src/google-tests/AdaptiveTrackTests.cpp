#include "assert-utility.h"
#include <adaptive-track/AdaptiveTrack.hpp>
#include <gtest/gtest.h>

namespace adaptive_track::tests {
    class AdaptiveTrackFacade {
        AdaptiveTrack track;
    public:
        explicit AdaptiveTrackFacade(
            const av_speech_in_noise::Track::Settings &s
        ) :
            track{s} {}
        
        void push(const std::string &directions) {
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
            assertEqual(expected, x());
        }
        
        void assertComplete() {
            assertTrue(complete());
        }
        
        void assertIncomplete() {
            assertFalse(complete());
        }
        
        void assertReversalsEquals(int expected) {
            assertEqual(expected, reversals());
        }
    };
    
    class AdaptiveTrackTests : public ::testing::Test {
    protected:
        AdaptiveTrack::Settings settings{};
        av_speech_in_noise::TrackingRule rule;
        
        AdaptiveTrackTests() {
            settings.rule = &rule;
            rule.resize(3);
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
        
        AdaptiveTrackFacade construct() {
            return AdaptiveTrackFacade{settings};
        }
        
        template<typename T>
        void setStartingX(T x) {
            settings.startingX = x;
        }
        
        void setCeiling(int x) {
            settings.ceiling = x;
        }
        
        void setBumpLimit(int n) {
            settings.bumpLimit = n;
        }
        
        void setFloor(int x) {
            settings.floor = x;
        }
        
        void setFirstSequenceStepSize(int x) {
            firstSequence().stepSize = x;
        }
        
        void setFirstSequenceRunCount(int x) {
            firstSequence().runCount = x;
        }
        
        void setFirstSequenceUp(int x) {
            firstSequence().up = x;
        }
        
        void setFirstSequenceDown(int x) {
            firstSequence().down = x;
        }
        
        void pushDown(AdaptiveTrackFacade &track) {
            track.pushDown();
        }
        
        void pushUp(AdaptiveTrackFacade &track) {
            track.pushUp();
        }
        
        template<typename T>
        void assertXEquals(AdaptiveTrackFacade &track, T x) {
            track.assertXEquals(x);
        }
        
        void assertIncomplete(AdaptiveTrackFacade &track) {
            track.assertIncomplete();
        }
        
        void assertComplete(AdaptiveTrackFacade &track) {
            track.assertComplete();
        }
        
        void push(AdaptiveTrackFacade &track, const std::string &s) {
            track.push(s);
        }
        
        void assertReversalsEquals(AdaptiveTrackFacade &track, int n) {
            track.assertReversalsEquals(n);
        }
        
        void assertIncompleteAfterPushDown(AdaptiveTrackFacade &track) {
            pushDown(track);
            assertIncomplete(track);
        }
        
        void assertIncompleteAfterPushUp(AdaptiveTrackFacade &track) {
            pushUp(track);
            assertIncomplete(track);
        }
        
        void assertCompleteAfterPushUp(AdaptiveTrackFacade &track) {
            pushUp(track);
            assertComplete(track);
        }
        
        void assertCompleteAfterPushDown(AdaptiveTrackFacade &track) {
            pushDown(track);
            assertComplete(track);
        }
        
        void assertXEqualsAfterPushDown(AdaptiveTrackFacade &track, int x) {
            pushDown(track);
            assertXEquals(track, x);
        }
    };

    TEST_F(AdaptiveTrackTests, xEqualToStartingX) {
        setStartingX(1);
        auto track = construct();
        assertXEquals(track, 1);
    }

    TEST_F(AdaptiveTrackTests, noRunSequencesMeansNoChanges) {
        setStartingX(5);
        auto track = construct();
        pushDown(track);
        assertXEquals(track, 5);
        pushUp(track);
        assertXEquals(track, 5);
    }

    TEST_F(AdaptiveTrackTests, stepsAccordingToStepSize1Down1Up) {
        setFirstSequenceUp(1);
        setFirstSequenceDown(1);
        setFirstSequenceStepSize(4);
        setFirstSequenceRunCount(999);
        setStartingX(5);
        auto track = construct();
        assertXEqualsAfterPushDown(track, 5 - 4);
        pushUp(track);
        assertXEquals(track, 5 - 4 + 4);
        assertXEqualsAfterPushDown(track, 5 - 4 + 4 - 4);
        assertXEqualsAfterPushDown(track, 5 - 4 + 4 - 4 - 4);
        pushUp(track);
        assertXEquals(track, 5 - 4 + 4 - 4 - 4 + 4);
    }

    TEST_F(AdaptiveTrackTests, stepsAccordingToStepSize2Down1Up) {
        setFirstSequenceUp(1);
        setFirstSequenceDown(2);
        setFirstSequenceStepSize(4);
        setFirstSequenceRunCount(999);
        setStartingX(5);
        auto track = construct();
        assertXEqualsAfterPushDown(track, 5);
        assertXEqualsAfterPushDown(track, 5 - 4);
        pushUp(track);
        assertXEquals(track, 5 - 4 + 4);
        assertXEqualsAfterPushDown(track, 5 - 4 + 4);
        assertXEqualsAfterPushDown(track, 5 - 4 + 4 - 4);
        pushUp(track);
        assertXEquals(track, 5 - 4 + 4 - 4 + 4);
    }

    TEST_F(AdaptiveTrackTests, stepsAccordingToStepSize1Down2Up) {
        setFirstSequenceDown(1);
        setFirstSequenceUp(2);
        setFirstSequenceStepSize(4);
        setFirstSequenceRunCount(999);
        setStartingX(5);
        auto track = construct();
        pushDown(track);
        assertXEquals(track, 5 - 4);
        pushUp(track);
        assertXEquals(track, 5 - 4);
        pushUp(track);
        assertXEquals(track, 5 - 4 + 4);
        pushDown(track);
        assertXEquals(track, 5 - 4 + 4 - 4);
        pushUp(track);
        assertXEquals(track, 5 - 4 + 4 - 4);
        pushUp(track);
        assertXEquals(track, 5 - 4 + 4 - 4 + 4);
    }

    TEST_F(AdaptiveTrackTests, exhaustedRunSequencesMeansNoMoreStepChanges) {
        setFirstSequenceUp(1);
        setFirstSequenceDown(1);
        setFirstSequenceRunCount(3);
        setFirstSequenceStepSize(4);
        setStartingX(5);
        auto track = construct();
        push(track, "dudu");
        assertXEquals(track, 5 - 4 + 4 - 4);
        pushDown(track);
        assertXEquals(track, 5 - 4 + 4 - 4);
        pushUp(track);
        assertXEquals(track, 5 - 4 + 4 - 4);
    }

    TEST_F(AdaptiveTrackTests, floorActsAsLowerLimit) {
        setFloor(0);
        setFirstSequenceDown(1);
        setFirstSequenceStepSize(4);
        setStartingX(5);
        setFirstSequenceRunCount(999);
        auto track = construct();
        assertXEquals(track, 5);
        pushDown(track);
        assertXEquals(track, 5 - 4);
        pushDown(track);
        assertXEquals(track, 0);
    }

    TEST_F(AdaptiveTrackTests, ceilingActsAsUpperLimit) {
        setCeiling(10);
        setFirstSequenceUp(1);
        setFirstSequenceStepSize(4);
        setStartingX(5);
        setFirstSequenceRunCount(999);
        auto track = construct();
        assertXEquals(track, 5);
        pushUp(track);
        assertXEquals(track, 5 + 4);
        pushUp(track);
        assertXEquals(track, 10);
    }

    TEST_F(AdaptiveTrackTests, incompleteWhenNonZeroRunCount) {
        setFirstSequenceRunCount(3);
        auto track = construct();
        assertIncomplete(track);
    }

    TEST_F(AdaptiveTrackTests, completeWhenExhausted) {
        setFirstSequenceUp(1);
        setFirstSequenceDown(1);
        setFirstSequenceRunCount(3);
        auto track = construct();
        assertIncompleteAfterPushDown(track);
        assertIncompleteAfterPushUp(track);
        assertIncompleteAfterPushDown(track);
        assertCompleteAfterPushUp(track);
    }

    TEST_F(AdaptiveTrackTests, completeIfPushedUpBumpLimitConsecutiveTimesAtCeiling) {
        setStartingX(10);
        setCeiling(10);
        setBumpLimit(3);
        setFirstSequenceRunCount(999);
        auto track = construct();
        assertIncompleteAfterPushUp(track);
        assertIncompleteAfterPushUp(track);
        assertCompleteAfterPushUp(track);
    }

    TEST_F(AdaptiveTrackTests, stillCompleteIfPushedUpBumpLimitPlusOneConsecutiveTimesAtCeiling) {
        setStartingX(10);
        setCeiling(10);
        setBumpLimit(3);
        setFirstSequenceRunCount(999);
        auto track = construct();
        push(track, "uuu");
        assertCompleteAfterPushUp(track);
    }

    TEST_F(AdaptiveTrackTests, stillCompleteIfPushedUpBumpLimitConsecutiveTimesAtCeilingThenPushedDown) {
        setStartingX(10);
        setCeiling(10);
        setBumpLimit(3);
        setFirstSequenceRunCount(999);
        auto track = construct();
        push(track, "uuu");
        assertCompleteAfterPushDown(track);
    }

    TEST_F(AdaptiveTrackTests, completeIfPushedUpBumpLimitConsecutiveTimesAtCeiling2) {
        setStartingX(5);
        setCeiling(7);
        setFirstSequenceStepSize(7 - 5);
        setFirstSequenceUp(1);
        setBumpLimit(3);
        setFirstSequenceRunCount(999);
        auto track = construct();
        assertXEquals(track, 5);
        pushUp(track);
        assertXEquals(track, 7);
        assertIncompleteAfterPushUp(track);
        assertIncompleteAfterPushUp(track);
        assertCompleteAfterPushUp(track);
    }

    TEST_F(AdaptiveTrackTests, completeIfPushedDownBumpLimitConsecutiveTimesAtFloor) {
        setStartingX(-10);
        setFloor(-10);
        setBumpLimit(3);
        setFirstSequenceRunCount(999);
        auto track = construct();
        assertIncompleteAfterPushDown(track);
        assertIncompleteAfterPushDown(track);
        assertCompleteAfterPushDown(track);
    }

    TEST_F(AdaptiveTrackTests, stillCompleteIfPushedDownBumpLimitPlusOneConsecutiveTimesAtFloor) {
        setStartingX(-10);
        setFloor(-10);
        setBumpLimit(3);
        setFirstSequenceRunCount(999);
        auto track = construct();
        push(track, "ddd");
        assertCompleteAfterPushDown(track);
    }

    TEST_F(AdaptiveTrackTests, stillCompleteIfPushedDownBumpLimitConsecutiveTimesAtFloorThenPushedUp) {
        setStartingX(-10);
        setFloor(-10);
        setBumpLimit(3);
        setFirstSequenceRunCount(999);
        auto track = construct();
        push(track, "ddd");
        assertCompleteAfterPushUp(track);
    }

    TEST_F(AdaptiveTrackTests, completeIfPushedDownBumpLimitConsecutiveTimesAtFloor2) {
        setStartingX(-5);
        setFloor(-7);
        setFirstSequenceStepSize(7 - 5);
        setFirstSequenceDown(1);
        setBumpLimit(3);
        setFirstSequenceRunCount(999);
        auto track = construct();
        assertXEquals(track, -5);
        pushDown(track);
        assertXEquals(track, -7);
        assertIncompleteAfterPushDown(track);
        assertIncompleteAfterPushDown(track);
        assertCompleteAfterPushDown(track);
    }

    TEST_F(AdaptiveTrackTests, incompleteIfPushedDownBumpLimitNonconsecutiveTimesAtFloor) {
        setStartingX(-5);
        setFloor(-5);
        setBumpLimit(3);
        setFirstSequenceRunCount(999);
        auto track = construct();
        push(track, "ddud");
        assertIncomplete(track);
    }

    TEST_F(AdaptiveTrackTests, incompleteIfPushedUpBumpLimitNonconsecutiveTimesAtCeiling) {
        setStartingX(5);
        setCeiling(5);
        setBumpLimit(3);
        setFirstSequenceRunCount(999);
        auto track = construct();
        push(track, "uudu");
        assertIncomplete(track);
    }

    // see https://doi.org/10.1121/1.1912375
    TEST_F(AdaptiveTrackTests, LevittFigure4) {
        setStartingX(0);
        setFirstSequenceRunCount(8);
        setFirstSequenceStepSize(1);
        setFirstSequenceDown(1);
        setFirstSequenceUp(1);
        auto track = construct();
        push(track, "dduuuudduuuddddduuudduu");
        assertXEquals(track, 1);
    }

    // see https://doi.org/10.1121/1.1912375
    TEST_F(AdaptiveTrackTests, LevittFigure5) {
        setStartingX(0);
        setFirstSequenceRunCount(5);
        setFirstSequenceStepSize(1);
        setFirstSequenceDown(2);
        setFirstSequenceUp(1);
        auto track = construct();
        push(track, "dddduduududdddduuuddddd");
        assertXEquals(track, 1);
    }

    TEST_F(AdaptiveTrackTests, twoSequences) {
        setStartingX(65);
        setFirstSequenceRunCount(2);
        setFirstSequenceStepSize(8);
        setFirstSequenceDown(2);
        setFirstSequenceUp(1);
        secondSequence().runCount = 1;
        secondSequence().stepSize = 4;
        secondSequence().down = 2;
        secondSequence().up = 1;
        auto track = construct();
        pushDown(track);
        assertXEquals(track, 65);
        pushDown(track);
        assertXEquals(track, 65 - 8);
        pushDown(track);
        assertXEquals(track, 65 - 8);
        pushDown(track);
        assertXEquals(track, 65 - 8 - 8);
        pushUp(track);
        assertXEquals(track, 65 - 8 - 8 + 8);
        pushUp(track);
        assertXEquals(track, 65 - 8 - 8 + 8 + 8);
        pushDown(track);
        assertXEquals(track, 65 - 8 - 8 + 8 + 8);
        pushDown(track);
        assertXEquals(track, 65 - 8 - 8 + 8 + 8 - 4);
        pushDown(track);
        assertXEquals(track, 65 - 8 - 8 + 8 + 8 - 4);
        pushDown(track);
        assertXEquals(track, 65 - 8 - 8 + 8 + 8 - 4 - 4);
        pushUp(track);
        assertXEquals(track, 65 - 8 - 8 + 8 + 8 - 4 - 4);
    }

    TEST_F(AdaptiveTrackTests, threeSequences) {
        setStartingX(0);
        setFirstSequenceRunCount(1);
        setFirstSequenceStepSize(10);
        setFirstSequenceDown(3);
        setFirstSequenceUp(1);
        secondSequence().runCount = 1;
        secondSequence().stepSize = 5;
        secondSequence().down = 3;
        secondSequence().up = 1;
        thirdSequence().runCount = 6;
        thirdSequence().stepSize = 2;
        thirdSequence().down = 3;
        thirdSequence().up = 1;
        auto track = construct();
        push(track, "ddudddudddddudddddduddd");
        assertXEquals(track, 3);
    }

    TEST_F(AdaptiveTrackTests, varyingDownUpRule) {
        setStartingX(65);
        setFirstSequenceRunCount(2);
        setFirstSequenceStepSize(8);
        setFirstSequenceUp(1);
        setFirstSequenceDown(2);
        secondSequence().runCount = 1;
        secondSequence().stepSize = 4;
        secondSequence().up = 2;
        secondSequence().down = 1;
        auto track = construct();
        pushDown(track);
        assertXEquals(track, 65);
        pushDown(track);
        assertXEquals(track, 65 - 8);
        pushDown(track);
        assertXEquals(track, 65 - 8);
        pushDown(track);
        assertXEquals(track, 65 - 8 - 8);
        pushUp(track);
        assertXEquals(track, 65 - 8 - 8 + 8);
        pushUp(track);
        assertXEquals(track, 65 - 8 - 8 + 8 + 8);
        pushDown(track);
        assertXEquals(track, 65 - 8 - 8 + 8 + 8);
        pushDown(track);
        assertXEquals(track, 65 - 8 - 8 + 8 + 8 - 4);
        pushDown(track);
        assertXEquals(track, 65 - 8 - 8 + 8 + 8 - 4 - 4);
        pushUp(track);
        assertXEquals(track, 65 - 8 - 8 + 8 + 8 - 4 - 4);
        pushUp(track);
        assertXEquals(track, 65 - 8 - 8 + 8 + 8 - 4 - 4);
    }
    
    TEST_F(AdaptiveTrackTests, reversals) {
        setStartingX(0);
        setFirstSequenceRunCount(1000);
        setFirstSequenceDown(2);
        setFirstSequenceUp(1);
        auto track = construct();
        assertReversalsEquals(track, 0);
        pushUp(track);
        assertReversalsEquals(track, 0);
        pushDown(track);
        assertReversalsEquals(track, 0);
        pushDown(track);
        assertReversalsEquals(track, 1);
        pushUp(track);
        assertReversalsEquals(track, 2);
        pushDown(track);
        assertReversalsEquals(track, 2);
        pushDown(track);
        assertReversalsEquals(track, 3);
    }
}
