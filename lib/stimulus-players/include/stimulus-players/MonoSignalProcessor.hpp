#ifndef AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_MONOSIGNALPROCESSOR_HPP_
#define AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_MONOSIGNALPROCESSOR_HPP_

#include <gsl/gsl>

namespace av_speech_in_noise {
template <typename T> class MonoSignalProcessor {
  public:
    virtual ~MonoSignalProcessor() = default;
    virtual void process(gsl::span<T>) = 0;
};
}

#endif
