#include "assert-utility.hpp"
#include <stimulus-players/FilterAdapter.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise {
namespace {
class FilterStub : public Filter<float> {
  public:
    void filter(gsl::span<float> s) override {
        signal_ = s;
        filterCalled_ = true;
    }

    auto signal() -> gsl::span<float> { return signal_; }

    [[nodiscard]] auto filterCalled() const -> bool { return filterCalled_; }

  private:
    bool filterCalled_{};
    gsl::span<float> signal_;
};

class FilterFactoryStub : public Filter<float>::Factory {
  public:
    explicit FilterFactoryStub(std::shared_ptr<FilterStub> filter)
        : filter{std::move(filter)} {}

    auto make(const std::vector<float> &taps)
        -> std::shared_ptr<Filter<float>> override {
        anyMade_ = true;
        taps_.push_back(taps);
        auto next = first ? filter : secondFilter;
        first = false;
        return std::move(next);
    }

    auto taps() -> std::vector<std::vector<float>> { return taps_; }

    void setSecondFilter(std::shared_ptr<FilterStub> s) {
        secondFilter = std::move(s);
    }

    [[nodiscard]] auto anyMade() const -> bool { return anyMade_; }

  private:
    bool anyMade_{};
    bool first{true};
    std::vector<std::vector<float>> taps_;
    std::shared_ptr<FilterStub> filter;
    std::shared_ptr<FilterStub> secondFilter;
};

class FilterAdapterTests : public ::testing::Test {
  protected:
    std::shared_ptr<FilterStub> filter{std::make_shared<FilterStub>()};
    FilterFactoryStub filterFactory{filter};
    FilterAdapter adapter{filterFactory};
};

#define FILTER_ADAPTER_TEST(a) TEST_F(FilterAdapterTests, a)

void initialize(FilterAdapter &adapter, const audio_type &x = {{}}) {
    adapter.initialize(x);
}

FILTER_ADAPTER_TEST(initializePassesFirstChannelToFactoryForLeftAndRight) {
    initialize(adapter, {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    assertEqual({1, 2, 3}, filterFactory.taps().at(0));
    assertEqual({1, 2, 3}, filterFactory.taps().at(1));
}

FILTER_ADAPTER_TEST(noFiltersMadeWhenAudioEmpty) {
    initialize(adapter, {});
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(filterFactory.anyMade());
}

template <typename T>
void assertEqual_(const std::vector<T> &expected, gsl::span<T> actual) {
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(expected.size(), actual.size());
    for (typename gsl::span<T>::size_type i{0}; i < expected.size(); ++i)
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.at(i), at(actual, i));
}

FILTER_ADAPTER_TEST(processPassesFirstChannelToFilter) {
    initialize(adapter);
    std::vector<float> first{1, 2, 3};
    std::vector<float> second{4, 5, 6};
    std::vector<float> third{7, 8, 9};
    adapter.process({first, second, third});
    assertEqual_({1, 2, 3}, filter->signal());
}

FILTER_ADAPTER_TEST(processPassesSecondChannelToSecondFilter) {
    auto secondFilter{std::make_shared<FilterStub>()};
    filterFactory.setSecondFilter(secondFilter);
    initialize(adapter);
    std::vector<float> first{1, 2, 3};
    std::vector<float> second{4, 5, 6};
    std::vector<float> third{7, 8, 9};
    adapter.process({first, second, third});
    assertEqual_({4, 5, 6}, secondFilter->signal());
}

FILTER_ADAPTER_TEST(processDoesNotFilterSecondChannelIfOnlyOneChannel) {
    auto secondFilter{std::make_shared<FilterStub>()};
    filterFactory.setSecondFilter(secondFilter);
    initialize(adapter);
    std::vector<float> first{1, 2, 3};
    adapter.process({first});
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(secondFilter->filterCalled());
}

FILTER_ADAPTER_TEST(clearDoesNotUseFilter) {
    initialize(adapter, {});
    adapter.clear();
    std::vector<float> first{1, 2, 3};
    std::vector<float> second{4, 5, 6};
    std::vector<float> third{7, 8, 9};
    adapter.process({first, second, third});
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(filter->filterCalled());
}
}
}
