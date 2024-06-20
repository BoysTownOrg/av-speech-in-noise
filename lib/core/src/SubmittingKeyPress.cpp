#include "SubmittingKeyPress.hpp"

#include <array>
#include <filesystem>
#include <limits>

namespace av_speech_in_noise::submitting_keypress {
InteractorImpl::InteractorImpl(FixedLevelMethod &method, RunningATest &model,
    OutputFile &outputFile, MaskerPlayer &maskerPlayer, Randomizer &randomizer)
    : method{method}, model{model}, outputFile{outputFile},
      maskerPlayer{maskerPlayer}, randomizer{randomizer} {}

template <std::size_t N>
auto randomSelection(Randomizer &randomizer, std::array<double, N> x)
    -> double {
    return x.at(randomizer.betweenInclusive(0, x.size() - 1));
}

void InteractorImpl::notifyThatTrialWillBegin(int) {
    const std::array<double, 4> delaysSeconds = {2.060, 2.110, 2.160, 2.190};
    const std::array<double, 2> durationsSeconds = {.100, .250};

    VibrotactileStimulus stimulus;
    stimulus.targetStartRelativeDelay.seconds =
        randomSelection(randomizer, delaysSeconds);
    stimulus.additionalPostFadeInDelay.seconds =
        RunningATest::targetOnsetFringeDuration.seconds;
    stimulus.duration.seconds = randomSelection(randomizer, durationsSeconds);
    stimulus.frequency.Hz = 250;
    maskerPlayer.prepareVibrotactileStimulus(stimulus);
    lastVibrotactileStimulus = stimulus;
}

static auto nanoToMilliseconds(std::uintmax_t t) -> std::uintmax_t {
    // integer rounding
    return (t + 500000) / 1000000;
}

void InteractorImpl::notifyThatTargetWillPlayAt(
    const PlayerTimeWithDelay &targetStart) {
    vibrotactileStartTimeMilliseconds =
        static_cast<double>(nanoToMilliseconds(
            maskerPlayer.nanoseconds(targetStart.playerTime))) +
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
    const auto response{*min};
    KeyPressTrial trial;
    static_cast<KeyPressResponse &>(trial) = response;
    trial.rt.milliseconds = minReactionTimeMilliseconds;
    writeSaveAndReadyNextTrial(trial);
    return true;
}

void InteractorImpl::notifyThatStimulusHasEnded() { readyForResponse = true; }

void InteractorImpl::forceSubmit(const std::vector<KeyPressResponse> &) {
    KeyPressTrial trial;
    trial.rt.milliseconds = std::numeric_limits<double>::infinity();
    writeSaveAndReadyNextTrial(trial);
}

void InteractorImpl::writeSaveAndReadyNextTrial(KeyPressTrial &trial) {
    trial.vibrotactileStimulus = lastVibrotactileStimulus;
    trial.target =
        std::filesystem::path{method.currentTarget().path}.filename();
    outputFile.write(trial);
    outputFile.save();
    if (!deferringNextTrial) {
        method.submit(Flaggable{false});
        model.prepareNextTrialIfNeeded();
    }
    readyForResponse = false;
}
}
