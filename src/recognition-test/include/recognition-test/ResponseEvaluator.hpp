#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_RESPONSEEVALUATOR_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_RESPONSEEVALUATOR_HPP_

#include "Model.hpp"

namespace av_speech_in_noise {
class ResponseEvaluatorImpl : public ResponseEvaluator {
  public:
    auto correct(const LocalUrl &,
        const coordinate_response_measure::Response &) -> bool override;
    auto correct(const LocalUrl &, const ConsonantResponse &) -> bool;
    static int invalidNumber;
    auto correctNumber(const LocalUrl &) -> int override;
    auto correctConsonant(const LocalUrl &) -> char override { return {}; }
    auto correctColor(const LocalUrl &)
        -> coordinate_response_measure::Color override;
    auto fileName(const LocalUrl &) -> std::string override;
};
}

#endif
