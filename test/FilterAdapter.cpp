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
        taps_ = taps;
        auto next = first ? filter : secondFilter;
        first = false;
        return std::move(next);
    }

    auto taps() -> std::vector<float> { return taps_; }

    void setSecondFilter(std::shared_ptr<FilterStub> s) {
        secondFilter = std::move(s);
    }

    [[nodiscard]] auto anyMade() const -> bool { return anyMade_; }

  private:
    bool anyMade_{};
    bool first{true};
    std::vector<float> taps_;
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

FILTER_ADAPTER_TEST(initializePassesFirstChannelToFactory) {
    adapter.initialize({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    assertEqual({1, 2, 3}, filterFactory.taps());
}

FILTER_ADAPTER_TEST(noFiltersMadeWhenAudioEmpty) {
    adapter.initialize({});
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(filterFactory.anyMade());
}

template <typename T>
void assertEqual_(const std::vector<T> &expected, gsl::span<T> actual) {
    AV_SPEECH_IN_NOISE_ASSERT_EQUAL(expected.size(), actual.size());
    for (typename gsl::span<T>::size_type i{0}; i < expected.size(); ++i)
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.at(i), at(actual, i));
}

FILTER_ADAPTER_TEST(processPassesFirstChannelToFilter) {
    adapter.initialize({{}});
    std::vector<float> first{1, 2, 3};
    std::vector<float> second{4, 5, 6};
    std::vector<float> third{7, 8, 9};
    adapter.process({first, second, third});
    assertEqual_({1, 2, 3}, filter->signal());
}

FILTER_ADAPTER_TEST(processPassesSecondChannelToSecondFilter) {
    auto secondFilter{std::make_shared<FilterStub>()};
    filterFactory.setSecondFilter(secondFilter);
    adapter.initialize({{}});
    std::vector<float> first{1, 2, 3};
    std::vector<float> second{4, 5, 6};
    std::vector<float> third{7, 8, 9};
    adapter.process({first, second, third});
    assertEqual_({4, 5, 6}, secondFilter->signal());
}

FILTER_ADAPTER_TEST(clearDoesNotUseFilter) {
    adapter.initialize({});
    adapter.clear();
    std::vector<float> first{1, 2, 3};
    std::vector<float> second{4, 5, 6};
    std::vector<float> third{7, 8, 9};
    adapter.process({first, second, third});
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(filter->filterCalled());
}
}
}
