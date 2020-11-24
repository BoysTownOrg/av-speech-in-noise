#ifndef AV_SPEECH_IN_NOISE_MACOS_APPKIT_UTILITY_H_
#define AV_SPEECH_IN_NOISE_MACOS_APPKIT_UTILITY_H_

#import <AppKit/AppKit.h>
#include <string>

auto nsTabViewControllerWithoutTabControl() -> NSTabViewController *;
auto button(const std::string &s, id target, SEL action) -> NSButton *;

#endif
