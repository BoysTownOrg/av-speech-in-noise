#include "SubmittingKeywords.hpp"
#include <filesystem>

namespace av_speech_in_noise::submitting_keywords {
InteractorImpl::InteractorImpl(FixedLevelMethod &method,
    RunningATest &recognitionTestModel, OutputFile &outputFile)
    : method{method}, recognitionTestModel{recognitionTestModel},
      outputFile{outputFile} {}

void InteractorImpl::submit(const ThreeKeywordsResponse &p) {
    method.submit(p);
    ThreeKeywordsTrial trial;
    trial.firstCorrect = p.firstCorrect;
    trial.secondCorrect = p.secondCorrect;
    trial.thirdCorrect = p.thirdCorrect;
    trial.target =
        std::filesystem::path{method.currentTarget().path}.filename();
    trial.flagged = p.flagged;
    outputFile.write(trial);
    outputFile.save();
    recognitionTestModel.prepareNextTrialIfNeeded();
}
}
