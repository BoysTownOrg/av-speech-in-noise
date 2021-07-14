#include "AppKit-utility.h"
#include "Foundation-utility.h"

@implementation ResizesToContentsViewController
- (instancetype)init {
    if ((self = [super init]) != nullptr) {
        [self setTabStyle:NSTabViewControllerTabStyleUnspecified];
    }
    return self;
}

- (void)viewWillAppear {
    [super viewWillAppear];
    self.preferredContentSize = self.view.fittingSize;
}
@end

namespace av_speech_in_noise {
auto nsTabViewControllerWithoutTabControl() -> NSTabViewController * {
    const auto controller{[[NSTabViewController alloc] init]};
    [controller setTabStyle:NSTabViewControllerTabStyleUnspecified];
    return controller;
}

auto nsButton(const std::string &s, id target, SEL action) -> NSButton * {
    return [NSButton buttonWithTitle:nsString(s) target:target action:action];
}

void addAutolayoutEnabledSubview(NSView *parent, NSView *child) {
    child.translatesAutoresizingMaskIntoConstraints = NO;
    [parent addSubview:child];
}

auto subjectWindow() -> NSWindow * {
    const auto screen{[[NSScreen screens] lastObject]};
    const auto screenFrame{screen.frame};
    const auto nsViewController{nsTabViewControllerWithoutTabControl()};
    nsViewController.view.frame = screenFrame;
    const auto window{
        [NSWindow windowWithContentViewController:nsViewController]};
    [window setStyleMask:NSWindowStyleMaskBorderless];
    [window setFrame:screenFrame display:YES];
    window.level = NSScreenSaverWindowLevel;
    return window;
}
}
