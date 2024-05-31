#include "SubmittingFixedPassFail.hpp"

namespace av_speech_in_noise::submitting_fixed_pass_fail {
InteractorImpl::InteractorImpl(
    FixedLevelMethod &method, RunningATest &model, OutputFile &outputFile)
    : method{method}, model{model}, outputFile{outputFile} {}

void InteractorImpl::submitCorrectResponse() {
    method.submit(Flaggable{false});
    PassFailTrial trial;
    trial.correct = true;
    outputFile.write(trial);
    outputFile.save();
    model.prepareNextTrialIfNeeded();
}

void InteractorImpl::submitIncorrectResponse() {
    method.submit(Flaggable{false});
    PassFailTrial trial;
    trial.correct = false;
    outputFile.write(trial);
    outputFile.save();
    model.prepareNextTrialIfNeeded();
}
}
