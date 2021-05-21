#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_RESPONSEEVALUATORHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_RESPONSEEVALUATORHPP_

#include "IResponseEvaluator.hpp"

namespace av_speech_in_noise {
class ResponseEvaluatorImpl : public ResponseEvaluator {
  public:
    auto correct(const LocalUrl &,
        const coordinate_response_measure::Response &) -> bool override;
    auto correct(const LocalUrl &, const ConsonantResponse &) -> bool override;
    auto correct(const LocalUrl &, const SyllableResponse &) -> bool override;
    const static int invalidNumber;
    auto correctNumber(const LocalUrl &) -> int override;
    auto correctConsonant(const LocalUrl &) -> char override;
    auto correctColor(const LocalUrl &)
        -> coordinate_response_measure::Color override;
    auto correctSyllable(const LocalUrl &) -> Syllable override;
    auto fileName(const LocalUrl &) -> std::string override;
};
}

#endif
