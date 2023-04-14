#include <av-speech-in-noise/Interface.hpp>

#include <gsl/gsl>

#include <vector>
#include <numeric>
#include <algorithm>

namespace av_speech_in_noise {
struct ImageRegion {
    double x;
    double y;
    double width;
    double height;
};

class NormallyMaskedImage {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(NormallyMaskedImage);
    virtual auto width() -> double = 0;
    virtual auto height() -> double = 0;
    virtual void reveal(ImageRegion) = 0;
};

class Shuffler {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Shuffler);
    virtual void shuffle(gsl::span<int>) = 0;
};

class RevealImage {
  public:
    RevealImage(
        NormallyMaskedImage &image, Shuffler &shuffler, int rows, int columns)
        : order(rows * columns), rows{rows}, columns{columns}, image{image},
          shuffler{shuffler} {
        reset();
    }

    void next() {
        if (index >= order.size())
            return;
        ImageRegion region{};
        const auto regionIndex = order.at(index++);
        region.x = (regionIndex % columns) * image.width() / columns;
        region.y = (regionIndex / columns) * image.height() / rows;
        region.width = image.width() / columns;
        region.height = image.height() / rows;
        image.reveal(region);
    }

    void reset() {
        std::iota(order.begin(), order.end(), 0);
        shuffler.shuffle(order);
    }

  private:
    std::vector<int> order;
    gsl::index index{};
    int rows;
    int columns;
    NormallyMaskedImage &image;
    Shuffler &shuffler;
};
}

#include "assert-utility.hpp"
#include "RandomizerStub.hpp"

#include <gtest/gtest.h>

namespace av_speech_in_noise {
namespace {
class NormallyMaskedImageStub : public NormallyMaskedImage {
  public:
    NormallyMaskedImageStub(double width, double height)
        : width_{width}, height_{height} {}

    auto width() -> double override { return width_; }

    auto height() -> double override { return height_; }

    auto lastRevealedRegion() -> ImageRegion { return lastRevealedRegion_; }

    void reveal(ImageRegion region) override { lastRevealedRegion_ = region; }

  private:
    ImageRegion lastRevealedRegion_{};
    double width_;
    double height_;
};

class ShufflerStub : public Shuffler {
  public:
    void shuffle(gsl::span<int> s) override {
        toShuffle_.resize(s.size());
        std::copy(s.begin(), s.end(), toShuffle_.begin());
        std::copy(shuffled.begin(), shuffled.end(), s.begin());
    }

    void setShuffled(std::vector<int> v) { shuffled = std::move(v); }

    auto toShuffle() -> std::vector<int> { return toShuffle_; }

    void clearToShuffle() { toShuffle_.clear(); }

  private:
    std::vector<int> shuffled;
    std::vector<int> toShuffle_;
};

class RevealImageTests : public ::testing::Test {};

#define ASSERT_EQUAL_IMAGE_REGIONS(a, b)                                       \
    EXPECT_EQ((a).x, (b).x);                                                   \
    EXPECT_EQ((a).y, (b).y);                                                   \
    EXPECT_EQ((a).width, (b).width);                                           \
    EXPECT_EQ((a).height, (b).height)

TEST_F(RevealImageTests, randomizesIndicesOfRevealableImageRegions) {
    NormallyMaskedImageStub image{0, 0};
    ShufflerStub randomizer;
    const auto rows{3};
    const auto columns{4};
    RevealImage reveal{image, randomizer, rows, columns};
    assertEqual({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}, randomizer.toShuffle());
}

TEST_F(RevealImageTests, revealsNextRegionAccordingToRandomOrder) {
    NormallyMaskedImageStub image{800, 600};
    ShufflerStub randomizer;
    const auto rows{3};
    const auto columns{4};
    randomizer.setShuffled({0, 2, 4, 6, 8, 10, 1, 3, 5, 7, 9, 11});
    RevealImage reveal{image, randomizer, rows, columns};
    reveal.next();
    auto actual{image.lastRevealedRegion()};
    auto expected{ImageRegion{}};
    expected.x = 0;
    expected.y = 0;
    expected.width = 800. / 4;
    expected.height = 600. / 3;
    ASSERT_EQUAL_IMAGE_REGIONS(expected, actual);
    reveal.next();
    actual = image.lastRevealedRegion();
    expected = ImageRegion{};
    expected.x = 400;
    expected.y = 0;
    expected.width = 800. / 4;
    expected.height = 600. / 3;
    ASSERT_EQUAL_IMAGE_REGIONS(expected, actual);
    reveal.next();
    actual = image.lastRevealedRegion();
    expected = ImageRegion{};
    expected.x = 0;
    expected.y = 200;
    expected.width = 800. / 4;
    expected.height = 600. / 3;
    ASSERT_EQUAL_IMAGE_REGIONS(expected, actual);
    reveal.next();
    actual = image.lastRevealedRegion();
    expected = ImageRegion{};
    expected.x = 400;
    expected.y = 200;
    expected.width = 800. / 4;
    expected.height = 600. / 3;
    ASSERT_EQUAL_IMAGE_REGIONS(expected, actual);
}

TEST_F(RevealImageTests, resetReshufflesIndicesOfRevealableImageRegions) {
    NormallyMaskedImageStub image{0, 0};
    ShufflerStub randomizer;
    const auto rows{3};
    const auto columns{4};
    RevealImage reveal{image, randomizer, rows, columns};
    randomizer.clearToShuffle();
    reveal.reset();
    assertEqual({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}, randomizer.toShuffle());
}
}
}
