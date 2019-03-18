#include "CocoaView.h"

static NSTextField *allocLabel(NSString *label, NSRect frame) {
    const auto text = [[NSTextField alloc] initWithFrame:frame];
    [text setStringValue:label];
    [text setBezeled:NO];
    [text setDrawsBackground:NO];
    [text setEditable:NO];
    [text setSelectable:NO];
    return text;
}

CocoaTesterView::CocoaTesterView() :
    deviceMenu{[
        [NSPopUpButton alloc] initWithFrame:NSMakeRect(50, 50, 140, 30)
        pullsDown:NO
    ]},
    view_{
        [[NSView alloc] initWithFrame:NSMakeRect(50, 50, 500, 600)]
    }
{
    [view_ setHidden:YES];
    [view_ addSubview:deviceMenu];
}

NSView *CocoaTesterView::view() {
    return view_;
}

void CocoaTesterView::show() {
    [view_ setHidden:NO];
}

void CocoaTesterView::hide() {
    [view_ setHidden:YES];
}

std::string CocoaTesterView::audioDevice() {
    return [deviceMenu.titleOfSelectedItem UTF8String];
}

void CocoaTesterView::populateAudioDeviceMenu(std::vector<std::string> items) {
    for (auto item : items) {
        auto title = [NSString stringWithCString:
            item.c_str()
            encoding:[NSString defaultCStringEncoding]
        ];
        [deviceMenu addItemWithTitle: title];
    }
}

CocoaTestSetupView::CocoaTestSetupView(CocoaView *parent_) :
    parent_{parent_},
    view_{
        [[NSView alloc] initWithFrame:NSMakeRect(100, 100, 500, 600)]
    },
    subjectIdLabel{allocLabel(
        @"subject id:",
        NSMakeRect(10, 490, 140, 25)
    )},
    subjectId_{
        [[NSTextField alloc]
            initWithFrame:NSMakeRect(155, 490, 150, 25)]
    },
    testerIdLabel{allocLabel(
        @"tester id:",
        NSMakeRect(10, 460, 140, 25))
    },
    testerId_{
        [[NSTextField alloc]
            initWithFrame:NSMakeRect(155, 460, 150, 25)]
    },
    signalLevel_dB_SPL_label{allocLabel(
        @"signal level (dB SPL):",
        NSMakeRect(10, 430, 140, 25))
    },
    signalLevel_dB_SPL_{
        [[NSTextField alloc]
            initWithFrame:NSMakeRect(155, 430, 150, 25)]
    },
    maskerLevel_dB_SPL_label{allocLabel(
        @"masker level (dB SPL):",
        NSMakeRect(10, 400, 140, 25))
    },
    maskerLevel_dB_SPL_{
        [[NSTextField alloc]
            initWithFrame:NSMakeRect(155, 400, 150, 25)]
    },
    stimulusListDirectoryLabel{allocLabel(
        @"stimulus directory:",
        NSMakeRect(10, 370, 140, 25))
    },
    stimulusListDirectory_{
        [[NSTextField alloc]
            initWithFrame:NSMakeRect(155, 370, 300, 25)]
    },
    maskerFilePath_label{allocLabel(
        @"masker file path:",
        NSMakeRect(10, 340, 140, 25))
    },
    maskerFilePath_{
        [[NSTextField alloc]
            initWithFrame:NSMakeRect(155, 340, 300, 25)]
    },
    conditionMenu{[
        [NSPopUpButton alloc] initWithFrame:NSMakeRect(155, 310, 150, 25)
        pullsDown:NO
    ]},
    actions{[SetupViewActions alloc]}
{
    const auto confirmButton = [NSButton buttonWithTitle:
        @"Confirm"
        target:actions
        action:@selector(confirmTestSetup)
    ];
    [view_ addSubview:confirmButton];
    [view_ setHidden:YES];
    [view_ addSubview:subjectIdLabel];
    [view_ addSubview:subjectId_];
    [view_ addSubview:testerIdLabel];
    [view_ addSubview:testerId_];
    [view_ addSubview:signalLevel_dB_SPL_label];
    [view_ addSubview:signalLevel_dB_SPL_];
    [view_ addSubview:maskerLevel_dB_SPL_label];
    [view_ addSubview:maskerLevel_dB_SPL_];
    [view_ addSubview:stimulusListDirectoryLabel];
    [view_ addSubview:stimulusListDirectory_];
    [view_ addSubview:maskerFilePath_label];
    [view_ addSubview:maskerFilePath_];
    [view_ addSubview:conditionMenu];
    stimulusListDirectory_.stringValue =
        @"/Users/basset/Documents/maxdetection/Stimuli/Video/List_Detection";
    maskerFilePath_.stringValue =
        @"/Users/basset/Documents/maxdetection/Stimuli/Masker/L1L2_EngEng.wav";
}

NSView *CocoaTestSetupView::view() {
    return view_;
}

void CocoaTestSetupView::show() {
    [view_ setHidden:NO];
}

void CocoaTestSetupView::hide() {
    [view_ setHidden:YES];
}

std::string CocoaTestSetupView::maskerLevel_dB_SPL() {
    return [maskerLevel_dB_SPL_.stringValue UTF8String];
}

std::string CocoaTestSetupView::signalLevel_dB_SPL() {
    return [signalLevel_dB_SPL_.stringValue UTF8String];
}

std::string CocoaTestSetupView::maskerFilePath() {
    return [maskerFilePath_.stringValue UTF8String];
}

std::string CocoaTestSetupView::stimulusListDirectory() {
    return [stimulusListDirectory_.stringValue UTF8String];
}

std::string CocoaTestSetupView::testerId() {
    return [testerId_.stringValue UTF8String];
}

std::string CocoaTestSetupView::subjectId() {
    return [subjectId_.stringValue UTF8String];
}

std::string CocoaTestSetupView::condition() {
    return [conditionMenu.titleOfSelectedItem UTF8String];
}

void CocoaTestSetupView::populateConditionMenu(std::vector<std::string> items) {
    for (auto item : items) {
        auto title = [NSString stringWithCString:
            item.c_str()
            encoding:[NSString defaultCStringEncoding]
        ];
        [conditionMenu addItemWithTitle: title];
    }
}

void CocoaTestSetupView::confirm() {
    parent_->confirmTestSetup();
}

@implementation SetupViewActions
@synthesize controller;
- (void)confirmTestSetup {
    controller->confirm();
}
@end


CocoaSubjectView::CocoaSubjectView() :
    // Defer may be critical here...
    window{
        [[NSWindow alloc]
            initWithContentRect: NSMakeRect(600, 400, 400, 400)
            styleMask:NSWindowStyleMaskBorderless
            backing:NSBackingStoreBuffered
            defer:YES
        ]
    }
{
    const auto greenButton = [NSButton buttonWithTitle:@"hello" target:nil action:nil];
    greenButton.bordered = false;
    greenButton.wantsLayer = true;
    [[greenButton layer] setBackgroundColor:[[NSColor systemGreenColor] CGColor]];
    [window.contentView addSubview:greenButton];
    [window makeKeyAndOrderFront:nil];
}

int CocoaSubjectView::numberResponse() {
    return 0;
}

bool CocoaSubjectView::greenResponse() {
    return true;
}

CocoaView::CocoaView() :
    app{[NSApplication sharedApplication]},
    window{
        [[NSWindow alloc] initWithContentRect:
            NSMakeRect(300, 500, 900, 800)
            styleMask:
                NSWindowStyleMaskClosable |
                NSWindowStyleMaskResizable |
                NSWindowStyleMaskTitled
            backing:NSBackingStoreBuffered
            defer:NO
        ]
    },
    tbdView{
        [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 500, 100)]
    },
    actions{[ViewActions alloc]}
{
    app.mainMenu = [[NSMenu alloc] init];
    auto appMenu = [[NSMenuItem alloc] init];
    [app.mainMenu addItem:appMenu];
    auto fileMenu = [[NSMenuItem alloc] init];
    [app.mainMenu addItem:fileMenu];
    auto appSubMenu = [[NSMenu alloc] init];
    [appSubMenu addItemWithTitle:
        @"Quit"
        action:@selector(terminate:)
        keyEquivalent:@"q"
    ];
    [appMenu setSubmenu:appSubMenu];
    auto fileSubMenu = [[NSMenu alloc] initWithTitle:@"File"];
    auto newTestItem = [[NSMenuItem alloc] initWithTitle:
        @"New Test..."
        action:@selector(newTest)
        keyEquivalent:@"n"
    ];
    newTestItem.target = actions;
    [fileSubMenu addItem:newTestItem];
    auto openTestItem = [[NSMenuItem alloc] initWithTitle:
        @"Open Test..."
        action:@selector(openTest)
        keyEquivalent:@"o"
    ];
    openTestItem.target = actions;
    [fileSubMenu addItem:openTestItem];
    [fileMenu setSubmenu:fileSubMenu];
    const auto playTrialButton = [NSButton buttonWithTitle:
        @"Play Next Trial"
        target:actions
        action:@selector(playTrial)
    ];
    playTrialButton.frame = NSMakeRect(200, 0, 130, 40);
    [tbdView addSubview:playTrialButton];
    [window.contentView addSubview:tbdView];
    [window.contentView addSubview:testerView_.view()];
    [window.contentView addSubview:testSetupView_.view()];
    actions.controller = this;
    [window makeKeyAndOrderFront:nil];
}

void CocoaView::confirmTestSetup() {
    listener->confirmTestSetup();
}

void CocoaView::playTrial() {
    listener->playTrial();
}

void CocoaView::newTest() {
    listener->newTest();
}

void CocoaView::openTest() {
    listener->openTest();
}

void CocoaView::subscribe(EventListener *listener_) {
    listener = listener_;
}

void CocoaView::eventLoop() {
    [app run];
}

auto CocoaView::testSetup() -> TestSetup * {
    return &testSetupView_;
}

auto CocoaView::tester() -> Tester * {
    return &testerView_;
}

auto CocoaView::subject() -> SubjectView * {
    return &subjectView_;
}

auto CocoaView::showConfirmationDialog() -> DialogResponse {
    const auto alert = [[NSAlert alloc] init];
    [alert setMessageText:@"Are you sure?"];
    [alert setInformativeText:@"huehuehue"];
    [alert addButtonWithTitle:@"Cancel"];
    [alert addButtonWithTitle:@"No"];
    [alert addButtonWithTitle:@"Yes"];
    switch([alert runModal]) {
        case NSAlertSecondButtonReturn:
            return DialogResponse::decline;
        case NSAlertThirdButtonReturn:
            return DialogResponse::accept;
        default:
            return DialogResponse::cancel;
    }
}

void CocoaView::showErrorMessage(std::string s) {
    auto alert = [[NSAlert alloc] init];
    [alert setMessageText:@"Error."];
    auto errorMessage_ = [
        NSString stringWithCString:s.c_str()
        encoding:[NSString defaultCStringEncoding]
    ];
    [alert setInformativeText:errorMessage_];
    [alert addButtonWithTitle:@"Ok"];
    [alert runModal];
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


