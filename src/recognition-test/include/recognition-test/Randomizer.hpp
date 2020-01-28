#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_RANDOMIZER_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_RANDOMIZER_HPP_

namespace av_speech_in_noise {
class Randomizer {
  public:
    virtual ~Randomizer() = default;
    virtual auto betweenInclusive(double, double) -> double = 0;
    virtual auto betweenInclusive(int, int) -> int = 0;
};
}

#endif