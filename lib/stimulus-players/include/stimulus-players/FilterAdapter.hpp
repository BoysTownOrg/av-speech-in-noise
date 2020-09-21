#ifndef AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_FILTERADAPTER_HPP_
#define AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_FILTERADAPTER_HPP_

#include "Filter.hpp"
#include "TargetPlayerImpl.hpp"
#include <memory>

namespace av_speech_in_noise {
class FilterAdapter : public SignalProcessor {
  public:
    explicit FilterAdapter(Filter<float>::Factory &factory)
        : factory{factory} {}

    void process(const std::vector<gsl::span<float>> &audio) override {
        if (filter)
            filter->filter(audio.front());
        if (secondFilter)
            secondFilter->filter(audio.at(1));
    }

    void initialize(const audio_type &audio) override {
        if (audio.empty())
            return;
        filter = factory.make(audio.front());
        secondFilter = factory.make(audio.front());
    }

    void clear() override { filter.reset(); }

  private:
    Filter<float>::Factory &factory;
    std::shared_ptr<Filter<float>> filter;
    std::shared_ptr<Filter<float>> secondFilter;
};
}

#endif
