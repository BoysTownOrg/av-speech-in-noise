#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_RESPONSEEVALUATOR_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_RESPONSEEVALUATOR_HPP_

#include "Model.hpp"

namespace av_speech_in_noise {
class ResponseEvaluatorImpl : public ResponseEvaluator {
  public:
    auto correct(const std::string &filePath,
        const coordinate_response_measure::Response &r) -> bool override;
    static int invalidNumber;
    auto correctNumber(const std::string &filePath) -> int override;
    auto correctColor(const std::string &filePath)
        -> coordinate_response_measure::Color override;
    auto fileName(const std::string &filePath) -> std::string override;

  private:
    auto correctNumber_(const std::string &filePath) -> int;
    auto colorNameLength(
        const std::string &filePath, unsigned long leadingPathLength_) -> long;
};
}

#endif
