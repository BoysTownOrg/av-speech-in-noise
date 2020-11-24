#include "AppKit-utility.h"
#include "Foundation-utility.h"

auto nsTabViewControllerWithoutTabControl() -> NSTabViewController * {
    const auto controller{[[NSTabViewController alloc] init]};
    [controller setTabStyle:NSTabViewControllerTabStyleUnspecified];
    return controller;
}

auto button(const std::string &s, id target, SEL action) -> NSButton * {
    return [NSButton buttonWithTitle:nsString(s) target:target action:action];
}
