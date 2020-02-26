#include "assert-utility.h"
#include <adaptive-track/AdaptiveTrack.hpp>
#include <gtest/gtest.h>

namespace adaptive_track::tests {
namespace {
void reset(AdaptiveTrack &track) { track.reset(); }

class AdaptiveTrackFacade {
    AdaptiveTrack track;

  public:
    explicit AdaptiveTrackFacade(const av_speech_in_noise::Track::Settings &s)
        : track{s} {}

    void update(const std::string &directions) {
        for (const auto &c : directions)
            if (c == 'd')
                down();
            else if (c == 'u')
                up();
    }

    void down() { track.down(); }

    void up() { track.up(); }

    auto x() { return track.x(); }

    auto complete() { return track.complete(); }

    auto reversals() { return track.reversals(); }

    template <typename T> void assertXEquals(T expected) {
        assertEqual(expected, x());
    }

    void assertComplete() { assertTrue(complete()); }

    void assertIncomplete() { assertFalse(complete()); }

    void assertReversalsEquals(int expected) {
        assertEqual(expected, reversals());
    }

    void reset() { tests::reset(track); }
};

void reset(AdaptiveTrackFacade &track) { track.reset(); }

class AdaptiveTrackTests : public ::testing::Test {
  protected:
    AdaptiveTrack::Settings settings{};
    av_speech_in_noise::TrackingRule rule;

    AdaptiveTrackTests() {
        settings.rule = &rule;
        rule.resize(3);
    }

    auto firstSequence() -> auto & { return rule.at(0); }

    auto secondSequence() -> auto & { return rule.at(1); }

    auto thirdSequence() -> auto & { return rule.at(2); }

    auto construct() -> AdaptiveTrackFacade {
        return AdaptiveTrackFacade{settings};
    }

    template <typename T> void setStartingX(T x) { settings.startingX = x; }

    void setCeiling(int x) { settings.ceiling = x; }

    void setBumpLimit(int n) { settings.bumpLimit = n; }

    void setFloor(int x) { settings.floor = x; }

    void setFirstSequenceStepSize(int x) { firstSequence().stepSize = x; }

    void setFirstSequenceRunCount(int x) { firstSequence().runCount = x; }

    void setFirstSequenceUp(int x) { firstSequence().up = x; }

    void setFirstSequenceDown(int x) { firstSequence().down = x; }

    static void down(AdaptiveTrackFacade &track) { track.down(); }

    static void up(AdaptiveTrackFacade &track) { track.up(); }

    template <typename T> void assertXEquals(AdaptiveTrackFacade &track, T x) {
        track.assertXEquals(x);
    }

    static void assertIncomplete(AdaptiveTrackFacade &track) {
        track.assertIncomplete();
    }

    static void assertComplete(AdaptiveTrackFacade &track) {
        track.assertComplete();
    }

    static void update(AdaptiveTrackFacade &track, const std::string &s) {
        track.update(s);
    }

    static void assertReversalsEquals(AdaptiveTrackFacade &track, int n) {
        track.assertReversalsEquals(n);
    }

    static void assertReversalsEqualsAfterDown(
        AdaptiveTrackFacade &track, int n) {
        down(track);
        assertReversalsEquals(track, n);
    }

    static void assertReversalsEqualsAfterUp(
        AdaptiveTrackFacade &track, int n) {
        up(track);
        assertReversalsEquals(track, n);
    }

    static void assertIncompleteAfterDown(AdaptiveTrackFacade &track) {
        down(track);
        assertIncomplete(track);
    }

    static void assertIncompleteAfterUp(AdaptiveTrackFacade &track) {
        up(track);
        assertIncomplete(track);
    }

    static void assertCompleteAfterUp(AdaptiveTrackFacade &track) {
        up(track);
        assertComplete(track);
    }

    static void assertCompleteAfterDown(AdaptiveTrackFacade &track) {
        down(track);
        assertComplete(track);
    }

    void assertXEqualsAfterDown(AdaptiveTrackFacade &track, int x) {
        down(track);
        assertXEquals(track, x);
    }

    void assertXEqualsAfterUp(AdaptiveTrackFacade &track, int x) {
        up(track);
        assertXEquals(track, x);
    }

    void assertXEqualsAfter(
        AdaptiveTrackFacade &track, const std::string &directions, int x) {
        update(track, directions);
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
    assertXEqualsAfterDown(track, 5);
    assertXEqualsAfterUp(track, 5);
}

TEST_F(AdaptiveTrackTests, stepsAccordingToStepSize1Down1Up) {
    setFirstSequenceUp(1);
    setFirstSequenceDown(1);
    setFirstSequenceStepSize(4);
    setFirstSequenceRunCount(999);
    setStartingX(5);
    auto track = construct();
    assertXEqualsAfterDown(track, 5 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4);
    assertXEqualsAfterDown(track, 5 - 4 + 4 - 4);
    assertXEqualsAfterDown(track, 5 - 4 + 4 - 4 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4 - 4 - 4 + 4);
}

TEST_F(AdaptiveTrackTests, stepsAccordingToStepSize2Down1Up) {
    setFirstSequenceUp(1);
    setFirstSequenceDown(2);
    setFirstSequenceStepSize(4);
    setFirstSequenceRunCount(999);
    setStartingX(5);
    auto track = construct();
    assertXEqualsAfterDown(track, 5);
    assertXEqualsAfterDown(track, 5 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4);
    assertXEqualsAfterDown(track, 5 - 4 + 4);
    assertXEqualsAfterDown(track, 5 - 4 + 4 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4 - 4 + 4);
}

TEST_F(AdaptiveTrackTests, stepsAccordingToStepSize1Down2Up) {
    setFirstSequenceDown(1);
    setFirstSequenceUp(2);
    setFirstSequenceStepSize(4);
    setFirstSequenceRunCount(999);
    setStartingX(5);
    auto track = construct();
    assertXEqualsAfterDown(track, 5 - 4);
    assertXEqualsAfterUp(track, 5 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4);
    assertXEqualsAfterDown(track, 5 - 4 + 4 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4 - 4 + 4);
}

TEST_F(AdaptiveTrackTests, exhaustedRunSequencesMeansNoMoreStepChanges) {
    setFirstSequenceUp(1);
    setFirstSequenceDown(1);
    setFirstSequenceRunCount(3);
    setFirstSequenceStepSize(4);
    setStartingX(5);
    auto track = construct();
    assertXEqualsAfter(track, "dudu", 5 - 4 + 4 - 4);
    assertXEqualsAfterDown(track, 5 - 4 + 4 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4 - 4);
}

TEST_F(AdaptiveTrackTests, floorActsAsLowerLimit) {
    setFloor(0);
    setFirstSequenceDown(1);
    setFirstSequenceStepSize(4);
    setStartingX(5);
    setFirstSequenceRunCount(999);
    auto track = construct();
    assertXEquals(track, 5);
    assertXEqualsAfterDown(track, 5 - 4);
    assertXEqualsAfterDown(track, 0);
}

TEST_F(AdaptiveTrackTests, ceilingActsAsUpperLimit) {
    setCeiling(10);
    setFirstSequenceUp(1);
    setFirstSequenceStepSize(4);
    setStartingX(5);
    setFirstSequenceRunCount(999);
    auto track = construct();
    assertXEquals(track, 5);
    assertXEqualsAfterUp(track, 5 + 4);
    assertXEqualsAfterUp(track, 10);
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
    assertIncompleteAfterDown(track);
    assertIncompleteAfterUp(track);
    assertIncompleteAfterDown(track);
    assertCompleteAfterUp(track);
}

TEST_F(
    AdaptiveTrackTests, completeIfPushedUpBumpLimitConsecutiveTimesAtCeiling) {
    setStartingX(10);
    setCeiling(10);
    setBumpLimit(3);
    setFirstSequenceRunCount(999);
    auto track = construct();
    assertIncompleteAfterUp(track);
    assertIncompleteAfterUp(track);
    assertCompleteAfterUp(track);
}

TEST_F(AdaptiveTrackTests,
    stillCompleteIfPushedUpBumpLimitPlusOneConsecutiveTimesAtCeiling) {
    setStartingX(10);
    setCeiling(10);
    setBumpLimit(3);
    setFirstSequenceRunCount(999);
    auto track = construct();
    update(track, "uuu");
    assertCompleteAfterUp(track);
}

TEST_F(AdaptiveTrackTests,
    stillCompleteIfPushedUpBumpLimitConsecutiveTimesAtCeilingThenPushedDown) {
    setStartingX(10);
    setCeiling(10);
    setBumpLimit(3);
    setFirstSequenceRunCount(999);
    auto track = construct();
    update(track, "uuu");
    assertCompleteAfterDown(track);
}

TEST_F(
    AdaptiveTrackTests, completeIfPushedUpBumpLimitConsecutiveTimesAtCeiling2) {
    setStartingX(5);
    setCeiling(7);
    setFirstSequenceStepSize(7 - 5);
    setFirstSequenceUp(1);
    setBumpLimit(3);
    setFirstSequenceRunCount(999);
    auto track = construct();
    assertXEquals(track, 5);
    assertXEqualsAfterUp(track, 7);
    assertIncompleteAfterUp(track);
    assertIncompleteAfterUp(track);
    assertCompleteAfterUp(track);
}

TEST_F(
    AdaptiveTrackTests, completeIfPushedDownBumpLimitConsecutiveTimesAtFloor) {
    setStartingX(-10);
    setFloor(-10);
    setBumpLimit(3);
    setFirstSequenceRunCount(999);
    auto track = construct();
    assertIncompleteAfterDown(track);
    assertIncompleteAfterDown(track);
    assertCompleteAfterDown(track);
}

TEST_F(AdaptiveTrackTests,
    stillCompleteIfPushedDownBumpLimitPlusOneConsecutiveTimesAtFloor) {
    setStartingX(-10);
    setFloor(-10);
    setBumpLimit(3);
    setFirstSequenceRunCount(999);
    auto track = construct();
    update(track, "ddd");
    assertCompleteAfterDown(track);
}

TEST_F(AdaptiveTrackTests,
    stillCompleteIfPushedDownBumpLimitConsecutiveTimesAtFloorThenPushedUp) {
    setStartingX(-10);
    setFloor(-10);
    setBumpLimit(3);
    setFirstSequenceRunCount(999);
    auto track = construct();
    update(track, "ddd");
    assertCompleteAfterUp(track);
}

TEST_F(
    AdaptiveTrackTests, completeIfPushedDownBumpLimitConsecutiveTimesAtFloor2) {
    setStartingX(-5);
    setFloor(-7);
    setFirstSequenceStepSize(7 - 5);
    setFirstSequenceDown(1);
    setBumpLimit(3);
    setFirstSequenceRunCount(999);
    auto track = construct();
    assertXEquals(track, -5);
    assertXEqualsAfterDown(track, -7);
    assertIncompleteAfterDown(track);
    assertIncompleteAfterDown(track);
    assertCompleteAfterDown(track);
}

TEST_F(AdaptiveTrackTests,
    incompleteIfPushedDownBumpLimitNonconsecutiveTimesAtFloor) {
    setStartingX(-5);
    setFloor(-5);
    setBumpLimit(3);
    setFirstSequenceRunCount(999);
    auto track = construct();
    update(track, "ddud");
    assertIncomplete(track);
}

TEST_F(AdaptiveTrackTests,
    incompleteIfPushedUpBumpLimitNonconsecutiveTimesAtCeiling) {
    setStartingX(5);
    setCeiling(5);
    setBumpLimit(3);
    setFirstSequenceRunCount(999);
    auto track = construct();
    update(track, "uudu");
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
    assertXEqualsAfter(track, "dduuuudduuuddddduuudduu", 1);
}

// see https://doi.org/10.1121/1.1912375
TEST_F(AdaptiveTrackTests, LevittFigure5) {
    setStartingX(0);
    setFirstSequenceRunCount(5);
    setFirstSequenceStepSize(1);
    setFirstSequenceDown(2);
    setFirstSequenceUp(1);
    auto track = construct();
    assertXEqualsAfter(track, "dddduduududdddduuuddddd", 1);
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
    assertXEqualsAfterDown(track, 65);
    assertXEqualsAfterDown(track, 65 - 8);
    assertXEqualsAfterDown(track, 65 - 8);
    assertXEqualsAfterDown(track, 65 - 8 - 8);
    assertXEqualsAfterUp(track, 65 - 8 - 8 + 8);
    assertXEqualsAfterUp(track, 65 - 8 - 8 + 8 + 8);
    assertXEqualsAfterDown(track, 65 - 8 - 8 + 8 + 8);
    assertXEqualsAfterDown(track, 65 - 8 - 8 + 8 + 8 - 4);
    assertXEqualsAfterDown(track, 65 - 8 - 8 + 8 + 8 - 4);
    assertXEqualsAfterDown(track, 65 - 8 - 8 + 8 + 8 - 4 - 4);
    assertXEqualsAfterUp(track, 65 - 8 - 8 + 8 + 8 - 4 - 4);
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
    assertXEqualsAfter(track, "ddudddudddddudddddduddd", 3);
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
    assertXEqualsAfterDown(track, 65);
    assertXEqualsAfterDown(track, 65 - 8);
    assertXEqualsAfterDown(track, 65 - 8);
    assertXEqualsAfterDown(track, 65 - 8 - 8);
    assertXEqualsAfterUp(track, 65 - 8 - 8 + 8);
    assertXEqualsAfterUp(track, 65 - 8 - 8 + 8 + 8);
    assertXEqualsAfterDown(track, 65 - 8 - 8 + 8 + 8);
    assertXEqualsAfterDown(track, 65 - 8 - 8 + 8 + 8 - 4);
    assertXEqualsAfterDown(track, 65 - 8 - 8 + 8 + 8 - 4 - 4);
    assertXEqualsAfterUp(track, 65 - 8 - 8 + 8 + 8 - 4 - 4);
    assertXEqualsAfterUp(track, 65 - 8 - 8 + 8 + 8 - 4 - 4);
}

TEST_F(AdaptiveTrackTests, reversals) {
    setStartingX(0);
    setFirstSequenceRunCount(1000);
    setFirstSequenceDown(2);
    setFirstSequenceUp(1);
    auto track = construct();
    assertReversalsEquals(track, 0);
    assertReversalsEqualsAfterUp(track, 0);
    assertReversalsEqualsAfterDown(track, 0);
    assertReversalsEqualsAfterDown(track, 1);
    assertReversalsEqualsAfterUp(track, 2);
    assertReversalsEqualsAfterDown(track, 2);
    assertReversalsEqualsAfterDown(track, 3);
}

TEST_F(AdaptiveTrackTests, sanityTest) {
    setStartingX(0);
    setFirstSequenceUp(1);
    setFirstSequenceDown(2);
    setFirstSequenceStepSize(3);
    setBumpLimit(5);
    setFloor(-10);
    setFirstSequenceRunCount(999);
    auto track = construct();
    assertIncomplete(track);
    assertXEqualsAfterDown(track, 0);
    assertXEqualsAfterDown(track, 0 - 3);
    assertXEqualsAfterDown(track, 0 - 3);
    assertXEqualsAfterDown(track, 0 - 3 - 3);
    assertXEqualsAfterDown(track, 0 - 3 - 3);
    assertXEqualsAfterDown(track, 0 - 3 - 3 - 3);
    assertXEqualsAfterDown(track, 0 - 3 - 3 - 3);
    assertXEqualsAfterDown(track, -10);
    assertIncompleteAfterDown(track);
    assertIncompleteAfterDown(track);
    assertIncompleteAfterDown(track);
    assertIncompleteAfterDown(track);
    assertIncompleteAfterUp(track);
    assertXEquals(track, -7);
    assertIncomplete(track);
    assertXEqualsAfterDown(track, -7);
    assertIncomplete(track);
    assertXEqualsAfterDown(track, -10);
    assertIncomplete(track);
    assertIncompleteAfterDown(track);
    assertIncompleteAfterDown(track);
    assertIncompleteAfterDown(track);
    assertIncompleteAfterDown(track);
    assertCompleteAfterDown(track);
    assertCompleteAfterUp(track);
    assertCompleteAfterUp(track);
    assertCompleteAfterDown(track);
    assertCompleteAfterDown(track);
    assertCompleteAfterUp(track);
    assertCompleteAfterDown(track);
    assertCompleteAfterDown(track);
    assertCompleteAfterUp(track);
    assertXEquals(track, -10);
}

TEST_F(AdaptiveTrackTests, reset) {
    setFirstSequenceUp(1);
    setFirstSequenceDown(1);
    setFirstSequenceStepSize(4);
    setFirstSequenceRunCount(999);
    setStartingX(5);
    auto track = construct();
    assertXEqualsAfterDown(track, 5 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4);
    assertXEqualsAfterDown(track, 5 - 4 + 4 - 4);
    assertXEqualsAfterDown(track, 5 - 4 + 4 - 4 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4 - 4 - 4 + 4);
    reset(track);
    assertXEqualsAfterDown(track, 5 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4);
    assertXEqualsAfterDown(track, 5 - 4 + 4 - 4);
    assertXEqualsAfterDown(track, 5 - 4 + 4 - 4 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4 - 4 - 4 + 4);
}
}
}