#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGPASSFAILHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGPASSFAILHPP_

#include "IModel.hpp"
#include "IAdaptiveMethod.hpp"
#include "IOutputFile.hpp"
#include "IRecognitionTestModel.hpp"

namespace av_speech_in_noise::submitting_pass_fail {
class InteractorImpl : public Interactor {
  public:
    InteractorImpl(AdaptiveMethod &adaptiveMethod, RecognitionTestModel &model,
        OutputFile &outputFile)
        : adaptiveMethod{adaptiveMethod}, model{model}, outputFile{outputFile} {
    }

    void submitCorrectResponse() override {
        adaptiveMethod.submitCorrectResponse();
        adaptiveMethod.writeLastCorrectResponse(outputFile);
        outputFile.save();
        model.prepareNextTrialIfNeeded();
    }

    void submitIncorrectResponse() override {
        adaptiveMethod.submitIncorrectResponse();
        adaptiveMethod.writeLastIncorrectResponse(outputFile);
        outputFile.save();
        model.prepareNextTrialIfNeeded();
    }

  private:
    AdaptiveMethod &adaptiveMethod;
    RecognitionTestModel &model;
    OutputFile &outputFile;
};
}

#endif
