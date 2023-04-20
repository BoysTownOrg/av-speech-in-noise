// https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_context/dq_context.html#//apple_ref/doc/uid/TP30001066-CH203-CJBHBFFE
// https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_images/dq_images.html#//apple_ref/doc/uid/TP30001066-CH212-TPXREF101

#include "masking-images.h"

#include <AppKit/AppKit.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>

#include <stdexcept>
#include <memory>

namespace av_speech_in_noise {
class ScopedMaskedImage;
}

@interface ImageView : NSView
- (instancetype)initWithImage:
    (std::unique_ptr<av_speech_in_noise::ScopedMaskedImage>)image;
@end

namespace av_speech_in_noise {
class ScopedMaskedImage {
  public:
    ScopedMaskedImage(CGImageRef image, CGImageRef mask)
        : masked{CGImageCreateWithMask(image, mask)} {
        if (masked == nullptr)
            throw std::runtime_error{"Unable to create masked image."};
    }

    ~ScopedMaskedImage() { CGImageRelease(masked); }

    CGImageRef masked;
};

class ScopedBitmapImage {
  public:
    explicit ScopedBitmapImage(CGContextRef context)
        : image{CGBitmapContextCreateImage(context)} {
        if (image == nullptr)
            throw std::runtime_error{"Unable to create bitmap image."};
    }

    ~ScopedBitmapImage() { CGImageRelease(image); }

    CGImageRef image;
};

class ScopedImageSource {
  public:
    explicit ScopedImageSource(CFURLRef url)
        : imageSource{CGImageSourceCreateWithURL(url, nullptr)} {
        if (imageSource == nullptr)
            throw std::runtime_error{"Unable to create image source."};
    }

    ~ScopedImageSource() { CFRelease(imageSource); }

    CGImageSourceRef imageSource;
};
ScopedImage::ScopedImage(CGImageSourceRef imageSource)
    : image{CGImageSourceCreateImageAtIndex(imageSource, 0, nullptr)} {
    if (imageSource == nullptr)
        throw std::runtime_error{"Unable to create image."};
}

ScopedImage::~ScopedImage() { CGImageRelease(image); }

static CGContextRef createBitmapContext(int pixelsWide, int pixelsHigh) {
    const auto bitmapBytesPerRow = (pixelsWide * 3); // 1
    auto *const colorSpace = CGColorSpaceCreateDeviceGray(); // 2
    auto *const context = CGBitmapContextCreate(nullptr, // 4
        pixelsWide, pixelsHigh,
        8, // bits per component
        bitmapBytesPerRow, colorSpace, kCGImageAlphaNone);
    CGColorSpaceRelease(colorSpace);
    return context;
};

ScopedBitmapContext::ScopedBitmapContext(CGImageRef image)
    : context{createBitmapContext(
          CGImageGetWidth(image), CGImageGetHeight(image))} {
    if (context == nullptr)
        throw std::runtime_error{"Unable to create bitmap context"};
}

auto ScopedBitmapContext::operator=(ScopedBitmapContext &&other) noexcept
    -> ScopedBitmapContext & {
    if (this != &other) {
        CGContextRelease(context);
        context = other.context;
        other.context = nullptr;
    }
    return *this;
}

ScopedBitmapContext::~ScopedBitmapContext() {
    if (context != nullptr)
        CGContextRelease(context);
}

static auto imageResource(NSString *imageName) -> NSURL * {
    NSURL *url = [[NSBundle mainBundle] URLForImageResource:imageName];
    if (url == nil) {
        throw std::runtime_error{"Cannot get URL for image."};
    }
    return url;
}

MaskedCoreGraphicsImage::MaskedCoreGraphicsImage(
    NSWindow *window, NSString *imageName)
    : image{ScopedImage{
          ScopedImageSource{(__bridge CFURLRef)(imageResource(imageName))}
              .imageSource}},
      window{window} {
    reset();
}

auto MaskedCoreGraphicsImage::width() -> double {
    return CGImageGetWidth(image.image);
}

auto MaskedCoreGraphicsImage::height() -> double {
    return CGImageGetHeight(image.image);
}

void MaskedCoreGraphicsImage::reset() {
    context = std::make_unique<ScopedBitmapContext>(image.image);
}

void MaskedCoreGraphicsImage::reveal(ImageRegion region) {
    CGContextSetRGBFillColor(context->context, 1, 1, 1, 1);
    CGContextFillRect(context->context,
        CGRectMake(region.x, region.y, region.width, region.height));

    addMaskedImageViewToWindow();
}

void MaskedCoreGraphicsImage::addMaskedImageViewToWindow() {
    const auto mask{ScopedBitmapImage{context->context}};
    const auto imageView =
        [[ImageView alloc] initWithImage:std::make_unique<ScopedMaskedImage>(
                                             image.image, mask.image)];

    if (lastImageView != nil)
        [lastImageView removeFromSuperview];
    lastImageView = imageView;

    [window.contentView addSubview:imageView];
}
}

@implementation ImageView {
    std::unique_ptr<av_speech_in_noise::ScopedMaskedImage> _image;
}
- (id)initWithImage:
    (std::unique_ptr<av_speech_in_noise::ScopedMaskedImage>)image {
    self = [super initWithFrame:NSMakeRect(0, 0, CGImageGetWidth(image->masked),
                                    CGImageGetHeight(image->masked))];
    if (self != nullptr) {
        _image = std::move(image);
    }
    return self;
}

- (void)drawRect:(NSRect)rect {
    CGContextRef myContext = [[NSGraphicsContext currentContext] CGContext];
    CGContextDrawImage(myContext,
        CGRectMake(0, 0, CGImageGetWidth(_image->masked),
            CGImageGetHeight(_image->masked)),
        _image->masked);
}
@end
