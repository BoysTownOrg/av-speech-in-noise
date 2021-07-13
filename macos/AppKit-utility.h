#ifndef AV_SPEECH_IN_NOISE_MACOS_APPKIT_UTILITY_H_
#define AV_SPEECH_IN_NOISE_MACOS_APPKIT_UTILITY_H_

#import <AppKit/AppKit.h>

#include <string>

@interface ResizesToContentsViewController : NSTabViewController
@end

namespace av_speech_in_noise {
auto nsTabViewControllerWithoutTabControl() -> NSTabViewController *;
auto nsButton(const std::string &s, id target, SEL action) -> NSButton *;
void addAutolayoutEnabledSubview(NSView *parent, NSView *child);
}

#endif
