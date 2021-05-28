#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGFREERESPONSEHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGFREERESPONSEHPP_

#include "IModel.hpp"
#include "FixedLevelMethod.hpp"
#include "IRecognitionTestModel.hpp"
#include "IOutputFile.hpp"

namespace av_speech_in_noise::submitting_free_response {
class InteractorImpl : public Interactor {
  public:
    InteractorImpl(FixedLevelMethod &, RecognitionTestModel &, OutputFile &);
    void submit(const FreeResponse &) override;

  private:
    FixedLevelMethod &method;
    RecognitionTestModel &model;
    OutputFile &outputFile;
};
}

#endif
