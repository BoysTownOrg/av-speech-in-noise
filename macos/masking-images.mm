// https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_context/dq_context.html#//apple_ref/doc/uid/TP30001066-CH203-CJBHBFFE
// https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_images/dq_images.html#//apple_ref/doc/uid/TP30001066-CH212-TPXREF101

#include "masking-images.h"
#include "Foundation-utility.h"

#include <AppKit/AppKit.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>

#include <cmath>
#include <sstream>
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
    class CreationError {};
    explicit ScopedImageSource(CFURLRef url)
        : imageSource{CGImageSourceCreateWithURL(url, nullptr)} {
        if (imageSource == nullptr)
            throw CreationError{};
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

MaskedCoreGraphicsImage::MaskedCoreGraphicsImage(NSWindow *window)
    : window{window} {
    hide();
}

auto createImage(const LocalUrl &url) -> std::unique_ptr<ScopedImage> {
    try {
        const auto *nsurl = [NSURL fileURLWithPath:nsString(url.path)
                                       isDirectory:NO];
        return std::make_unique<ScopedImage>(
            ScopedImageSource{(__bridge CFURLRef)nsurl}.imageSource);
    } catch (const ScopedImageSource::CreationError &) {
        std::stringstream stream;
        stream << "Unable to create image: " << url.path;
        throw std::runtime_error{stream.str()};
    }
}

void MaskedCoreGraphicsImage::initialize(const LocalUrl &url) {
    image = createImage(url);
    context = std::make_unique<ScopedBitmapContext>(image->image);

    addMaskedImageViewToWindow();
}

auto MaskedCoreGraphicsImage::width() -> double {
    return CGImageGetWidth(image->image);
}

auto MaskedCoreGraphicsImage::height() -> double {
    return CGImageGetHeight(image->image);
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
                                             image->image, mask.image)];
    if (lastImageView != nil)
        [lastImageView removeFromSuperview];
    lastImageView = imageView;

    [window.contentView addSubview:imageView];
    // https://stackoverflow.com/a/4681677
    [imageView
        setFrameOrigin:NSMakePoint(
                           std::round((NSWidth([window.contentView bounds]) -
                                          NSWidth([imageView frame])) /
                               2),
                           std::round((NSHeight([window.contentView bounds]) -
                                          NSHeight([imageView frame])) /
                               2))];
}

void MaskedCoreGraphicsImage::show() { [lastImageView setHidden:NO]; }

void MaskedCoreGraphicsImage::hide() { [lastImageView setHidden:YES]; }
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
