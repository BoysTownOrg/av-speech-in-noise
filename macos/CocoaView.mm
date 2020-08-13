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

static constexpr auto labelHeight{22};
static constexpr auto labelWidth{120};
static constexpr auto buttonHeight{25};
static constexpr auto buttonWidth{100};
constexpr auto reasonableSpacing{15};

constexpr auto width(const NSRect &r) -> CGFloat { return r.size.width; }

constexpr auto height(const NSRect &r) -> CGFloat { return r.size.height; }

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

static auto firstAboveSecondConstraint(NSView *first, NSView *second, CGFloat x)
    -> NSLayoutConstraint * {
    return [first.bottomAnchor constraintEqualToAnchor:second.topAnchor
                                              constant:-x];
}

static auto widthConstraint(NSView *a) -> NSLayoutConstraint * {
    return [a.widthAnchor constraintEqualToConstant:NSWidth(a.frame)];
}

static void activateLabeledElementConstraintBelow(
    NSView *above, NSView *below, NSView *belowLabel) {
    [NSLayoutConstraint activateConstraints:@[
        [above.leadingAnchor constraintEqualToAnchor:below.leadingAnchor],
        [below.topAnchor constraintEqualToAnchor:above.bottomAnchor constant:8],
        firstToTheRightOfSecondConstraint(below, belowLabel, 8),
        yCenterConstraint(below, belowLabel)
    ]];
}

CocoaTestSetupView::CocoaTestSetupView(NSRect r)
    : view_{[[NSView alloc] initWithFrame:r]},
      subjectIdLabel{[NSTextField labelWithString:@"subject:"]},
      subjectIdField{[NSTextField textFieldWithString:@""]},
      testerIdLabel{[NSTextField labelWithString:@"tester:"]},
      testerIdField{[NSTextField textFieldWithString:@""]},
      sessionLabel{[NSTextField labelWithString:@"session:"]},
      sessionField{[NSTextField textFieldWithString:@""]},
      rmeSettingLabel{[NSTextField labelWithString:@"RME setting:"]},
      rmeSettingField{[NSTextField textFieldWithString:@""]},
      transducerLabel{[NSTextField labelWithString:@"transducer:"]},
      transducerMenu{[[NSPopUpButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)
                                                pullsDown:NO]},
      testSettingsLabel{[NSTextField labelWithString:@"test settings:"]},
      testSettingsField{[NSTextField textFieldWithString:@""]},
      startingSnrLabel{[NSTextField labelWithString:@"starting SNR (dB):"]},
      startingSnrField{[NSTextField textFieldWithString:@""]},
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
    [subjectIdField setPlaceholderString:@"abc123"];
    [subjectIdField sizeToFit];
    [testerIdField setPlaceholderString:@"abc123"];
    [testerIdField sizeToFit];
    [sessionField setPlaceholderString:@"abc123"];
    [sessionField sizeToFit];
    [rmeSettingField setPlaceholderString:@"ihavenoideawhatgoeshere"];
    [rmeSettingField sizeToFit];
    [testSettingsField
        setPlaceholderString:@"/Users/username/Desktop/file.txt"];
    [testSettingsField sizeToFit];
    [startingSnrField setPlaceholderString:@"15"];
    [startingSnrField sizeToFit];
    addAutolayoutEnabledSubview(view_, browseForTestSettingsButton);
    addAutolayoutEnabledSubview(view_, confirmButton);
    addAutolayoutEnabledSubview(view_, playCalibrationButton);
    addAutolayoutEnabledSubview(view_, subjectIdLabel);
    addAutolayoutEnabledSubview(view_, subjectIdField);
    addAutolayoutEnabledSubview(view_, testerIdLabel);
    addAutolayoutEnabledSubview(view_, testerIdField);
    addAutolayoutEnabledSubview(view_, sessionLabel);
    addAutolayoutEnabledSubview(view_, sessionField);
    addAutolayoutEnabledSubview(view_, rmeSettingLabel);
    addAutolayoutEnabledSubview(view_, rmeSettingField);
    addAutolayoutEnabledSubview(view_, transducerLabel);
    addAutolayoutEnabledSubview(view_, transducerMenu);
    addAutolayoutEnabledSubview(view_, testSettingsLabel);
    addAutolayoutEnabledSubview(view_, testSettingsField);
    addAutolayoutEnabledSubview(view_, startingSnrLabel);
    addAutolayoutEnabledSubview(view_, startingSnrField);
    [NSLayoutConstraint activateConstraints:@[
        [subjectIdField.topAnchor constraintEqualToAnchor:view_.topAnchor
                                                 constant:8],
        firstToTheRightOfSecondConstraint(subjectIdField, subjectIdLabel, 8),
        yCenterConstraint(subjectIdField, subjectIdLabel),
        widthConstraint(subjectIdField),
        [subjectIdField.centerXAnchor
            constraintEqualToAnchor:view_.centerXAnchor],
        firstToTheRightOfSecondConstraint(
            browseForTestSettingsButton, testSettingsField, 8),
        yCenterConstraint(browseForTestSettingsButton, testSettingsField),
        [confirmButton.trailingAnchor
            constraintEqualToAnchor:view_.trailingAnchor
                           constant:-8],
        [confirmButton.bottomAnchor constraintEqualToAnchor:view_.bottomAnchor
                                                   constant:-8],
        firstToTheRightOfSecondConstraint(
            playCalibrationButton, browseForTestSettingsButton, 8),
        yCenterConstraint(playCalibrationButton, browseForTestSettingsButton),
        widthConstraint(testerIdField), widthConstraint(sessionField),
        widthConstraint(rmeSettingField), widthConstraint(testSettingsField),
        widthConstraint(startingSnrField)
    ]];
    activateLabeledElementConstraintBelow(
        subjectIdField, testerIdField, testerIdLabel);
    activateLabeledElementConstraintBelow(
        testerIdField, sessionField, sessionLabel);
    activateLabeledElementConstraintBelow(
        sessionField, rmeSettingField, rmeSettingLabel);
    activateLabeledElementConstraintBelow(
        rmeSettingField, transducerMenu, transducerLabel);
    activateLabeledElementConstraintBelow(
        transducerMenu, testSettingsField, testSettingsLabel);
    activateLabeledElementConstraintBelow(
        testSettingsField, startingSnrField, startingSnrLabel);
    av_speech_in_noise::show(view_);
}

auto CocoaTestSetupView::view() -> NSView * { return view_; }

void CocoaTestSetupView::show() { av_speech_in_noise::show(view_); }

void CocoaTestSetupView::hide() { av_speech_in_noise::hide(view_); }

auto CocoaTestSetupView::testSettingsFile() -> std::string {
    return string(testSettingsField);
}

auto CocoaTestSetupView::testerId() -> std::string {
    return string(testerIdField);
}

auto CocoaTestSetupView::startingSnr() -> std::string {
    return string(startingSnrField);
}

auto CocoaTestSetupView::subjectId() -> std::string {
    return string(subjectIdField);
}

auto CocoaTestSetupView::session() -> std::string {
    return string(sessionField);
}

auto CocoaTestSetupView::transducer() -> std::string {
    return transducerMenu.titleOfSelectedItem.UTF8String;
}

auto CocoaTestSetupView::rmeSetting() -> std::string {
    return string(rmeSettingField);
}

void CocoaTestSetupView::populateTransducerMenu(
    std::vector<std::string> items) {
    for (const auto &item : items)
        [transducerMenu addItemWithTitle:asNsString(item)];
    [transducerMenu sizeToFit];
}

void CocoaTestSetupView::setTestSettingsFile(std::string s) {
    set(testSettingsField, s);
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

constexpr auto leadingPrimaryTextEdge{buttonWidth + reasonableSpacing};
constexpr auto primaryTextWidth{labelWidth};
constexpr auto leadingSecondaryTextEdge{
    leadingPrimaryTextEdge + primaryTextWidth + reasonableSpacing};
constexpr auto continueTestingDialogHeight{2 * labelHeight};

constexpr auto lowerPrimaryTextEdge(const NSRect &r) -> CGFloat {
    return height(r) - labelHeight;
}

static void activateChildConstraintNestledInBottomRightCorner(
    NSView *child, NSView *parent, CGFloat x) {

    [NSLayoutConstraint activateConstraints:@[
        [child.trailingAnchor constraintEqualToAnchor:parent.trailingAnchor
                                             constant:-x],
        [child.bottomAnchor constraintEqualToAnchor:parent.bottomAnchor
                                           constant:-x]
    ]];
}

static auto trailingAnchorConstraint(NSView *a, NSView *b)
    -> NSLayoutConstraint * {
    return [a.trailingAnchor constraintEqualToAnchor:b.trailingAnchor];
}

CocoaExperimenterView::CocoaExperimenterView(NSRect r)
    : view_{[[NSView alloc] initWithFrame:r]}, freeResponseView{[[NSView alloc]
                                                   initWithFrame:r]},
      correctKeywordsView{[[NSView alloc] initWithFrame:r]},
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
      freeResponseField{[NSTextField textFieldWithString:@""]},
      correctKeywordsField{[NSTextField textFieldWithString:@""]},
      freeResponseFlaggedButton{[[NSButton alloc] init]},
      actions{[[ExperimenterViewActions alloc] init]} {
    exitTestButton = button("exit test", actions, @selector(exitTest));
    setStaticLike(displayedText_);
    setStaticLike(secondaryDisplayedText_);
    setStaticLike(continueTestingDialogMessage_);
    [freeResponseFlaggedButton setButtonType:NSButtonTypeSwitch];
    [freeResponseFlaggedButton setTitle:@"flagged"];
    [freeResponseFlaggedButton sizeToFit];
    nextTrialButton = button("play trial", actions, @selector(playTrial));
    const auto submitFreeResponseButton {
        button("submit", actions, @selector(submitFreeResponse))
    };
    passButton = button("correct", actions, @selector(submitPassedTrial));
    failButton = button("incorrect", actions, @selector(submitFailedTrial));
    const auto continueButton {
        button("continue", actions, @selector(acceptContinuingTesting))
    };
    const auto exitButton {
        button("exit", actions, @selector(declineContinuingTesting))
    };
    [continueButton setFrame:NSMakeRect(width(r) - buttonWidth, 0, buttonWidth,
                                 buttonHeight)];
    [exitButton setFrame:NSMakeRect(width(r) - 3 * buttonWidth, 0, buttonWidth,
                             buttonHeight)];
    const auto submitCorrectKeywordsButton {
        button("submit", actions, @selector(submitCorrectKeywords))
    };
    [correctKeywordsField setPlaceholderString:@"2"];
    [correctKeywordsField sizeToFit];
    [freeResponseField setPlaceholderString:@"This is a sentence."];
    [freeResponseField sizeToFit];
    addAutolayoutEnabledSubview(view_, exitTestButton);
    addSubview(view_, displayedText_);
    addSubview(view_, secondaryDisplayedText_);
    addAutolayoutEnabledSubview(freeResponseView, submitFreeResponseButton);
    addAutolayoutEnabledSubview(freeResponseView, freeResponseField);
    addAutolayoutEnabledSubview(freeResponseView, freeResponseFlaggedButton);
    addAutolayoutEnabledSubview(view_, passButton);
    addAutolayoutEnabledSubview(view_, failButton);
    addSubview(continueTestingDialog.contentView, continueButton);
    addSubview(continueTestingDialog.contentView, exitButton);
    addSubview(
        continueTestingDialog.contentView, continueTestingDialogMessage_);
    addAutolayoutEnabledSubview(correctKeywordsView, correctKeywordsField);
    addAutolayoutEnabledSubview(
        correctKeywordsView, submitCorrectKeywordsButton);
    addAutolayoutEnabledSubview(view_, nextTrialButton);
    addSubview(view_, freeResponseView);
    addAutolayoutEnabledSubview(view_, correctKeywordsView);
    [NSLayoutConstraint activateConstraints:@[
        [exitTestButton.leadingAnchor
            constraintEqualToAnchor:view_.leadingAnchor
                           constant:8],
        [exitTestButton.topAnchor constraintEqualToAnchor:view_.topAnchor
                                                 constant:8],
        firstToTheRightOfSecondConstraint(passButton, failButton, 8),
        yCenterConstraint(passButton, failButton),
        firstAboveSecondConstraint(
            correctKeywordsField, submitCorrectKeywordsButton, 8),
        firstAboveSecondConstraint(
            freeResponseField, submitFreeResponseButton, 8),
        trailingAnchorConstraint(
            correctKeywordsField, submitCorrectKeywordsButton),
        trailingAnchorConstraint(freeResponseField, submitFreeResponseButton),
        widthConstraint(correctKeywordsField),
        widthConstraint(freeResponseField),
        widthConstraint(freeResponseFlaggedButton),
        firstToTheRightOfSecondConstraint(
            freeResponseField, freeResponseFlaggedButton, 8),
        yCenterConstraint(freeResponseField, freeResponseFlaggedButton)
    ]];
    activateChildConstraintNestledInBottomRightCorner(passButton, view_, 8);
    activateChildConstraintNestledInBottomRightCorner(
        submitCorrectKeywordsButton, view_, 8);
    activateChildConstraintNestledInBottomRightCorner(
        nextTrialButton, view_, 8);
    activateChildConstraintNestledInBottomRightCorner(
        submitFreeResponseButton, view_, 8);
    av_speech_in_noise::hide(passButton);
    av_speech_in_noise::hide(failButton);
    av_speech_in_noise::hide(nextTrialButton);
    av_speech_in_noise::hide(freeResponseView);
    av_speech_in_noise::hide(correctKeywordsView);
    av_speech_in_noise::hide(view_);
    actions->controller = this;
}

void CocoaExperimenterView::subscribe(EventListener *e) { listener_ = e; }

void CocoaExperimenterView::showExitTestButton() {
    av_speech_in_noise::show(exitTestButton);
}

void CocoaExperimenterView::hideExitTestButton() {
    av_speech_in_noise::hide(exitTestButton);
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
    av_speech_in_noise::show(nextTrialButton);
}

void CocoaExperimenterView::hideNextTrialButton() {
    av_speech_in_noise::hide(nextTrialButton);
}

void CocoaExperimenterView::showEvaluationButtons() {
    av_speech_in_noise::show(passButton);
    av_speech_in_noise::show(failButton);
}

void CocoaExperimenterView::showFreeResponseSubmission() {
    av_speech_in_noise::show(freeResponseView);
}

void CocoaExperimenterView::hideFreeResponseSubmission() {
    av_speech_in_noise::hide(freeResponseView);
}

void CocoaExperimenterView::hideEvaluationButtons() {
    av_speech_in_noise::hide(passButton);
    av_speech_in_noise::hide(failButton);
}

void CocoaExperimenterView::showCorrectKeywordsSubmission() {
    av_speech_in_noise::show(correctKeywordsView);
}

void CocoaExperimenterView::hideCorrectKeywordsSubmission() {
    av_speech_in_noise::hide(correctKeywordsView);
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

void CocoaExperimenterView::clearFreeResponse() { set(freeResponseField, ""); }

auto CocoaExperimenterView::freeResponse() -> std::string {
    return string(freeResponseField);
}

auto CocoaExperimenterView::correctKeywords() -> std::string {
    return string(correctKeywordsField);
}

auto CocoaExperimenterView::flagged() -> bool {
    return freeResponseFlaggedButton.state == NSControlStateValueOn;
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
      audioDeviceLabel{[NSTextField labelWithString:@"audio output:"]},
      audioDeviceMenu{
          [[NSPopUpButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)
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
    addAutolayoutEnabledSubview(view, audioDeviceLabel);
    addAutolayoutEnabledSubview(view, audioDeviceMenu);
    addSubview(window.contentView, view);
    [NSLayoutConstraint activateConstraints:@[
        firstToTheRightOfSecondConstraint(audioDeviceMenu, audioDeviceLabel, 8),
        yCenterConstraint(audioDeviceMenu, audioDeviceLabel),
        [audioDeviceMenu.bottomAnchor constraintEqualToAnchor:view.bottomAnchor
                                                     constant:-8]
    ]];
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
    return audioDeviceMenu.titleOfSelectedItem.UTF8String;
}

void CocoaView::populateAudioDeviceMenu(std::vector<std::string> items) {
    for (const auto &item : items)
        [audioDeviceMenu addItemWithTitle:asNsString(item)];
    [audioDeviceMenu sizeToFit];
}

void CocoaView::setDelegate(id<NSWindowDelegate> delegate) {
    [window setDelegate:delegate];
}

void CocoaView::center() { [window center]; }

auto CocoaView::testSetup() -> View::TestSetup & { return testSetup_; }

auto CocoaView::experimenter() -> View::Experimenter & { return experimenter_; }
}
