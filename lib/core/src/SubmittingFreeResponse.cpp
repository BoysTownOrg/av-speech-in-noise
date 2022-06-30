#include "SubmittingFreeResponse.hpp"
#include <filesystem>

namespace av_speech_in_noise::submitting_free_response {
InteractorImpl::InteractorImpl(
    FixedLevelMethod &method, RunningATest &model, OutputFile &outputFile)
    : method{method}, model{model}, outputFile{outputFile} {}

void InteractorImpl::submit(const FreeResponse &response) {
    method.submit(response);
    FreeResponseTrial trial;
    trial.response = response.response;
    trial.target =
        std::filesystem::path{method.currentTarget().path}.filename();
    trial.flagged = response.flagged;
    trial.time = model.playTrialTime();
    outputFile.write(trial);
    outputFile.save();
    model.prepareNextTrialIfNeeded();
}
}
