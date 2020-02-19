#include "CocoaView.h"
#include "common-objc.h"
#include <iterator>

@interface SetupViewActions : NSObject
@property av_speech_in_noise::CocoaTestSetupView *controller;
- (void)confirmTestSetup;
- (void)browseForTargetList;
- (void)browseForMasker;
- (void)browseForCalibration;
- (void)browseForTrackSettings;
- (void)playCalibration;
@end

@interface SubjectViewActions : NSObject
@property av_speech_in_noise::CocoaSubjectView *controller;
- (void)respond:(id)sender;
- (void)playTrial;
@end

@interface ExperimenterViewActions : NSObject
@property av_speech_in_noise::CocoaExperimenterView *controller;
- (void)exitTest;
@end

@interface TestingViewActions : NSObject
@property av_speech_in_noise::CocoaExperimenterView *controller;
- (void)playTrial;
- (void)submitFreeResponse;
- (void)submitPassedTrial;
- (void)submitFailedTrial;
- (void)submitCorrectKeywords;
@end

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

- (void)browseForTrackSettings {
    controller->browseForTrackSettings();
}

- (void)playCalibration {
    controller->playCalibration();
}
@end

@implementation SubjectViewActions
@synthesize controller;

- (void)respond:(id)sender {
    controller->respond(sender);
}

- (void)playTrial {
    controller->playTrial();
}
@end

@implementation ExperimenterViewActions
@synthesize controller;

- (void)exitTest {
    controller->exitTest();
}

- (void)playTrial {
    controller->playTrial();
}

- (void)submitFreeResponse {
    controller->submitFreeResponse();
}

- (void)submitPassedTrial {
    controller->submitPassedTrial();
}

- (void)submitFailedTrial {
    controller->submitFailedTrial();
}

- (void)submitCorrectKeywords {
    controller->submitCorrectKeywords();
}
@end

namespace av_speech_in_noise {
static auto textFieldWithFrame(NSRect r) -> NSTextField * {
    return [[NSTextField alloc] initWithFrame:r];
}

static auto allocLabel(NSString *label, NSRect frame) -> NSTextField * {
    const auto text{textFieldWithFrame(frame)};
    [text setStringValue:label];
    [text setBezeled:NO];
    [text setDrawsBackground:NO];
    [text setEditable:NO];
    [text setSelectable:NO];
    [text setAlignment:NSTextAlignmentRight];
    [text setTextColor:NSColor.labelColor];
    return text;
}

static constexpr auto labelHeight{22};
static constexpr auto labelWidth{180};
static constexpr auto labelToTextFieldSpacing{5};
static constexpr auto textFieldLeadingEdge{
    labelWidth + labelToTextFieldSpacing};
static constexpr auto shortTextFieldWidth{75};
static constexpr auto normalTextFieldWidth{150};
static constexpr auto menuWidth{180};
static constexpr auto filePathTextFieldWidth{500};
static constexpr auto buttonHeight{25};
static constexpr auto buttonWidth{100};

static auto textFieldSizeAtHeightWithWidth(CGFloat height, CGFloat buttonWidth)
    -> NSRect {
    return NSMakeRect(textFieldLeadingEdge, height, buttonWidth, labelHeight);
}

static auto shortTextFieldSizeAtHeight(CGFloat y) -> NSRect {
    return textFieldSizeAtHeightWithWidth(y, shortTextFieldWidth);
}

static auto normalTextFieldSizeAtHeight(CGFloat y) -> NSRect {
    return textFieldSizeAtHeightWithWidth(y, normalTextFieldWidth);
}

static auto filePathTextFieldSizeAtHeight(CGFloat y) -> NSRect {
    return textFieldSizeAtHeightWithWidth(y, filePathTextFieldWidth);
}

static auto normalTextFieldWithHeight(CGFloat x) -> NSTextField * {
    return textFieldWithFrame(normalTextFieldSizeAtHeight(x));
}

static auto normalLabelWithHeight(CGFloat x, std::string s) -> NSTextField * {
    return allocLabel(
        asNsString(std::move(s)), NSMakeRect(0, x, labelWidth, labelHeight));
}

static auto filePathTextFieldSizeWithHeight(CGFloat x) -> NSTextField * {
    return textFieldWithFrame(filePathTextFieldSizeAtHeight(x));
}

static auto shortTextFieldWithHeight(CGFloat x) -> NSTextField * {
    return textFieldWithFrame(shortTextFieldSizeAtHeight(x));
}

static auto popUpButtonAtHeightWithWidth(CGFloat height, CGFloat width)
    -> NSPopUpButton * {
    return [[NSPopUpButton alloc] initWithFrame:NSMakeRect(textFieldLeadingEdge,
                                                    height, width, labelHeight)
                                      pullsDown:NO];
}

static auto button(std::string s, id target, SEL action) -> NSButton * {
    return [NSButton buttonWithTitle:asNsString(std::move(s))
                              target:target
                              action:action];
}

static auto button(std::string s, id target, SEL action, NSRect frame)
    -> NSButton * {
    auto button_{[NSButton buttonWithTitle:asNsString(std::move(s))
                                    target:target
                                    action:action]};
    [button_ setFrame:frame];
    return button_;
}

CocoaTestSetupView::CocoaTestSetupView(NSRect r)
    : view_{[[NSView alloc] initWithFrame:r]},
      subjectIdLabel{normalLabelWithHeight(360, "subject:")},
      subjectId_{normalTextFieldWithHeight(360)},
      testerIdLabel{normalLabelWithHeight(330, "tester:")},
      testerId_{normalTextFieldWithHeight(330)},
      sessionLabel{normalLabelWithHeight(300, "session:")},
      session_{normalTextFieldWithHeight(300)},
      targetListDirectoryLabel{normalLabelWithHeight(270, "targets:")},
      targetListDirectory_{filePathTextFieldSizeWithHeight(270)},
      maskerFilePath_label{normalLabelWithHeight(240, "masker:")},
      maskerFilePath_{filePathTextFieldSizeWithHeight(240)},
      trackSettingsFile_label{normalLabelWithHeight(210, "track settings:")},
      trackSettingsFile_{filePathTextFieldSizeWithHeight(210)},
      calibrationFilePath_label{normalLabelWithHeight(180, "calibration:")},
      calibrationFilePath_{filePathTextFieldSizeWithHeight(180)},
      calibrationLevel_dB_SPL_label{
          normalLabelWithHeight(150, "calibration level (dB SPL):")},
      calibrationLevel_dB_SPL_{shortTextFieldWithHeight(150)},
      maskerLevel_dB_SPL_label{
          normalLabelWithHeight(120, "masker level (dB SPL):")},
      maskerLevel_dB_SPL_{shortTextFieldWithHeight(120)},
      startingSnr_dB_label{normalLabelWithHeight(90, "starting SNR (dB):")},
      startingSnr_dB_{shortTextFieldWithHeight(90)},
      condition_label{normalLabelWithHeight(60, "condition:")},
      conditionMenu{popUpButtonAtHeightWithWidth(60, 120)},
      method_label{normalLabelWithHeight(30, "method:")},
      methodMenu{popUpButtonAtHeightWithWidth(30, 270)},
      actions{[SetupViewActions alloc]} {
    actions.controller = this;
    const auto browseForTargetListButton {
        button("browse", actions, @selector(browseForTargetList),
            NSMakeRect(filePathTextFieldWidth + textFieldLeadingEdge + 10, 270,
                buttonWidth, buttonHeight))
    };
    const auto browseForMaskerButton {
        button("browse", actions, @selector(browseForMasker),
            NSMakeRect(filePathTextFieldWidth + textFieldLeadingEdge + 10, 240,
                buttonWidth, buttonHeight))
    };
    const auto browseForTrackSettingsButton {
        button("browse", actions, @selector(browseForTrackSettings),
            NSMakeRect(filePathTextFieldWidth + textFieldLeadingEdge + 10, 210,
                buttonWidth, buttonHeight))
    };
    const auto browseForCalibrationButton {
        button("browse", actions, @selector(browseForCalibration),
            NSMakeRect(filePathTextFieldWidth + textFieldLeadingEdge + 10, 180,
                buttonWidth, buttonHeight))
    };
    const auto confirmButton {
        button("Confirm", actions, @selector(confirmTestSetup),
            NSMakeRect(
                r.size.width - buttonWidth, 0, buttonWidth, buttonHeight))
    };
    const auto playCalibrationButton {
        button("play", actions, @selector(playCalibration),
            NSMakeRect(shortTextFieldWidth + textFieldLeadingEdge + 10, 150,
                buttonWidth, buttonHeight))
    };
    addSubview(browseForMaskerButton);
    addSubview(browseForTargetListButton);
    addSubview(browseForCalibrationButton);
    addSubview(browseForTrackSettingsButton);
    addSubview(confirmButton);
    addSubview(playCalibrationButton);
    addSubview(subjectIdLabel);
    addSubview(subjectId_);
    addSubview(testerIdLabel);
    addSubview(testerId_);
    addSubview(sessionLabel);
    addSubview(session_);
    addSubview(maskerLevel_dB_SPL_label);
    addSubview(maskerLevel_dB_SPL_);
    addSubview(calibrationLevel_dB_SPL_label);
    addSubview(calibrationLevel_dB_SPL_);
    addSubview(startingSnr_dB_label);
    addSubview(startingSnr_dB_);
    addSubview(targetListDirectoryLabel);
    addSubview(targetListDirectory_);
    addSubview(maskerFilePath_label);
    addSubview(maskerFilePath_);
    addSubview(trackSettingsFile_label);
    addSubview(trackSettingsFile_);
    addSubview(calibrationFilePath_label);
    addSubview(calibrationFilePath_);
    addSubview(condition_label);
    addSubview(conditionMenu);
    addSubview(method_label);
    addSubview(methodMenu);
    [view_ setHidden:NO];
}

void CocoaTestSetupView::addSubview(NSView *subview) {
    [view_ addSubview:subview];
}

auto CocoaTestSetupView::view() -> NSView * { return view_; }

void CocoaTestSetupView::show() { [view_ setHidden:NO]; }

void CocoaTestSetupView::hide() { [view_ setHidden:YES]; }

auto CocoaTestSetupView::startingSnr_dB() -> std::string {
    return stringValue(startingSnr_dB_);
}

auto CocoaTestSetupView::stringValue(NSTextField *field) -> const char * {
    return field.stringValue.UTF8String;
}

auto CocoaTestSetupView::maskerLevel_dB_SPL() -> std::string {
    return stringValue(maskerLevel_dB_SPL_);
}

auto CocoaTestSetupView::calibrationLevel_dB_SPL() -> std::string {
    return stringValue(calibrationLevel_dB_SPL_);
}

auto CocoaTestSetupView::maskerFilePath() -> std::string {
    return stringValue(maskerFilePath_);
}

auto CocoaTestSetupView::calibrationFilePath() -> std::string {
    return stringValue(calibrationFilePath_);
}

auto CocoaTestSetupView::targetListDirectory() -> std::string {
    return stringValue(targetListDirectory_);
}

auto CocoaTestSetupView::testerId() -> std::string {
    return stringValue(testerId_);
}

auto CocoaTestSetupView::subjectId() -> std::string {
    return stringValue(subjectId_);
}

auto CocoaTestSetupView::session() -> std::string {
    return stringValue(session_);
}

auto CocoaTestSetupView::method() -> std::string {
    return methodMenu.titleOfSelectedItem.UTF8String;
}

auto CocoaTestSetupView::condition() -> std::string {
    return conditionMenu.titleOfSelectedItem.UTF8String;
}

auto CocoaTestSetupView::trackSettingsFile() -> std::string {
    return stringValue(trackSettingsFile_);
}

void CocoaTestSetupView::setTrackSettingsFile(std::string s) {
    [trackSettingsFile_ setStringValue:asNsString(std::move(s))];
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

void CocoaTestSetupView::setCalibration(std::string s) {
    [calibrationFilePath_ setStringValue:asNsString(std::move(s))];
}

void CocoaTestSetupView::setMaskerLevel_dB_SPL(std::string s) {
    [maskerLevel_dB_SPL_ setStringValue:asNsString(std::move(s))];
}

void CocoaTestSetupView::setCalibrationLevel_dB_SPL(std::string s) {
    [calibrationLevel_dB_SPL_ setStringValue:asNsString(std::move(s))];
}

void CocoaTestSetupView::setStartingSnr_dB(std::string s) {
    [startingSnr_dB_ setStringValue:asNsString(std::move(s))];
}

void CocoaTestSetupView::populateConditionMenu(std::vector<std::string> items) {
    for (const auto &item : items)
        [conditionMenu addItemWithTitle:asNsString(item)];
}

void CocoaTestSetupView::populateMethodMenu(std::vector<std::string> items) {
    for (const auto &item : items)
        [methodMenu addItemWithTitle:asNsString(item)];
}

void CocoaTestSetupView::confirm() { listener_->confirmTestSetup(); }

void CocoaTestSetupView::subscribe(EventListener *listener) {
    listener_ = listener;
}

void CocoaTestSetupView::browseForMasker() { listener_->browseForMasker(); }

void CocoaTestSetupView::browseForTargetList() {
    listener_->browseForTargetList();
}

void CocoaTestSetupView::browseForCalibration() {
    listener_->browseForCalibration();
}

void CocoaTestSetupView::browseForTrackSettings() {
    listener_->browseForTrackSettingsFile();
}

void CocoaTestSetupView::playCalibration() { listener_->playCalibration(); }

static auto greenColor{NSColor.greenColor};
static auto redColor{NSColor.redColor};
static auto blueColor{NSColor.blueColor};
static auto whiteColor{NSColor.whiteColor};
static constexpr int numbers[] = {1, 2, 3, 4, 5, 6, 8, 9};
static constexpr auto responseNumbers{std::size(numbers)};
static constexpr auto responseColors{4};

CocoaSubjectView::CocoaSubjectView(NSRect r)
    : // Defer may be critical here...
      window{[[NSWindow alloc] initWithContentRect:r
                                         styleMask:NSWindowStyleMaskBorderless
                                           backing:NSBackingStoreBuffered
                                             defer:YES]},
      responseButtons{[[NSView alloc]
          initWithFrame:NSMakeRect(0, 0, r.size.width, r.size.height)]},
      nextTrialButton{[[NSView alloc]
          initWithFrame:NSMakeRect(0, 0, r.size.width, r.size.height)]},
      actions{[SubjectViewActions alloc]} {
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
}

void CocoaSubjectView::addButtonRow(NSColor *color, int row) {
    for (std::size_t col{0}; col < responseNumbers; ++col)
        addNumberButton(color, numbers[col], row, col);
}

void CocoaSubjectView::addNumberButton(
    NSColor *color, int number, int row, std::size_t col) {
    auto title{asNsString(std::to_string(number))};
    const auto button {
        [NSButton buttonWithTitle:title
                           target:actions
                           action:@selector(respond:)]
    };
    auto responseWidth{responseButtons.frame.size.width / responseNumbers};
    auto responseHeight{responseButtons.frame.size.height / responseColors};
    [button setFrame:NSMakeRect(responseWidth * col, responseHeight * row,
                         responseWidth, responseHeight)];
    [button setBezelStyle:NSBezelStyleTexturedSquare];
    auto style{[[NSMutableParagraphStyle alloc] init]};
    [style setAlignment:NSTextAlignmentCenter];
    auto attrsDictionary{[NSDictionary
        dictionaryWithObjectsAndKeys:color, NSForegroundColorAttributeName,
        [NSNumber numberWithFloat:-4.0], NSStrokeWidthAttributeName,
        NSColor.blackColor, NSStrokeColorAttributeName, style,
        NSParagraphStyleAttributeName,
        [NSFont fontWithName:@"Arial-Black" size:48], NSFontAttributeName,
        nil]};
    auto attrString{
        [[NSAttributedString alloc] initWithString:title
                                        attributes:attrsDictionary]};
    [button setAttributedTitle:attrString];
    [responseButtons addSubview:button];
}

void CocoaSubjectView::addNextTrialButton() {
    const auto button_ { button("", actions, @selector(playTrial)) };
    [button_ setBezelStyle:NSBezelStyleTexturedSquare];
    auto style{[[NSMutableParagraphStyle alloc] init]};
    [style setAlignment:NSTextAlignmentCenter];
    auto font{[NSFont fontWithName:@"Courier" size:36]};
    auto attrsDictionary{[NSDictionary
        dictionaryWithObjectsAndKeys:font, NSFontAttributeName, nil]};
    auto attrString{
        [[NSAttributedString alloc] initWithString:@"Press when ready"
                                        attributes:attrsDictionary]};
    [button_ setAttributedTitle:attrString];
    [button_ setFrame:NSMakeRect(0, 0, nextTrialButton.frame.size.width,
                          nextTrialButton.frame.size.height)];
    [nextTrialButton addSubview:button_];
}

auto CocoaSubjectView::numberResponse() -> std::string {
    return lastButtonPressed.title.UTF8String;
}

auto CocoaSubjectView::greenResponse() -> bool {
    return lastPressedColor() == greenColor;
}

auto CocoaSubjectView::lastPressedColor() -> NSColor * {
    return [lastButtonPressed.attributedTitle
             attribute:NSForegroundColorAttributeName
               atIndex:0
        effectiveRange:nil];
}

auto CocoaSubjectView::blueResponse() -> bool {
    return lastPressedColor() == blueColor;
}

auto CocoaSubjectView::whiteResponse() -> bool {
    return lastPressedColor() == whiteColor;
}

void CocoaSubjectView::respond(id sender) {
    lastButtonPressed = sender;
    listener_->submitResponse();
}

void CocoaSubjectView::showResponseButtons() { [responseButtons setHidden:NO]; }

void CocoaSubjectView::showNextTrialButton() { [nextTrialButton setHidden:NO]; }

void CocoaSubjectView::hideNextTrialButton() {
    [nextTrialButton setHidden:YES];
}

void CocoaSubjectView::playTrial() { listener_->playTrial(); }

void CocoaSubjectView::hideResponseButtons() {
    [responseButtons setHidden:YES];
}

void CocoaSubjectView::subscribe(EventListener *e) { listener_ = e; }

void CocoaSubjectView::show() { [window makeKeyAndOrderFront:nil]; }

void CocoaSubjectView::hide() { [window orderOut:nil]; }

void CocoaExperimenterView::subscribe(EventListener *e) { listener_ = e; }

void CocoaExperimenterView::showExitTestButton() {
    [exitTestButton_ setHidden:NO];
}

void CocoaExperimenterView::hideExitTestButton() {
    [exitTestButton_ setHidden:YES];
}

void CocoaExperimenterView::show() { [view_ setHidden:NO]; }

void CocoaExperimenterView::hide() { [view_ setHidden:YES]; }

auto CocoaExperimenterView::view() -> NSView * { return view_; }

void CocoaExperimenterView::exitTest() { listener_->exitTest(); }

void CocoaExperimenterView::display(std::string s) {
    [displayedText_ setStringValue:asNsString(std::move(s))];
}

void CocoaExperimenterView::secondaryDisplay(std::string s) {
    [secondaryDisplayedText_ setStringValue:asNsString(std::move(s))];
}

CocoaExperimenterView::CocoaExperimenterView(NSRect r)
    : view_{[[NSView alloc] initWithFrame:r]},
      displayedText_{
          [[NSTextField alloc] initWithFrame:NSMakeRect(buttonWidth + 15, r.size.height - labelHeight,
                                                 labelWidth, labelHeight)]},
      secondaryDisplayedText_{[[NSTextField alloc]
          initWithFrame:NSMakeRect(buttonWidth + 15 + labelWidth + 15, r.size.height - labelHeight,
                            r.size.width - buttonWidth - labelWidth - 30, labelHeight)]},
      evaluationButtons{
          [[NSView alloc] initWithFrame:NSMakeRect(r.size.width - 3 * buttonWidth, 0, 3 * buttonWidth,
                                            buttonHeight)]},
      responseSubmission{
          [[NSView alloc] initWithFrame:NSMakeRect(r.size.width - 250, 0, 250,
                                            buttonHeight + 15 + 2*labelHeight + 15)]},
      correctKeywordsSubmission{
          [[NSView alloc] initWithFrame:NSMakeRect(r.size.width - normalTextFieldWidth, 0, normalTextFieldWidth,
                                            buttonHeight + 15 + labelHeight)]},
      response_{[[NSTextField alloc]
          initWithFrame:NSMakeRect(0,
                            buttonHeight + 15 + labelHeight + 15, 250,
                            labelHeight)]},
      correctKeywordsEntry_{[[NSTextField alloc]
          initWithFrame:NSMakeRect(0,
                            buttonHeight + 15, normalTextFieldWidth,
                            labelHeight)]},
      flagged_{[[NSButton alloc]
          initWithFrame:NSMakeRect(0, buttonHeight + 15,
                            normalTextFieldWidth, labelHeight)]},
      actions{[ExperimenterViewActions alloc]} {
    exitTestButton_ = button("exit test", actions, @selector(exitTest));
    [exitTestButton_ setFrame:NSMakeRect(0, r.size.height - buttonHeight, buttonWidth, buttonHeight)];
    [displayedText_ setBezeled:NO];
    [displayedText_ setDrawsBackground:NO];
    [displayedText_ setEditable:NO];
    [secondaryDisplayedText_ setBezeled:NO];
    [secondaryDisplayedText_ setDrawsBackground:NO];
    [secondaryDisplayedText_ setEditable:NO];
    [view_ addSubview:exitTestButton_];
    [view_ addSubview:displayedText_];
    [view_ addSubview:secondaryDisplayedText_];
    [view_ setHidden:YES];
    [flagged_ setButtonType:NSButtonTypeSwitch];
    [flagged_ setTitle:@"flagged"];
    nextTrialButton_ = button("play trial", actions, @selector(playTrial));
    [nextTrialButton_ setFrame:NSMakeRect(r.size.width - buttonWidth, 0,
                                   buttonWidth, buttonHeight)];
    const auto submitFreeResponse_ {
        button("submit", actions, @selector(submitFreeResponse))
    };
    [submitFreeResponse_ setFrame:NSMakeRect(responseSubmission.frame.size.width - buttonWidth, 0,
                                      buttonWidth, buttonHeight)];
    const auto passButton_ {
        button("pass", actions, @selector(submitPassedTrial))
    };
    [passButton_ setFrame:NSMakeRect(evaluationButtons.frame.size.width - 3 * buttonWidth, 0,
                              buttonWidth, buttonHeight)];
    const auto failButton_ {
        button("fail", actions, @selector(submitFailedTrial))
    };
    [failButton_ setFrame:NSMakeRect(evaluationButtons.frame.size.width - buttonWidth, 0, buttonWidth,
                              buttonHeight)];
    const auto submitCorrectKeywords_ {
        button("submit", actions, @selector(submitCorrectKeywords))
    };
    [submitCorrectKeywords_ setFrame:NSMakeRect(correctKeywordsSubmission.frame.size.width - buttonWidth, 0,
                                         buttonWidth, buttonHeight)];
    [responseSubmission addSubview:submitFreeResponse_];
    [responseSubmission addSubview:response_];
    [responseSubmission addSubview:flagged_];
    [evaluationButtons addSubview:passButton_];
    [evaluationButtons addSubview:failButton_];
    [correctKeywordsSubmission addSubview:correctKeywordsEntry_];
    [correctKeywordsSubmission addSubview:submitCorrectKeywords_];
    [view_ addSubview:nextTrialButton_];
    [view_ addSubview:responseSubmission];
    [view_ addSubview:evaluationButtons];
    [view_ addSubview:correctKeywordsSubmission];
    [evaluationButtons setHidden:YES];
    [nextTrialButton_ setHidden:YES];
    [responseSubmission setHidden:YES];
    [correctKeywordsSubmission setHidden:YES];
    [view_ setHidden:YES];
    actions.controller = this;
}

void CocoaExperimenterView::showNextTrialButton() { [nextTrialButton_ setHidden:NO]; }

void CocoaExperimenterView::hideNextTrialButton() {
    [nextTrialButton_ setHidden:YES];
}

void CocoaExperimenterView::showEvaluationButtons() {
    [evaluationButtons setHidden:NO];
}

void CocoaExperimenterView::showFreeResponseSubmission() {
    [responseSubmission setHidden:NO];
}

void CocoaExperimenterView::hideFreeResponseSubmission() {
    [responseSubmission setHidden:YES];
}

void CocoaExperimenterView::hideEvaluationButtons() {
    [evaluationButtons setHidden:YES];
}

void CocoaExperimenterView::showCorrectKeywordsSubmission() {
    [correctKeywordsSubmission setHidden:NO];
}

void CocoaExperimenterView::hideCorrectKeywordsSubmission() {
    [correctKeywordsSubmission setHidden:YES];
}

auto CocoaExperimenterView::freeResponse() -> std::string {
    return response_.stringValue.UTF8String;
}

auto CocoaExperimenterView::correctKeywords() -> std::string {
    return correctKeywordsEntry_.stringValue.UTF8String;
}

auto CocoaExperimenterView::flagged() -> bool {
    return flagged_.state == NSControlStateValueOn;
}

void CocoaExperimenterView::playTrial() { listener_->playTrial(); }

void CocoaExperimenterView::submitFreeResponse() { listener_->submitFreeResponse(); }

void CocoaExperimenterView::submitPassedTrial() { listener_->submitPassedTrial(); }

void CocoaExperimenterView::submitFailedTrial() { listener_->submitFailedTrial(); }

void CocoaExperimenterView::submitCorrectKeywords() {
    listener_->submitCorrectKeywords();
}

CocoaView::CocoaView(NSRect r)
    : app{[NSApplication sharedApplication]},
      window{[[NSWindow alloc]
          initWithContentRect:r
                    styleMask:NSWindowStyleMaskClosable |
                    NSWindowStyleMaskResizable | NSWindowStyleMaskTitled
                      backing:NSBackingStoreBuffered
                        defer:NO]},
      audioDevice_label{
          allocLabel(@"audio output:", NSMakeRect(15, 15, 140, labelHeight))},
      deviceMenu{[[NSPopUpButton alloc]
          initWithFrame:NSMakeRect(15 + 140 + labelToTextFieldSpacing, 15,
                            menuWidth, labelHeight)
              pullsDown:NO]} {
    app.mainMenu = [[NSMenu alloc] init];

    auto appMenu{[[NSMenuItem alloc] init]};
    auto appSubMenu{[[NSMenu alloc] init]};
    [appSubMenu addItemWithTitle:@"Quit"
                          action:@selector(stop:)
                   keyEquivalent:@"q"];
    [appMenu setSubmenu:appSubMenu];
    [app.mainMenu addItem:appMenu];

    [window.contentView addSubview:audioDevice_label];
    [window.contentView addSubview:deviceMenu];
    [window makeKeyAndOrderFront:nil];
}

void CocoaView::eventLoop() { [app run]; }

void CocoaView::showErrorMessage(std::string s) {
    auto alert{[[NSAlert alloc] init]};
    [alert setMessageText:@"Error."];
    [alert setInformativeText:asNsString(std::move(s))];
    [alert addButtonWithTitle:@"Ok"];
    [alert runModal];
}

auto CocoaView::browseForDirectory() -> std::string {
    auto panel{[NSOpenPanel openPanel]};
    panel.canChooseDirectories = YES;
    panel.canChooseFiles = NO;
    return browseModal(panel);
}

auto CocoaView::browseCancelled() -> bool { return browseCancelled_; }

auto CocoaView::browseForOpeningFile() -> std::string {
    auto panel{[NSOpenPanel openPanel]};
    panel.canChooseDirectories = NO;
    panel.canChooseFiles = YES;
    return browseModal(panel);
}

auto CocoaView::browseModal(NSOpenPanel *panel) -> std::string {
    switch ([panel runModal]) {
    case NSModalResponseOK:
        browseCancelled_ = false;
        return panel.URLs.lastObject.path.UTF8String;
    default:
        browseCancelled_ = true;
        return {};
    }
}

auto CocoaView::audioDevice() -> std::string {
    return deviceMenu.titleOfSelectedItem.UTF8String;
}

void CocoaView::populateAudioDeviceMenu(std::vector<std::string> items) {
    for (const auto &item : items)
        [deviceMenu addItemWithTitle:asNsString(item)];
}

void CocoaView::addSubview(NSView *view) {
    [window.contentView addSubview:view];
}

void CocoaView::setDelegate(id<NSWindowDelegate> delegate) {
    [window setDelegate:delegate];
}

void CocoaView::center() { [window center]; }
}
