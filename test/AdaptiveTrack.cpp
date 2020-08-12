#include "assert-utility.hpp"
#include <adaptive-track/AdaptiveTrack.hpp>
#include <gtest/gtest.h>
#include <cmath>

namespace adaptive_track {
namespace {
void reset(AdaptiveTrack &track) { track.reset(); }

void down(AdaptiveTrack &track) { track.down(); }

void up(AdaptiveTrack &track) { track.up(); }

void assertXEquals(AdaptiveTrack &track, int expected) {
    assertEqual(expected, track.x());
}

auto complete(AdaptiveTrack &track) -> bool { return track.complete(); }

void assertIncomplete(AdaptiveTrack &track) { AV_SPEECH_IN_NOISE_EXPECT_FALSE(complete(track)); }

void assertComplete(AdaptiveTrack &track) { AV_SPEECH_IN_NOISE_EXPECT_TRUE(complete(track)); }

void update(AdaptiveTrack &track, const std::string &directions) {
    for (const auto &c : directions)
        if (c == 'd')
            down(track);
        else if (c == 'u')
            up(track);
}

void assertReversalsEquals(AdaptiveTrack &track, int n) {
    assertEqual(n, track.reversals());
}

void assertReversalsEqualsAfterDown(AdaptiveTrack &track, int n) {
    down(track);
    assertReversalsEquals(track, n);
}

void assertReversalsEqualsAfterUp(AdaptiveTrack &track, int n) {
    up(track);
    assertReversalsEquals(track, n);
}

void assertIncompleteAfterDown(AdaptiveTrack &track) {
    down(track);
    assertIncomplete(track);
}

void assertIncompleteAfterUp(AdaptiveTrack &track) {
    up(track);
    assertIncomplete(track);
}

void assertCompleteAfterUp(AdaptiveTrack &track) {
    up(track);
    assertComplete(track);
}

void assertCompleteAfterDown(AdaptiveTrack &track) {
    down(track);
    assertComplete(track);
}

void assertXEqualsAfterDown(AdaptiveTrack &track, int x) {
    down(track);
    assertXEquals(track, x);
}

void assertXEqualsAfterUp(AdaptiveTrack &track, int x) {
    up(track);
    assertXEquals(track, x);
}

void assertXEqualsAfter(
    AdaptiveTrack &track, const std::string &directions, int x) {
    update(track, directions);
    assertXEquals(track, x);
}

void assertThresholdEquals(AdaptiveTrack &track, int lastReversals, double x) {
    assertEqual(x, track.threshold(lastReversals));
}

auto construct(const AdaptiveTrack::Settings &settings) -> AdaptiveTrack {
    return AdaptiveTrack{settings};
}

class AdaptiveTrackTests : public ::testing::Test {
  protected:
    AdaptiveTrack::Settings settings{};
    av_speech_in_noise::TrackingRule rule;

    AdaptiveTrackTests() {
        settings.rule = &rule;
        rule.resize(3);
        setFirstSequenceRunCount(999);
    }

    auto firstSequence() -> auto & { return rule.at(0); }

    auto secondSequence() -> auto & { return rule.at(1); }

    auto thirdSequence() -> auto & { return rule.at(2); }

    void setStartingX(int x) { settings.startingX = x; }

    void setCeiling(int x) { settings.ceiling = x; }

    void setBumpLimit(int n) { settings.bumpLimit = n; }

    void setFloor(int x) { settings.floor = x; }

    void setFirstSequenceStepSize(int x) { firstSequence().stepSize = x; }

    void setFirstSequenceRunCount(int x) { firstSequence().runCount = x; }

    void setFirstSequenceUp(int x) { firstSequence().up = x; }

    void setFirstSequenceDown(int x) { firstSequence().down = x; }
};

#define ADAPTIVE_TRACK_TEST(a) TEST_F(AdaptiveTrackTests, a)

ADAPTIVE_TRACK_TEST(xEqualToStartingX) {
    setStartingX(1);
    auto track{construct(settings)};
    assertXEquals(track, 1);
}

ADAPTIVE_TRACK_TEST(noRunSequencesMeansNoChanges) {
    setStartingX(5);
    auto track{construct(settings)};
    assertXEqualsAfterDown(track, 5);
    assertXEqualsAfterUp(track, 5);
}

ADAPTIVE_TRACK_TEST(stepsAccordingToStepSize1Down1Up) {
    setFirstSequenceUp(1);
    setFirstSequenceDown(1);
    setFirstSequenceStepSize(4);
    setStartingX(5);
    auto track{construct(settings)};
    assertXEqualsAfterDown(track, 5 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4);
    assertXEqualsAfterDown(track, 5 - 4 + 4 - 4);
    assertXEqualsAfterDown(track, 5 - 4 + 4 - 4 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4 - 4 - 4 + 4);
}

ADAPTIVE_TRACK_TEST(stepsAccordingToStepSize2Down1Up) {
    setFirstSequenceUp(1);
    setFirstSequenceDown(2);
    setFirstSequenceStepSize(4);
    setStartingX(5);
    auto track{construct(settings)};
    assertXEqualsAfterDown(track, 5);
    assertXEqualsAfterDown(track, 5 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4);
    assertXEqualsAfterDown(track, 5 - 4 + 4);
    assertXEqualsAfterDown(track, 5 - 4 + 4 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4 - 4 + 4);
}

ADAPTIVE_TRACK_TEST(stepsAccordingToStepSize1Down2Up) {
    setFirstSequenceDown(1);
    setFirstSequenceUp(2);
    setFirstSequenceStepSize(4);
    setStartingX(5);
    auto track{construct(settings)};
    assertXEqualsAfterDown(track, 5 - 4);
    assertXEqualsAfterUp(track, 5 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4);
    assertXEqualsAfterDown(track, 5 - 4 + 4 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4 - 4 + 4);
}

ADAPTIVE_TRACK_TEST(exhaustedRunSequencesMeansNoMoreStepChanges) {
    setFirstSequenceUp(1);
    setFirstSequenceDown(1);
    setFirstSequenceRunCount(3);
    setFirstSequenceStepSize(4);
    setStartingX(5);
    auto track{construct(settings)};
    assertXEqualsAfter(track, "dudu", 5 - 4 + 4 - 4);
    assertXEqualsAfterDown(track, 5 - 4 + 4 - 4);
    assertXEqualsAfterUp(track, 5 - 4 + 4 - 4);
}

ADAPTIVE_TRACK_TEST(floorActsAsLowerLimit) {
    setFloor(0);
    setFirstSequenceDown(1);
    setFirstSequenceStepSize(4);
    setStartingX(5);
    auto track{construct(settings)};
    assertXEquals(track, 5);
    assertXEqualsAfterDown(track, 5 - 4);
    assertXEqualsAfterDown(track, 0);
}

ADAPTIVE_TRACK_TEST(ceilingActsAsUpperLimit) {
    setCeiling(10);
    setFirstSequenceUp(1);
    setFirstSequenceStepSize(4);
    setStartingX(5);
    auto track{construct(settings)};
    assertXEquals(track, 5);
    assertXEqualsAfterUp(track, 5 + 4);
    assertXEqualsAfterUp(track, 10);
}

ADAPTIVE_TRACK_TEST(incompleteWhenNonZeroRunCount) {
    setFirstSequenceRunCount(3);
    auto track{construct(settings)};
    assertIncomplete(track);
}

ADAPTIVE_TRACK_TEST(completeWhenExhausted) {
    setFirstSequenceUp(1);
    setFirstSequenceDown(1);
    setFirstSequenceRunCount(3);
    auto track{construct(settings)};
    assertIncompleteAfterDown(track);
    assertIncompleteAfterUp(track);
    assertIncompleteAfterDown(track);
    assertCompleteAfterUp(track);
}

ADAPTIVE_TRACK_TEST(completeIfPushedUpBumpLimitConsecutiveTimesAtCeiling) {
    setStartingX(10);
    setCeiling(10);
    setBumpLimit(3);
    auto track{construct(settings)};
    assertIncompleteAfterUp(track);
    assertIncompleteAfterUp(track);
    assertCompleteAfterUp(track);
}

ADAPTIVE_TRACK_TEST(
    stillCompleteIfPushedUpBumpLimitPlusOneConsecutiveTimesAtCeiling) {
    setStartingX(10);
    setCeiling(10);
    setBumpLimit(3);
    auto track{construct(settings)};
    update(track, "uuu");
    assertCompleteAfterUp(track);
}

ADAPTIVE_TRACK_TEST(
    stillCompleteIfPushedUpBumpLimitConsecutiveTimesAtCeilingThenPushedDown) {
    setStartingX(10);
    setCeiling(10);
    setBumpLimit(3);
    auto track{construct(settings)};
    update(track, "uuu");
    assertCompleteAfterDown(track);
}

ADAPTIVE_TRACK_TEST(completeIfPushedUpBumpLimitConsecutiveTimesAtCeiling2) {
    setStartingX(5);
    setCeiling(7);
    setFirstSequenceStepSize(7 - 5);
    setFirstSequenceUp(1);
    setBumpLimit(3);
    auto track{construct(settings)};
    assertXEquals(track, 5);
    assertXEqualsAfterUp(track, 7);
    assertIncompleteAfterUp(track);
    assertIncompleteAfterUp(track);
    assertCompleteAfterUp(track);
}

ADAPTIVE_TRACK_TEST(completeIfPushedDownBumpLimitConsecutiveTimesAtFloor) {
    setStartingX(-10);
    setFloor(-10);
    setBumpLimit(3);
    auto track{construct(settings)};
    assertIncompleteAfterDown(track);
    assertIncompleteAfterDown(track);
    assertCompleteAfterDown(track);
}

ADAPTIVE_TRACK_TEST(
    stillCompleteIfPushedDownBumpLimitPlusOneConsecutiveTimesAtFloor) {
    setStartingX(-10);
    setFloor(-10);
    setBumpLimit(3);
    auto track{construct(settings)};
    update(track, "ddd");
    assertCompleteAfterDown(track);
}

ADAPTIVE_TRACK_TEST(
    stillCompleteIfPushedDownBumpLimitConsecutiveTimesAtFloorThenPushedUp) {
    setStartingX(-10);
    setFloor(-10);
    setBumpLimit(3);
    auto track{construct(settings)};
    update(track, "ddd");
    assertCompleteAfterUp(track);
}

ADAPTIVE_TRACK_TEST(completeIfPushedDownBumpLimitConsecutiveTimesAtFloor2) {
    setStartingX(-5);
    setFloor(-7);
    setFirstSequenceStepSize(7 - 5);
    setFirstSequenceDown(1);
    setBumpLimit(3);
    auto track{construct(settings)};
    assertXEquals(track, -5);
    assertXEqualsAfterDown(track, -7);
    assertIncompleteAfterDown(track);
    assertIncompleteAfterDown(track);
    assertCompleteAfterDown(track);
}

ADAPTIVE_TRACK_TEST(incompleteIfPushedDownBumpLimitNonconsecutiveTimesAtFloor) {
    setStartingX(-5);
    setFloor(-5);
    setBumpLimit(3);
    auto track{construct(settings)};
    update(track, "ddud");
    assertIncomplete(track);
}

ADAPTIVE_TRACK_TEST(incompleteIfPushedUpBumpLimitNonconsecutiveTimesAtCeiling) {
    setStartingX(5);
    setCeiling(5);
    setBumpLimit(3);
    auto track{construct(settings)};
    update(track, "uudu");
    assertIncomplete(track);
}

ADAPTIVE_TRACK_TEST(threshold) {
    setStartingX(0);
    setFirstSequenceRunCount(7);
    setFirstSequenceStepSize(3);
    setFirstSequenceDown(2);
    setFirstSequenceUp(1);
    auto track{construct(settings)};
    update(track, "dduudd");
    update(track, "uuuu");
    assertXEquals(track, 12);
    update(track, "dddd");
    assertXEquals(track, 6);
    up(track);
    assertXEquals(track, 9);
    update(track, "dddd");
    assertXEquals(track, 3);
    up(track);
    assertThresholdEquals(track, 4, (12 + 6 + 9 + 3) / 4.);
}

ADAPTIVE_TRACK_TEST(thresholdFromTwoSequences) {
    setStartingX(0);
    setFirstSequenceRunCount(4);
    setFirstSequenceStepSize(3);
    setFirstSequenceDown(2);
    setFirstSequenceUp(1);
    secondSequence().runCount = 5;
    secondSequence().stepSize = 6;
    secondSequence().down = 2;
    secondSequence().up = 1;
    auto track{construct(settings)};
    update(track, "dduudddduuu");
    assertXEquals(track, 6);
    update(track, "dd");
    assertXEquals(track, 0);
    update(track, "uuuu");
    assertXEquals(track, 24);
    update(track, "dd");
    assertXEquals(track, 18);
    update(track, "uuu");
    assertXEquals(track, 36);
    update(track, "dd");
    assertXEquals(track, 30);
    up(track);
    assertThresholdEquals(track, 6, (6 + 0 + 24 + 18 + 36 + 30) / 6.);
}

ADAPTIVE_TRACK_TEST(thresholdTooManyReversals) {
    setStartingX(0);
    setFirstSequenceRunCount(4);
    setFirstSequenceStepSize(3);
    setFirstSequenceDown(2);
    setFirstSequenceUp(1);
    auto track{construct(settings)};
    update(track, "dddddd");
    assertXEquals(track, -9);
    update(track, "uu");
    assertXEquals(track, -3);
    update(track, "dd");
    assertXEquals(track, -6);
    update(track, "uuu");
    assertXEquals(track, 3);
    update(track, "dd");
    assertThresholdEquals(track, 5, (-9 - 3 - 6 + 3) / 4.);
}

ADAPTIVE_TRACK_TEST(thresholdNegativeReversals) {
    setStartingX(0);
    setFirstSequenceRunCount(4);
    setFirstSequenceStepSize(3);
    setFirstSequenceDown(2);
    setFirstSequenceUp(1);
    auto track{construct(settings)};
    update(track, "dduddudd");
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(std::isnan(track.threshold(-1)));
}

// https://doi.org/10.1121/1.1912375
ADAPTIVE_TRACK_TEST(LevittFigure4) {
    setStartingX(0);
    setFirstSequenceRunCount(8);
    setFirstSequenceStepSize(1);
    setFirstSequenceDown(1);
    setFirstSequenceUp(1);
    auto track{construct(settings)};
    assertXEqualsAfter(track, "dduuuudduuuddddduuudduu", 1);
}

// https://doi.org/10.1121/1.1912375
ADAPTIVE_TRACK_TEST(LevittFigure5) {
    setStartingX(0);
    setFirstSequenceRunCount(5);
    setFirstSequenceStepSize(1);
    setFirstSequenceDown(2);
    setFirstSequenceUp(1);
    auto track{construct(settings)};
    assertXEqualsAfter(track, "dddduduududdddduuuddddd", 1);
}

ADAPTIVE_TRACK_TEST(twoSequences) {
    setStartingX(65);
    setFirstSequenceRunCount(2);
    setFirstSequenceStepSize(8);
    setFirstSequenceDown(2);
    setFirstSequenceUp(1);
    secondSequence().runCount = 1;
    secondSequence().stepSize = 4;
    secondSequence().down = 2;
    secondSequence().up = 1;
    auto track{construct(settings)};
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

ADAPTIVE_TRACK_TEST(threeSequences) {
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
    auto track{construct(settings)};
    assertXEqualsAfter(track, "ddudddudddddudddddduddd", 3);
}

ADAPTIVE_TRACK_TEST(varyingDownUpRule) {
    setStartingX(65);
    setFirstSequenceRunCount(2);
    setFirstSequenceStepSize(8);
    setFirstSequenceUp(1);
    setFirstSequenceDown(2);
    secondSequence().runCount = 1;
    secondSequence().stepSize = 4;
    secondSequence().up = 2;
    secondSequence().down = 1;
    auto track{construct(settings)};
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

ADAPTIVE_TRACK_TEST(reversals) {
    setStartingX(0);
    setFirstSequenceDown(2);
    setFirstSequenceUp(1);
    auto track{construct(settings)};
    assertReversalsEquals(track, 0);
    assertReversalsEqualsAfterUp(track, 0);
    assertReversalsEqualsAfterDown(track, 0);
    assertReversalsEqualsAfterDown(track, 1);
    assertReversalsEqualsAfterUp(track, 2);
    assertReversalsEqualsAfterDown(track, 2);
    assertReversalsEqualsAfterDown(track, 3);
}

ADAPTIVE_TRACK_TEST(sanityTest) {
    setStartingX(0);
    setFirstSequenceUp(1);
    setFirstSequenceDown(2);
    setFirstSequenceStepSize(3);
    setBumpLimit(5);
    setFloor(-10);
    auto track{construct(settings)};
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

ADAPTIVE_TRACK_TEST(resetResetsReversals) {
    setFirstSequenceUp(1);
    setFirstSequenceDown(1);
    auto track{construct(settings)};
    down(track);
    up(track);
    reset(track);
    assertReversalsEquals(track, 0);
    down(track);
    assertReversalsEquals(track, 0);
    up(track);
    assertReversalsEquals(track, 1);
}

ADAPTIVE_TRACK_TEST(twoSequencesWithReset) {
    setStartingX(65);
    setFirstSequenceRunCount(2);
    setFirstSequenceStepSize(8);
    setFirstSequenceDown(2);
    setFirstSequenceUp(1);
    secondSequence().runCount = 1;
    secondSequence().stepSize = 4;
    secondSequence().down = 2;
    secondSequence().up = 1;
    auto track{construct(settings)};
    update(track, "dddduuddddu");
    reset(track);
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

ADAPTIVE_TRACK_TEST(twoSequencesWithReset2) {
    setStartingX(65);
    setFirstSequenceRunCount(2);
    setFirstSequenceStepSize(8);
    setFirstSequenceDown(2);
    setFirstSequenceUp(1);
    secondSequence().runCount = 1;
    secondSequence().stepSize = 4;
    secondSequence().down = 2;
    secondSequence().up = 1;
    auto track{construct(settings)};
    update(track, "dddduud");
    reset(track);
    assertXEqualsAfterDown(track, 65);
    assertXEqualsAfterDown(track, 65 - 8);
    assertXEqualsAfterDown(track, 65 - 8);
    assertXEqualsAfterDown(track, 65 - 8 - 8);
    assertXEqualsAfterUp(track, 65 - 8 - 8 + 8);
}

ADAPTIVE_TRACK_TEST(
    completeIfPushedDownBumpLimitConsecutiveTimesAtFloorAfterReset) {
    setStartingX(-10);
    setFloor(-10);
    setBumpLimit(3);
    auto track{construct(settings)};
    down(track);
    reset(track);
    assertIncompleteAfterDown(track);
    assertIncompleteAfterDown(track);
    assertCompleteAfterDown(track);
}
}
}
