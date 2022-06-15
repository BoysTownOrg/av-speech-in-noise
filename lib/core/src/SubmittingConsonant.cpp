#include "SubmittingConsonant.hpp"

namespace av_speech_in_noise::submitting_consonant {
InteractorImpl::InteractorImpl(FixedLevelMethod &method,
    RecognitionTestModel &model, OutputFile &outputFile)
    : method{method}, model{model}, outputFile{outputFile} {}

void InteractorImpl::submit(const ConsonantResponse &r) {
    method.submit(r);
    method.writeLastConsonant(outputFile);
    outputFile.save();
    model.prepareNextTrialIfNeeded();
}
}
