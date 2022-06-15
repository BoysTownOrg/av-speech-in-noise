#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGCONSONANTHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGCONSONANTHPP_

#include "IModel.hpp"
#include "FixedLevelMethod.hpp"
#include "IRecognitionTestModel.hpp"
#include "IOutputFile.hpp"

#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise::submitting_consonant {
class InteractorImpl : public Interactor {
  public:
    InteractorImpl(FixedLevelMethod &method, RecognitionTestModel &model,
        OutputFile &outputFile)
        : method{method}, model{model}, outputFile{outputFile} {}

    void submit(const ConsonantResponse &r) override {
        method.submit(r);
        method.writeLastConsonant(outputFile);
        model.submit(r);
    }

  private:
    FixedLevelMethod &method;
    RecognitionTestModel &model;
    OutputFile &outputFile;
};
}

#endif
