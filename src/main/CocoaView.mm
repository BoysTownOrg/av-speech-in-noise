#include "CocoaView.h"
#include "common-objc.h"

CocoaTesterView::CocoaTesterView() :
    view_{[[NSView alloc]
        initWithFrame:NSMakeRect(15, 15, 900 - 15 * 2, 400 - 15 * 2)]
    },
    actions{[TesterViewActions alloc]}
{
    actions.controller = this;
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

void CocoaTesterView::playTrial() {
    parent_->playTrial();
}

@implementation TesterViewActions
@synthesize controller;

- (void)playTrial {
    controller->playTrial();
}
@end

CocoaTestSetupView::CocoaTestSetupView() :
    view_{[[NSView alloc]
        initWithFrame:NSMakeRect(15, 15, 900 - 15 * 2, 400 - 15 * 2)
    ]},
    subjectIdLabel{allocLabel(
        @"subject id:",
        NSMakeRect(0, 270, 140, 25)
    )},
    subjectId_{[[NSTextField alloc]
        initWithFrame:NSMakeRect(145, 270, 150, 25)
    ]},
    testerIdLabel{allocLabel(
        @"tester id:",
        NSMakeRect(0, 240, 140, 25)
    )},
    testerId_{[[NSTextField alloc]
        initWithFrame:NSMakeRect(145, 240, 150, 25)
    ]},
    sessionLabel{allocLabel(
        @"session:",
        NSMakeRect(0, 210, 140, 25)
    )},
    session_{[[NSTextField alloc]
        initWithFrame:NSMakeRect(145, 210, 150, 25)
    ]},
    signalLevel_dB_SPL_label{allocLabel(
        @"signal level (dB SPL):",
        NSMakeRect(0, 180, 140, 25)
    )},
    signalLevel_dB_SPL_{[[NSTextField alloc]
        initWithFrame:NSMakeRect(145, 180, 150, 25)
    ]},
    startingSnr_dB_label{allocLabel(
        @"starting SNR (dB):",
        NSMakeRect(0, 150, 140, 25)
    )},
    startingSnr_dB_{[[NSTextField alloc]
        initWithFrame:NSMakeRect(145, 150, 150, 25)
    ]},
    stimulusListDirectoryLabel{allocLabel(
        @"stimulus directory:",
        NSMakeRect(0, 120, 140, 25)
    )},
    stimulusListDirectory_{[[NSTextField alloc]
        initWithFrame:NSMakeRect(145, 120, 500, 25)
    ]},
    maskerFilePath_label{allocLabel(
        @"masker file path:",
        NSMakeRect(0, 90, 140, 25)
    )},
    maskerFilePath_{[[NSTextField alloc]
        initWithFrame:NSMakeRect(145, 90, 500, 25)
    ]},
    calibrationFilePath_label{allocLabel(
        @"calibration file path:",
        NSMakeRect(500, 90, 140, 25)
    )},
    calibrationFilePath_{[[NSTextField alloc]
        initWithFrame:NSMakeRect(645, 90, 500, 25)
    ]},
    conditionMenu{[[NSPopUpButton alloc]
        initWithFrame:NSMakeRect(145, 60, 150, 25)
        pullsDown:NO
    ]},
    actions{[SetupViewActions alloc]}
{
    actions.controller = this;
    const auto browseForStimulusListButton = [NSButton
        buttonWithTitle:@"browse"
        target:actions
        action:@selector(browseForStimulusList)
    ];
    [browseForStimulusListButton setFrame:NSMakeRect(500 + 145 + 10, 120, 100, 25)];
    const auto browseForMaskerButton = [NSButton
        buttonWithTitle:@"browse"
        target:actions
        action:@selector(browseForMasker)
    ];
    [browseForMaskerButton setFrame:NSMakeRect(500 + 145 + 10, 90, 100, 25)];
    const auto confirmButton = [NSButton
        buttonWithTitle:@"Confirm"
        target:actions
        action:@selector(confirmTestSetup)
    ];
    [confirmButton setFrame:NSMakeRect(900 - 100 - 2*15, 15, 100, 25)];
    [view_ addSubview:browseForMaskerButton];
    [view_ addSubview:browseForStimulusListButton];
    [view_ addSubview:confirmButton];
    [view_ addSubview:subjectIdLabel];
    [view_ addSubview:subjectId_];
    [view_ addSubview:testerIdLabel];
    [view_ addSubview:testerId_];
    [view_ addSubview:sessionLabel];
    [view_ addSubview:session_];
    [view_ addSubview:signalLevel_dB_SPL_label];
    [view_ addSubview:signalLevel_dB_SPL_];
    [view_ addSubview:startingSnr_dB_label];
    [view_ addSubview:startingSnr_dB_];
    [view_ addSubview:stimulusListDirectoryLabel];
    [view_ addSubview:stimulusListDirectory_];
    [view_ addSubview:maskerFilePath_label];
    [view_ addSubview:maskerFilePath_];
    [view_ addSubview:calibrationFilePath_label];
    [view_ addSubview:calibrationFilePath_];
    [view_ addSubview:conditionMenu];
    stimulusListDirectory_.stringValue =
        @"/Users/basset/Documents/maxdetection/Stimuli/Video/List_Detection";
    maskerFilePath_.stringValue =
        @"/Users/basset/Documents/maxdetection/Stimuli/Masker/L1L2_EngEng.wav";
    signalLevel_dB_SPL_.stringValue = @"65";
    startingSnr_dB_.stringValue = @"0";
    [view_ setHidden:NO];
}

NSTextField *CocoaTestSetupView::allocLabel(NSString *label, NSRect frame) {
    const auto text = [[NSTextField alloc] initWithFrame:frame];
    [text setStringValue:label];
    [text setBezeled:NO];
    [text setDrawsBackground:NO];
    [text setEditable:NO];
    [text setSelectable:NO];
    [text setAlignment:NSTextAlignmentRight];
    return text;
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

std::string CocoaTestSetupView::signalLevel_dB_SPL() {
    return signalLevel_dB_SPL_.stringValue.UTF8String;
}

std::string CocoaTestSetupView::maskerFilePath() {
    return maskerFilePath_.stringValue.UTF8String;
}

std::string CocoaTestSetupView::calibrationFilePath() {
    return calibrationFilePath_.stringValue.UTF8String;
}

std::string CocoaTestSetupView::targetListDirectory() {
    return stimulusListDirectory_.stringValue.UTF8String;
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

@implementation SetupViewActions
@synthesize controller;

- (void)confirmTestSetup {
    controller->confirm();
}

- (void)browseForStimulusList { 
    controller->browseForStimulusList();
}

- (void)browseForMasker { 
    controller->browseForMasker();
}
@end


void CocoaSubjectView::addButtonRow(NSColor *color, int row) {
    for (int i = 0; i < 8; ++i) {
        auto title = asNsString(std::to_string(i+1));
        const auto button = [NSButton
            buttonWithTitle:title
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
}

static auto greenColor = NSColor.greenColor;
static auto redColor = NSColor.redColor;
static auto blueColor = NSColor.blueColor;
static auto grayColor = NSColor.lightGrayColor;

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
    addButtonRow(grayColor, 2);
    addButtonRow(redColor, 3);
    const auto button = [NSButton
        buttonWithTitle:@"Press when ready"
        target:actions
        action:@selector(playTrial)
    ];
    [button setFrame:NSMakeRect(80*1, 80*1, 80, 80)];
    [nextTrialButton addSubview:button];
    [window.contentView addSubview:nextTrialButton];
    [window.contentView addSubview:responseButtons];
    hideResponseButtons();
    hideNextTrialButton();
    [window makeKeyAndOrderFront:nil];
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

bool CocoaSubjectView::grayResponse() {
    return lastPressedColor() == grayColor;
}

void CocoaSubjectView::respond(id sender) {
    lastButtonPressed = sender;
    parent_->submitResponse();
}

void CocoaSubjectView::showResponseButtons() {
    [responseButtons setHidden:NO];
}

void CocoaTestSetupView::browseForMasker() {
    listener_->browseForMasker();
}

void CocoaSubjectView::hideResponseButtons() {
    [responseButtons setHidden:YES];
}

void CocoaSubjectView::showNextTrialButton() {
    [nextTrialButton setHidden:NO];
}

void CocoaSubjectView::hideNextTrialButton() {
    [nextTrialButton setHidden:YES];
}

void CocoaSubjectView::playTrial() {
    parent_->playTrial();
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

CocoaView::CocoaView() :
    app{[NSApplication sharedApplication]},
    window{[[NSWindow alloc]
        initWithContentRect:NSMakeRect(15, 15, 900, 400)
        styleMask:
            NSWindowStyleMaskClosable |
            NSWindowStyleMaskResizable |
            NSWindowStyleMaskTitled
        backing:NSBackingStoreBuffered
        defer:NO
    ]},
    deviceMenu{[[NSPopUpButton alloc]
        initWithFrame:NSMakeRect(0, 0, 140, 30)
        pullsDown:NO
    ]},
    actions{[ViewActions alloc]}
{
    testerView_.becomeChild(this);
    subjectView_.becomeChild(this);
    
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
    
    [window.contentView addSubview:testerView_.view()];
    [window.contentView addSubview:deviceMenu];
    [window makeKeyAndOrderFront:nil];
    actions.controller = this;
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

auto CocoaView::tester() -> Tester * {
    return &testerView_;
}

auto CocoaView::subject() -> Subject * {
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
    [alert setInformativeText:asNsString(std::move(s))];
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
@end

void CocoaTesterView::becomeChild(CocoaView *p) {
    parent_ = p;
}

void CocoaView::submitResponse() { 
    listener->submitResponse();
}

void CocoaSubjectView::becomeChild(CocoaView *p) {
    parent_ = p;
}

void CocoaTestSetupView::setStimulusList(std::string s) {
    [stimulusListDirectory_ setStringValue:asNsString(std::move(s))];
}

void CocoaTestSetupView::setMasker(std::string s) {
    [maskerFilePath_ setStringValue:asNsString(std::move(s))];
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

void CocoaTestSetupView::browseForStimulusList() { 
    listener_->browseForStimulusList();
}

void CocoaView::browseForStimulusList() { 
    listener->browseForTargetList();
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
            break;
        default:
            browseCancelled_ = true;
    }
    auto url = panel.URLs.lastObject;
    return url.path.UTF8String;
}

void CocoaView::browseForMasker() {
    listener->browseForMasker();
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
