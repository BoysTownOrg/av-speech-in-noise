#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_RESPONSEEVALUATOR_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_RESPONSEEVALUATOR_HPP_

#include "Model.hpp"

namespace av_speech_in_noise {
class ResponseEvaluatorImpl : public ResponseEvaluator {
  public:
    auto correct(const LocalUrl &filePath,
        const coordinate_response_measure::Response &r) -> bool override;
    static int invalidNumber;
    auto correctNumber(const LocalUrl &filePath) -> int override;
    auto correctColor(const LocalUrl &filePath)
        -> coordinate_response_measure::Color override;
    auto fileName(const LocalUrl &filePath) -> std::string override;
};
}

#endif
