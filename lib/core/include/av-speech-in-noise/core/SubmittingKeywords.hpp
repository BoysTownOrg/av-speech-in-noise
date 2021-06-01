#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGKEYWORDSHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGKEYWORDSHPP_

#include "IModel.hpp"
#include "IOutputFile.hpp"
#include "IRecognitionTestModel.hpp"
#include "IFixedLevelMethod.hpp"

namespace av_speech_in_noise::submitting_keywords {
class InteractorImpl : public Interactor {
  public:
    InteractorImpl(FixedLevelMethod &, RecognitionTestModel &, OutputFile &);
    void submit(const ThreeKeywordsResponse &p) override;

  private:
    FixedLevelMethod &method;
    RecognitionTestModel &recognitionTestModel;
    OutputFile &outputFile;
};
}

#endif
