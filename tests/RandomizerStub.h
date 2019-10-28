#ifndef TESTS_RANDOMIZERSTUB_H_
#define TESTS_RANDOMIZERSTUB_H_

#include <recognition-test/Model.hpp>

namespace av_speech_in_noise::tests {
class RandomizerStub : public Randomizer {
    double lowerBound_{};
    double upperBound_{};
    double randomFloat_{};
    int randomInt_{};
    int lowerIntBound_{};
    int upperIntBound_{};

  public:
    auto lowerIntBound() const { return lowerIntBound_; }

    auto upperIntBound() const { return upperIntBound_; }

    void setRandomInt(int x) { randomInt_ = x; }

    void setRandomFloat(double x) { randomFloat_ = x; }

    auto lowerFloatBound() const { return lowerBound_; }

    auto upperFloatBound() const { return upperBound_; }

    double randomFloatBetween(double a, double b) override {
        lowerBound_ = a;
        upperBound_ = b;
        return randomFloat_;
    }

    int randomIntBetween(int a, int b) override {
        lowerIntBound_ = a;
        upperIntBound_ = b;
        return randomInt_;
    }
};
} // namespace av_speech_in_noise::tests

#endif
