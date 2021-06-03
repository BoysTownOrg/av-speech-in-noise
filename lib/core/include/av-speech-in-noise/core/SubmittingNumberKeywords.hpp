#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGNUMBERKEYWORDSHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGNUMBERKEYWORDSHPP_

#include "IModel.hpp"
#include "IAdaptiveMethod.hpp"
#include "IOutputFile.hpp"
#include "IRecognitionTestModel.hpp"

namespace av_speech_in_noise::submitting_number_keywords {
class InteractorImpl : public Interactor {
  public:
    InteractorImpl(AdaptiveMethod &method, RecognitionTestModel &model,
        OutputFile &outputFile)
        : method{method}, model{model}, outputFile{outputFile} {}

    void submit(const CorrectKeywords &k) override {
        method.submit(k);
        method.writeLastCorrectKeywords(outputFile);
        outputFile.save();
        model.prepareNextTrialIfNeeded();
    }

  private:
    AdaptiveMethod &method;
    RecognitionTestModel &model;
    OutputFile &outputFile;
};
}

#endif
