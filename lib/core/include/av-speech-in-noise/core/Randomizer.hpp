#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_RANDOMIZERHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_RANDOMIZERHPP_

namespace av_speech_in_noise {
class Randomizer {
  public:
    virtual ~Randomizer() = default;
    virtual auto betweenInclusive(double, double) -> double = 0;
    virtual auto betweenInclusive(int, int) -> int = 0;
};
}

#endif
