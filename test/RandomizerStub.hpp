#ifndef TESTS_RANDOMIZERSTUB_HPP_
#define TESTS_RANDOMIZERSTUB_HPP_

#include <av-speech-in-noise/core/Randomizer.hpp>

namespace av_speech_in_noise {
class RandomizerStub : public Randomizer {
  public:
    [[nodiscard]] auto lowerIntBound() const { return lowerIntBound_; }

    [[nodiscard]] auto upperIntBound() const { return upperIntBound_; }

    void setRandomInt(int x) { randomInt_ = x; }

    void setRandomFloat(double x) { randomFloat_ = x; }

    [[nodiscard]] auto lowerFloatBound() const { return lowerBound_; }

    [[nodiscard]] auto upperFloatBound() const { return upperBound_; }

    auto betweenInclusive(double a, double b) -> double override {
        lowerBound_ = a;
        upperBound_ = b;
        return randomFloat_;
    }

    auto betweenInclusive(int a, int b) -> int override {
        lowerIntBound_ = a;
        upperIntBound_ = b;
        return randomInt_;
    }

  private:
    double lowerBound_{};
    double upperBound_{};
    double randomFloat_{};
    int randomInt_{};
    int lowerIntBound_{};
    int upperIntBound_{};
};
}

#endif
