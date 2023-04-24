#ifndef AV_SPEECH_IN_NOISE_UI_INCLUDE_AV_SPEECH_IN_NOISE_UI_REVEALIMAGE_HPP_
#define AV_SPEECH_IN_NOISE_UI_INCLUDE_AV_SPEECH_IN_NOISE_UI_REVEALIMAGE_HPP_

#include "FreeResponse.hpp"
#include "av-speech-in-noise/Model.hpp"

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
    virtual void initialize(const LocalUrl &) = 0;
    virtual auto width() -> double = 0;
    virtual auto height() -> double = 0;
    virtual void reveal(ImageRegion) = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
};

class Shuffler {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Shuffler);
    virtual void shuffle(gsl::span<int>) = 0;
};

class RevealImage : public submitting_free_response::Puzzle {
  public:
    RevealImage(
        NormallyMaskedImage &image, Shuffler &shuffler, int rows, int columns);
    void initialize(const LocalUrl &) override;
    void advance() override;
    void reset() override;
    void show() override;
    void hide() override;

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
