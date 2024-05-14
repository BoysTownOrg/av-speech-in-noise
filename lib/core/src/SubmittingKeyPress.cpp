#include "SubmittingKeyPress.hpp"

#include <filesystem>

namespace av_speech_in_noise::submitting_keypress {
InteractorImpl::InteractorImpl(FixedLevelMethod &method, RunningATest &model,
    OutputFile &outputFile, MaskerPlayer &maskerPlayer)
    : method{method}, model{model}, outputFile{outputFile}, maskerPlayer{
                                                                maskerPlayer} {}

void InteractorImpl::notifyThatTargetWillPlayAt(const PlayerTimeWithDelay &t) {
    targetStartTimeMilliseconds =
        static_cast<double>(
            (maskerPlayer.nanoseconds(t.playerTime) + 500000) / 1000000) +
        t.delay.seconds * 1000;
}

void InteractorImpl::submit(const KeyPressResponse &response) {
    // TODO
    // method.submit(response);
    KeyPressTrial trial;
    trial.rt.milliseconds =
        response.seconds * 1000 - targetStartTimeMilliseconds;
    // trial.target =
    //     std::filesystem::path{method.currentTarget().path}.filename();
    // trial.time = model.playTrialTime();
    outputFile.write(trial);
    // outputFile.save();
    // model.prepareNextTrialIfNeeded();
}
}
