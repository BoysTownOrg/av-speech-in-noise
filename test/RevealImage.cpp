#include "assert-utility.hpp"
#include "RandomizerStub.hpp"

#include <av-speech-in-noise/ui/RevealImage.hpp>

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

TEST_F(RevealImageTests, resetResetsOrder) {
    NormallyMaskedImageStub image{800, 600};
    ShufflerStub randomizer;
    const auto rows{3};
    const auto columns{4};
    RevealImage reveal{image, randomizer, rows, columns};
    randomizer.setShuffled({0, 2, 4, 6, 8, 10, 1, 3, 5, 7, 9, 11});
    reveal.next();
    reveal.next();
    reveal.next();
    randomizer.setShuffled({11, 0, 2, 4, 6, 8, 10, 1, 3, 5, 7, 9});
    reveal.reset();
    reveal.next();
    auto actual = image.lastRevealedRegion();
    auto expected = ImageRegion{};
    expected.x = 600;
    expected.y = 400;
    expected.width = 800. / 4;
    expected.height = 600. / 3;
    ASSERT_EQUAL_IMAGE_REGIONS(expected, actual);
}
}
}
