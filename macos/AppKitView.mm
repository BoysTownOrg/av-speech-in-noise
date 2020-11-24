#include "AppKitView.h"
#include "common-objc.h"
#include "AppKit-utility.h"
#include "Foundation-utility.h"
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

@interface FreeResponseViewActions : NSObject
@end

@implementation SetupViewActions {
  @public
    av_speech_in_noise::AppKitTestSetupView *controller;
}

- (void)notifyThatConfirmButtonHasBeenClicked {
    controller->notifyThatConfirmButtonHasBeenClicked();
}

- (void)notifyThatBrowseForTestSettingsButtonHasBeenClicked {
    controller->notifyThatBrowseForTestSettingsButtonHasBeenClicked();
}

- (void)notifyThatPlayCalibrationButtonHasBeenClicked {
    controller->notifyThatPlayCalibrationButtonHasBeenClicked();
}
@end

@implementation CoordinateResponseMeasureViewActions {
  @public
    av_speech_in_noise::AppKitCoordinateResponseMeasureView *controller;
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
    av_speech_in_noise::AppKitConsonantView *controller;
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
    av_speech_in_noise::AppKitExperimenterView *controller;
}

- (void)exitTest {
    controller->exitTest();
}

- (void)playTrial {
    controller->playTrial();
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

@implementation FreeResponseViewActions {
  @public
    av_speech_in_noise::AppKitExperimenterView *controller;
}

- (void)submitFreeResponse {
    controller->submitFreeResponse();
}
@end

namespace av_speech_in_noise {
constexpr auto defaultMarginPoints{8};

static void addSubview(NSView *parent, NSView *child) {
    [parent addSubview:child];
}

static void hide(NSView *v) { [v setHidden:YES]; }

static void show(NSView *v) { [v setHidden:NO]; }

static void set(NSTextField *field, const std::string &s) {
    [field setStringValue:nsString(s)];
}

constexpr auto width(const NSRect &r) -> CGFloat { return r.size.width; }

constexpr auto height(const NSRect &r) -> CGFloat { return r.size.height; }

static auto button(const std::string &s, id target, SEL action) -> NSButton * {
    return [NSButton buttonWithTitle:nsString(s) target:target action:action];
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

static void activateChildConstraintNestledInBottomRightCorner(
    NSView *child, NSView *parent, CGFloat x) {

    [NSLayoutConstraint activateConstraints:@[
        [child.trailingAnchor constraintEqualToAnchor:parent.trailingAnchor
                                             constant:-x],
        [child.bottomAnchor constraintEqualToAnchor:parent.bottomAnchor
                                           constant:-x]
    ]];
}

static auto emptyTextField() -> NSTextField * {
    return [NSTextField textFieldWithString:@""];
}

static auto label(const std::string &s) -> NSTextField * {
    return [NSTextField labelWithString:nsString(s)];
}

static void setPlaceholder(NSTextField *field, const std::string &s) {
    [field setPlaceholderString:nsString(s)];
}

static auto labeledView(NSView *field, const std::string &s) -> NSStackView * {
    const auto label_{label(s)};
    [label_ setContentHuggingPriority:251
                       forOrientation:NSLayoutConstraintOrientationHorizontal];
    const auto stack { [NSStackView stackViewWithViews:@[ label_, field ]] };
    return stack;
}

static void activateConstraints(NSArray<NSLayoutConstraint *> *constraints) {
    [NSLayoutConstraint activateConstraints:constraints];
}

static auto view(NSViewController *viewController) -> NSView * {
    return viewController.view;
}

AppKitTestSetupView::AppKitTestSetupView(NSViewController *viewController)
    : viewController{viewController}, subjectIdField{emptyTextField()},
      testerIdField{emptyTextField()}, sessionField{emptyTextField()},
      rmeSettingField{emptyTextField()},
      transducerMenu{[[NSPopUpButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)
                                                pullsDown:NO]},
      testSettingsField{emptyTextField()}, startingSnrField{emptyTextField()},
      actions{[[SetupViewActions alloc] init]} {
    actions->controller = this;
    const auto confirmButton {
        button("Confirm", actions,
            @selector(notifyThatConfirmButtonHasBeenClicked))
    };
    [confirmButton setKeyEquivalent:@"\r"];
    const auto stack {
        [NSStackView stackViewWithViews:@[
            labeledView(subjectIdField, "subject:"),
            labeledView(testerIdField, "tester:"),
            labeledView(sessionField, "session:"),
            labeledView(rmeSettingField, "RME setting:"),
            labeledView(transducerMenu, "transducer:"),
            [NSStackView stackViewWithViews:@[
                labeledView(testSettingsField, "test settings:"),
                button("Browse...", actions,
                    @selector
                    (notifyThatBrowseForTestSettingsButtonHasBeenClicked)),
                button("Play Calibration", actions,
                    @selector(notifyThatPlayCalibrationButtonHasBeenClicked))
            ]],
            labeledView(startingSnrField, "starting SNR (dB):")
        ]]
    };
    stack.orientation = NSUserInterfaceLayoutOrientationVertical;
    setPlaceholder(subjectIdField, "abc123");
    setPlaceholder(testerIdField, "abc123");
    setPlaceholder(sessionField, "abc123");
    setPlaceholder(rmeSettingField, "ihavenoideawhatgoeshere");
    setPlaceholder(
        testSettingsField, "/Users/username/Documents/test-settings.txt");
    setPlaceholder(startingSnrField, "5");
    addAutolayoutEnabledSubview(view(viewController), confirmButton);
    addAutolayoutEnabledSubview(view(viewController), stack);
    activateConstraints(@[
        [stack.topAnchor constraintEqualToAnchor:view(viewController).topAnchor
                                        constant:defaultMarginPoints],
        [stack.leadingAnchor
            constraintEqualToAnchor:view(viewController).leadingAnchor
                           constant:defaultMarginPoints],
        [subjectIdField.leadingAnchor
            constraintEqualToAnchor:testerIdField.leadingAnchor],
        [subjectIdField.leadingAnchor
            constraintEqualToAnchor:sessionField.leadingAnchor],
        [subjectIdField.leadingAnchor
            constraintEqualToAnchor:rmeSettingField.leadingAnchor],
        [subjectIdField.leadingAnchor
            constraintEqualToAnchor:testSettingsField.leadingAnchor],
        [subjectIdField.leadingAnchor
            constraintEqualToAnchor:startingSnrField.leadingAnchor],
        [subjectIdField.leadingAnchor
            constraintEqualToAnchor:transducerMenu.leadingAnchor],
        [subjectIdField.trailingAnchor
            constraintEqualToAnchor:testerIdField.trailingAnchor],
        [subjectIdField.trailingAnchor
            constraintEqualToAnchor:sessionField.trailingAnchor],
        [subjectIdField.trailingAnchor
            constraintEqualToAnchor:rmeSettingField.trailingAnchor],
        [subjectIdField.trailingAnchor
            constraintEqualToAnchor:testSettingsField.trailingAnchor],
        [subjectIdField.trailingAnchor
            constraintEqualToAnchor:startingSnrField.trailingAnchor],
        [subjectIdField.trailingAnchor
            constraintEqualToAnchor:transducerMenu.trailingAnchor]
    ]);
    activateChildConstraintNestledInBottomRightCorner(
        confirmButton, view(viewController), defaultMarginPoints);
    av_speech_in_noise::show(view(viewController));
}

void AppKitTestSetupView::show() {
    av_speech_in_noise::show(view(viewController));
}

void AppKitTestSetupView::hide() {
    av_speech_in_noise::hide(view(viewController));
}

auto AppKitTestSetupView::testSettingsFile() -> std::string {
    return string(testSettingsField);
}

auto AppKitTestSetupView::testerId() -> std::string {
    return string(testerIdField);
}

auto AppKitTestSetupView::startingSnr() -> std::string {
    return string(startingSnrField);
}

auto AppKitTestSetupView::subjectId() -> std::string {
    return string(subjectIdField);
}

auto AppKitTestSetupView::session() -> std::string {
    return string(sessionField);
}

auto AppKitTestSetupView::rmeSetting() -> std::string {
    return string(rmeSettingField);
}

void AppKitTestSetupView::setTestSettingsFile(std::string s) {
    set(testSettingsField, s);
}

void AppKitTestSetupView::populateTransducerMenu(
    std::vector<std::string> items) {
    for (const auto &item : items)
        [transducerMenu addItemWithTitle:nsString(item)];
}

auto AppKitTestSetupView::transducer() -> std::string {
    return transducerMenu.titleOfSelectedItem.UTF8String;
}

void AppKitTestSetupView::attach(Observer *listener) { listener_ = listener; }

void AppKitTestSetupView::notifyThatConfirmButtonHasBeenClicked() {
    listener_->notifyThatConfirmButtonHasBeenClicked();
}

void AppKitTestSetupView::
    notifyThatBrowseForTestSettingsButtonHasBeenClicked() {
    listener_->notifyThatBrowseForTestSettingsButtonHasBeenClicked();
}

void AppKitTestSetupView::notifyThatPlayCalibrationButtonHasBeenClicked() {
    listener_->notifyThatPlayCalibrationButtonHasBeenClicked();
}

static auto consonantImageButton(
    std::unordered_map<void *, std::string> &consonants,
    ConsonantViewActions *actions, const std::string &consonant) -> NSButton * {
    const auto image{[[NSImage alloc]
        initWithContentsOfFile:nsString(resourcePath(consonant, "bmp"))]};
    const auto button {
        [NSButton
            buttonWithImage:image
                     target:actions
                     action:@selector(notifyThatResponseButtonHasBeenClicked:)]
    };
    consonants[(__bridge void *)button] = consonant;
    button.bordered = NO;
    button.imageScaling = NSImageScaleProportionallyUpOrDown;
    return button;
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

AppKitConsonantView::AppKitConsonantView(NSRect r)
    : // Defer may be critical here...
      window{[[NSWindow alloc] initWithContentRect:r
                                         styleMask:NSWindowStyleMaskBorderless
                                           backing:NSBackingStoreBuffered
                                             defer:YES]},
      readyButton{
          [[NSView alloc] initWithFrame:NSMakeRect(0, 0, width(r), height(r))]},
      actions{[[ConsonantViewActions alloc] init]} {
    actions->controller = this;
    auto firstRow {
        [NSStackView stackViewWithViews:@[
            consonantImageButton(consonants, actions, "b"),
            consonantImageButton(consonants, actions, "c"),
            consonantImageButton(consonants, actions, "d"),
            consonantImageButton(consonants, actions, "h")
        ]]
    };
    auto secondRow {
        [NSStackView stackViewWithViews:@[
            consonantImageButton(consonants, actions, "k"),
            consonantImageButton(consonants, actions, "m"),
            consonantImageButton(consonants, actions, "n"),
            consonantImageButton(consonants, actions, "p")
        ]]
    };
    auto thirdRow {
        [NSStackView stackViewWithViews:@[
            consonantImageButton(consonants, actions, "s"),
            consonantImageButton(consonants, actions, "t"),
            consonantImageButton(consonants, actions, "v"),
            consonantImageButton(consonants, actions, "z")
        ]]
    };
    responseButtons =
        [NSStackView stackViewWithViews:@[ firstRow, secondRow, thirdRow ]];
    responseButtons.orientation = NSUserInterfaceLayoutOrientationVertical;
    responseButtons.distribution = NSStackViewDistributionFillEqually;
    firstRow.distribution = NSStackViewDistributionFillEqually;
    secondRow.distribution = NSStackViewDistributionFillEqually;
    thirdRow.distribution = NSStackViewDistributionFillEqually;
    addReadyButton(readyButton, actions);
    addSubview(window.contentView, readyButton);
    addAutolayoutEnabledSubview(window.contentView, responseButtons);
    const auto contentView{window.contentView};
    [NSLayoutConstraint activateConstraints:@[
        [responseButtons.topAnchor
            constraintEqualToAnchor:contentView.topAnchor],
        [responseButtons.bottomAnchor
            constraintEqualToAnchor:contentView.bottomAnchor],
        [responseButtons.leadingAnchor
            constraintEqualToAnchor:contentView.leadingAnchor],
        [responseButtons.trailingAnchor
            constraintEqualToAnchor:contentView.trailingAnchor],
        [firstRow.leadingAnchor
            constraintEqualToAnchor:responseButtons.leadingAnchor],
        [firstRow.trailingAnchor
            constraintEqualToAnchor:responseButtons.trailingAnchor],
        [secondRow.leadingAnchor
            constraintEqualToAnchor:responseButtons.leadingAnchor],
        [secondRow.trailingAnchor
            constraintEqualToAnchor:responseButtons.trailingAnchor],
        [thirdRow.leadingAnchor
            constraintEqualToAnchor:responseButtons.leadingAnchor],
        [thirdRow.trailingAnchor
            constraintEqualToAnchor:responseButtons.trailingAnchor]
    ]];
    for (NSView *view in firstRow.views) {
        [NSLayoutConstraint activateConstraints:@[
            [view.topAnchor constraintEqualToAnchor:firstRow.topAnchor],
            [view.bottomAnchor constraintEqualToAnchor:firstRow.bottomAnchor]
        ]];
    }
    for (NSView *view in secondRow.views) {
        [NSLayoutConstraint activateConstraints:@[
            [view.topAnchor constraintEqualToAnchor:secondRow.topAnchor],
            [view.bottomAnchor constraintEqualToAnchor:secondRow.bottomAnchor]
        ]];
    }
    for (NSView *view in thirdRow.views) {
        [NSLayoutConstraint activateConstraints:@[
            [view.topAnchor constraintEqualToAnchor:thirdRow.topAnchor],
            [view.bottomAnchor constraintEqualToAnchor:thirdRow.bottomAnchor]
        ]];
    }
    hideResponseButtons();
    hideReadyButton();
}

void AppKitConsonantView::show() { [window makeKeyAndOrderFront:nil]; }

void AppKitConsonantView::hide() { [window orderOut:nil]; }

void AppKitConsonantView::showReadyButton() {
    av_speech_in_noise::show(readyButton);
}

void AppKitConsonantView::hideReadyButton() {
    av_speech_in_noise::hide(readyButton);
}

void AppKitConsonantView::attach(Observer *e) { listener_ = e; }

void AppKitConsonantView::notifyThatReadyButtonHasBeenClicked() {
    listener_->notifyThatReadyButtonHasBeenClicked();
}

void AppKitConsonantView::notifyThatResponseButtonHasBeenClicked(id sender) {
    lastButtonPressed = sender;
    listener_->notifyThatResponseButtonHasBeenClicked();
}

void AppKitConsonantView::showResponseButtons() {
    av_speech_in_noise::show(responseButtons);
}

void AppKitConsonantView::hideResponseButtons() {
    av_speech_in_noise::hide(responseButtons);
}

auto AppKitConsonantView::consonant() -> std::string {
    return consonants.at((__bridge void *)lastButtonPressed);
}

void AppKitConsonantView::hideCursor() { [NSCursor hide]; }

void AppKitConsonantView::showCursor() { [NSCursor unhide]; }

static const auto greenColor{NSColor.greenColor};
static const auto redColor{NSColor.redColor};
static const auto blueColor{NSColor.blueColor};
static const auto whiteColor{NSColor.whiteColor};
constexpr std::array<int, 8> numbers{{1, 2, 3, 4, 5, 6, 8, 9}};
constexpr auto responseNumbers{std::size(numbers)};
constexpr auto responseColors{4};

AppKitCoordinateResponseMeasureView::AppKitCoordinateResponseMeasureView(
    NSRect r)
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

void AppKitCoordinateResponseMeasureView::addButtonRow(
    NSColor *color, int row) {
    for (std::size_t col{0}; col < responseNumbers; ++col)
        addNumberButton(color, numbers.at(col), row, col);
}

void AppKitCoordinateResponseMeasureView::addNumberButton(
    NSColor *color, int number, int row, std::size_t col) {
    auto title{nsString(std::to_string(number))};
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

void AppKitCoordinateResponseMeasureView::addNextTrialButton() {
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

auto AppKitCoordinateResponseMeasureView::numberResponse() -> std::string {
    return lastButtonPressed.title.UTF8String;
}

auto AppKitCoordinateResponseMeasureView::greenResponse() -> bool {
    return lastPressedColor() == greenColor;
}

auto AppKitCoordinateResponseMeasureView::lastPressedColor() -> NSColor * {
    return [lastButtonPressed.attributedTitle
             attribute:NSForegroundColorAttributeName
               atIndex:0
        effectiveRange:nil];
}

auto AppKitCoordinateResponseMeasureView::blueResponse() -> bool {
    return lastPressedColor() == blueColor;
}

auto AppKitCoordinateResponseMeasureView::whiteResponse() -> bool {
    return lastPressedColor() == whiteColor;
}

void AppKitCoordinateResponseMeasureView::
    notifyThatResponseButtonHasBeenClicked(id sender) {
    lastButtonPressed = sender;
    listener_->notifyThatResponseButtonHasBeenClicked();
}

void AppKitCoordinateResponseMeasureView::showResponseButtons() {
    av_speech_in_noise::show(responseButtons);
}

void AppKitCoordinateResponseMeasureView::showNextTrialButton() {
    av_speech_in_noise::show(nextTrialButton);
}

void AppKitCoordinateResponseMeasureView::hideNextTrialButton() {
    av_speech_in_noise::hide(nextTrialButton);
}

void AppKitCoordinateResponseMeasureView::
    notifyThatReadyButtonHasBeenClicked() {
    listener_->notifyThatReadyButtonHasBeenClicked();
}

void AppKitCoordinateResponseMeasureView::hideResponseButtons() {
    av_speech_in_noise::hide(responseButtons);
}

void AppKitCoordinateResponseMeasureView::attach(Observer *e) { listener_ = e; }

void AppKitCoordinateResponseMeasureView::show() {
    [window makeKeyAndOrderFront:nil];
}

void AppKitCoordinateResponseMeasureView::hide() { [window orderOut:nil]; }

AppKitExperimenterView::AppKitExperimenterView(NSViewController *viewController)
    : viewController{viewController}, continueTestingDialogField{label("")},
      primaryTextField{label("")}, secondaryTextField{label("")},
      freeResponseField{emptyTextField()},
      correctKeywordsField{emptyTextField()},
      freeResponseFlaggedButton{[NSButton checkboxWithTitle:@"flagged"
                                                     target:nil
                                                     action:nil]},
      actions{[[ExperimenterViewActions alloc] init]},
      freeResponseActions{[[FreeResponseViewActions alloc] init]} {
    const auto continueTestingDialogController{
        nsTabViewControllerWithoutTabControl()};
    continueTestingDialog = [NSWindow
        windowWithContentViewController:continueTestingDialogController];
    continueTestingDialog.styleMask = NSWindowStyleMaskBorderless;
    exitTestButton = button("Exit Test", actions, @selector(exitTest));
    nextTrialButton = button("Play Trial", actions, @selector(playTrial));
    const auto submitFreeResponseButton {
        button("Submit", freeResponseActions, @selector(submitFreeResponse))
    };
    evaluationButtons = [NSStackView stackViewWithViews:@[
        button("Incorrect", actions, @selector(submitFailedTrial)),
        button("Correct", actions, @selector(submitPassedTrial))
    ]];
    const auto submitCorrectKeywordsButton {
        button("Submit", actions, @selector(submitCorrectKeywords))
    };
    setPlaceholder(correctKeywordsField, "2");
    setPlaceholder(freeResponseField, "This is a sentence.");
    const auto topRow {
        [NSStackView stackViewWithViews:@[
            exitTestButton, primaryTextField, secondaryTextField
        ]]
    };
    correctKeywordsView = [NSStackView stackViewWithViews:@[
        correctKeywordsField, submitCorrectKeywordsButton
    ]];
    correctKeywordsView.orientation = NSUserInterfaceLayoutOrientationVertical;
    const auto innerFreeResponseView {
        [NSStackView stackViewWithViews:@[
            freeResponseFlaggedButton, freeResponseField
        ]]
    };
    freeResponseView = [NSStackView stackViewWithViews:@[
        innerFreeResponseView, submitFreeResponseButton
    ]];
    freeResponseView.orientation = NSUserInterfaceLayoutOrientationVertical;
    innerFreeResponseView.distribution = NSStackViewDistributionFill;
    [freeResponseFlaggedButton
        setContentHuggingPriority:251
                   forOrientation:NSLayoutConstraintOrientationHorizontal];
    [freeResponseField
        setContentHuggingPriority:48
                   forOrientation:NSLayoutConstraintOrientationHorizontal];
    [freeResponseField
        setContentCompressionResistancePriority:749
                                 forOrientation:
                                     NSLayoutConstraintOrientationHorizontal];
    addAutolayoutEnabledSubview(view(viewController), topRow);
    addAutolayoutEnabledSubview(view(viewController), evaluationButtons);
    const auto continueTestingDialogStack {
        [NSStackView stackViewWithViews:@[
            continueTestingDialogField, [NSStackView stackViewWithViews:@[
                button("Exit", actions, @selector(declineContinuingTesting)),
                button("Continue", actions, @selector(acceptContinuingTesting))
            ]]
        ]]
    };
    continueTestingDialogStack.orientation =
        NSUserInterfaceLayoutOrientationVertical;
    addAutolayoutEnabledSubview(
        view(continueTestingDialogController), continueTestingDialogStack);
    addAutolayoutEnabledSubview(view(viewController), nextTrialButton);
    addAutolayoutEnabledSubview(view(viewController), freeResponseView);
    addAutolayoutEnabledSubview(view(viewController), correctKeywordsView);
    activateConstraints(@[
        [topRow.leadingAnchor
            constraintEqualToAnchor:view(viewController).leadingAnchor
                           constant:defaultMarginPoints],
        [topRow.topAnchor constraintEqualToAnchor:view(viewController).topAnchor
                                         constant:defaultMarginPoints],
        [correctKeywordsField.leadingAnchor
            constraintEqualToAnchor:submitCorrectKeywordsButton.leadingAnchor],
        [correctKeywordsField.trailingAnchor
            constraintEqualToAnchor:submitCorrectKeywordsButton.trailingAnchor],
        [freeResponseView.leadingAnchor
            constraintEqualToAnchor:view(viewController).centerXAnchor],
        [innerFreeResponseView.leadingAnchor
            constraintEqualToAnchor:freeResponseView.leadingAnchor],
        [innerFreeResponseView.trailingAnchor
            constraintEqualToAnchor:freeResponseView.trailingAnchor]
    ]);
    activateChildConstraintNestledInBottomRightCorner(
        evaluationButtons, view(viewController), defaultMarginPoints);
    activateChildConstraintNestledInBottomRightCorner(
        correctKeywordsView, view(viewController), defaultMarginPoints);
    activateChildConstraintNestledInBottomRightCorner(
        nextTrialButton, view(viewController), defaultMarginPoints);
    activateChildConstraintNestledInBottomRightCorner(
        freeResponseView, view(viewController), defaultMarginPoints);
    av_speech_in_noise::hide(evaluationButtons);
    av_speech_in_noise::hide(nextTrialButton);
    av_speech_in_noise::hide(freeResponseView);
    av_speech_in_noise::hide(correctKeywordsView);
    av_speech_in_noise::hide(view(viewController));
    actions->controller = this;
    freeResponseActions->controller = this;
}

void AppKitExperimenterView::attach(TestControl::Observer *e) { listener_ = e; }

void AppKitExperimenterView::attach(FreeResponseControl::Observer *e) {
    freeResponseListener = e;
}

void AppKitExperimenterView::attach(CorrectKeywordsControl::Observer *e) {
    correctKeywordsListener = e;
}

void AppKitExperimenterView::attach(PassFailControl::Observer *e) {
    passFailListener = e;
}

void AppKitExperimenterView::showExitTestButton() {
    av_speech_in_noise::show(exitTestButton);
}

void AppKitExperimenterView::hideExitTestButton() {
    av_speech_in_noise::hide(exitTestButton);
}

void AppKitExperimenterView::show() {
    av_speech_in_noise::show(view(viewController));
}

void AppKitExperimenterView::hide() {
    av_speech_in_noise::hide(view(viewController));
}

void AppKitExperimenterView::exitTest() { listener_->exitTest(); }

void AppKitExperimenterView::display(std::string s) {
    set(primaryTextField, s);
}

void AppKitExperimenterView::secondaryDisplay(std::string s) {
    set(secondaryTextField, s);
}

void AppKitExperimenterView::showNextTrialButton() {
    av_speech_in_noise::show(nextTrialButton);
}

void AppKitExperimenterView::hideNextTrialButton() {
    av_speech_in_noise::hide(nextTrialButton);
}

void AppKitExperimenterView::showEvaluationButtons() {
    av_speech_in_noise::show(evaluationButtons);
}

void AppKitExperimenterView::showFreeResponseSubmission() {
    av_speech_in_noise::show(freeResponseView);
}

void AppKitExperimenterView::hideFreeResponseSubmission() {
    av_speech_in_noise::hide(freeResponseView);
}

void AppKitExperimenterView::hideEvaluationButtons() {
    av_speech_in_noise::hide(evaluationButtons);
}

void AppKitExperimenterView::showCorrectKeywordsSubmission() {
    av_speech_in_noise::show(correctKeywordsView);
}

void AppKitExperimenterView::hideCorrectKeywordsSubmission() {
    av_speech_in_noise::hide(correctKeywordsView);
}

void AppKitExperimenterView::showContinueTestingDialog() {
    [view(viewController).window beginSheet:continueTestingDialog
                          completionHandler:^(NSModalResponse){
                          }];
}

void AppKitExperimenterView::hideContinueTestingDialog() {
    [view(viewController).window endSheet:continueTestingDialog];
}

void AppKitExperimenterView::setContinueTestingDialogMessage(
    const std::string &s) {
    set(continueTestingDialogField, s);
}

void AppKitExperimenterView::clearFreeResponse() { set(freeResponseField, ""); }

auto AppKitExperimenterView::freeResponse() -> std::string {
    return string(freeResponseField);
}

auto AppKitExperimenterView::correctKeywords() -> std::string {
    return string(correctKeywordsField);
}

auto AppKitExperimenterView::flagged() -> bool {
    return freeResponseFlaggedButton.state == NSControlStateValueOn;
}

void AppKitExperimenterView::playTrial() { listener_->playTrial(); }

void AppKitExperimenterView::submitFreeResponse() {
    freeResponseListener->notifyThatSubmitButtonHasBeenClicked();
}

void AppKitExperimenterView::submitPassedTrial() {
    passFailListener->notifyThatCorrectButtonHasBeenClicked();
}

void AppKitExperimenterView::submitFailedTrial() {
    passFailListener->notifyThatIncorrectButtonHasBeenClicked();
}

void AppKitExperimenterView::submitCorrectKeywords() {
    correctKeywordsListener->notifyThatSubmitButtonHasBeenClicked();
}

void AppKitExperimenterView::acceptContinuingTesting() {
    listener_->acceptContinuingTesting();
}

void AppKitExperimenterView::declineContinuingTesting() {
    listener_->declineContinuingTesting();
}

AppKitView::AppKitView(NSApplication *app, NSViewController *viewController)
    : app{app}, audioDeviceMenu{
                    [[NSPopUpButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)
                                               pullsDown:NO]} {
    const auto audioDeviceStack {
        [NSStackView
            stackViewWithViews:@[ label("audio output:"), audioDeviceMenu ]]
    };
    addAutolayoutEnabledSubview(view(viewController), audioDeviceStack);
    activateConstraints(@[
        [audioDeviceStack.topAnchor
            constraintEqualToAnchor:view(viewController).topAnchor
                           constant:defaultMarginPoints],
        [audioDeviceStack.bottomAnchor
            constraintEqualToAnchor:view(viewController).bottomAnchor
                           constant:-defaultMarginPoints],
        [audioDeviceStack.leadingAnchor
            constraintEqualToAnchor:view(viewController).leadingAnchor
                           constant:defaultMarginPoints],
        [audioDeviceStack.trailingAnchor
            constraintEqualToAnchor:view(viewController).trailingAnchor
                           constant:-defaultMarginPoints]
    ]);
}

void AppKitView::eventLoop() { [app run]; }

void AppKitView::showErrorMessage(std::string s) {
    const auto alert{[[NSAlert alloc] init]};
    [alert setMessageText:@"Error."];
    [alert setInformativeText:nsString(s)];
    [alert addButtonWithTitle:@"Ok"];
    [alert runModal];
}

auto AppKitView::browseForDirectory() -> std::string {
    const auto panel{[NSOpenPanel openPanel]};
    panel.canChooseDirectories = YES;
    panel.canChooseFiles = NO;
    return browseModal(panel);
}

auto AppKitView::browseCancelled() -> bool { return browseCancelled_; }

auto AppKitView::browseForOpeningFile() -> std::string {
    const auto panel{[NSOpenPanel openPanel]};
    panel.canChooseDirectories = NO;
    panel.canChooseFiles = YES;
    return browseModal(panel);
}

auto AppKitView::browseModal(NSOpenPanel *panel) -> std::string {
    switch ([panel runModal]) {
    case NSModalResponseOK:
        browseCancelled_ = false;
        return panel.URLs.lastObject.path.UTF8String;
    default:
        browseCancelled_ = true;
        return {};
    }
}

auto AppKitView::audioDevice() -> std::string {
    return audioDeviceMenu.titleOfSelectedItem.UTF8String;
}

void AppKitView::populateAudioDeviceMenu(std::vector<std::string> items) {
    for (const auto &item : items)
        [audioDeviceMenu addItemWithTitle:nsString(item)];
}
}
