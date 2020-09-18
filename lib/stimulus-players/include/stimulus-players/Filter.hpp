#ifndef AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_FILTER_HPP_
#define AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_FILTER_HPP_

#include <gsl/gsl>
#include <memory>
#include <vector>

namespace av_speech_in_noise {
template <typename T> class Filter {
  public:
    virtual ~Filter() = default;
    virtual void filter(gsl::span<T>) = 0;

    class Factory {
      public:
        virtual ~Factory() = default;
        virtual auto make(const std::vector<T> &taps)
            -> std::shared_ptr<Filter<T>> = 0;
    };
};
}

#endif
