#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_IRESPONSEEVALUATOR_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_IRESPONSEEVALUATOR_HPP_

#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class ResponseEvaluator {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(ResponseEvaluator);
    virtual auto correct(const LocalUrl &,
        const coordinate_response_measure::Response &) -> bool = 0;
    virtual auto correct(const LocalUrl &, const ConsonantResponse &)
        -> bool = 0;
    virtual auto correctColor(const LocalUrl &)
        -> coordinate_response_measure::Color = 0;
    virtual auto correctNumber(const LocalUrl &) -> int = 0;
    virtual auto correctConsonant(const LocalUrl &) -> char = 0;
    virtual auto correctSyllable(const LocalUrl &) -> Syllable = 0;
    virtual auto fileName(const LocalUrl &) -> std::string = 0;
};
}

#endif
