#include "AppKitView.h"
#include "AppKit-utility.h"
#include "Foundation-utility.h"
#include <gsl/gsl>
#include <iterator>
#include <array>
#include <algorithm>
#include <functional>

@interface TestSetupUIActions : NSObject
@end

@interface CoordinateResponseMeasureUIActions : NSObject
@end

@interface ConsonantUIActions : NSObject
@end

@interface TestUIActions : NSObject
@end

@interface FreeResponseUIActions : NSObject
@end

@interface ChooseKeywordsUIActions : NSObject
@end

@interface CorrectKeywordsUIActions : NSObject
@end

@interface PassFailUIActions : NSObject
@end

@interface SyllablesUIActions : NSObject
@end

@implementation TestSetupUIActions {
  @public
    av_speech_in_noise::AppKitTestSetupUI *controller;
}

- (void)notifyThatConfirmButtonHasBeenClicked {
    controller->notifyThatConfirmButtonHasBeenClicked();
}

- (void)notifyThatPlayCalibrationButtonHasBeenClicked {
    controller->notifyThatPlayCalibrationButtonHasBeenClicked();
}
@end

@implementation CoordinateResponseMeasureUIActions {
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

@implementation ConsonantUIActions {
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

@implementation TestUIActions {
  @public
    av_speech_in_noise::TestControl::Observer *observer;
    std::function<void()> onSheetsOkButtonClicked;
}

- (void)notifyThatExitTestButtonHasBeenClicked {
    observer->exitTest();
}

- (void)notifyThatPlayTrialButtonHasBeenClicked {
    observer->playTrial();
}

- (void)notifyThatContinueTestingButtonHasBeenClicked {
    observer->acceptContinuingTesting();
}

- (void)notifyThatDeclineContinueTestingButtonHasBeenClicked {
    observer->declineContinuingTesting();
}

- (void)notifyThatSheetsOkButtonHasBeenClicked {
    onSheetsOkButtonClicked();
}
@end

@implementation PassFailUIActions {
  @public
    av_speech_in_noise::PassFailControl::Observer *observer;
}

- (void)notifyThatCorrectButtonHasBeenClicked {
    observer->notifyThatCorrectButtonHasBeenClicked();
}

- (void)notifyThatIncorrectButtonHasBeenClicked {
    observer->notifyThatIncorrectButtonHasBeenClicked();
}
@end

@implementation SyllablesUIActions {
  @public
    av_speech_in_noise::SyllablesControl::Observer *observer;
    std::function<void(id)> onResponseButtonClick;
}

- (void)notifyThatResponseButtonHasBeenClicked:(id)sender {
    onResponseButtonClick(sender);
    observer->notifyThatResponseButtonHasBeenClicked();
}
@end

@implementation CorrectKeywordsUIActions {
  @public
    av_speech_in_noise::CorrectKeywordsControl::Observer *observer;
}

- (void)notifyThatSubmitButtonHasBeenClicked {
    observer->notifyThatSubmitButtonHasBeenClicked();
}
@end

@implementation FreeResponseUIActions {
  @public
    av_speech_in_noise::FreeResponseControl::Observer *observer;
}

- (void)notifyThatSubmitButtonHasBeenClicked {
    observer->notifyThatSubmitButtonHasBeenClicked();
}
@end

@implementation ChooseKeywordsUIActions {
  @public
    av_speech_in_noise::ChooseKeywordsControl::Observer *observer;
}

- (void)notifyThatFirstKeywordButtonHasBeenClicked {
    observer->notifyThatFirstKeywordButtonIsClicked();
}

- (void)notifyThatSecondKeywordButtonHasBeenClicked {
    observer->notifyThatSecondKeywordButtonIsClicked();
}

- (void)notifyThatThirdKeywordButtonHasBeenClicked {
    observer->notifyThatThirdKeywordButtonIsClicked();
}

- (void)notifyThatResetButtonHasBeenClicked {
    observer->notifyThatResetButtonIsClicked();
}

- (void)notifyThatAllWrongButtonHasBeenClicked {
    observer->notifyThatAllWrongButtonHasBeenClicked();
}

- (void)notifyThatSubmitButtonHasBeenClicked {
    observer->notifyThatSubmitButtonHasBeenClicked();
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

AppKitSessionUI::AppKitSessionUI(
    NSApplication *app, NSViewController *viewController)
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

void AppKitSessionUI::eventLoop() { [app run]; }

void AppKitSessionUI::showErrorMessage(std::string_view s) {
    const auto alert{[[NSAlert alloc] init]};
    [alert setMessageText:@"Error."];
    [alert setInformativeText:nsString(std::string{s})];
    [alert addButtonWithTitle:@"Ok"];
    [alert runModal];
}

auto AppKitSessionUI::audioDevice() -> std::string {
    return audioDeviceMenu.titleOfSelectedItem.UTF8String;
}

void AppKitSessionUI::populateAudioDeviceMenu(std::vector<std::string> items) {
    for (const auto &item : items)
        [audioDeviceMenu addItemWithTitle:nsString(item)];
}

AppKitTestSetupUI::AppKitTestSetupUI(NSViewController *viewController)
    : viewController{viewController}, subjectIdField{emptyTextField()},
      testerIdField{emptyTextField()}, sessionField{emptyTextField()},
      rmeSettingField{emptyTextField()},
      transducerMenu{[[NSPopUpButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)
                                                pullsDown:NO]},
      testSettingsFilePathControl{[[NSPathControl alloc] init]},
      startingSnrField{emptyTextField()},
      actions{[[TestSetupUIActions alloc] init]} {
    actions->controller = this;
    const auto confirmButton {
        nsButton("Confirm", actions,
            @selector(notifyThatConfirmButtonHasBeenClicked))
    };
    [confirmButton setKeyEquivalent:@"\r"];
    testSettingsFilePathControl.pathStyle = NSPathStylePopUp;
    testSettingsFilePathControl.allowedTypes = @[ @"txt" ];
    auto stack {
        [NSStackView stackViewWithViews:@[
            labeledView(subjectIdField, "subject:"),
            labeledView(testerIdField, "tester:"),
            labeledView(sessionField, "session:"),
            labeledView(rmeSettingField, "RME setting:"),
            labeledView(transducerMenu, "transducer:"),
            [NSStackView stackViewWithViews:@[
                label("test settings file:"), testSettingsFilePathControl,
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
    setPlaceholder(rmeSettingField, "abc123");
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
            constraintEqualToAnchor:testSettingsFilePathControl.leadingAnchor],
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
    return testSettingsFilePathControl.URL == nil
        ? ""
        : testSettingsFilePathControl.URL.fileSystemRepresentation;
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

void AppKitTestSetupUI::notifyThatPlayCalibrationButtonHasBeenClicked() {
    observer->notifyThatPlayCalibrationButtonHasBeenClicked();
}

static auto emptyLabel() -> NSTextField * { return label(""); }

AppKitTestUI::AppKitTestUI(NSViewController *viewController)
    : viewController{viewController}, sheetField{emptyLabel()},
      continueTestingDialogField{emptyLabel()}, primaryTextField{emptyLabel()},
      secondaryTextField{emptyLabel()}, actions{[[TestUIActions alloc] init]} {
    const auto sheetController{nsTabViewControllerWithoutTabControl()};
    sheet = [NSWindow windowWithContentViewController:sheetController];
    sheet.styleMask = NSWindowStyleMaskBorderless;
    const auto sheetStack {
        [NSStackView stackViewWithViews:@[
            sheetField,
            nsButton("Ok", actions,
                @selector(notifyThatSheetsOkButtonHasBeenClicked))

        ]]
    };
    addAutolayoutEnabledSubview(view(sheetController), sheetStack);
    const auto continueTestingDialogController{
        nsTabViewControllerWithoutTabControl()};
    continueTestingDialog = [NSWindow
        windowWithContentViewController:continueTestingDialogController];
    continueTestingDialog.styleMask = NSWindowStyleMaskBorderless;
    exitTestButton = nsButton("Exit Test", actions,
        @selector(notifyThatExitTestButtonHasBeenClicked));
    nextTrialButton = nsButton("Play Trial", actions,
        @selector(notifyThatPlayTrialButtonHasBeenClicked));
    const auto topRow {
        [NSStackView stackViewWithViews:@[
            exitTestButton, primaryTextField, secondaryTextField
        ]]
    };
    addAutolayoutEnabledSubview(view(viewController), topRow);
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
    activateConstraints(@[
        [topRow.leadingAnchor
            constraintEqualToAnchor:view(viewController).leadingAnchor
                           constant:defaultMarginPoints],
        [topRow.topAnchor constraintEqualToAnchor:view(viewController).topAnchor
                                         constant:defaultMarginPoints]
    ]);
    activateChildConstraintNestledInBottomRightCorner(
        nextTrialButton, view(viewController), defaultMarginPoints);
    actions->onSheetsOkButtonClicked = [=]() {
        [view(viewController).window endSheet:sheet];
    };
    av_speech_in_noise::hide(nextTrialButton);
    av_speech_in_noise::hide(view(viewController));
}

void AppKitTestUI::attach(Observer *e) { actions->observer = e; }

void AppKitTestUI::showExitTestButton() {
    av_speech_in_noise::show(exitTestButton);
}

void AppKitTestUI::hideExitTestButton() {
    av_speech_in_noise::hide(exitTestButton);
}

void AppKitTestUI::show() { av_speech_in_noise::show(view(viewController)); }

void AppKitTestUI::hide() { av_speech_in_noise::hide(view(viewController)); }

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

void AppKitTestUI::showSheet(std::string_view s) {
    set(sheetField, std::string{s});
    [view(viewController).window beginSheet:sheet
                          completionHandler:^(NSModalResponse){
                          }];
}

static auto consonantImageButton(
    std::unordered_map<void *, std::string> &consonants,
    ConsonantUIActions *actions, const std::string &consonant) -> NSButton * {
    const auto image{[[NSImage alloc]
        initWithContentsOfFile:nsString(resourceUrl(consonant, "bmp").path)]};
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

static void addReadyButton(NSView *parent, ConsonantUIActions *actions) {
    const auto button {
        nsButton("", actions, @selector(notifyThatReadyButtonHasBeenClicked))
    };
    [button setBezelStyle:NSBezelStyleTexturedSquare];
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

static auto nsArray(const std::vector<NSView *> &v) -> NSArray * {
    return [NSArray arrayWithObjects:&v.front() count:v.size()];
}

static auto equallyDistributedConsonantImageButtonGrid(
    std::unordered_map<void *, std::string> &consonants,
    ConsonantUIActions *actions,
    const std::vector<std::vector<std::string>> &consonantRows)
    -> NSStackView * {
    std::vector<NSView *> rows(consonantRows.size());
    std::transform(consonantRows.begin(), consonantRows.end(), rows.begin(),
        [&](const std::vector<std::string> &consonantRow) {
            std::vector<NSView *> buttons(consonantRow.size());
            std::transform(consonantRow.begin(), consonantRow.end(),
                buttons.begin(), [&](const std::string &consonant) {
                    return consonantImageButton(consonants, actions, consonant);
                });
            const auto row{[NSStackView stackViewWithViews:nsArray(buttons)]};
            row.distribution = NSStackViewDistributionFillEqually;
            return row;
        });
    const auto grid{[NSStackView stackViewWithViews:nsArray(rows)]};
    for (auto row : rows)
        [NSLayoutConstraint activateConstraints:@[
            [row.leadingAnchor constraintEqualToAnchor:grid.leadingAnchor],
            [row.trailingAnchor constraintEqualToAnchor:grid.trailingAnchor]
        ]];
    return grid;
}

AppKitConsonantUI::AppKitConsonantUI(NSRect r)
    : // Defer may be critical here...
      window{[[NSWindow alloc] initWithContentRect:r
                                         styleMask:NSWindowStyleMaskBorderless
                                           backing:NSBackingStoreBuffered
                                             defer:YES]},
      readyButton{
          [[NSView alloc] initWithFrame:NSMakeRect(0, 0, width(r), height(r))]},
      actions{[[ConsonantUIActions alloc] init]} {
    actions->controller = this;
    responseButtons =
        equallyDistributedConsonantImageButtonGrid(consonants, actions,
            {{"b", "c", "d", "h"}, {"k", "m", "n", "p"}, {"s", "t", "v", "z"}});
    responseButtons.orientation = NSUserInterfaceLayoutOrientationVertical;
    responseButtons.distribution = NSStackViewDistributionFillEqually;
    addReadyButton(readyButton, actions);
    const auto contentView{window.contentView};
    addSubview(contentView, readyButton);
    addAutolayoutEnabledSubview(contentView, responseButtons);
    [NSLayoutConstraint activateConstraints:@[
        [responseButtons.topAnchor
            constraintEqualToAnchor:contentView.topAnchor],
        [responseButtons.bottomAnchor
            constraintEqualToAnchor:contentView.bottomAnchor],
        [responseButtons.leadingAnchor
            constraintEqualToAnchor:contentView.leadingAnchor],
        [responseButtons.trailingAnchor
            constraintEqualToAnchor:contentView.trailingAnchor],
    ]];
    for (NSStackView *row in responseButtons.views)
        for (NSView *view in row.views)
            [NSLayoutConstraint activateConstraints:@[
                [view.topAnchor constraintEqualToAnchor:row.topAnchor],
                [view.bottomAnchor constraintEqualToAnchor:row.bottomAnchor]
            ]];
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
      actions{[[CoordinateResponseMeasureUIActions alloc] init]} {
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
    const auto title{nsString(std::to_string(number))};
    const auto button {
        [NSButton
            buttonWithTitle:title
                     target:actions
                     action:@selector(notifyThatResponseButtonHasBeenClicked:)]
    };
    const auto responseWidth{width(responseButtons.frame) / responseNumbers};
    const auto responseHeight{height(responseButtons.frame) / responseColors};
    [button setFrame:NSMakeRect(responseWidth * col, responseHeight * row,
                         responseWidth, responseHeight)];
    [button setBezelStyle:NSBezelStyleTexturedSquare];
    const auto style{[[NSMutableParagraphStyle alloc] init]};
    [style setAlignment:NSTextAlignmentCenter];
    [button
        setAttributedTitle:
            [[NSAttributedString alloc]
                initWithString:title
                    attributes:[NSDictionary dictionaryWithObjectsAndKeys:color,
                                             NSForegroundColorAttributeName,
                                             [NSNumber numberWithFloat:-4.0],
                                             NSStrokeWidthAttributeName,
                                             NSColor.blackColor,
                                             NSStrokeColorAttributeName, style,
                                             NSParagraphStyleAttributeName,
                                             [NSFont fontWithName:@"Arial-Black"
                                                             size:48],
                                             NSFontAttributeName, nil]]];
    addSubview(responseButtons, button);
}

void AppKitCoordinateResponseMeasureUI::addNextTrialButton() {
    const auto button {
        nsButton("", actions, @selector(notifyThatReadyButtonHasBeenClicked))
    };
    [button setBezelStyle:NSBezelStyleTexturedSquare];
    auto font{[NSFont fontWithName:@"Courier" size:36]};
    [button
        setAttributedTitle:
            [[NSAttributedString alloc]
                initWithString:@"Press when ready"
                    attributes:[NSDictionary dictionaryWithObjectsAndKeys:font,
                                             NSFontAttributeName, nil]]];
    [button setFrame:NSMakeRect(0, 0, width(nextTrialButton.frame),
                         height(nextTrialButton.frame))];
    addSubview(nextTrialButton, button);
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

ChooseKeywordsUI::ChooseKeywordsUI(NSViewController *viewController)
    : textFieldBeforeFirstKeywordButton{emptyLabel()},
      textFieldAfterFirstKeywordButton{emptyLabel()},
      textFieldAfterSecondKeywordButton{emptyLabel()},
      textFieldAfterThirdKeywordButton{emptyLabel()},
      flaggedButton{[NSButton checkboxWithTitle:@"flagged"
                                         target:nil
                                         action:nil]},
      actions{[[ChooseKeywordsUIActions alloc] init]} {
    firstKeywordButton = nsButton(
        "", actions, @selector(notifyThatFirstKeywordButtonHasBeenClicked));
    secondKeywordButton = nsButton(
        "", actions, @selector(notifyThatSecondKeywordButtonHasBeenClicked));
    thirdKeywordButton = nsButton(
        "", actions, @selector(notifyThatThirdKeywordButtonHasBeenClicked));
    [firstKeywordButton setButtonType:NSButtonTypePushOnPushOff];
    [secondKeywordButton setButtonType:NSButtonTypePushOnPushOff];
    [thirdKeywordButton setButtonType:NSButtonTypePushOnPushOff];
    const auto keywordButtons {
        [NSStackView stackViewWithViews:@[
            textFieldBeforeFirstKeywordButton, firstKeywordButton,
            textFieldAfterFirstKeywordButton, secondKeywordButton,
            textFieldAfterSecondKeywordButton, thirdKeywordButton,
            textFieldAfterThirdKeywordButton
        ]]
    };
    const auto resetButton {
        nsButton(
            "Reset", actions, @selector(notifyThatResetButtonHasBeenClicked))
    };
    const auto allWrongButton {
        nsButton("All wrong", actions,
            @selector(notifyThatAllWrongButtonHasBeenClicked))
    };
    const auto submitButton {
        nsButton(
            "Submit", actions, @selector(notifyThatSubmitButtonHasBeenClicked))
    };
    const auto actionButtons {
        [NSStackView
            stackViewWithViews:@[ resetButton, allWrongButton, submitButton ]]
    };
    responseView =
        [NSStackView stackViewWithViews:@[ keywordButtons, actionButtons ]];
    responseView.orientation = NSUserInterfaceLayoutOrientationVertical;
    responseView.alignment = NSLayoutAttributeRight;
    addAutolayoutEnabledSubview(view(viewController), responseView);
    activateChildConstraintNestledInBottomRightCorner(
        responseView, view(viewController), defaultMarginPoints);
    av_speech_in_noise::hide(responseView);
}

auto ChooseKeywordsUI::flagged() -> bool {
    return flaggedButton.state == NSControlStateValueOn;
}

void ChooseKeywordsUI::clearFlag() {
    flaggedButton.state = NSControlStateValueOff;
}

void ChooseKeywordsUI::markThirdKeywordCorrect() {
    [thirdKeywordButton setState:NSControlStateValueOff];
}

void ChooseKeywordsUI::markSecondKeywordCorrect() {
    [secondKeywordButton setState:NSControlStateValueOff];
}

void ChooseKeywordsUI::markFirstKeywordCorrect() {
    [firstKeywordButton setState:NSControlStateValueOff];
}

void ChooseKeywordsUI::markThirdKeywordIncorrect() {
    [thirdKeywordButton setState:NSControlStateValueOn];
}

void ChooseKeywordsUI::markSecondKeywordIncorrect() {
    [secondKeywordButton setState:NSControlStateValueOn];
}

void ChooseKeywordsUI::markFirstKeywordIncorrect() {
    [firstKeywordButton setState:NSControlStateValueOn];
}

auto ChooseKeywordsUI::thirdKeywordCorrect() -> bool {
    return thirdKeywordButton.state == NSControlStateValueOff;
}

auto ChooseKeywordsUI::secondKeywordCorrect() -> bool {
    return secondKeywordButton.state == NSControlStateValueOff;
}

auto ChooseKeywordsUI::firstKeywordCorrect() -> bool {
    return firstKeywordButton.state == NSControlStateValueOff;
}

void ChooseKeywordsUI::attach(Observer *observer) {
    actions->observer = observer;
}

void ChooseKeywordsUI::showResponseSubmission() {
    av_speech_in_noise::show(responseView);
}

void ChooseKeywordsUI::hideResponseSubmission() {
    av_speech_in_noise::hide(responseView);
}

void ChooseKeywordsUI::setThirdKeywordButtonText(const std::string &s) {
    [thirdKeywordButton setTitle:nsString(s)];
}

void ChooseKeywordsUI::setSecondKeywordButtonText(const std::string &s) {
    [secondKeywordButton setTitle:nsString(s)];
}

void ChooseKeywordsUI::setFirstKeywordButtonText(const std::string &s) {
    [firstKeywordButton setTitle:nsString(s)];
}

void ChooseKeywordsUI::setTextPrecedingFirstKeywordButton(
    const std::string &s) {
    [textFieldBeforeFirstKeywordButton setStringValue:nsString(s)];
}

void ChooseKeywordsUI::setTextFollowingFirstKeywordButton(
    const std::string &s) {
    [textFieldAfterFirstKeywordButton setStringValue:nsString(s)];
}

void ChooseKeywordsUI::setTextFollowingSecondKeywordButton(
    const std::string &s) {
    [textFieldAfterSecondKeywordButton setStringValue:nsString(s)];
}

void ChooseKeywordsUI::setTextFollowingThirdKeywordButton(
    const std::string &s) {
    [textFieldAfterThirdKeywordButton setStringValue:nsString(s)];
}

FreeResponseUI::FreeResponseUI(NSViewController *viewController)
    : responseField{emptyTextField()}, flaggedButton{[NSButton
                                           checkboxWithTitle:@"flagged"
                                                      target:nil
                                                      action:nil]},
      actions{[[FreeResponseUIActions alloc] init]} {
    const auto submitButton {
        nsButton(
            "Submit", actions, @selector(notifyThatSubmitButtonHasBeenClicked))
    };
    setPlaceholder(responseField, "This is a sentence.");
    const auto innerView {
        [NSStackView stackViewWithViews:@[ flaggedButton, responseField ]]
    };
    responseView =
        [NSStackView stackViewWithViews:@[ innerView, submitButton ]];
    responseView.orientation = NSUserInterfaceLayoutOrientationVertical;
    innerView.distribution = NSStackViewDistributionFill;
    [flaggedButton
        setContentHuggingPriority:251
                   forOrientation:NSLayoutConstraintOrientationHorizontal];
    [responseField
        setContentHuggingPriority:48
                   forOrientation:NSLayoutConstraintOrientationHorizontal];
    [responseField
        setContentCompressionResistancePriority:749
                                 forOrientation:
                                     NSLayoutConstraintOrientationHorizontal];
    addAutolayoutEnabledSubview(view(viewController), responseView);
    activateConstraints(@[
        [responseView.leadingAnchor
            constraintEqualToAnchor:view(viewController).centerXAnchor],
        [innerView.leadingAnchor
            constraintEqualToAnchor:responseView.leadingAnchor],
        [innerView.trailingAnchor
            constraintEqualToAnchor:responseView.trailingAnchor]
    ]);
    activateChildConstraintNestledInBottomRightCorner(
        responseView, view(viewController), defaultMarginPoints);
    av_speech_in_noise::hide(responseView);
}

void FreeResponseUI::attach(Observer *e) { actions->observer = e; }

void FreeResponseUI::showFreeResponseSubmission() {
    av_speech_in_noise::show(responseView);
}

void FreeResponseUI::hideFreeResponseSubmission() {
    av_speech_in_noise::hide(responseView);
}

void FreeResponseUI::clearFreeResponse() { set(responseField, ""); }

void FreeResponseUI::clearFlag() {
    flaggedButton.state = NSControlStateValueOff;
}

auto FreeResponseUI::freeResponse() -> std::string {
    return string(responseField);
}

auto FreeResponseUI::flagged() -> bool {
    return flaggedButton.state == NSControlStateValueOn;
}

CorrectKeywordsUI::CorrectKeywordsUI(NSViewController *viewController)
    : responseField{emptyTextField()},
      actions{[[CorrectKeywordsUIActions alloc] init]} {
    setPlaceholder(responseField, "2");
    const auto submitButton {
        nsButton(
            "Submit", actions, @selector(notifyThatSubmitButtonHasBeenClicked))
    };
    responseView =
        [NSStackView stackViewWithViews:@[ responseField, submitButton ]];
    responseView.orientation = NSUserInterfaceLayoutOrientationVertical;
    addAutolayoutEnabledSubview(view(viewController), responseView);
    activateConstraints(@[
        [responseField.leadingAnchor
            constraintEqualToAnchor:submitButton.leadingAnchor],
        [responseField.trailingAnchor
            constraintEqualToAnchor:submitButton.trailingAnchor]
    ]);
    activateChildConstraintNestledInBottomRightCorner(
        responseView, view(viewController), defaultMarginPoints);
    av_speech_in_noise::hide(responseView);
}

void CorrectKeywordsUI::attach(Observer *e) { actions->observer = e; }

void CorrectKeywordsUI::showCorrectKeywordsSubmission() {
    av_speech_in_noise::show(responseView);
}

void CorrectKeywordsUI::hideCorrectKeywordsSubmission() {
    av_speech_in_noise::hide(responseView);
}

auto CorrectKeywordsUI::correctKeywords() -> std::string {
    return string(responseField);
}

PassFailUI::PassFailUI(NSViewController *viewController)
    : actions{[[PassFailUIActions alloc] init]} {
    responseView = [NSStackView stackViewWithViews:@[
        nsButton("Incorrect", actions,
            @selector(notifyThatIncorrectButtonHasBeenClicked)),
        nsButton("Correct", actions,
            @selector(notifyThatCorrectButtonHasBeenClicked))
    ]];
    addAutolayoutEnabledSubview(view(viewController), responseView);
    activateChildConstraintNestledInBottomRightCorner(
        responseView, view(viewController), defaultMarginPoints);
    av_speech_in_noise::hide(responseView);
}

void PassFailUI::attach(Observer *e) { actions->observer = e; }

void PassFailUI::showEvaluationButtons() {
    av_speech_in_noise::show(responseView);
}

void PassFailUI::hideEvaluationButtons() {
    av_speech_in_noise::hide(responseView);
}

SyllablesUI::SyllablesUI(NSViewController *viewController)
    : actions{[[SyllablesUIActions alloc] init]},
      flaggedButton{[NSButton checkboxWithTitle:@"flagged"
                                         target:nil
                                         action:nil]} {
    actions->onResponseButtonClick = [&](id sender) {
        lastButtonPressed = sender;
    };
    std::vector<std::vector<std::string>> syllables{
        {"B", "D", "G", "F", "Ghee", "H", "Yee"},
        {"K", "L", "M", "N", "P", "R", "Sh"},
        {"S", "Th", "T", "Ch", "V", "W", "Z"}};
    std::vector<NSView *> rows(syllables.size());
    std::transform(syllables.begin(), syllables.end(), rows.begin(),
        [&](const std::vector<std::string> &syllableRow) {
            std::vector<NSView *> buttons(syllableRow.size());
            std::transform(syllableRow.begin(), syllableRow.end(),
                buttons.begin(), [&](const std::string &syllable) {
                    return nsButton(syllable, actions,
                        @selector(notifyThatResponseButtonHasBeenClicked:));
                });
            const auto row{[NSStackView stackViewWithViews:nsArray(buttons)]};
            row.distribution = NSStackViewDistributionFillEqually;
            return row;
        });
    const auto responseButtons{[NSStackView stackViewWithViews:nsArray(rows)]};
    for (auto row : rows)
        [NSLayoutConstraint activateConstraints:@[
            [row.leadingAnchor
                constraintEqualToAnchor:responseButtons.leadingAnchor],
            [row.trailingAnchor
                constraintEqualToAnchor:responseButtons.trailingAnchor]
        ]];
    responseButtons.orientation = NSUserInterfaceLayoutOrientationVertical;
    responseButtons.distribution = NSStackViewDistributionFillEqually;
    view = [NSStackView stackViewWithViews:@[ responseButtons ]];
    view.orientation = NSUserInterfaceLayoutOrientationVertical;
    addAutolayoutEnabledSubview(av_speech_in_noise::view(viewController), view);
    activateChildConstraintNestledInBottomRightCorner(
        view, av_speech_in_noise::view(viewController), defaultMarginPoints);
    av_speech_in_noise::hide(view);
}

void SyllablesUI::attach(Observer *a) { actions->observer = a; }

void SyllablesUI::hide() { av_speech_in_noise::hide(view); }

void SyllablesUI::show() { av_speech_in_noise::show(view); }

auto SyllablesUI::syllable() -> std::string {
    return lastButtonPressed.title.UTF8String;
}

auto SyllablesUI::flagged() -> bool {
    return flaggedButton.state == NSControlStateValueOn;
}

void SyllablesUI::clearFlag() { flaggedButton.state = NSControlStateValueOff; }
}
