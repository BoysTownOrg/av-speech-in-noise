#include "CocoaView.h"


void CocoaTesterView::show() {
    [view_ setHidden:NO];
}

@implementation ViewActions
@synthesize controller;
- (void)newTest {
    controller->newTest();
}

- (void)openTest {
    controller->openTest();
}

- (void)confirmTestSetup {
    controller->confirmTestSetup();
}

- (void)playTrial {
    controller->playTrial();
}
@end
