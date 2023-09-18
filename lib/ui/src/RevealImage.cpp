#include "RevealImage.hpp"

#include <cstddef>
#include <numeric>
#include <algorithm>

namespace av_speech_in_noise {
RevealImage::RevealImage(
    NormallyMaskedImage &image, Shuffler &shuffler, int rows, int columns)
    : order(static_cast<std::vector<int>::size_type>(rows) * columns),
      rows{rows}, columns{columns}, image{image}, shuffler{shuffler} {
    reset();
}

void RevealImage::initialize(const LocalUrl &url) { image.initialize(url); }

void RevealImage::advance() {
    if (orderIterator == order.end())
        return;
    const auto regionIndex = *orderIterator;
    ++orderIterator;

    ImageRegion region{};
    region.x = (regionIndex % columns) * image.width() / columns;
    region.y = (regionIndex / columns) * image.height() / rows;
    region.width = image.width() / columns;
    region.height = image.height() / rows;
    image.reveal(region);
}

void RevealImage::reset() {
    std::iota(order.begin(), order.end(), 0);
    shuffler.shuffle(order);
    orderIterator = order.begin();
}

void RevealImage::show() { image.show(); }

void RevealImage::hide() { image.hide(); }
}
