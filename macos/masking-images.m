// https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_context/dq_context.html#//apple_ref/doc/uid/TP30001066-CH203-CJBHBFFE
// https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_images/dq_images.html#//apple_ref/doc/uid/TP30001066-CH212-TPXREF101

#include <AppKit/AppKit.h>
#include <Foundation/Foundation.h>
#include <CoreGraphics/CoreGraphics.h>

@interface View : NSView
@property CGImageRef image;
@end

@implementation View
- (id)initWithImage:(CGImageRef)image {
    self = [super initWithFrame:NSMakeRect(0, 0, CGImageGetWidth(image),
                                    CGImageGetHeight(image))];
    self.image = image;
    return self;
}

- (void)drawRect:(NSRect)rect {
    CGContextRef myContext = [[NSGraphicsContext // 1
        currentContext] CGContext];
    // ********** Your drawing code here ********** // 2
    CGContextDrawImage(myContext,
        CGRectMake(
            0, 0, CGImageGetWidth(self.image), CGImageGetHeight(self.image)),
        self.image);
}
@end

@interface AppDelegate : NSObject <NSApplicationDelegate>

@end

CGContextRef MyCreateBitmapContext(int pixelsWide, int pixelsHigh)

{
    CGContextRef context = NULL;
    CGColorSpaceRef colorSpace;
    void *bitmapData;
    int bitmapByteCount;
    int bitmapBytesPerRow;
    bitmapBytesPerRow = (pixelsWide * 3); // 1
    bitmapByteCount = (bitmapBytesPerRow * pixelsHigh);
    colorSpace = CGColorSpaceCreateDeviceGray(); // 2
    bitmapData = calloc(bitmapByteCount, sizeof(uint8_t)); // 3
    if (bitmapData == NULL) {
        fprintf(stderr, "Memory not allocated!");
        return NULL;
    }
    context = CGBitmapContextCreate(bitmapData, // 4
        pixelsWide, pixelsHigh,
        8, // bits per component
        bitmapBytesPerRow, colorSpace, kCGImageAlphaNone);
    if (context == NULL) {
        free(bitmapData); // 5
        fprintf(stderr, "Context not created!");
        return NULL;
    }
    CGColorSpaceRelease(colorSpace); // 6
    return context; // 7
}

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    NSURL *url = [[NSBundle mainBundle] URLForImageResource:@"wally.jpg"];
    if (url == nil) {
        NSLog(@"URL is nil. exiting.");
        return;
    }
    CGImageSourceRef imageSource =
        CGImageSourceCreateWithURL((CFURLRef)url, nil);
    NSLog(@"image count: %zu", CGImageSourceGetCount(imageSource));
    CGImageRef image = CGImageSourceCreateImageAtIndex(imageSource, 0, nil);
    NSLog(@"image width: %zu", CGImageGetWidth(image));
    CGContextRef context =
        MyCreateBitmapContext(CGImageGetWidth(image), CGImageGetHeight(image));
    CGContextSetRGBFillColor(context, 1, 1, 1, 1);
    CGContextFillRect(context, CGRectMake(0, 0, 100, 100));
    CGContextSetRGBFillColor(context, 1, 1, 1, 1);
    CGContextFillRect(context, CGRectMake(100, 100, 100, 100));
    CGContextSetRGBFillColor(context, 1, 1, 1, 0);
    CGContextFillRect(context, CGRectMake(0, 100, 100, 100));
    CGContextSetRGBFillColor(context, 0, 0, 0, 0);
    CGContextFillRect(context, CGRectMake(100, 0, 100, 100));
    CGImageRef mask = CGBitmapContextCreateImage(context);
    CGImageRef masked = CGImageCreateWithMask(image, mask);
    if (masked == nil) {
        NSLog(@"Masked is nil");
    }
    // CGImageRelease(masked);
    // CGImageRelease(mask);
    CGContextRelease(context);
    // CGImageRelease(image);
    CFRelease(imageSource);

    NSTabViewController *controller = [[NSTabViewController alloc] init];
    [controller setTabStyle:NSTabViewControllerTabStyleUnspecified];
    NSWindow *window = [NSWindow windowWithContentViewController:controller];
    [window makeKeyAndOrderFront:nil];
    View *view = [[View alloc] initWithImage:masked];
    [window.contentView addSubview:view];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
}

@end
