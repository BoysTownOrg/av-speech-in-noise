#include "CocoaView.h"
#include "common-objc.h"
#include <gsl/gsl>
#include <iterator>
#include <array>

@interface SetupViewActions : NSObject
@end

@interface CoordinateResponseMeasureViewActions : NSObject
@end

@interface ConsonantViewActions : NSObject
@end

@interface ExperimenterViewActions : NSObject
@end

@implementation SetupViewActions {
  @public
    av_speech_in_noise::CocoaTestSetupView *controller;
}

- (void)confirmTestSetup {
    controller->confirm();
}

- (void)browseForTestSettings {
    controller->browseForTestSettings();
}

- (void)playCalibration {
    controller->playCalibration();
}
@end

@implementation CoordinateResponseMeasureViewActions {
  @public
    av_speech_in_noise::CocoaCoordinateResponseMeasureView *controller;
}

- (void)notifyThatResponseButtonHasBeenClicked:(id)sender {
    controller->notifyThatResponseButtonHasBeenClicked(sender);
}

- (void)notifyThatReadyButtonHasBeenClicked {
    controller->notifyThatReadyButtonHasBeenClicked();
}
@end

@implementation ConsonantViewActions {
  @public
    av_speech_in_noise::CocoaConsonantView *controller;
}

- (void)notifyThatResponseButtonHasBeenClicked:(id)sender {
    controller->notifyThatResponseButtonHasBeenClicked(sender);
}

- (void)notifyThatReadyButtonHasBeenClicked {
    controller->notifyThatReadyButtonHasBeenClicked();
}
@end

@implementation ExperimenterViewActions {
  @public
    av_speech_in_noise::CocoaExperimenterView *controller;
}

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

- (void)acceptContinuingTesting {
    controller->acceptContinuingTesting();
}

- (void)declineContinuingTesting {
    controller->declineContinuingTesting();
}
@end

namespace av_speech_in_noise {
static auto textFieldWithFrame(NSRect r) -> NSTextField * {
    return [[NSTextField alloc] initWithFrame:r];
}

static void addSubview(NSView *parent, NSView *child) {
    [parent addSubview:child];
}

static void hide(NSView *v) { [v setHidden:YES]; }

static void show(NSView *v) { [v setHidden:NO]; }

static void setStaticLike(NSTextField *f) {
    [f setBezeled:NO];
    [f setDrawsBackground:NO];
    [f setEditable:NO];
}

static void set(NSTextField *field, const std::string &s) {
    [field setStringValue:asNsString(s)];
}

static auto allocLabel(const std::string &label, NSRect frame)
    -> NSTextField * {
    const auto text{textFieldWithFrame(frame)};
    set(text, label);
    setStaticLike(text);
    [text setSelectable:NO];
    [text setAlignment:NSTextAlignmentRight];
    [text setTextColor:NSColor.labelColor];
    return text;
}

static constexpr auto labelHeight{22};
static constexpr auto labelWidth{120};
static constexpr auto labelToTextFieldSpacing{5};
static constexpr auto textFieldLeadingEdge{
    labelWidth + labelToTextFieldSpacing};
static constexpr auto normalTextFieldWidth{150};
static constexpr auto menuWidth{180};
static constexpr auto buttonHeight{25};
static constexpr auto buttonWidth{100};
constexpr auto reasonableSpacing{15};

constexpr auto width(const NSRect &r) -> CGFloat { return r.size.width; }

constexpr auto height(const NSRect &r) -> CGFloat { return r.size.height; }

static auto normalLabelWithHeight(CGFloat x, const std::string &s)
    -> NSTextField * {
    return allocLabel(s, NSMakeRect(0, x, labelWidth, labelHeight));
}

static auto button(const std::string &s, id target, SEL action) -> NSButton * {
    return [NSButton buttonWithTitle:asNsString(s) target:target action:action];
}

static auto string(NSTextField *field) -> const char * {
    return field.stringValue.UTF8String;
}

static void enableAutoLayout(NSView *view) {
    view.translatesAutoresizingMaskIntoConstraints = NO;
}

static void addAutolayoutEnabledSubview(NSView *parent, NSView *child) {
    enableAutoLayout(child);
    [parent addSubview:child];
}

static auto constraint(NSLayoutYAxisAnchor *a, NSLayoutYAxisAnchor *b)
    -> NSLayoutConstraint * {
    return [a constraintEqualToAnchor:b];
}

static auto yCenterConstraint(NSView *a, NSView *b) -> NSLayoutConstraint * {
    return constraint(a.centerYAnchor, b.centerYAnchor);
}

static auto firstToTheRightOfSecondConstraint(
    NSView *first, NSView *second, CGFloat x) -> NSLayoutConstraint * {
    return [first.leadingAnchor constraintEqualToAnchor:second.trailingAnchor
                                               constant:x];
}

static void activateLabeledElementConstraintBelow(
    NSView *above, NSView *below, NSView *belowLabel) {
    [NSLayoutConstraint activateConstraints:@[
        [above.leadingAnchor constraintEqualToAnchor:below.leadingAnchor],
        [below.topAnchor constraintEqualToAnchor:above.bottomAnchor constant:8],
        firstToTheRightOfSecondConstraint(below, belowLabel, 8),
        yCenterConstraint(below, belowLabel),
        [below.widthAnchor constraintEqualToConstant:NSWidth(below.frame)]
    ]];
}

CocoaTestSetupView::CocoaTestSetupView(NSRect r)
    : view_{[[NSView alloc] initWithFrame:r]},
      subjectIdLabel{[NSTextField labelWithString:@"subject:"]},
      subjectId_{[NSTextField textFieldWithString:@""]},
      testerIdLabel{[NSTextField labelWithString:@"tester:"]},
      testerId_{[NSTextField textFieldWithString:@""]},
      sessionLabel{[NSTextField labelWithString:@"session:"]},
      session_{[NSTextField textFieldWithString:@""]},
      rmeSettingLabel{[NSTextField labelWithString:@"RME setting:"]},
      rmeSetting_{[NSTextField textFieldWithString:@""]},
      transducerLabel{[NSTextField labelWithString:@"transducer:"]},
      transducerMenu{
          [[NSPopUpButton alloc] initWithFrame:NSMakeRect(0, 0, menuWidth, 0)
                                     pullsDown:NO]},
      testSettingsFile_label{[NSTextField labelWithString:@"test settings:"]},
      testSettingsFile_{[NSTextField textFieldWithString:@""]},
      startingSnr_label{[NSTextField labelWithString:@"starting SNR (dB):"]},
      startingSnr_{[NSTextField textFieldWithString:@""]},
      actions{[[SetupViewActions alloc] init]} {
    actions->controller = this;
    const auto browseForTestSettingsButton {
        button("browse", actions, @selector(browseForTestSettings))
    };

    const auto confirmButton {
        button("Confirm", actions, @selector(confirmTestSetup))
    };
    const auto playCalibrationButton {
        button("play calibration", actions, @selector(playCalibration))
    };
    [browseForTestSettingsButton sizeToFit];
    [confirmButton sizeToFit];
    [playCalibrationButton sizeToFit];
    [subjectId_ setPlaceholderString:@"abc123"];
    [subjectId_ sizeToFit];
    [testerId_ setPlaceholderString:@"abc123"];
    [testerId_ sizeToFit];
    [session_ setPlaceholderString:@"abc123"];
    [session_ sizeToFit];
    [rmeSetting_ setPlaceholderString:@"ihavenoideawhatgoeshere"];
    [rmeSetting_ sizeToFit];
    [testSettingsFile_
        setPlaceholderString:@"/Users/username/Desktop/file.txt"];
    [testSettingsFile_ sizeToFit];
    [startingSnr_ setPlaceholderString:@"15"];
    [startingSnr_ sizeToFit];
    addAutolayoutEnabledSubview(view_, browseForTestSettingsButton);
    addAutolayoutEnabledSubview(view_, confirmButton);
    addAutolayoutEnabledSubview(view_, playCalibrationButton);
    addAutolayoutEnabledSubview(view_, subjectIdLabel);
    addAutolayoutEnabledSubview(view_, subjectId_);
    addAutolayoutEnabledSubview(view_, testerIdLabel);
    addAutolayoutEnabledSubview(view_, testerId_);
    addAutolayoutEnabledSubview(view_, sessionLabel);
    addAutolayoutEnabledSubview(view_, session_);
    addAutolayoutEnabledSubview(view_, rmeSettingLabel);
    addAutolayoutEnabledSubview(view_, rmeSetting_);
    addAutolayoutEnabledSubview(view_, transducerLabel);
    addAutolayoutEnabledSubview(view_, transducerMenu);
    addAutolayoutEnabledSubview(view_, testSettingsFile_label);
    addAutolayoutEnabledSubview(view_, testSettingsFile_);
    addAutolayoutEnabledSubview(view_, startingSnr_label);
    addAutolayoutEnabledSubview(view_, startingSnr_);
    [NSLayoutConstraint activateConstraints:@[
        [subjectId_.topAnchor constraintEqualToAnchor:view_.topAnchor
                                             constant:8],
        firstToTheRightOfSecondConstraint(subjectId_, subjectIdLabel, 8),
        yCenterConstraint(subjectId_, subjectIdLabel),
        [subjectId_.widthAnchor
            constraintEqualToConstant:NSWidth(subjectId_.frame)],
        [subjectId_.centerXAnchor constraintEqualToAnchor:view_.centerXAnchor],
        firstToTheRightOfSecondConstraint(
            browseForTestSettingsButton, testSettingsFile_, 8),
        yCenterConstraint(browseForTestSettingsButton, testSettingsFile_),
        [confirmButton.trailingAnchor
            constraintEqualToAnchor:view_.trailingAnchor
                           constant:-8],
        [confirmButton.bottomAnchor constraintEqualToAnchor:view_.bottomAnchor
                                                   constant:-8],
        firstToTheRightOfSecondConstraint(
            playCalibrationButton, browseForTestSettingsButton, 8),
        yCenterConstraint(playCalibrationButton, browseForTestSettingsButton),
    ]];
    activateLabeledElementConstraintBelow(subjectId_, testerId_, testerIdLabel);
    activateLabeledElementConstraintBelow(testerId_, session_, sessionLabel);
    activateLabeledElementConstraintBelow(
        session_, rmeSetting_, rmeSettingLabel);
    activateLabeledElementConstraintBelow(
        rmeSetting_, transducerMenu, transducerLabel);
    activateLabeledElementConstraintBelow(
        transducerMenu, testSettingsFile_, testSettingsFile_label);
    activateLabeledElementConstraintBelow(
        testSettingsFile_, startingSnr_, startingSnr_label);
    av_speech_in_noise::show(view_);
}

auto CocoaTestSetupView::view() -> NSView * { return view_; }

void CocoaTestSetupView::show() { av_speech_in_noise::show(view_); }

void CocoaTestSetupView::hide() { av_speech_in_noise::hide(view_); }

auto CocoaTestSetupView::testSettingsFile() -> std::string {
    return string(testSettingsFile_);
}

auto CocoaTestSetupView::testerId() -> std::string { return string(testerId_); }

auto CocoaTestSetupView::startingSnr() -> std::string {
    return string(startingSnr_);
}

auto CocoaTestSetupView::subjectId() -> std::string {
    return string(subjectId_);
}

auto CocoaTestSetupView::session() -> std::string { return string(session_); }

auto CocoaTestSetupView::transducer() -> std::string {
    return transducerMenu.titleOfSelectedItem.UTF8String;
}

auto CocoaTestSetupView::rmeSetting() -> std::string {
    return string(rmeSetting_);
}

void CocoaTestSetupView::populateTransducerMenu(
    std::vector<std::string> items) {
    for (const auto &item : items)
        [transducerMenu addItemWithTitle:asNsString(item)];
}

void CocoaTestSetupView::setTestSettingsFile(std::string s) {
    set(testSettingsFile_, s);
}

void CocoaTestSetupView::confirm() { listener_->confirmTestSetup(); }

void CocoaTestSetupView::subscribe(EventListener *listener) {
    listener_ = listener;
}

void CocoaTestSetupView::browseForTestSettings() {
    listener_->browseForTestSettingsFile();
}

void CocoaTestSetupView::playCalibration() { listener_->playCalibration(); }

static auto resourcePath(const std::string &stem, const std::string &extension)
    -> std::string {
    return [[NSBundle mainBundle] pathForResource:asNsString(stem)
                                           ofType:asNsString(extension)]
        .UTF8String;
}

static void addConsonantImageButton(
    std::unordered_map<id, std::string> &consonants, NSView *parent,
    ConsonantViewActions *actions, const std::string &consonant, gsl::index row,
    gsl::index column, gsl::index totalRows, gsl::index totalColumns) {
    constexpr auto spacing{8};
    const auto image{[[NSImage alloc]
        initWithContentsOfFile:asNsString(resourcePath(consonant, "bmp"))]};
    const auto button {
        [NSButton
            buttonWithImage:image
                     target:actions
                     action:@selector(notifyThatResponseButtonHasBeenClicked:)]
    };
    consonants[button] = consonant;
    const auto imageWidth{
        (width(parent.frame) - (totalColumns + 1) * spacing) / totalColumns};
    const auto imageHeight{
        (height(parent.frame) - (totalRows + 1) * spacing) / totalRows};
    [button setFrame:NSMakeRect(imageWidth * column + spacing * (column + 1),
                         imageHeight * (totalRows - row - 1) +
                             spacing * (totalRows - row),
                         imageWidth, imageHeight)];
    button.bordered = NO;
    button.imageScaling = NSImageScaleProportionallyUpOrDown;
    addSubview(parent, button);
}

static void addReadyButton(NSView *parent, ConsonantViewActions *actions) {
    const auto button_ {
        button("", actions, @selector(notifyThatReadyButtonHasBeenClicked))
    };
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
    [button_
        setFrame:NSMakeRect(0, 0, width(parent.frame), height(parent.frame))];
    addSubview(parent, button_);
}

CocoaConsonantView::CocoaConsonantView(NSRect r)
    : // Defer may be critical here...
      window{[[NSWindow alloc] initWithContentRect:r
                                         styleMask:NSWindowStyleMaskBorderless
                                           backing:NSBackingStoreBuffered
                                             defer:YES]},
      responseButtons{
          [[NSView alloc] initWithFrame:NSMakeRect(0, 0, width(r), height(r))]},
      readyButton{
          [[NSView alloc] initWithFrame:NSMakeRect(0, 0, width(r), height(r))]},
      actions{[[ConsonantViewActions alloc] init]} {
    actions->controller = this;
    addConsonantImageButton(
        consonants, responseButtons, actions, "b", 0, 0, 3, 4);
    addConsonantImageButton(
        consonants, responseButtons, actions, "c", 0, 1, 3, 4);
    addConsonantImageButton(
        consonants, responseButtons, actions, "d", 0, 2, 3, 4);
    addConsonantImageButton(
        consonants, responseButtons, actions, "h", 0, 3, 3, 4);
    addConsonantImageButton(
        consonants, responseButtons, actions, "k", 1, 0, 3, 4);
    addConsonantImageButton(
        consonants, responseButtons, actions, "m", 1, 1, 3, 4);
    addConsonantImageButton(
        consonants, responseButtons, actions, "n", 1, 2, 3, 4);
    addConsonantImageButton(
        consonants, responseButtons, actions, "p", 1, 3, 3, 4);
    addConsonantImageButton(
        consonants, responseButtons, actions, "s", 2, 0, 3, 4);
    addConsonantImageButton(
        consonants, responseButtons, actions, "t", 2, 1, 3, 4);
    addConsonantImageButton(
        consonants, responseButtons, actions, "v", 2, 2, 3, 4);
    addConsonantImageButton(
        consonants, responseButtons, actions, "z", 2, 3, 3, 4);
    addReadyButton(readyButton, actions);
    addSubview(window.contentView, readyButton);
    addSubview(window.contentView, responseButtons);
    hideResponseButtons();
    hideReadyButton();
}

void CocoaConsonantView::show() { [window makeKeyAndOrderFront:nil]; }

void CocoaConsonantView::hide() { [window orderOut:nil]; }

void CocoaConsonantView::showReadyButton() {
    av_speech_in_noise::show(readyButton);
}

void CocoaConsonantView::hideReadyButton() {
    av_speech_in_noise::hide(readyButton);
}

void CocoaConsonantView::subscribe(EventListener *e) { listener_ = e; }

void CocoaConsonantView::notifyThatReadyButtonHasBeenClicked() {
    listener_->notifyThatReadyButtonHasBeenClicked();
}

void CocoaConsonantView::notifyThatResponseButtonHasBeenClicked(id sender) {
    lastButtonPressed = sender;
    listener_->notifyThatResponseButtonHasBeenClicked();
}

void CocoaConsonantView::showResponseButtons() {
    av_speech_in_noise::show(responseButtons);
}

void CocoaConsonantView::hideResponseButtons() {
    av_speech_in_noise::hide(responseButtons);
}

auto CocoaConsonantView::consonant() -> std::string {
    return consonants.at(lastButtonPressed);
}

static auto greenColor{NSColor.greenColor};
static auto redColor{NSColor.redColor};
static auto blueColor{NSColor.blueColor};
static auto whiteColor{NSColor.whiteColor};
constexpr std::array<int, 8> numbers{{1, 2, 3, 4, 5, 6, 8, 9}};
constexpr auto responseNumbers{std::size(numbers)};
constexpr auto responseColors{4};

CocoaCoordinateResponseMeasureView::CocoaCoordinateResponseMeasureView(NSRect r)
    : // Defer may be critical here...
      window{[[NSWindow alloc] initWithContentRect:r
                                         styleMask:NSWindowStyleMaskBorderless
                                           backing:NSBackingStoreBuffered
                                             defer:YES]},
      responseButtons{
          [[NSView alloc] initWithFrame:NSMakeRect(0, 0, width(r), height(r))]},
      nextTrialButton{
          [[NSView alloc] initWithFrame:NSMakeRect(0, 0, width(r), height(r))]},
      actions{[[CoordinateResponseMeasureViewActions alloc] init]} {
    actions->controller = this;
    addButtonRow(blueColor, 0);
    addButtonRow(greenColor, 1);
    addButtonRow(whiteColor, 2);
    addButtonRow(redColor, 3);
    addNextTrialButton();
    addSubview(window.contentView, nextTrialButton);
    addSubview(window.contentView, responseButtons);
    hideResponseButtons();
    hideNextTrialButton();
}

void CocoaCoordinateResponseMeasureView::addButtonRow(NSColor *color, int row) {
    for (std::size_t col{0}; col < responseNumbers; ++col)
        addNumberButton(color, numbers.at(col), row, col);
}

void CocoaCoordinateResponseMeasureView::addNumberButton(
    NSColor *color, int number, int row, std::size_t col) {
    auto title{asNsString(std::to_string(number))};
    const auto button {
        [NSButton
            buttonWithTitle:title
                     target:actions
                     action:@selector(notifyThatResponseButtonHasBeenClicked:)]
    };
    auto responseWidth{width(responseButtons.frame) / responseNumbers};
    auto responseHeight{height(responseButtons.frame) / responseColors};
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
    addSubview(responseButtons, button);
}

void CocoaCoordinateResponseMeasureView::addNextTrialButton() {
    const auto button_ {
        button("", actions, @selector(notifyThatReadyButtonHasBeenClicked))
    };
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
    [button_ setFrame:NSMakeRect(0, 0, width(nextTrialButton.frame),
                          height(nextTrialButton.frame))];
    addSubview(nextTrialButton, button_);
}

auto CocoaCoordinateResponseMeasureView::numberResponse() -> std::string {
    return lastButtonPressed.title.UTF8String;
}

auto CocoaCoordinateResponseMeasureView::greenResponse() -> bool {
    return lastPressedColor() == greenColor;
}

auto CocoaCoordinateResponseMeasureView::lastPressedColor() -> NSColor * {
    return [lastButtonPressed.attributedTitle
             attribute:NSForegroundColorAttributeName
               atIndex:0
        effectiveRange:nil];
}

auto CocoaCoordinateResponseMeasureView::blueResponse() -> bool {
    return lastPressedColor() == blueColor;
}

auto CocoaCoordinateResponseMeasureView::whiteResponse() -> bool {
    return lastPressedColor() == whiteColor;
}

void CocoaCoordinateResponseMeasureView::notifyThatResponseButtonHasBeenClicked(
    id sender) {
    lastButtonPressed = sender;
    listener_->notifyThatResponseButtonHasBeenClicked();
}

void CocoaCoordinateResponseMeasureView::showResponseButtons() {
    av_speech_in_noise::show(responseButtons);
}

void CocoaCoordinateResponseMeasureView::showNextTrialButton() {
    av_speech_in_noise::show(nextTrialButton);
}

void CocoaCoordinateResponseMeasureView::hideNextTrialButton() {
    av_speech_in_noise::hide(nextTrialButton);
}

void CocoaCoordinateResponseMeasureView::notifyThatReadyButtonHasBeenClicked() {
    listener_->notifyThatReadyButtonHasBeenClicked();
}

void CocoaCoordinateResponseMeasureView::hideResponseButtons() {
    av_speech_in_noise::hide(responseButtons);
}

void CocoaCoordinateResponseMeasureView::subscribe(EventListener *e) {
    listener_ = e;
}

void CocoaCoordinateResponseMeasureView::show() {
    [window makeKeyAndOrderFront:nil];
}

void CocoaCoordinateResponseMeasureView::hide() { [window orderOut:nil]; }

constexpr auto responseSubmissionWidth{250};
constexpr auto leadingPrimaryTextEdge{buttonWidth + reasonableSpacing};
constexpr auto primaryTextWidth{labelWidth};
constexpr auto leadingSecondaryTextEdge{
    leadingPrimaryTextEdge + primaryTextWidth + reasonableSpacing};
constexpr auto evaluationButtonsInnerGap{0};
constexpr auto evaluationButtonsWidth{
    2 * buttonWidth + evaluationButtonsInnerGap};
constexpr auto continueTestingDialogHeight{2 * labelHeight};

constexpr auto lowerPrimaryTextEdge(const NSRect &r) -> CGFloat {
    return height(r) - labelHeight;
}

CocoaExperimenterView::CocoaExperimenterView(NSRect r)
    : view_{[[NSView alloc] initWithFrame:r]},
      evaluationButtons{[[NSView alloc]
          initWithFrame:NSMakeRect(
                            width(r) - evaluationButtonsWidth - buttonWidth, 0,
                            evaluationButtonsWidth, buttonHeight)]},
      responseSubmission{[[NSView alloc]
          initWithFrame:NSMakeRect(width(r) - responseSubmissionWidth, 0,
                            responseSubmissionWidth,
                            buttonHeight + reasonableSpacing + 2 * labelHeight +
                                reasonableSpacing)]},
      correctKeywordsSubmission{[[NSView alloc]
          initWithFrame:NSMakeRect(width(r) - normalTextFieldWidth, 0,
                            normalTextFieldWidth,
                            buttonHeight + reasonableSpacing + labelHeight)]},
      continueTestingDialog{[[NSWindow alloc]
          initWithContentRect:NSMakeRect(0, 0, width(r),
                                  buttonHeight + continueTestingDialogHeight)
                    styleMask:NSWindowStyleMaskBorderless
                      backing:NSBackingStoreBuffered
                        defer:YES]},
      continueTestingDialogMessage_{[[NSTextField alloc]
          initWithFrame:NSMakeRect(0, buttonHeight, width(r),
                            continueTestingDialogHeight)]},
      displayedText_{[[NSTextField alloc]
          initWithFrame:NSMakeRect(leadingPrimaryTextEdge,
                            lowerPrimaryTextEdge(r), primaryTextWidth,
                            labelHeight)]},
      secondaryDisplayedText_{[[NSTextField alloc]
          initWithFrame:NSMakeRect(leadingSecondaryTextEdge,
                            lowerPrimaryTextEdge(r),
                            width(r) - leadingSecondaryTextEdge, labelHeight)]},
      response_{[[NSTextField alloc]
          initWithFrame:NSMakeRect(0,
                            buttonHeight + reasonableSpacing + labelHeight +
                                reasonableSpacing,
                            responseSubmissionWidth, labelHeight)]},
      correctKeywordsEntry_{[[NSTextField alloc]
          initWithFrame:NSMakeRect(0, buttonHeight + reasonableSpacing,
                            normalTextFieldWidth, labelHeight)]},
      flagged_{[[NSButton alloc]
          initWithFrame:NSMakeRect(0, buttonHeight + reasonableSpacing,
                            normalTextFieldWidth, labelHeight)]},
      actions{[[ExperimenterViewActions alloc] init]} {
    exitTestButton_ = button("exit test", actions, @selector(exitTest));
    [exitTestButton_ setFrame:NSMakeRect(0, height(r) - buttonHeight,
                                  buttonWidth, buttonHeight)];
    setStaticLike(displayedText_);
    setStaticLike(secondaryDisplayedText_);
    setStaticLike(continueTestingDialogMessage_);
    addSubview(view_, exitTestButton_);
    addSubview(view_, displayedText_);
    addSubview(view_, secondaryDisplayedText_);
    av_speech_in_noise::hide(view_);
    [flagged_ setButtonType:NSButtonTypeSwitch];
    [flagged_ setTitle:@"flagged"];
    nextTrialButton_ = button("play trial", actions, @selector(playTrial));
    [nextTrialButton_ setFrame:NSMakeRect(width(r) - buttonWidth, 0,
                                   buttonWidth, buttonHeight)];
    const auto submitFreeResponse_ {
        button("submit", actions, @selector(submitFreeResponse))
    };
    [submitFreeResponse_
        setFrame:NSMakeRect(width(responseSubmission.frame) - buttonWidth, 0,
                     buttonWidth, buttonHeight)];
    const auto passButton_ {
        button("correct", actions, @selector(submitPassedTrial))
    };
    [passButton_ setFrame:NSMakeRect(width(evaluationButtons.frame) -
                                  evaluationButtonsWidth,
                              0, buttonWidth, buttonHeight)];
    const auto failButton_ {
        button("incorrect", actions, @selector(submitFailedTrial))
    };
    [failButton_ setFrame:NSMakeRect(width(evaluationButtons.frame) -
                                  evaluationButtonsWidth + buttonWidth +
                                  evaluationButtonsInnerGap,
                              0, buttonWidth, buttonHeight)];
    const auto continueButton_ {
        button("continue", actions, @selector(acceptContinuingTesting))
    };
    const auto exitButton_ {
        button("exit", actions, @selector(declineContinuingTesting))
    };
    [continueButton_ setFrame:NSMakeRect(width(r) - buttonWidth, 0, buttonWidth,
                                  buttonHeight)];
    [exitButton_ setFrame:NSMakeRect(width(r) - 3 * buttonWidth, 0, buttonWidth,
                              buttonHeight)];
    const auto submitCorrectKeywords_ {
        button("submit", actions, @selector(submitCorrectKeywords))
    };
    [submitCorrectKeywords_
        setFrame:NSMakeRect(
                     width(correctKeywordsSubmission.frame) - buttonWidth, 0,
                     buttonWidth, buttonHeight)];
    addSubview(responseSubmission, submitFreeResponse_);
    addSubview(responseSubmission, response_);
    addSubview(responseSubmission, flagged_);
    addSubview(evaluationButtons, passButton_);
    addSubview(evaluationButtons, failButton_);
    addSubview(continueTestingDialog.contentView, continueButton_);
    addSubview(continueTestingDialog.contentView, exitButton_);
    addSubview(
        continueTestingDialog.contentView, continueTestingDialogMessage_);
    addSubview(correctKeywordsSubmission, correctKeywordsEntry_);
    addSubview(correctKeywordsSubmission, submitCorrectKeywords_);
    addSubview(view_, nextTrialButton_);
    addSubview(view_, responseSubmission);
    addSubview(view_, evaluationButtons);
    addSubview(view_, correctKeywordsSubmission);
    av_speech_in_noise::hide(evaluationButtons);
    av_speech_in_noise::hide(nextTrialButton_);
    av_speech_in_noise::hide(responseSubmission);
    av_speech_in_noise::hide(correctKeywordsSubmission);
    av_speech_in_noise::hide(view_);
    actions->controller = this;
}

void CocoaExperimenterView::subscribe(EventListener *e) { listener_ = e; }

void CocoaExperimenterView::showExitTestButton() {
    av_speech_in_noise::show(exitTestButton_);
}

void CocoaExperimenterView::hideExitTestButton() {
    av_speech_in_noise::hide(exitTestButton_);
}

void CocoaExperimenterView::show() { av_speech_in_noise::show(view_); }

void CocoaExperimenterView::hide() { av_speech_in_noise::hide(view_); }

auto CocoaExperimenterView::view() -> NSView * { return view_; }

void CocoaExperimenterView::exitTest() { listener_->exitTest(); }

void CocoaExperimenterView::display(std::string s) { set(displayedText_, s); }

void CocoaExperimenterView::secondaryDisplay(std::string s) {
    set(secondaryDisplayedText_, s);
}

void CocoaExperimenterView::showNextTrialButton() {
    av_speech_in_noise::show(nextTrialButton_);
}

void CocoaExperimenterView::hideNextTrialButton() {
    av_speech_in_noise::hide(nextTrialButton_);
}

void CocoaExperimenterView::showEvaluationButtons() {
    av_speech_in_noise::show(evaluationButtons);
}

void CocoaExperimenterView::showFreeResponseSubmission() {
    av_speech_in_noise::show(responseSubmission);
}

void CocoaExperimenterView::hideFreeResponseSubmission() {
    av_speech_in_noise::hide(responseSubmission);
}

void CocoaExperimenterView::hideEvaluationButtons() {
    av_speech_in_noise::hide(evaluationButtons);
}

void CocoaExperimenterView::showCorrectKeywordsSubmission() {
    av_speech_in_noise::show(correctKeywordsSubmission);
}

void CocoaExperimenterView::hideCorrectKeywordsSubmission() {
    av_speech_in_noise::hide(correctKeywordsSubmission);
}

void CocoaExperimenterView::showContinueTestingDialog() {
    [view_.window beginSheet:continueTestingDialog
           completionHandler:^(NSModalResponse){
           }];
}

void CocoaExperimenterView::hideContinueTestingDialog() {
    [view_.window endSheet:continueTestingDialog];
}

void CocoaExperimenterView::setContinueTestingDialogMessage(
    const std::string &s) {
    set(continueTestingDialogMessage_, s);
}

void CocoaExperimenterView::clearFreeResponse() { set(response_, ""); }

auto CocoaExperimenterView::freeResponse() -> std::string {
    return string(response_);
}

auto CocoaExperimenterView::correctKeywords() -> std::string {
    return string(correctKeywordsEntry_);
}

auto CocoaExperimenterView::flagged() -> bool {
    return flagged_.state == NSControlStateValueOn;
}

void CocoaExperimenterView::playTrial() { listener_->playTrial(); }

void CocoaExperimenterView::submitFreeResponse() {
    listener_->submitFreeResponse();
}

void CocoaExperimenterView::submitPassedTrial() {
    listener_->submitPassedTrial();
}

void CocoaExperimenterView::submitFailedTrial() {
    listener_->submitFailedTrial();
}

void CocoaExperimenterView::submitCorrectKeywords() {
    listener_->submitCorrectKeywords();
}

void CocoaExperimenterView::acceptContinuingTesting() {
    listener_->acceptContinuingTesting();
}

void CocoaExperimenterView::declineContinuingTesting() {
    listener_->declineContinuingTesting();
}

constexpr auto windowPerimeterSpace{reasonableSpacing};

constexpr auto innerWidth(NSRect r) -> CGFloat {
    return width(r) - 2 * windowPerimeterSpace;
}

constexpr auto innerHeight(NSRect r) -> CGFloat {
    return height(r) - 2 * windowPerimeterSpace;
}

static auto embeddedFrame(NSRect r) -> NSRect {
    return NSMakeRect(r.origin.x + windowPerimeterSpace,
        r.origin.y + windowPerimeterSpace, innerWidth(r), innerHeight(r));
}

static auto innerFrame(NSRect r) -> NSRect {
    return NSMakeRect(0, 0, innerWidth(r), innerHeight(r));
}

CocoaView::CocoaView(NSRect r)
    : testSetup_{innerFrame(r)},
      experimenter_{innerFrame(r)}, app{[NSApplication sharedApplication]},
      window{[[NSWindow alloc] initWithContentRect:r
                                         styleMask:NSWindowStyleMaskClosable |
                                         NSWindowStyleMaskTitled
                                           backing:NSBackingStoreBuffered
                                             defer:NO]},
      view{[[NSView alloc] initWithFrame:embeddedFrame(r)]},
      audioDevice_label{normalLabelWithHeight(0, "audio output:")},
      deviceMenu{
          [[NSPopUpButton alloc] initWithFrame:NSMakeRect(textFieldLeadingEdge,
                                                   0, menuWidth, labelHeight)
                                     pullsDown:NO]} {
    app.mainMenu = [[NSMenu alloc] init];

    auto appMenu{[[NSMenuItem alloc] init]};
    auto appSubMenu{[[NSMenu alloc] init]};
    [appSubMenu addItemWithTitle:@"Quit"
                          action:@selector(stop:)
                   keyEquivalent:@"q"];
    [appMenu setSubmenu:appSubMenu];
    [app.mainMenu addItem:appMenu];
    addSubview(view, testSetup_.view());
    addSubview(view, experimenter_.view());
    addSubview(view, audioDevice_label);
    addSubview(view, deviceMenu);
    addSubview(window.contentView, view);
    [window makeKeyAndOrderFront:nil];
}

void CocoaView::eventLoop() { [app run]; }

void CocoaView::showErrorMessage(std::string s) {
    auto alert{[[NSAlert alloc] init]};
    [alert setMessageText:@"Error."];
    [alert setInformativeText:asNsString(s)];
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

void CocoaView::setDelegate(id<NSWindowDelegate> delegate) {
    [window setDelegate:delegate];
}

void CocoaView::center() { [window center]; }

auto CocoaView::testSetup() -> View::TestSetup & { return testSetup_; }

auto CocoaView::experimenter() -> View::Experimenter & { return experimenter_; }
}
