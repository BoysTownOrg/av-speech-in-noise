#include "SubmittingKeyPress.hpp"

#include <array>
#include <filesystem>
#include <limits>

namespace av_speech_in_noise::submitting_keypress {
InteractorImpl::InteractorImpl(FixedLevelMethod &method, RunningATest &model,
    OutputFile &outputFile, MaskerPlayer &maskerPlayer, Randomizer &randomizer)
    : method{method}, model{model}, outputFile{outputFile},
      maskerPlayer{maskerPlayer}, randomizer{randomizer} {}

void InteractorImpl::notifyThatTrialWillBegin(int) {
    const std::array<double, 4> delaysSeconds = {.060, .110, .160, .190};
    const std::array<double, 2> durationsSeconds = {.100, .250};

    VibrotactileStimulus stimulus;
    stimulus.targetStartRelativeDelay.seconds = delaysSeconds.at(
        randomizer.betweenInclusive(0, delaysSeconds.size() - 1));
    stimulus.additionalPostFadeInDelay.seconds =
        RunningATest::targetOnsetFringeDuration.seconds;
    stimulus.duration.seconds = durationsSeconds.at(
        randomizer.betweenInclusive(0, durationsSeconds.size() - 1));
    stimulus.frequency.Hz = 250;
    maskerPlayer.prepareVibrotactileStimulus(stimulus);
    lastVibrotactileStimulus = stimulus;
}

void InteractorImpl::notifyThatTargetWillPlayAt(
    const PlayerTimeWithDelay &targetStart) {
    vibrotactileStartTimeMilliseconds =
        static_cast<double>(
            (maskerPlayer.nanoseconds(targetStart.playerTime) + 500000) /
            1000000) +
        (targetStart.delay.seconds +
            lastVibrotactileStimulus.targetStartRelativeDelay.seconds) *
            1000;
}

auto InteractorImpl::submits(const std::vector<KeyPressResponse> &responses)
    -> bool {
    if (!readyForResponse)
        return false;
    auto min{responses.end()};
    auto minReactionTimeMilliseconds{std::numeric_limits<double>::max()};
    for (auto it{responses.begin()}; it != responses.end(); ++it) {
        const auto rt{it->seconds * 1000 - vibrotactileStartTimeMilliseconds};
        if (rt >= 0 && rt < minReactionTimeMilliseconds) {
            min = it;
            minReactionTimeMilliseconds = rt;
        }
    }
    if (min == responses.end())
        return false;
    KeyPressResponse response = *min;
    method.submit(response);
    KeyPressTrial trial;
    static_cast<KeyPressResponse &>(trial) = response;
    trial.vibrotactileStimulus = lastVibrotactileStimulus;
    trial.rt.milliseconds = minReactionTimeMilliseconds;
    trial.target =
        std::filesystem::path{method.currentTarget().path}.filename();
    outputFile.write(trial);
    outputFile.save();
    model.prepareNextTrialIfNeeded();
    readyForResponse = false;
    return true;
}

void InteractorImpl::notifyThatStimulusHasEnded() { readyForResponse = true; }
}
