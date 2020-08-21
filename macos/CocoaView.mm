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
constexpr auto defaultMarginPoints{8};

static void addSubview(NSView *parent, NSView *child) {
    [parent addSubview:child];
}

static void hide(NSView *v) { [v setHidden:YES]; }

static void show(NSView *v) { [v setHidden:NO]; }

static void set(NSTextField *field, const std::string &s) {
    [field setStringValue:nsString(s)];
}

static constexpr auto labelHeight{22};
static constexpr auto buttonHeight{25};

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

static auto widthConstraint(NSView *a) -> NSLayoutConstraint * {
    return [a.widthAnchor constraintEqualToConstant:NSWidth(a.frame)];
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

static void setPlaceholderAndFit(NSTextField *field, const std::string &s) {
    setPlaceholder(field, s);
    [field sizeToFit];
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

CocoaTestSetupView::CocoaTestSetupView(NSViewController *viewController)
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

void CocoaTestSetupView::show() {
    av_speech_in_noise::show(view(viewController));
}

void CocoaTestSetupView::hide() {
    av_speech_in_noise::hide(view(viewController));
}

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

auto CocoaTestSetupView::rmeSetting() -> std::string {
    return string(rmeSettingField);
}

void CocoaTestSetupView::setTestSettingsFile(std::string s) {
    set(testSettingsField, s);
}

void CocoaTestSetupView::populateTransducerMenu(
    std::vector<std::string> items) {
    for (const auto &item : items)
        [transducerMenu addItemWithTitle:nsString(item)];
}

auto CocoaTestSetupView::transducer() -> std::string {
    return transducerMenu.titleOfSelectedItem.UTF8String;
}

void CocoaTestSetupView::subscribe(EventListener *listener) {
    listener_ = listener;
}

void CocoaTestSetupView::notifyThatConfirmButtonHasBeenClicked() {
    listener_->notifyThatConfirmButtonHasBeenClicked();
}

void CocoaTestSetupView::notifyThatBrowseForTestSettingsButtonHasBeenClicked() {
    listener_->notifyThatBrowseForTestSettingsButtonHasBeenClicked();
}

void CocoaTestSetupView::notifyThatPlayCalibrationButtonHasBeenClicked() {
    listener_->notifyThatPlayCalibrationButtonHasBeenClicked();
}

static auto resourcePath(const std::string &stem, const std::string &extension)
    -> std::string {
    return [[NSBundle mainBundle] pathForResource:nsString(stem)
                                           ofType:nsString(extension)]
        .UTF8String;
}

static auto consonantImageButton(
    std::unordered_map<id, std::string> &consonants,
    ConsonantViewActions *actions, const std::string &consonant) -> NSButton * {
    const auto image{[[NSImage alloc]
        initWithContentsOfFile:nsString(resourcePath(consonant, "bmp"))]};
    const auto button {
        [NSButton
            buttonWithImage:image
                     target:actions
                     action:@selector(notifyThatResponseButtonHasBeenClicked:)]
    };
    consonants[button] = consonant;
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

CocoaConsonantView::CocoaConsonantView(NSRect r)
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

void CocoaConsonantView::hideCursor() { [NSCursor hide]; }

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

constexpr auto continueTestingDialogHeight{2 * labelHeight};

static auto trailingAnchorConstraint(NSView *a, NSView *b)
    -> NSLayoutConstraint * {
    return [a.trailingAnchor constraintEqualToAnchor:b.trailingAnchor];
}

static auto nsTabViewControllerWithoutTabControl() -> NSTabViewController * {
    const auto controller{[[NSTabViewController alloc] init]};
    [controller setTabStyle:NSTabViewControllerTabStyleUnspecified];
    return controller;
}

CocoaExperimenterView::CocoaExperimenterView(
    NSRect r, NSViewController *viewController)
    : viewController{viewController}, continueTestingDialogField{label("")},
      primaryTextField{label("")}, secondaryTextField{label("")},
      freeResponseField{emptyTextField()},
      correctKeywordsField{emptyTextField()}, freeResponseFlaggedButton{[
                                                  [NSButton alloc] init]},
      actions{[[ExperimenterViewActions alloc] init]} {
    const auto continueTestingDialogController{
        nsTabViewControllerWithoutTabControl()};
    continueTestingDialog = [NSWindow
        windowWithContentViewController:continueTestingDialogController];
    continueTestingDialog.styleMask = NSWindowStyleMaskBorderless;
    exitTestButton = button("Exit Test", actions, @selector(exitTest));
    [freeResponseFlaggedButton setButtonType:NSButtonTypeSwitch];
    [freeResponseFlaggedButton setTitle:@"flagged"];
    [freeResponseFlaggedButton sizeToFit];
    nextTrialButton = button("Play Trial", actions, @selector(playTrial));
    const auto submitFreeResponseButton {
        button("Submit", actions, @selector(submitFreeResponse))
    };
    evaluationButtons = [NSStackView stackViewWithViews:@[
        button("Incorrect", actions, @selector(submitFailedTrial)),
        button("Correct", actions, @selector(submitPassedTrial))
    ]];
    const auto submitCorrectKeywordsButton {
        button("Submit", actions, @selector(submitCorrectKeywords))
    };
    setPlaceholder(correctKeywordsField, "2");
    setPlaceholderAndFit(freeResponseField, "This is a sentence.");
    const auto topRow {
        [NSStackView stackViewWithViews:@[
            exitTestButton, primaryTextField, secondaryTextField
        ]]
    };
    correctKeywordsView = [NSStackView stackViewWithViews:@[
        correctKeywordsField, submitCorrectKeywordsButton
    ]];
    correctKeywordsView.orientation = NSUserInterfaceLayoutOrientationVertical;
    freeResponseView = [NSStackView stackViewWithViews:@[
        [NSStackView stackViewWithViews:@[
            freeResponseFlaggedButton, freeResponseField
        ]],
        submitFreeResponseButton
    ]];
    freeResponseView.orientation = NSUserInterfaceLayoutOrientationVertical;
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
        trailingAnchorConstraint(freeResponseField, submitFreeResponseButton),
        [correctKeywordsField.leadingAnchor
            constraintEqualToAnchor:submitCorrectKeywordsButton.leadingAnchor],
        [correctKeywordsField.trailingAnchor
            constraintEqualToAnchor:submitCorrectKeywordsButton.trailingAnchor],
        widthConstraint(freeResponseField),
        widthConstraint(freeResponseFlaggedButton)
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
}

void CocoaExperimenterView::subscribe(EventListener *e) { listener_ = e; }

void CocoaExperimenterView::showExitTestButton() {
    av_speech_in_noise::show(exitTestButton);
}

void CocoaExperimenterView::hideExitTestButton() {
    av_speech_in_noise::hide(exitTestButton);
}

void CocoaExperimenterView::show() {
    av_speech_in_noise::show(view(viewController));
}

void CocoaExperimenterView::hide() {
    av_speech_in_noise::hide(view(viewController));
}

void CocoaExperimenterView::exitTest() { listener_->exitTest(); }

void CocoaExperimenterView::display(std::string s) { set(primaryTextField, s); }

void CocoaExperimenterView::secondaryDisplay(std::string s) {
    set(secondaryTextField, s);
}

void CocoaExperimenterView::showNextTrialButton() {
    av_speech_in_noise::show(nextTrialButton);
}

void CocoaExperimenterView::hideNextTrialButton() {
    av_speech_in_noise::hide(nextTrialButton);
}

void CocoaExperimenterView::showEvaluationButtons() {
    av_speech_in_noise::show(evaluationButtons);
}

void CocoaExperimenterView::showFreeResponseSubmission() {
    av_speech_in_noise::show(freeResponseView);
}

void CocoaExperimenterView::hideFreeResponseSubmission() {
    av_speech_in_noise::hide(freeResponseView);
}

void CocoaExperimenterView::hideEvaluationButtons() {
    av_speech_in_noise::hide(evaluationButtons);
}

void CocoaExperimenterView::showCorrectKeywordsSubmission() {
    av_speech_in_noise::show(correctKeywordsView);
}

void CocoaExperimenterView::hideCorrectKeywordsSubmission() {
    av_speech_in_noise::hide(correctKeywordsView);
}

void CocoaExperimenterView::showContinueTestingDialog() {
    [view(viewController).window beginSheet:continueTestingDialog
                          completionHandler:^(NSModalResponse){
                          }];
}

void CocoaExperimenterView::hideContinueTestingDialog() {
    [view(viewController).window endSheet:continueTestingDialog];
}

void CocoaExperimenterView::setContinueTestingDialogMessage(
    const std::string &s) {
    set(continueTestingDialogField, s);
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

CocoaView::CocoaView(NSApplication *app, NSViewController *viewController)
    : app{app}, audioDeviceMenu{
                    [[NSPopUpButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)
                                               pullsDown:NO]} {
    const auto audioDeviceStack {
        [NSStackView
            stackViewWithViews:@[ label("audio output:"), audioDeviceMenu ]]
    };
    addAutolayoutEnabledSubview(view(viewController), audioDeviceStack);
    activateConstraints(@[
        [audioDeviceStack.bottomAnchor
            constraintEqualToAnchor:view(viewController).bottomAnchor
                           constant:-defaultMarginPoints],
        [audioDeviceStack.leadingAnchor
            constraintEqualToAnchor:view(viewController).leadingAnchor
                           constant:defaultMarginPoints]
    ]);
}

void CocoaView::eventLoop() { [app run]; }

void CocoaView::showErrorMessage(std::string s) {
    const auto alert{[[NSAlert alloc] init]};
    [alert setMessageText:@"Error."];
    [alert setInformativeText:nsString(s)];
    [alert addButtonWithTitle:@"Ok"];
    [alert runModal];
}

auto CocoaView::browseForDirectory() -> std::string {
    const auto panel{[NSOpenPanel openPanel]};
    panel.canChooseDirectories = YES;
    panel.canChooseFiles = NO;
    return browseModal(panel);
}

auto CocoaView::browseCancelled() -> bool { return browseCancelled_; }

auto CocoaView::browseForOpeningFile() -> std::string {
    const auto panel{[NSOpenPanel openPanel]};
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
        [audioDeviceMenu addItemWithTitle:nsString(item)];
}

void CocoaView::showCursor() { [NSCursor unhide]; }
}
