#include "EyeTracking.hpp"
#include "Configuration.hpp"
#include "RunningATest.hpp"

namespace av_speech_in_noise {
static auto nanoseconds(Delay x) -> std::uintmax_t {
    return gsl::narrow_cast<std::uintmax_t>(x.seconds * 1e9);
}

static auto nanoseconds(MaskerPlayer &player, const PlayerTime &t)
    -> std::uintmax_t {
    return player.nanoseconds(t);
}

static auto nanoseconds(MaskerPlayer &player, const PlayerTimeWithDelay &t)
    -> std::uintmax_t {
    return nanoseconds(player, t.playerTime) + nanoseconds(t.delay);
}

EyeTracking::EyeTracking(ConfigurationRegistry &registry,
    EyeTracker &eyeTracker, MaskerPlayer &maskerPlayer,
    TargetPlayer &targetPlayer, OutputFile &outputFile,
    RunningATest &runningATest)
    : eyeTracker{eyeTracker}, maskerPlayer{maskerPlayer},
      targetPlayer{targetPlayer}, outputFile{outputFile},
      runningATest{runningATest} {
    registry.subscribe(*this, "method");
}

void EyeTracking::notifyThatTrialWillBegin(int /*trialNumber*/) {
    eyeTracker.allocateRecordingTimeSeconds(
        Duration{trialDuration(targetPlayer, maskerPlayer)}.seconds);
    eyeTracker.start();
}

void EyeTracking::notifyThatTargetWillPlayAt(
    const PlayerTimeWithDelay &timeToPlayWithDelay) {
    lastTargetStartTime.nanoseconds =
        nanoseconds(maskerPlayer, timeToPlayWithDelay);

    lastEyeTrackerTargetPlayerSynchronization.eyeTrackerSystemTime =
        eyeTracker.currentSystemTime();
    lastEyeTrackerTargetPlayerSynchronization.targetPlayerSystemTime =
        TargetPlayerSystemTime{
            nanoseconds(maskerPlayer, maskerPlayer.currentSystemTime())};
}

void EyeTracking::notifyThatStimulusHasEnded() { eyeTracker.stop(); }

void EyeTracking::notifyThatSubjectHasResponded() {
    outputFile.write(lastTargetStartTime);
    outputFile.write(lastEyeTrackerTargetPlayerSynchronization);
    outputFile.write(eyeTracker.gazeSamples());
    outputFile.save();
}

void EyeTracking::notifyThatNewTestIsReady(std::string_view /*session*/) {}

void EyeTracking::configure(const std::string &key, const std::string &value) {
    if (key == "method") {
        if (contains(value, "eye tracking"))
            runningATest.add(*this);
        else
            runningATest.remove(*this);
    }
}
}
