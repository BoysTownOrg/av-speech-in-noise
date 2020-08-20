#include "FacemaskStudySetupView.h"

@interface WindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation WindowDelegate
- (void)windowWillClose:(NSNotification *)__unused notification {
    [NSApp terminate:self];
}
@end

int main() {
    const auto viewController{[[NSTabViewController alloc] init]};
    [viewController setTabStyle:NSTabViewControllerTabStyleUnspecified];
    NSWindow *window{[NSWindow windowWithContentViewController:viewController]};
    av_speech_in_noise::FacemaskStudySetupView view{viewController};
    [window makeKeyAndOrderFront:nil];
    auto app{[NSApplication sharedApplication]};
    app.mainMenu = [[NSMenu alloc] init];
    const auto appMenu{[[NSMenuItem alloc] init]};
    const auto appSubMenu{[[NSMenu alloc] init]};
    [appSubMenu addItemWithTitle:@"Quit"
                          action:@selector(stop:)
                   keyEquivalent:@"q"];
    [appMenu setSubmenu:appSubMenu];
    [app.mainMenu addItem:appMenu];
    [window setDelegate:[[WindowDelegate alloc] init]];
    [[NSApplication sharedApplication] run];
}
