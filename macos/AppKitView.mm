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
    av_speech_in_noise::submitting_consonant::AppKitUI *controller;
}

- (void)notifyThatResponseButtonHasBeenClicked:(id)sender {
    controller->notifyThatResponseButtonHasBeenClicked(sender);
}

- (void)notifyThatReadyButtonHasBeenClicked {
    controller->notifyThatReadyButtonHasBeenClicked();
}
@end

namespace av_speech_in_noise {
static void addSubview(NSView *parent, NSView *child) {
    [parent addSubview:child];
}

static void hide(NSView *v) { [v setHidden:YES]; }

static void show(NSView *v) { [v setHidden:NO]; }

constexpr auto width(const NSRect &r) -> CGFloat { return r.size.width; }

constexpr auto height(const NSRect &r) -> CGFloat { return r.size.height; }

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
            return row;
        });
    const auto grid{[NSStackView stackViewWithViews:nsArray(rows)]};
    return grid;
}

namespace submitting_consonant {
AppKitUI::AppKitUI(NSView *view)
    : // Defer may be critical here...
      view{view}, actions{[[ConsonantUIActions alloc] init]} {
    actions->controller = this;
    responseButtons =
        equallyDistributedConsonantImageButtonGrid(consonants, actions,
            {{"b", "c", "d", "h"}, {"k", "m", "n", "p"}, {"s", "t", "v", "z"}});
    responseButtons.orientation = NSUserInterfaceLayoutOrientationVertical;

    readyButton =
        nsButton("", actions, @selector(notifyThatReadyButtonHasBeenClicked));
    [readyButton setBezelStyle:NSBezelStyleTexturedSquare];
    const auto font{[NSFont fontWithName:@"Courier" size:36]};
    [readyButton
        setAttributedTitle:
            [[NSAttributedString alloc]
                initWithString:@"Press when ready"
                    attributes:[NSDictionary dictionaryWithObjectsAndKeys:font,
                                             NSFontAttributeName, nil]]];
    addAutolayoutEnabledSubview(view, readyButton);
    addAutolayoutEnabledSubview(view, responseButtons);
    [NSLayoutConstraint activateConstraints:@[
        [responseButtons.widthAnchor constraintEqualToAnchor:view.widthAnchor],
        [responseButtons.heightAnchor
            constraintEqualToAnchor:view.heightAnchor],
        [readyButton.widthAnchor constraintEqualToAnchor:view.widthAnchor],
        [readyButton.heightAnchor constraintEqualToAnchor:view.heightAnchor],
    ]];
    hideResponseButtons();
    hideReadyButton();
}

void AppKitUI::show() { av_speech_in_noise::show(view); }

void AppKitUI::hide() { av_speech_in_noise::hide(view); }

void AppKitUI::showReadyButton() { av_speech_in_noise::show(readyButton); }

void AppKitUI::hideReadyButton() { av_speech_in_noise::hide(readyButton); }

void AppKitUI::attach(Observer *e) { listener_ = e; }

void AppKitUI::notifyThatReadyButtonHasBeenClicked() {
    listener_->notifyThatReadyButtonHasBeenClicked();
}

void AppKitUI::notifyThatResponseButtonHasBeenClicked(id sender) {
    lastButtonPressed = sender;
    listener_->notifyThatResponseButtonHasBeenClicked();
}

void AppKitUI::showResponseButtons() {
    av_speech_in_noise::show(responseButtons);
}

void AppKitUI::hideResponseButtons() {
    av_speech_in_noise::hide(responseButtons);
}

auto AppKitUI::consonant() -> std::string {
    return consonants.at((__bridge void *)lastButtonPressed);
}

void AppKitUI::hideCursor() { [NSCursor hide]; }

void AppKitUI::showCursor() { [NSCursor unhide]; }
}

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
      view{view}, actions{[[CoordinateResponseMeasureUIActions alloc] init]} {
    actions->controller = this;
    std::vector<NSColor *> colors{blueColor, greenColor, whiteColor, redColor};
    std::vector<NSView *> rows(responseColors);
    std::transform(
        colors.begin(), colors.end(), rows.begin(), [&](NSColor *color) {
            std::vector<NSView *> buttons(responseNumbers);
            std::transform(numbers.begin(), numbers.end(), buttons.begin(),
                [&](int number) {
                    const auto title{nsString(std::to_string(number))};
                    const auto button {
                        [NSButton
                            buttonWithTitle:title
                                     target:actions
                                     action:@selector
                                     (notifyThatResponseButtonHasBeenClicked:)]
                    };
                    [button setBezelStyle:NSBezelStyleTexturedSquare];
                    const auto style{[[NSMutableParagraphStyle alloc] init]};
                    [style setAlignment:NSTextAlignmentCenter];
                    [button
                        setAttributedTitle:
                            [[NSAttributedString alloc]
                                initWithString:title
                                    attributes:
                                        [NSDictionary
                                            dictionaryWithObjectsAndKeys:color,
                                            NSForegroundColorAttributeName,
                                            [NSNumber numberWithFloat:-4.0],
                                            NSStrokeWidthAttributeName,
                                            NSColor.blackColor,
                                            NSStrokeColorAttributeName, style,
                                            NSParagraphStyleAttributeName,
                                            [NSFont fontWithName:@"Arial-Black"
                                                            size:48],
                                            NSFontAttributeName, nil]]];
                    [NSLayoutConstraint activateConstraints:@[
                        [button.widthAnchor
                            constraintEqualToAnchor:button.heightAnchor],
                    ]];
                    return button;
                });
            const auto row{[NSStackView stackViewWithViews:nsArray(buttons)]};
            return row;
        });
    responseButtons = [NSStackView stackViewWithViews:nsArray(rows)];
    responseButtons.orientation = NSUserInterfaceLayoutOrientationVertical;
    nextTrialButton =
        nsButton("", actions, @selector(notifyThatReadyButtonHasBeenClicked));
    [nextTrialButton setBezelStyle:NSBezelStyleTexturedSquare];
    auto font{[NSFont fontWithName:@"Courier" size:36]};
    [nextTrialButton
        setAttributedTitle:
            [[NSAttributedString alloc]
                initWithString:@"Press when ready"
                    attributes:[NSDictionary dictionaryWithObjectsAndKeys:font,
                                             NSFontAttributeName, nil]]];
    addAutolayoutEnabledSubview(view, nextTrialButton);
    addAutolayoutEnabledSubview(view, responseButtons);
    [NSLayoutConstraint activateConstraints:@[
        [responseButtons.widthAnchor constraintEqualToAnchor:view.widthAnchor],
        [responseButtons.heightAnchor
            constraintEqualToAnchor:view.heightAnchor],
        [nextTrialButton.widthAnchor constraintEqualToAnchor:view.widthAnchor],
        [nextTrialButton.heightAnchor
            constraintEqualToAnchor:view.heightAnchor],
    ]];
    hideResponseButtons();
    hideNextTrialButton();
}

auto AppKitCoordinateResponseMeasureUI::numberResponse() -> std::string {
    return string(lastButtonPressed.title);
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
