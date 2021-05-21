#ifndef MACOS_MAIN_MERSENNETWISTERRANDOMIZER_HPP_
#define MACOS_MAIN_MERSENNETWISTERRANDOMIZER_HPP_

#include <av-speech-in-noise/playlist/RandomizedTargetPlaylists.hpp>
#include <av-speech-in-noise/core/RecognitionTestModel.hpp>
#include <random>

namespace av_speech_in_noise {
class MersenneTwisterRandomizer : public target_list::Randomizer,
                                  public Randomizer {
    std::mt19937 engine{std::random_device{}()};

  public:
    void shuffle(gsl::span<av_speech_in_noise::LocalUrl> s) override {
        std::shuffle(s.begin(), s.end(), engine);
    }

    void shuffle(gsl::span<int> s) override {
        std::shuffle(s.begin(), s.end(), engine);
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
}

#endif
