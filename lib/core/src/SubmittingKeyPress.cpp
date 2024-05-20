#include "SubmittingKeyPress.hpp"

#include <array>
#include <filesystem>

namespace av_speech_in_noise::submitting_keypress {
InteractorImpl::InteractorImpl(FixedLevelMethod &method, RunningATest &model,
    OutputFile &outputFile, MaskerPlayer &maskerPlayer, Randomizer &randomizer)
    : method{method}, model{model}, outputFile{outputFile},
      maskerPlayer{maskerPlayer}, randomizer{randomizer} {}

void InteractorImpl::notifyThatTrialWillBegin(int) {
    const std::array<double, 4> delaysSeconds = {.060, .110, .160, .190};
    const std::array<double, 2> durationsSeconds = {.100, .250};

    VibrotactileStimulus stimulus;
    stimulus.delay.seconds = delaysSeconds.at(
        randomizer.betweenInclusive(0, delaysSeconds.size() - 1));
    stimulus.duration.seconds = durationsSeconds.at(
        randomizer.betweenInclusive(0, durationsSeconds.size() - 1));
    stimulus.frequency.Hz = 250;
    maskerPlayer.prepareVibrotactileStimulus(stimulus);
}

void InteractorImpl::notifyThatTargetWillPlayAt(const PlayerTimeWithDelay &t) {
    targetStartTimeMilliseconds =
        static_cast<double>(
            (maskerPlayer.nanoseconds(t.playerTime) + 500000) / 1000000) +
        t.delay.seconds * 1000;
}

void InteractorImpl::submit(const KeyPressResponse &response) {
    method.submit(response);
    KeyPressTrial trial;
    static_cast<KeyPressResponse &>(trial) = response;
    trial.rt.milliseconds =
        response.seconds * 1000 - targetStartTimeMilliseconds;
    trial.target =
        std::filesystem::path{method.currentTarget().path}.filename();
    outputFile.write(trial);
    outputFile.save();
    model.prepareNextTrialIfNeeded();
}
}
