#include <stimulus-players/Filter.hpp>
#include <stimulus-players/TargetPlayerImpl.hpp>

namespace av_speech_in_noise {
class FilterAdapter : public SignalProcessor {
  public:
    explicit FilterAdapter(Filter<float>::Factory &factory)
        : factory{factory} {}
    void process(const std::vector<gsl::span<float>> &) override {}
    void initialize(const audio_type &audio) override {
        factory.make(audio.front());
    }

  private:
    Filter<float>::Factory &factory;
};
}

#include "assert-utility.hpp"
#include <gtest/gtest.h>

namespace av_speech_in_noise {
namespace {

class FilterStub : public Filter<float> {
  public:
    void filter(gsl::span<float>) override {}
};

class FilterFactoryStub : public Filter<float>::Factory {
  public:
    explicit FilterFactoryStub(std::shared_ptr<FilterStub> filter)
        : filter{std::move(filter)} {}

    auto make(const std::vector<float> &taps)
        -> std::shared_ptr<Filter<float>> override {
        taps_ = taps;
        return filter;
    }

    auto taps() -> std::vector<float> { return taps_; }

  private:
    std::vector<float> taps_;
    std::shared_ptr<FilterStub> filter;
};

class FilterAdapterTests : public ::testing::Test {
  protected:
    std::shared_ptr<FilterStub> filter{std::make_shared<FilterStub>()};
    FilterFactoryStub filterFactory{filter};
    FilterAdapter adapter{filterFactory};
};

#define FILTER_ADAPTER_TEST(a) TEST_F(FilterAdapterTests, a)

FILTER_ADAPTER_TEST(initializePassesTapsToFactory) {
    adapter.initialize({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    assertEqual({1, 2, 3}, filterFactory.taps());
}
}
}
