#ifndef AV_SPEECH_IN_NOISE_UI_INCLUDE_AV_SPEECH_IN_NOISE_UI_REVEALIMAGE_HPP_
#define AV_SPEECH_IN_NOISE_UI_INCLUDE_AV_SPEECH_IN_NOISE_UI_REVEALIMAGE_HPP_

#include <av-speech-in-noise/Interface.hpp>

#include <gsl/gsl>

#include <vector>

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
        NormallyMaskedImage &image, Shuffler &shuffler, int rows, int columns);
    void next();
    void reset();

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
