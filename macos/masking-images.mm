// https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_context/dq_context.html#//apple_ref/doc/uid/TP30001066-CH203-CJBHBFFE
// https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_images/dq_images.html#//apple_ref/doc/uid/TP30001066-CH212-TPXREF101

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
    ScopedBitmapImage(CGContextRef context)
        : image{CGBitmapContextCreateImage(context)} {
        if (image == nullptr)
            throw std::runtime_error{"Unable to create bitmap image."};
    }

    ~ScopedBitmapImage() { CGImageRelease(image); }
    CGImageRef image;
};

class ScopedImageSource {
  public:
    ScopedImageSource(CFURLRef url)
        : imageSource{CGImageSourceCreateWithURL(url, nullptr)} {
        if (imageSource == nullptr)
            throw std::runtime_error{"Unable to create image source."};
    }

    ~ScopedImageSource() { CFRelease(imageSource); }
    CGImageSourceRef imageSource;
};

class ScopedImage {
  public:
    ScopedImage(CGImageSourceRef imageSource)
        : image{CGImageSourceCreateImageAtIndex(imageSource, 0, nullptr)} {
        if (imageSource == nullptr)
            throw std::runtime_error{"Unable to create image."};
    }

    ~ScopedImage() { CGImageRelease(image); }
    CGImageRef image;
};

static CGContextRef createBitmapContext(int pixelsWide, int pixelsHigh) {
    const auto bitmapBytesPerRow = (pixelsWide * 3); // 1
    const auto colorSpace = CGColorSpaceCreateDeviceGray(); // 2
    const auto context = CGBitmapContextCreate(nullptr, // 4
        pixelsWide, pixelsHigh,
        8, // bits per component
        bitmapBytesPerRow, colorSpace, kCGImageAlphaNone);
    CGColorSpaceRelease(colorSpace);
    return context;
};

class ScopedBitmapContext {
  public:
    ScopedBitmapContext(CGImageRef image)
        : context{createBitmapContext(
              CGImageGetWidth(image), CGImageGetHeight(image))} {
        if (context == nullptr)
            throw std::runtime_error{"Unable to create bitmap context"};
    }

    ~ScopedBitmapContext() { CGContextRelease(context); }
    CGContextRef context;
};

static auto scopedMaskedImage() -> std::unique_ptr<ScopedMaskedImage> {
    NSURL *url = [[NSBundle mainBundle] URLForImageResource:@"wally.jpg"];
    if (url == nil) {
        throw std::runtime_error{"URL is nil. exiting."};
    }
    const auto imageSource{ScopedImageSource{(__bridge CFURLRef)(url)}};
    const auto image{ScopedImage{imageSource.imageSource}};
    const auto context{ScopedBitmapContext{image.image}};
    CGContextSetRGBFillColor(context.context, 1, 1, 1, 1);
    CGContextFillRect(context.context, CGRectMake(0, 0, 100, 100));
    CGContextSetRGBFillColor(context.context, 1, 1, 1, 1);
    CGContextFillRect(context.context, CGRectMake(100, 100, 100, 100));
    CGContextSetRGBFillColor(context.context, 1, 1, 1, 0);
    CGContextFillRect(context.context, CGRectMake(0, 100, 100, 100));
    CGContextSetRGBFillColor(context.context, 0, 0, 0, 0);
    CGContextFillRect(context.context, CGRectMake(100, 0, 100, 100));
    const auto mask{ScopedBitmapImage{context.context}};
    return std::make_unique<ScopedMaskedImage>(image.image, mask.image);
}

struct ImageRegion {
    double x;
    double y;
    double width;
    double height;
};

static auto imageResource(NSString *imageName) -> NSURL * {
    NSURL *url = [[NSBundle mainBundle] URLForImageResource:imageName];
    if (url == nil) {
        throw std::runtime_error{"Cannot get URL for image."};
    }
    return url;
}

class TBD {
    TBD(NSWindow *window, NSString *imageName)
        : image{ScopedImage{
              ScopedImageSource{(__bridge CFURLRef)(imageResource(imageName))}
                  .imageSource}},
          context{ScopedBitmapContext{image.image}}, window{window} {}

    void drawSomething(ImageRegion region) {
        CGContextSetRGBFillColor(context.context, 1, 1, 1, 1);
        CGContextFillRect(context.context,
            CGRectMake(region.x, region.y, region.width, region.height));
    }

    void addMaskedImageViewToWindow() {
        const auto mask{ScopedBitmapImage{context.context}};
        const auto imageView = [[ImageView alloc]
            initWithImage:std::make_unique<ScopedMaskedImage>(
                              image.image, mask.image)];

        if (lastImageView != nil)
            [lastImageView removeFromSuperview];
        lastImageView = imageView;

        [window.contentView addSubview:imageView];
    }

  private:
    ScopedImage image;
    ScopedBitmapContext context;
    NSWindow *window;
    NSView *lastImageView;
};
}
@implementation ImageView {
    std::unique_ptr<av_speech_in_noise::ScopedMaskedImage> _image;
}
- (id)initWithImage:
    (std::unique_ptr<av_speech_in_noise::ScopedMaskedImage>)image {
    self = [super initWithFrame:NSMakeRect(0, 0, CGImageGetWidth(image->masked),
                                    CGImageGetHeight(image->masked))];
    if (self) {
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

@interface AppDelegate : NSObject <NSApplicationDelegate>

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    NSTabViewController *controller = [[NSTabViewController alloc] init];
    [controller setTabStyle:NSTabViewControllerTabStyleUnspecified];
    const auto window = [NSWindow windowWithContentViewController:controller];
    [window makeKeyAndOrderFront:nil];
    const auto view = [[ImageView alloc]
        initWithImage:av_speech_in_noise::scopedMaskedImage()];
    [window.contentView addSubview:view];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
    return YES;
}

@end
