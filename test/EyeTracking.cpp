#include "OutputFileStub.hpp"
#include "EyeTrackerStub.hpp"
#include "MaskerPlayerStub.hpp"
#include "TargetPlayerStub.hpp"
#include "assert-utility.hpp"
#include "av-speech-in-noise/Model.hpp"
#include "av-speech-in-noise/core/Player.hpp"

#include <av-speech-in-noise/core/EyeTracking.hpp>
#include <av-speech-in-noise/core/RecognitionTestModel.hpp>

#include <gtest/gtest.h>

namespace av_speech_in_noise {
constexpr auto operator==(const Point2D &a, const Point2D &b) -> bool {
    return a.x == b.x && a.y == b.y;
}

constexpr auto operator==(const Point3D &a, const Point3D &b) -> bool {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

constexpr auto operator==(const GazePosition &a, const GazePosition &b)
    -> bool {
    return a.relativeTrackbox == b.relativeTrackbox &&
        a.relativeScreen == b.relativeScreen;
}

constexpr auto operator==(const GazeOrigin &a, const GazeOrigin &b) -> bool {
    return a.relativeTrackbox == b.relativeTrackbox;
}

constexpr auto operator==(const Gaze &a, const Gaze &b) -> bool {
    return a.origin == b.origin && a.position == b.position;
}

constexpr auto operator==(
    const BinocularGazeSample &a, const BinocularGazeSample &b) -> bool {
    return a.systemTime.microseconds == b.systemTime.microseconds &&
        a.left == b.left && a.right == b.right;
}

class EyeTrackingTests : public ::testing::Test {
  protected:
    EyeTrackerStub eyeTracker;
    MaskerPlayerStub maskerPlayer;
    TargetPlayerStub targetPlayer;
    OutputFileStub outputFile;
    EyeTracking eyeTracking{eyeTracker, maskerPlayer, targetPlayer, outputFile};
};

#define EYE_TRACKING_TEST(a) TEST_F(EyeTrackingTests, a)

EYE_TRACKING_TEST(
    playTrialForTestWithEyeTrackingAllocatesTrialDurationsWorthRecordingTimeForEyeTracking) {
    targetPlayer.setDurationSeconds(3);
    maskerPlayer.setFadeTimeSeconds(4);
    eyeTracking.notifyThatTrialWillBegin(1);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(3 + 2 * 4. +
            RunningATestImpl::targetOnsetFringeDuration.seconds +
            RunningATestImpl::targetOffsetFringeDuration.seconds,
        eyeTracker.recordingTimeAllocatedSeconds());
}

EYE_TRACKING_TEST(startsTrackerWhenTrialWillBegin) {
    eyeTracking.notifyThatTrialWillBegin(1);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(eyeTracker.started());
}

EYE_TRACKING_TEST(startsTrackerAfterAllocatingRecordingTime) {
    eyeTracking.notifyThatTrialWillBegin(1);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"allocateRecordingTimeSeconds start "},
        string(eyeTracker.log()));
}

EYE_TRACKING_TEST(stopsTrackerWhenStimulusHasEnded) {
    eyeTracking.notifyThatStimulusHasEnded();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(eyeTracker.stopped());
}

EYE_TRACKING_TEST(passesTargetStartSystemTimeForConversion) {
    PlayerTimeWithDelay playerTime;
    playerTime.playerTime.system = 1;
    eyeTracking.notifyThatTargetWillPlayAt(playerTime);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(player_system_time_type{1},
        maskerPlayer.toNanosecondsSystemTime().at(0));
}

EYE_TRACKING_TEST(submittingCoordinateResponseWritesEyeGazes) {
    eyeTracker.setGazes({{{1}, {{}, {{}, {2, 3}}}, {{}, {{}, {4, 5}}}},
        {{6}, {{}, {{}, {7, 8}}}, {{}, {{}, {9, 10}}}}});
    eyeTracking.notifyThatSubjectHasResponded();
    ::assertEqual({{{1}, {{}, {{}, {2, 3}}}, {{}, {{}, {4, 5}}}},
                      {{6}, {{}, {{}, {7, 8}}}, {{}, {{}, {9, 10}}}}},
        outputFile.eyeGazes());
}

EYE_TRACKING_TEST(
    submitCoordinateResponseWritesTargetStartTimeWhenEyeTracking) {
    maskerPlayer.setNanosecondsFromPlayerTime(1);
    PlayerTimeWithDelay playerTime;
    playerTime.delay.seconds = 2;
    eyeTracking.notifyThatTargetWillPlayAt(playerTime);
    eyeTracking.notifyThatSubjectHasResponded();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        1 + gsl::narrow_cast<std::uintmax_t>(2 * 1e9),
        outputFile.targetStartTime().nanoseconds);
}

static auto eyeTrackerTargetPlayerSynchronization(OutputFileStub &file)
    -> EyeTrackerTargetPlayerSynchronization {
    return file.eyeTrackerTargetPlayerSynchronization();
}

EYE_TRACKING_TEST(submitCoordinateResponseWritesSyncTimes) {
    maskerPlayer.setNanosecondsFromPlayerTime(1);
    eyeTracker.setCurrentSystemTime(EyeTrackerSystemTime{2});
    eyeTracking.notifyThatTargetWillPlayAt({});
    eyeTracking.notifyThatSubjectHasResponded();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::uintmax_t{1},
        eyeTrackerTargetPlayerSynchronization(outputFile)
            .targetPlayerSystemTime.nanoseconds);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::int_least64_t{2},
        eyeTrackerTargetPlayerSynchronization(outputFile)
            .eyeTrackerSystemTime.microseconds);
}

EYE_TRACKING_TEST(passesCurrentMaskerTimeForNanosecondConversion) {
    av_speech_in_noise::PlayerTime t{};
    t.system = 1;
    maskerPlayer.setCurrentSystemTime(t);
    eyeTracking.notifyThatTargetWillPlayAt({});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(player_system_time_type{1},
        maskerPlayer.toNanosecondsSystemTime().at(1));
}
}
