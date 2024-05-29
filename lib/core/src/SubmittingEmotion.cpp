#include "SubmittingEmotion.hpp"

#include <filesystem>

namespace av_speech_in_noise::submitting_emotion {
InteractorImpl::InteractorImpl(
    FixedLevelMethod &method, RunningATest &model, OutputFile &outputFile)
    : method{method}, model{model}, outputFile{outputFile} {}

void InteractorImpl::submit(const EmotionResponse &r) {
    method.submit(r);
    EmotionTrial trial;
    static_cast<EmotionResponse &>(trial) = r;
    trial.target =
        std::filesystem::path{method.currentTarget().path}.filename();
    outputFile.write(trial);
    outputFile.save();
    model.prepareNextTrialIfNeeded();
}
}
