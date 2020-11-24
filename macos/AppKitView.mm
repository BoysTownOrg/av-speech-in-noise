#include "AppKitView.h"
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
    av_speech_in_noise::AppKitTestSetupUI *controller;
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
    av_speech_in_noise::AppKitCoordinateResponseMeasureUI *controller;
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
    av_speech_in_noise::AppKitConsonantUI *controller;
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
    av_speech_in_noise::AppKitTestUI *controller;
}

- (void)notifyThatExitTestButtonHasBeenClicked {
    controller->notifyThatExitTestButtonHasBeenClicked();
}

- (void)notifyThatPlayTrialButtonHasBeenClicked {
    controller->notifyThatPlayTrialButtonHasBeenClicked();
}

- (void)notifyThatCorrectButtonHasBeenClicked {
    controller->notifyThatCorrectButtonHasBeenClicked();
}

- (void)notifyThatIncorrectButtonHasBeenClicked {
    controller->notifyThatIncorrectButtonHasBeenClicked();
}

- (void)notifyThatSubmitCorrectKeywordsButtonHasBeenClicked {
    controller->notifyThatSubmitCorrectKeywordsButtonHasBeenClicked();
}

- (void)notifyThatContinueTestingButtonHasBeenClicked {
    controller->notifyThatContinueTestingButtonHasBeenClicked();
}

- (void)notifyThatDeclineContinueTestingButtonHasBeenClicked {
    controller->notifyThatDeclineContinueTestingButtonHasBeenClicked();
}
@end

@implementation FreeResponseViewActions {
  @public
    av_speech_in_noise::AppKitTestUI *controller;
}

- (void)notifyThatSubmitFreeResponseButtonHasBeenClicked {
    controller->notifyThatSubmitFreeResponseButtonHasBeenClicked();
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

static auto string(NSTextField *field) -> const char * {
    return field.stringValue.UTF8String;
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

AppKitTestSetupUI::AppKitTestSetupUI(NSViewController *viewController)
    : viewController{viewController}, subjectIdField{emptyTextField()},
      testerIdField{emptyTextField()}, sessionField{emptyTextField()},
      rmeSettingField{emptyTextField()},
      transducerMenu{[[NSPopUpButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)
                                                pullsDown:NO]},
      testSettingsField{emptyTextField()}, startingSnrField{emptyTextField()},
      actions{[[SetupViewActions alloc] init]} {
    actions->controller = this;
    const auto confirmButton {
        nsButton("Confirm", actions,
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
                nsButton("Browse...", actions,
                    @selector
                    (notifyThatBrowseForTestSettingsButtonHasBeenClicked)),
                nsButton("Play Calibration", actions,
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

void AppKitTestSetupUI::show() {
    av_speech_in_noise::show(view(viewController));
}

void AppKitTestSetupUI::hide() {
    av_speech_in_noise::hide(view(viewController));
}

auto AppKitTestSetupUI::testSettingsFile() -> std::string {
    return string(testSettingsField);
}

auto AppKitTestSetupUI::testerId() -> std::string {
    return string(testerIdField);
}

auto AppKitTestSetupUI::startingSnr() -> std::string {
    return string(startingSnrField);
}

auto AppKitTestSetupUI::subjectId() -> std::string {
    return string(subjectIdField);
}

auto AppKitTestSetupUI::session() -> std::string {
    return string(sessionField);
}

auto AppKitTestSetupUI::rmeSetting() -> std::string {
    return string(rmeSettingField);
}

void AppKitTestSetupUI::setTestSettingsFile(std::string s) {
    set(testSettingsField, s);
}

void AppKitTestSetupUI::populateTransducerMenu(std::vector<std::string> items) {
    for (const auto &item : items)
        [transducerMenu addItemWithTitle:nsString(item)];
}

auto AppKitTestSetupUI::transducer() -> std::string {
    return transducerMenu.titleOfSelectedItem.UTF8String;
}

void AppKitTestSetupUI::attach(Observer *e) { observer = e; }

void AppKitTestSetupUI::notifyThatConfirmButtonHasBeenClicked() {
    observer->notifyThatConfirmButtonHasBeenClicked();
}

void AppKitTestSetupUI::notifyThatBrowseForTestSettingsButtonHasBeenClicked() {
    observer->notifyThatBrowseForTestSettingsButtonHasBeenClicked();
}

void AppKitTestSetupUI::notifyThatPlayCalibrationButtonHasBeenClicked() {
    observer->notifyThatPlayCalibrationButtonHasBeenClicked();
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
    const auto button {
        nsButton("", actions, @selector(notifyThatReadyButtonHasBeenClicked))
    };
    [button setBezelStyle:NSBezelStyleTexturedSquare];
    const auto style{[[NSMutableParagraphStyle alloc] init]};
    [style setAlignment:NSTextAlignmentCenter];
    const auto font{[NSFont fontWithName:@"Courier" size:36]};
    [button
        setAttributedTitle:
            [[NSAttributedString alloc]
                initWithString:@"Press when ready"
                    attributes:[NSDictionary dictionaryWithObjectsAndKeys:font,
                                             NSFontAttributeName, nil]]];
    [button
        setFrame:NSMakeRect(0, 0, width(parent.frame), height(parent.frame))];
    addSubview(parent, button);
}

static auto equallyDistributedConsonantImageButtonRow(
    std::unordered_map<void *, std::string> &consonants,
    ConsonantViewActions *actions, const std::vector<std::string> &each)
    -> NSStackView * {
    std::vector<NSButton *> buttons;
    buttons.reserve(each.size());
    for (const auto &x : each)
        buttons.push_back(consonantImageButton(consonants, actions, x));
    auto row{[NSStackView
        stackViewWithViews:[NSArray arrayWithObjects:&buttons.front()
                                               count:buttons.size()]]};
    row.distribution = NSStackViewDistributionFillEqually;
    return row;
}

AppKitConsonantUI::AppKitConsonantUI(NSRect r)
    : // Defer may be critical here...
      window{[[NSWindow alloc] initWithContentRect:r
                                         styleMask:NSWindowStyleMaskBorderless
                                           backing:NSBackingStoreBuffered
                                             defer:YES]},
      readyButton{
          [[NSView alloc] initWithFrame:NSMakeRect(0, 0, width(r), height(r))]},
      actions{[[ConsonantViewActions alloc] init]} {
    actions->controller = this;
    const auto firstRow{equallyDistributedConsonantImageButtonRow(
        consonants, actions, {"b", "c", "d", "h"})};
    const auto secondRow{equallyDistributedConsonantImageButtonRow(
        consonants, actions, {"k", "m", "n", "p"})};
    const auto thirdRow{equallyDistributedConsonantImageButtonRow(
        consonants, actions, {"s", "t", "v", "z"})};
    responseButtons =
        [NSStackView stackViewWithViews:@[ firstRow, secondRow, thirdRow ]];
    responseButtons.orientation = NSUserInterfaceLayoutOrientationVertical;
    responseButtons.distribution = NSStackViewDistributionFillEqually;
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

void AppKitConsonantUI::show() { [window makeKeyAndOrderFront:nil]; }

void AppKitConsonantUI::hide() { [window orderOut:nil]; }

void AppKitConsonantUI::showReadyButton() {
    av_speech_in_noise::show(readyButton);
}

void AppKitConsonantUI::hideReadyButton() {
    av_speech_in_noise::hide(readyButton);
}

void AppKitConsonantUI::attach(Observer *e) { listener_ = e; }

void AppKitConsonantUI::notifyThatReadyButtonHasBeenClicked() {
    listener_->notifyThatReadyButtonHasBeenClicked();
}

void AppKitConsonantUI::notifyThatResponseButtonHasBeenClicked(id sender) {
    lastButtonPressed = sender;
    listener_->notifyThatResponseButtonHasBeenClicked();
}

void AppKitConsonantUI::showResponseButtons() {
    av_speech_in_noise::show(responseButtons);
}

void AppKitConsonantUI::hideResponseButtons() {
    av_speech_in_noise::hide(responseButtons);
}

auto AppKitConsonantUI::consonant() -> std::string {
    return consonants.at((__bridge void *)lastButtonPressed);
}

void AppKitConsonantUI::hideCursor() { [NSCursor hide]; }

void AppKitConsonantUI::showCursor() { [NSCursor unhide]; }

static const auto greenColor{NSColor.greenColor};
static const auto redColor{NSColor.redColor};
static const auto blueColor{NSColor.blueColor};
static const auto whiteColor{NSColor.whiteColor};
constexpr std::array<int, 8> numbers{{1, 2, 3, 4, 5, 6, 8, 9}};
constexpr auto responseNumbers{std::size(numbers)};
constexpr auto responseColors{4};

AppKitCoordinateResponseMeasureUI::AppKitCoordinateResponseMeasureUI(NSRect r)
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

void AppKitCoordinateResponseMeasureUI::addButtonRow(NSColor *color, int row) {
    for (std::size_t col{0}; col < responseNumbers; ++col)
        addNumberButton(color, numbers.at(col), row, col);
}

void AppKitCoordinateResponseMeasureUI::addNumberButton(
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

void AppKitCoordinateResponseMeasureUI::addNextTrialButton() {
    const auto button_ {
        nsButton("", actions, @selector(notifyThatReadyButtonHasBeenClicked))
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

auto AppKitCoordinateResponseMeasureUI::numberResponse() -> std::string {
    return lastButtonPressed.title.UTF8String;
}

auto AppKitCoordinateResponseMeasureUI::greenResponse() -> bool {
    return lastPressedColor() == greenColor;
}

auto AppKitCoordinateResponseMeasureUI::lastPressedColor() -> NSColor * {
    return [lastButtonPressed.attributedTitle
             attribute:NSForegroundColorAttributeName
               atIndex:0
        effectiveRange:nil];
}

auto AppKitCoordinateResponseMeasureUI::blueResponse() -> bool {
    return lastPressedColor() == blueColor;
}

auto AppKitCoordinateResponseMeasureUI::whiteResponse() -> bool {
    return lastPressedColor() == whiteColor;
}

void AppKitCoordinateResponseMeasureUI::notifyThatResponseButtonHasBeenClicked(
    id sender) {
    lastButtonPressed = sender;
    listener_->notifyThatResponseButtonHasBeenClicked();
}

void AppKitCoordinateResponseMeasureUI::showResponseButtons() {
    av_speech_in_noise::show(responseButtons);
}

void AppKitCoordinateResponseMeasureUI::showNextTrialButton() {
    av_speech_in_noise::show(nextTrialButton);
}

void AppKitCoordinateResponseMeasureUI::hideNextTrialButton() {
    av_speech_in_noise::hide(nextTrialButton);
}

void AppKitCoordinateResponseMeasureUI::notifyThatReadyButtonHasBeenClicked() {
    listener_->notifyThatReadyButtonHasBeenClicked();
}

void AppKitCoordinateResponseMeasureUI::hideResponseButtons() {
    av_speech_in_noise::hide(responseButtons);
}

void AppKitCoordinateResponseMeasureUI::attach(Observer *e) { listener_ = e; }

void AppKitCoordinateResponseMeasureUI::show() {
    [window makeKeyAndOrderFront:nil];
}

void AppKitCoordinateResponseMeasureUI::hide() { [window orderOut:nil]; }

AppKitTestUI::AppKitTestUI(NSViewController *viewController)
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
    exitTestButton = nsButton("Exit Test", actions,
        @selector(notifyThatExitTestButtonHasBeenClicked));
    nextTrialButton = nsButton("Play Trial", actions,
        @selector(notifyThatPlayTrialButtonHasBeenClicked));
    const auto submitFreeResponseButton {
        nsButton("Submit", freeResponseActions,
            @selector(notifyThatSubmitFreeResponseButtonHasBeenClicked))
    };
    evaluationButtons = [NSStackView stackViewWithViews:@[
        nsButton("Incorrect", actions,
            @selector(notifyThatIncorrectButtonHasBeenClicked)),
        nsButton("Correct", actions,
            @selector(notifyThatCorrectButtonHasBeenClicked))
    ]];
    const auto submitCorrectKeywordsButton {
        nsButton("Submit", actions,
            @selector(notifyThatSubmitCorrectKeywordsButtonHasBeenClicked))
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
                nsButton("Exit", actions,
                    @selector
                    (notifyThatDeclineContinueTestingButtonHasBeenClicked)),
                nsButton("Continue", actions,
                    @selector(notifyThatContinueTestingButtonHasBeenClicked))
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

void AppKitTestUI::attach(TestControl::Observer *e) { listener_ = e; }

void AppKitTestUI::attach(FreeResponseControl::Observer *e) {
    freeResponseListener = e;
}

void AppKitTestUI::attach(CorrectKeywordsControl::Observer *e) {
    correctKeywordsListener = e;
}

void AppKitTestUI::attach(PassFailControl::Observer *e) {
    passFailListener = e;
}

void AppKitTestUI::showExitTestButton() {
    av_speech_in_noise::show(exitTestButton);
}

void AppKitTestUI::hideExitTestButton() {
    av_speech_in_noise::hide(exitTestButton);
}

void AppKitTestUI::show() { av_speech_in_noise::show(view(viewController)); }

void AppKitTestUI::hide() { av_speech_in_noise::hide(view(viewController)); }

void AppKitTestUI::notifyThatExitTestButtonHasBeenClicked() {
    listener_->exitTest();
}

void AppKitTestUI::display(std::string s) { set(primaryTextField, s); }

void AppKitTestUI::secondaryDisplay(std::string s) {
    set(secondaryTextField, s);
}

void AppKitTestUI::showNextTrialButton() {
    av_speech_in_noise::show(nextTrialButton);
}

void AppKitTestUI::hideNextTrialButton() {
    av_speech_in_noise::hide(nextTrialButton);
}

void AppKitTestUI::showEvaluationButtons() {
    av_speech_in_noise::show(evaluationButtons);
}

void AppKitTestUI::showFreeResponseSubmission() {
    av_speech_in_noise::show(freeResponseView);
}

void AppKitTestUI::hideFreeResponseSubmission() {
    av_speech_in_noise::hide(freeResponseView);
}

void AppKitTestUI::hideEvaluationButtons() {
    av_speech_in_noise::hide(evaluationButtons);
}

void AppKitTestUI::showCorrectKeywordsSubmission() {
    av_speech_in_noise::show(correctKeywordsView);
}

void AppKitTestUI::hideCorrectKeywordsSubmission() {
    av_speech_in_noise::hide(correctKeywordsView);
}

void AppKitTestUI::showContinueTestingDialog() {
    [view(viewController).window beginSheet:continueTestingDialog
                          completionHandler:^(NSModalResponse){
                          }];
}

void AppKitTestUI::hideContinueTestingDialog() {
    [view(viewController).window endSheet:continueTestingDialog];
}

void AppKitTestUI::setContinueTestingDialogMessage(const std::string &s) {
    set(continueTestingDialogField, s);
}

void AppKitTestUI::clearFreeResponse() { set(freeResponseField, ""); }

auto AppKitTestUI::freeResponse() -> std::string {
    return string(freeResponseField);
}

auto AppKitTestUI::correctKeywords() -> std::string {
    return string(correctKeywordsField);
}

auto AppKitTestUI::flagged() -> bool {
    return freeResponseFlaggedButton.state == NSControlStateValueOn;
}

void AppKitTestUI::notifyThatPlayTrialButtonHasBeenClicked() {
    listener_->playTrial();
}

void AppKitTestUI::notifyThatSubmitFreeResponseButtonHasBeenClicked() {
    freeResponseListener->notifyThatSubmitButtonHasBeenClicked();
}

void AppKitTestUI::notifyThatCorrectButtonHasBeenClicked() {
    passFailListener->notifyThatCorrectButtonHasBeenClicked();
}

void AppKitTestUI::notifyThatIncorrectButtonHasBeenClicked() {
    passFailListener->notifyThatIncorrectButtonHasBeenClicked();
}

void AppKitTestUI::notifyThatSubmitCorrectKeywordsButtonHasBeenClicked() {
    correctKeywordsListener->notifyThatSubmitButtonHasBeenClicked();
}

void AppKitTestUI::notifyThatContinueTestingButtonHasBeenClicked() {
    listener_->acceptContinuingTesting();
}

void AppKitTestUI::notifyThatDeclineContinueTestingButtonHasBeenClicked() {
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
