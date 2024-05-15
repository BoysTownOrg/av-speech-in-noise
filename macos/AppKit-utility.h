#ifndef AV_SPEECH_IN_NOISE_MACOS_APPKIT_UTILITY_H_
#define AV_SPEECH_IN_NOISE_MACOS_APPKIT_UTILITY_H_

#include <av-speech-in-noise/Interface.hpp>

#import <AppKit/AppKit.h>

#include <string>

@interface ResizesToContentsViewController : NSTabViewController
@end

namespace av_speech_in_noise {
class KeyPressListener {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(KeyPressListener);
    virtual void onPress(NSEvent *) = 0;
};
}

@interface KeyableSubjectWindow : NSWindow {
  @public
    av_speech_in_noise::KeyPressListener *listener;
}
@end

namespace av_speech_in_noise {
auto nsTabViewControllerWithoutTabControl() -> NSTabViewController *;
auto nsButton(const std::string &s, id target, SEL action) -> NSButton *;
void addAutolayoutEnabledSubview(NSView *parent, NSView *child);
auto subjectWindow() -> KeyableSubjectWindow *;
}

#endif
