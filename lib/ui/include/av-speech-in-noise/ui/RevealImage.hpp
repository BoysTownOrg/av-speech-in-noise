#ifndef AV_SPEECH_IN_NOISE_UI_INCLUDE_AV_SPEECH_IN_NOISE_UI_REVEALIMAGE_HPP_
#define AV_SPEECH_IN_NOISE_UI_INCLUDE_AV_SPEECH_IN_NOISE_UI_REVEALIMAGE_HPP_

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
        index = 0;
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

#endif
