#include "assert-utility.hpp"
#include <algorithm>
#include <av-speech-in-noise/Interface.hpp>

#include <gsl/gsl>
#include <numeric>

namespace av_speech_in_noise {
class Image {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Image);
};

class Shuffler {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Shuffler);
    virtual void shuffle(gsl::span<int>) = 0;
};

class RevealImage {
  public:
    RevealImage(Image &, Shuffler &shuffler, int rows, int columns)
        : order(rows * columns) {
        std::iota(order.begin(), order.end(), 0);
        shuffler.shuffle(order);
    }
    void next() {}

  private:
    std::vector<int> order;
};
}

#include "RandomizerStub.hpp"

#include <gtest/gtest.h>

namespace av_speech_in_noise {
namespace {
class ImageStub : public Image {
  public:
    ImageStub(int width, int height) : width_{width}, height_{height} {}

    auto width() -> int { return width_; }

    auto height() -> int { return height_; }

  private:
    int width_;
    int height_;
};

class ShufflerStub : public Shuffler {
  public:
    void shuffle(gsl::span<int> s) {
        toShuffle_.resize(s.size());
        std::copy(s.begin(), s.end(), toShuffle_.begin());
        std::copy(shuffled.begin(), shuffled.end(), s.begin());
    }

    void setShuffled(std::vector<int> v) { shuffled = std::move(v); }

    auto toShuffle() -> std::vector<int> { return toShuffle_; }

  private:
    std::vector<int> shuffled;
    std::vector<int> toShuffle_;
};

class RevealImageTests : public ::testing::Test {};

TEST_F(RevealImageTests, tbd) {
    ImageStub image{0, 0};
    ShufflerStub randomizer;
    const auto rows{3};
    const auto columns{4};
    RevealImage reveal{image, randomizer, rows, columns};
    assertEqual({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}, randomizer.toShuffle());
}
}
}
