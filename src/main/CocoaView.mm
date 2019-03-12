#include "CocoaView.h"

CocoaTesterView::CocoaTesterView() {
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
    for (const auto &item : items) {
        auto title = [NSString stringWithCString:
            item.c_str()
            encoding:[NSString defaultCStringEncoding]
        ];
        [deviceMenu addItemWithTitle: title];
    }
}

CocoaTestSetupView::CocoaTestSetupView() {
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
    return "";
}

CocoaView::CocoaView() {
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
    const auto confirmButton = [NSButton buttonWithTitle:
        @"Confirm"
        target:actions
        action:@selector(confirmTestSetup)
    ];
    confirmButton.frame = NSMakeRect(0, 0, 130, 40);
    playTrialButton.frame = NSMakeRect(200, 0, 130, 40);
    [tbdView addSubview:confirmButton];
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
