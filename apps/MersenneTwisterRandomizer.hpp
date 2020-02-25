#ifndef MACOS_MAIN_MERSENNETWISTERRANDOMIZER_HPP_
#define MACOS_MAIN_MERSENNETWISTERRANDOMIZER_HPP_

#include <target-list/RandomizedTargetList.hpp>
#include <recognition-test/RecognitionTestModel.hpp>
#include <random>

class MersenneTwisterRandomizer : public target_list::Randomizer,
                                  public av_speech_in_noise::Randomizer {
    std::mt19937 engine{std::random_device{}()};

  public:
    void shuffle(string_vector_iterator begin, string_vector_iterator end) override {
        std::shuffle(begin, end, engine);
    }

    void shuffle(
        int_vector_iterator begin, int_vector_iterator end) override {
        std::shuffle(begin, end, engine);
    }

    auto betweenInclusive(double a, double b) -> double override {
        std::uniform_real_distribution<> distribution{a, b};
        return distribution(engine);
    }

    auto betweenInclusive(int a, int b) -> int override {
        std::uniform_int_distribution<> distribution{a, b};
        return distribution(engine);
    }
};

#endif
