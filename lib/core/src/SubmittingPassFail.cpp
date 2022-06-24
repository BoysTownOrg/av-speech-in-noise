#include "SubmittingPassFail.hpp"

namespace av_speech_in_noise::submitting_pass_fail {
InteractorImpl::InteractorImpl(
    AdaptiveMethod &adaptiveMethod, RunningATest &model, OutputFile &outputFile)
    : adaptiveMethod{adaptiveMethod}, model{model}, outputFile{outputFile} {}

void InteractorImpl::submitCorrectResponse() {
    adaptiveMethod.submitCorrectResponse();
    adaptiveMethod.writeLastCorrectResponse(outputFile);
    outputFile.save();
    model.prepareNextTrialIfNeeded();
}

void InteractorImpl::submitIncorrectResponse() {
    adaptiveMethod.submitIncorrectResponse();
    adaptiveMethod.writeLastIncorrectResponse(outputFile);
    outputFile.save();
    model.prepareNextTrialIfNeeded();
}
}
