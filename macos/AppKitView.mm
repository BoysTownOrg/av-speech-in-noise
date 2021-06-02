#include "AppKitView.h"
#include "AppKit-utility.h"
#include "Foundation-utility.h"
#include <gsl/gsl>
#include <iterator>
#include <array>
#include <algorithm>
#include <functional>

@interface CoordinateResponseMeasureUIActions : NSObject
@end

@interface ConsonantUIActions : NSObject
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

static auto consonantImageButton(
    std::unordered_map<void *, std::string> &consonants,
    ConsonantUIActions *actions, const std::string &consonant) -> NSButton * {
    const auto image{[NSImage imageNamed:nsString(consonant + ".bmp")]};
    const auto button {
        [NSButton
            buttonWithImage:image != nil
                ? image
                : [NSImage imageNamed:NSImageNameApplicationIcon]
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

AppKitConsonantUI::AppKitConsonantUI(NSWindow *window)
    : // Defer may be critical here...
      window{window}, readyButton{[[NSView alloc]
                          initWithFrame:NSMakeRect(0, 0, width(window.frame),
                                            height(window.frame))]},
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

AppKitCoordinateResponseMeasureUI::AppKitCoordinateResponseMeasureUI(
    NSView *view)
    : // Defer may be critical here...
      view{view}, responseButtons{[[NSView alloc]
                      initWithFrame:NSMakeRect(0, 0, width(view.frame),
                                        height(view.frame))]},
      nextTrialButton{
          [[NSView alloc] initWithFrame:NSMakeRect(0, 0, width(view.frame),
                                            height(view.frame))]},
      actions{[[CoordinateResponseMeasureUIActions alloc] init]} {
    actions->controller = this;
    addButtonRow(blueColor, 0);
    addButtonRow(greenColor, 1);
    addButtonRow(whiteColor, 2);
    addButtonRow(redColor, 3);
    addNextTrialButton();
    addSubview(view, nextTrialButton);
    addSubview(view, responseButtons);
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
    av_speech_in_noise::show(view);
}

void AppKitCoordinateResponseMeasureUI::hide() {
    av_speech_in_noise::hide(view);
}
}
