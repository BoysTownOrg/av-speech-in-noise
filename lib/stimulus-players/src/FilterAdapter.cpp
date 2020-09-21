#include "FilterAdapter.hpp"

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

FilterAdapter::FilterAdapter(Filter<float>::Factory &factory)
    : factory{factory} {}

void FilterAdapter::process(const std::vector<gsl::span<float>> &audio) {
    if (leftFilter && hasAtLeastOneChannel(audio))
        leftFilter->filter(firstChannel(audio));
    if (rightFilter && hasAtLeastTwoChannels(audio))
        rightFilter->filter(secondChannel(audio));
}

void FilterAdapter::initialize(const audio_type &audio) {
    if (audio.empty())
        return;
    leftFilter = factory.make(audio.front());
    rightFilter = factory.make(audio.front());
}

void FilterAdapter::clear() {
    leftFilter.reset();
    rightFilter.reset();
}
}
