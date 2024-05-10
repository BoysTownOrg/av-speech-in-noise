#include "SubmittingKeyPress.hpp"

#include <filesystem>

namespace av_speech_in_noise::submitting_keypress {
InteractorImpl::InteractorImpl(
    FixedLevelMethod &method, RunningATest &model, OutputFile &outputFile)
    : method{method}, model{model}, outputFile{outputFile} {}

void InteractorImpl::submit(const KeyPressResponse &response) {
    // TODO
    // method.submit(response);
    // KeyPressResponseTrial trial;
    // trial.target =
    //     std::filesystem::path{method.currentTarget().path}.filename();
    // trial.time = model.playTrialTime();
    // outputFile.write(trial);
    // outputFile.save();
    // model.prepareNextTrialIfNeeded();
}
}
