#include "SubmittingFixedPassFail.hpp"

#include <filesystem>

namespace av_speech_in_noise::submitting_fixed_pass_fail {
InteractorImpl::InteractorImpl(
    FixedLevelMethod &method, RunningATest &model, OutputFile &outputFile)
    : method{method}, model{model}, outputFile{outputFile} {}

void InteractorImpl::submitCorrectResponse() { submit(true); }

void InteractorImpl::submitIncorrectResponse() { submit(false); }

void InteractorImpl::submit(bool correct) {
    method.submit(Flaggable{false});
    PassFailTrial trial;
    trial.correct = correct;
    trial.target =
        std::filesystem::path{method.currentTarget().path}.filename();
    outputFile.write(trial);
    outputFile.save();
    model.prepareNextTrialIfNeeded();
}
}
