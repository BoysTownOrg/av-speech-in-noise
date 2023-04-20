#ifndef AV_SPEECH_IN_NOISE_MACOS_MASKING_IMAGES_H_
#define AV_SPEECH_IN_NOISE_MACOS_MASKING_IMAGES_H_

#include <av-speech-in-noise/ui/RevealImage.hpp>

#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>

#import <AppKit/AppKit.h>

#include <memory>

namespace av_speech_in_noise {
class ScopedBitmapContext {
  public:
    explicit ScopedBitmapContext(CGImageRef image);
    auto operator=(ScopedBitmapContext &&other) noexcept
        -> ScopedBitmapContext &;
    ~ScopedBitmapContext();

    CGContextRef context;
};

class ScopedImage {
  public:
    explicit ScopedImage(CGImageSourceRef imageSource);

    ~ScopedImage();
    CGImageRef image;
};

class MaskedCoreGraphicsImage : public NormallyMaskedImage {
  public:
    MaskedCoreGraphicsImage(NSWindow *window, NSString *imageName);
    auto width() -> double override;
    auto height() -> double override;
    void reset();
    void reveal(ImageRegion region) override;
    void addMaskedImageViewToWindow();
    void show() override;
    void hide() override;

  private:
    ScopedImage image;
    std::unique_ptr<ScopedBitmapContext> context;
    NSWindow *window;
    NSView *lastImageView;
};
}

#endif
