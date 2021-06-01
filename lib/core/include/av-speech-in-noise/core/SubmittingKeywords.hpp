#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGKEYWORDSHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGKEYWORDSHPP_

#include "IModel.hpp"
#include "IOutputFile.hpp"
#include "IRecognitionTestModel.hpp"
#include "IFixedLevelMethod.hpp"
#include <filesystem>

namespace av_speech_in_noise::submitting_keywords {
class InteractorImpl : public Interactor {
  public:
    InteractorImpl(FixedLevelMethod &method,
        RecognitionTestModel &recognitionTestModel, OutputFile &outputFile)
        : method{method}, recognitionTestModel{recognitionTestModel},
          outputFile{outputFile} {}

    void submit(const ThreeKeywordsResponse &p) override {
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

  private:
    FixedLevelMethod &method;
    RecognitionTestModel &recognitionTestModel;
    OutputFile &outputFile;
};
}

#endif
