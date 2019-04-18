#include "CocoaView.h"
#include "common-objc.h"

static NSTextField *allocLabel(NSString *label, NSRect frame) {
    const auto text = [[NSTextField alloc] initWithFrame:frame];
    [text setStringValue:label];
    [text setBezeled:NO];
    [text setDrawsBackground:NO];
    [text setEditable:NO];
    [text setSelectable:NO];
    [text setAlignment:NSTextAlignmentRight];
    return text;
}

CocoaTesterView::CocoaTesterView(NSRect r) :
    view_{[[NSView alloc] initWithFrame:r]}
{
    [view_ setHidden:YES];
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

static constexpr auto labelHeight = 22;
static constexpr auto labelWidth = 140;
static constexpr auto labelToTextFieldSpacing = 5;
static constexpr auto textFieldLeadingEdge =
    labelWidth + labelToTextFieldSpacing;
static constexpr auto normalTextFieldWidth = 150;
static constexpr auto filePathTextFieldWidth = 500;
static constexpr auto buttonHeight = 25;
static constexpr auto buttonWidth = 100;

static NSRect normalTextFieldSizeAtHeight(CGFloat y) {
    return NSMakeRect(
        textFieldLeadingEdge,
        y,
        normalTextFieldWidth,
        labelHeight
    );
}

static NSRect filePathTextFieldSizeAtHeight(CGFloat y) {
    return NSMakeRect(
        textFieldLeadingEdge,
        y,
        filePathTextFieldWidth,
        labelHeight
    );
}

CocoaTestSetupView::CocoaTestSetupView(NSRect r) :
    view_{[[NSView alloc] initWithFrame:r]},
    subjectIdLabel{allocLabel(
        @"subject:",
        NSMakeRect(0, 270, labelWidth, labelHeight)
    )},
    subjectId_{[[NSTextField alloc]
        initWithFrame:normalTextFieldSizeAtHeight(270)
    ]},
    testerIdLabel{allocLabel(
        @"tester:",
        NSMakeRect(0, 240, labelWidth, labelHeight)
    )},
    testerId_{[[NSTextField alloc]
        initWithFrame:normalTextFieldSizeAtHeight(240)
    ]},
    sessionLabel{allocLabel(
        @"session:",
        NSMakeRect(0, 210, labelWidth, labelHeight)
    )},
    session_{[[NSTextField alloc]
        initWithFrame:normalTextFieldSizeAtHeight(210)
    ]},
    maskerLevel_dB_SPL_label{allocLabel(
        @"masker level (dB SPL):",
        NSMakeRect(0, 180, labelWidth, labelHeight)
    )},
    maskerLevel_dB_SPL_{[[NSTextField alloc]
        initWithFrame:normalTextFieldSizeAtHeight(180)
    ]},
    calibrationLevel_dB_SPL_label{allocLabel(
        @"calibration level (dB SPL):",
        NSMakeRect(350, 30, 150, labelHeight)
    )},
    calibrationLevel_dB_SPL_{[[NSTextField alloc]
        initWithFrame:NSMakeRect(505, 30, 80, labelHeight)
    ]},
    startingSnr_dB_label{allocLabel(
        @"starting SNR (dB):",
        NSMakeRect(0, 150, labelWidth, labelHeight)
    )},
    startingSnr_dB_{[[NSTextField alloc]
        initWithFrame:normalTextFieldSizeAtHeight(150)
    ]},
    targetListDirectoryLabel{allocLabel(
        @"targets:",
        NSMakeRect(0, 120, labelWidth, labelHeight)
    )},
    targetListDirectory_{[[NSTextField alloc]
        initWithFrame:filePathTextFieldSizeAtHeight(120)
    ]},
    maskerFilePath_label{allocLabel(
        @"masker:",
        NSMakeRect(0, 90, labelWidth, labelHeight)
    )},
    maskerFilePath_{[[NSTextField alloc]
        initWithFrame:filePathTextFieldSizeAtHeight(90)
    ]},
    calibrationFilePath_label{allocLabel(
        @"calibration:",
        NSMakeRect(0, 60, labelWidth, labelHeight)
    )},
    calibrationFilePath_{[[NSTextField alloc]
        initWithFrame:filePathTextFieldSizeAtHeight(60)
    ]},
    condition_label{allocLabel(
        @"condition:",
        NSMakeRect(0, 30, labelWidth, labelHeight)
    )},
    conditionMenu{[[NSPopUpButton alloc]
        initWithFrame:NSMakeRect(textFieldLeadingEdge, 30, normalTextFieldWidth, labelHeight)
        pullsDown:NO
    ]},
    actions{[SetupViewActions alloc]}
{
    actions.controller = this;
    const auto browseForStimulusListButton = [NSButton
        buttonWithTitle:@"browse"
        target:actions
        action:@selector(browseForTargetList)
    ];
    [browseForStimulusListButton setFrame:NSMakeRect(filePathTextFieldWidth + textFieldLeadingEdge + 10, 120, buttonWidth, buttonHeight)];
    const auto browseForMaskerButton = [NSButton
        buttonWithTitle:@"browse"
        target:actions
        action:@selector(browseForMasker)
    ];
    [browseForMaskerButton setFrame:NSMakeRect(filePathTextFieldWidth + textFieldLeadingEdge + 10, 90, buttonWidth, buttonHeight)];
    const auto browseForCalibrationButton = [NSButton
        buttonWithTitle:@"browse"
        target:actions
        action:@selector(browseForCalibration)
    ];
    [browseForCalibrationButton setFrame:NSMakeRect(filePathTextFieldWidth + textFieldLeadingEdge + 10, 60, buttonWidth, buttonHeight)];
    const auto confirmButton = [NSButton
        buttonWithTitle:@"Confirm"
        target:actions
        action:@selector(confirmTestSetup)
    ];
    [confirmButton setFrame:NSMakeRect(900 - 100 - 2*15, 15, buttonWidth, buttonHeight)];
    const auto playCalibrationButton = [NSButton
        buttonWithTitle:@"play"
        target:actions
        action:@selector(playCalibration)
    ];
    [playCalibrationButton setFrame:NSMakeRect(filePathTextFieldWidth + textFieldLeadingEdge + 10, 30, buttonWidth, buttonHeight)];
    [view_ addSubview:browseForMaskerButton];
    [view_ addSubview:browseForStimulusListButton];
    [view_ addSubview:browseForCalibrationButton];
    [view_ addSubview:confirmButton];
    [view_ addSubview:playCalibrationButton];
    [view_ addSubview:subjectIdLabel];
    [view_ addSubview:subjectId_];
    [view_ addSubview:testerIdLabel];
    [view_ addSubview:testerId_];
    [view_ addSubview:sessionLabel];
    [view_ addSubview:session_];
    [view_ addSubview:maskerLevel_dB_SPL_label];
    [view_ addSubview:maskerLevel_dB_SPL_];
    [view_ addSubview:calibrationLevel_dB_SPL_label];
    [view_ addSubview:calibrationLevel_dB_SPL_];
    [view_ addSubview:startingSnr_dB_label];
    [view_ addSubview:startingSnr_dB_];
    [view_ addSubview:targetListDirectoryLabel];
    [view_ addSubview:targetListDirectory_];
    [view_ addSubview:maskerFilePath_label];
    [view_ addSubview:maskerFilePath_];
    [view_ addSubview:calibrationFilePath_label];
    [view_ addSubview:calibrationFilePath_];
    [view_ addSubview:condition_label];
    [view_ addSubview:conditionMenu];
    targetListDirectory_.stringValue =
        @"/Users/basset/Documents/maxdetection/Stimuli/Video/List_Detection";
    maskerFilePath_.stringValue =
        @"/Users/basset/Documents/maxdetection/Stimuli/Masker/L1L2_EngEng.wav";
    maskerLevel_dB_SPL_.stringValue = @"65";
    startingSnr_dB_.stringValue = @"0";
    [view_ setHidden:NO];
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

std::string CocoaTestSetupView::startingSnr_dB() {
    return startingSnr_dB_.stringValue.UTF8String;
}

std::string CocoaTestSetupView::maskerLevel_dB_SPL() {
    return maskerLevel_dB_SPL_.stringValue.UTF8String;
}

std::string CocoaTestSetupView::calibrationLevel_dB_SPL() {
    return calibrationLevel_dB_SPL_.stringValue.UTF8String;
}

std::string CocoaTestSetupView::maskerFilePath() {
    return maskerFilePath_.stringValue.UTF8String;
}

std::string CocoaTestSetupView::calibrationFilePath() {
    return calibrationFilePath_.stringValue.UTF8String;
}

std::string CocoaTestSetupView::targetListDirectory() {
    return targetListDirectory_.stringValue.UTF8String;
}

std::string CocoaTestSetupView::testerId() {
    return testerId_.stringValue.UTF8String;
}

std::string CocoaTestSetupView::subjectId() {
    return subjectId_.stringValue.UTF8String;
}

std::string CocoaTestSetupView::condition() {
    return conditionMenu.titleOfSelectedItem.UTF8String;
}

std::string CocoaTestSetupView::session() {
    return session_.stringValue.UTF8String;
}

void CocoaTestSetupView::setTargetListDirectory(std::string s) {
    [targetListDirectory_ setStringValue:asNsString(std::move(s))];
}

void CocoaTestSetupView::setCalibrationFilePath(std::string s) {
    [calibrationFilePath_ setStringValue:asNsString(std::move(s))];
}

void CocoaTestSetupView::setMasker(std::string s) {
    [maskerFilePath_ setStringValue:asNsString(std::move(s))];
}

void CocoaTestSetupView::populateConditionMenu(std::vector<std::string> items) {
    for (auto item : items)
        [conditionMenu addItemWithTitle:asNsString(item)];
}

void CocoaTestSetupView::confirm() {
    listener_->confirmTestSetup();
}

void CocoaTestSetupView::subscribe(EventListener *listener) {
    listener_ = listener;
}

void CocoaTestSetupView::browseForMasker() {
    listener_->browseForMasker();
}

void CocoaTestSetupView::browseForTargetList() {
    listener_->browseForTargetList();
}

void CocoaTestSetupView::browseForCalibration() {
    listener_->browseForCalibration();
}

void CocoaTestSetupView::playCalibration() {
    listener_->playCalibration();
}

@implementation SetupViewActions
@synthesize controller;

- (void)confirmTestSetup {
    controller->confirm();
}

- (void)browseForTargetList { 
    controller->browseForTargetList();
}

- (void)browseForMasker { 
    controller->browseForMasker();
}

- (void)browseForCalibration {
    controller->browseForCalibration();
}

- (void)playCalibration { 
    controller->playCalibration();
}
@end

static auto greenColor = NSColor.greenColor;
static auto redColor = NSColor.redColor;
static auto blueColor = NSColor.blueColor;
static auto whiteColor = NSColor.whiteColor;

CocoaSubjectView::CocoaSubjectView() :
    // Defer may be critical here...
    window{[[NSWindow alloc]
        initWithContentRect:NSMakeRect(1150, 15, 750, 400)
        styleMask:NSWindowStyleMaskBorderless
        backing:NSBackingStoreBuffered
        defer:YES
    ]},
    responseButtons{[[NSView alloc]
        initWithFrame:NSMakeRect(0, 0, 750, 400)
    ]},
    nextTrialButton{[[NSView alloc]
        initWithFrame:NSMakeRect(0, 0, 750, 400)
    ]},
    actions{[SubjectViewActions alloc]}
{
    actions.controller = this;
    addButtonRow(blueColor, 0);
    addButtonRow(greenColor, 1);
    addButtonRow(whiteColor, 2);
    addButtonRow(redColor, 3);
    addNextTrialButton();
    [window.contentView addSubview:nextTrialButton];
    [window.contentView addSubview:responseButtons];
    hideResponseButtons();
    hideNextTrialButton();
    [window makeKeyAndOrderFront:nil];
}

void CocoaSubjectView::addButtonRow(NSColor *color, int row) {
    for (int i = 0; i < 8; ++i)
        addNumberButton(color, i, row);
}

void CocoaSubjectView::addNumberButton(NSColor *color, int i, int row) {
    auto title = asNsString(std::to_string(i+1));
    const auto button = [NSButton
        buttonWithTitle:@""
        target:actions
        action:@selector(respond:)
    ];
    [button setFrame:NSMakeRect(80*i, 80*row, 80, 80)];
    [button setBezelStyle:NSBezelStyleTexturedSquare];
    auto style = [[NSMutableParagraphStyle alloc] init];
    [style setAlignment:NSTextAlignmentCenter];
    auto attrsDictionary = [NSDictionary
        dictionaryWithObjectsAndKeys:
        color, NSForegroundColorAttributeName,
        style, NSParagraphStyleAttributeName,
        [NSFont fontWithName:@"Courier" size:36], NSFontAttributeName,
        nil
    ];
    auto attrString = [[NSAttributedString alloc]
        initWithString:title
        attributes:attrsDictionary
    ];
    [button setAttributedTitle:attrString];
    [responseButtons addSubview:button];
}

void CocoaSubjectView::addNextTrialButton() {
    const auto button = [NSButton
        buttonWithTitle:@""
        target:actions
        action:@selector(playTrial)
    ];
    [button setBezelStyle:NSBezelStyleTexturedSquare];
    auto style = [[NSMutableParagraphStyle alloc] init];
    [style setAlignment:NSTextAlignmentCenter];
    auto font = [NSFont fontWithName:@"Courier" size:36];
    auto attrsDictionary = [NSDictionary
        dictionaryWithObjectsAndKeys:
        font, NSFontAttributeName,
        nil
    ];
    auto attrString = [[NSAttributedString alloc]
        initWithString:@"Press when ready"
        attributes:attrsDictionary
    ];
    [button setAttributedTitle:attrString];
    [button setFrame:NSMakeRect(150, 175, 400, 100)];
    [nextTrialButton addSubview:button];
}

std::string CocoaSubjectView::numberResponse() {
    return lastButtonPressed.title.UTF8String;
}

bool CocoaSubjectView::greenResponse() {
    return lastPressedColor() == greenColor;
}

NSColor *CocoaSubjectView::lastPressedColor() {
    return [lastButtonPressed.attributedTitle
        attribute:NSForegroundColorAttributeName
        atIndex:0
        effectiveRange:nil
    ];
}

bool CocoaSubjectView::blueResponse() {
    return lastPressedColor() == blueColor;
}

bool CocoaSubjectView::whiteResponse() {
    return lastPressedColor() == whiteColor;
}

void CocoaSubjectView::respond(id sender) {
    lastButtonPressed = sender;
    listener_->submitResponse();
}

void CocoaSubjectView::showResponseButtons() {
    [responseButtons setHidden:NO];
}

void CocoaSubjectView::showNextTrialButton() {
    [nextTrialButton setHidden:NO];
}

void CocoaSubjectView::hideNextTrialButton() {
    [nextTrialButton setHidden:YES];
}

void CocoaSubjectView::playTrial() {
    listener_->playTrial();
}

void CocoaSubjectView::hideResponseButtons() {
    [responseButtons setHidden:YES];
}

void CocoaSubjectView::subscribe(EventListener *e) {
    listener_ = e;
}

@implementation SubjectViewActions
@synthesize controller;

- (void)respond:(id)sender {
    controller->respond(sender);
}

- (void)playTrial { 
    controller->playTrial();
}
@end

CocoaView::CocoaView(NSRect r) :
    app{[NSApplication sharedApplication]},
    window{[[NSWindow alloc]
        initWithContentRect:r
        styleMask:
            NSWindowStyleMaskClosable |
            NSWindowStyleMaskResizable |
            NSWindowStyleMaskTitled
        backing:NSBackingStoreBuffered
        defer:NO
    ]},
    audioDevice_label{allocLabel(
        @"audio output:",
        NSMakeRect(15, 15, labelWidth, labelHeight)
    )},
    deviceMenu{[[NSPopUpButton alloc]
        initWithFrame:NSMakeRect(160, 15, 140, 30)
        pullsDown:NO
    ]},
    actions{[ViewActions alloc]}
{
    app.mainMenu = [[NSMenu alloc] init];
    
    auto appMenu = [[NSMenuItem alloc] init];
    auto appSubMenu = [[NSMenu alloc] init];
    [appSubMenu addItemWithTitle:
        @"Quit"
        action:@selector(stop:)
        keyEquivalent:@"q"
    ];
    [appMenu setSubmenu:appSubMenu];
    [app.mainMenu addItem:appMenu];
    
    auto fileMenu = [[NSMenuItem alloc] init];
    auto fileSubMenu = [[NSMenu alloc] initWithTitle:@"File"];
    auto newTestItem = [[NSMenuItem alloc]
        initWithTitle:@"New Test..."
        action:@selector(newTest)
        keyEquivalent:@"n"
    ];
    newTestItem.target = actions;
    [fileSubMenu addItem:newTestItem];
    auto openTestItem = [[NSMenuItem alloc]
        initWithTitle:@"Open Test..."
        action:@selector(openTest)
        keyEquivalent:@"o"
    ];
    openTestItem.target = actions;
    [fileSubMenu addItem:openTestItem];
    [fileMenu setSubmenu:fileSubMenu];
    [app.mainMenu addItem:fileMenu];
    
    [window.contentView addSubview:audioDevice_label];
    [window.contentView addSubview:deviceMenu];
    [window makeKeyAndOrderFront:nil];
    actions.controller = this;
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
    [alert setInformativeText:asNsString(std::move(s))];
    [alert addButtonWithTitle:@"Ok"];
    [alert runModal];
}

std::string CocoaView::browseForDirectory() {
    auto panel = [NSOpenPanel openPanel];
    panel.canChooseDirectories = true;
    panel.canChooseFiles = false;
    return browseModal(panel);
}

bool CocoaView::browseCancelled() { 
    return browseCancelled_;
}

std::string CocoaView::browseForOpeningFile() {
    auto panel = [NSOpenPanel openPanel];
    panel.canChooseDirectories = false;
    panel.canChooseFiles = true;
    return browseModal(panel);
}

std::string CocoaView::browseModal(NSOpenPanel *panel) {
    switch([panel runModal]) {
        case NSModalResponseOK:
            browseCancelled_ = false;
            return panel.URLs.lastObject.path.UTF8String;
        default:
            browseCancelled_ = true;
            return {};
    }
}

std::string CocoaView::audioDevice() {
    return deviceMenu.titleOfSelectedItem.UTF8String;
}

void CocoaView::populateAudioDeviceMenu(std::vector<std::string> items) {
    for (auto item : items)
        [deviceMenu addItemWithTitle:asNsString(item)];
}

void CocoaView::addSubview(NSView *view) {
    [window.contentView addSubview:view];
}

@implementation ViewActions
@synthesize controller;
- (void)newTest {
    controller->newTest();
}

- (void)openTest {
    controller->openTest();
}
@end

