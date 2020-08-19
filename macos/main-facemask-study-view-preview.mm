#include "FacemaskStudySetupView.h"

int main() {
    const auto viewController{[[NSTabViewController alloc] init]};
    [viewController setTabStyle:NSTabViewControllerTabStyleUnspecified];
    NSWindow *window{[NSWindow windowWithContentViewController:viewController]};
    av_speech_in_noise::FacemaskStudySetupView view{viewController};
    [window makeKeyAndOrderFront:nil];
    [[NSApplication sharedApplication] run];
}
