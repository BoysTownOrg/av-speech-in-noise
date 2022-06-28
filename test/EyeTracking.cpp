#include "OutputFileStub.hpp"
#include "TargetPlayerStub.hpp"
#include "MaskerPlayerStub.hpp"
#include "EyeTrackerStub.hpp"
#include "assert-utility.hpp"
#include "av-speech-in-noise/core/Player.hpp"

#include <av-speech-in-noise/core/RecognitionTestModel.hpp>
#include <av-speech-in-noise/core/EyeTracking.hpp>

#include <gtest/gtest.h>

namespace av_speech_in_noise {
class EyeTrackingTests : public ::testing::Test {
  protected:
    EyeTrackerStub eyeTracker;
    MaskerPlayerStub maskerPlayer;
    TargetPlayerStub targetPlayer;
    OutputFileStub outputFile;
    EyeTracking eyeTracking{eyeTracker, maskerPlayer, targetPlayer, outputFile};
};

TEST_F(EyeTrackingTests,
    playTrialForTestWithEyeTrackingAllocatesTrialDurationsWorthRecordingTimeForEyeTracking) {
    targetPlayer.setDurationSeconds(3);
    maskerPlayer.setFadeTimeSeconds(4);
    eyeTracking.notifyThatTrialWillBegin(1);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(3 + 2 * 4. +
            RunningATestImpl::targetOnsetFringeDuration.seconds +
            RunningATestImpl::targetOffsetFringeDuration.seconds,
        eyeTracker.recordingTimeAllocatedSeconds());
}

TEST_F(EyeTrackingTests, startsTrackerWhenTrialWillBegin) {
    eyeTracking.notifyThatTrialWillBegin(1);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(eyeTracker.started());
}

TEST_F(EyeTrackingTests, startsTrackerAfterAllocatingRecordingTime) {
    eyeTracking.notifyThatTrialWillBegin(1);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"allocateRecordingTimeSeconds start "},
        string(eyeTracker.log()));
}

TEST_F(EyeTrackingTests, stopsTrackerWhenStimulusHasEnded) {
    eyeTracking.notifyThatStimulusHasEnded();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(eyeTracker.stopped());
}

TEST_F(EyeTrackingTests, passesTargetStartSystemTimeForConversion) {
    PlayerTimeWithDelay playerTime;
    playerTime.playerTime.system = 1;
    eyeTracking.notifyThatTargetWillPlayAt(playerTime);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(player_system_time_type{1},
        maskerPlayer.toNanosecondsSystemTime().at(0));
}
}
