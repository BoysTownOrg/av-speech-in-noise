#ifndef AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_FILTERADAPTER_HPP_
#define AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_FILTERADAPTER_HPP_

#include "Filter.hpp"
#include "TargetPlayerImpl.hpp"
#include <memory>

namespace av_speech_in_noise {
static auto hasAtLeastOneChannel(const std::vector<gsl::span<float>> &audio)
    -> bool {
    return !audio.empty();
}

static auto hasAtLeastTwoChannels(const std::vector<gsl::span<float>> &audio)
    -> bool {
    return audio.size() > 1;
}

static auto firstChannel(const std::vector<gsl::span<float>> &audio)
    -> gsl::span<float> {
    return audio.front();
}

static auto secondChannel(const std::vector<gsl::span<float>> &audio)
    -> gsl::span<float> {
    return audio.at(1);
}

class FilterAdapter : public SignalProcessor {
  public:
    explicit FilterAdapter(Filter<float>::Factory &factory)
        : factory{factory} {}

    void process(const std::vector<gsl::span<float>> &audio) override {
        if (leftFilter && hasAtLeastOneChannel(audio))
            leftFilter->filter(firstChannel(audio));
        if (rightFilter && hasAtLeastTwoChannels(audio))
            rightFilter->filter(secondChannel(audio));
    }

    void initialize(const audio_type &audio) override {
        if (audio.empty())
            return;
        leftFilter = factory.make(audio.front());
        rightFilter = factory.make(audio.front());
    }

    void clear() override {
        leftFilter.reset();
        rightFilter.reset();
    }

  private:
    Filter<float>::Factory &factory;
    std::shared_ptr<Filter<float>> leftFilter;
    std::shared_ptr<Filter<float>> rightFilter;
};
}

#endif
