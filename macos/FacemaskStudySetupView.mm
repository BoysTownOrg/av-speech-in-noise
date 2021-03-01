#include "FacemaskStudySetupView.h"
#include "AppKit-utility.h"
#include "Foundation-utility.h"
#include <presentation/TestSettingsInterpreter.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

@interface FacemaskStudySetupViewActions : NSObject
@end

@implementation FacemaskStudySetupViewActions {
  @public
    av_speech_in_noise::FacemaskStudySetupView *controller;
}

- (void)notifyThatConfirmButtonHasBeenClicked {
    controller->notifyThatConfirmButtonHasBeenClicked();
}

- (void)notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked {
    controller->notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked();
}

- (void)notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked {
    controller->notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked();
}
@end

namespace av_speech_in_noise {
static void setAttributedTitle(NSButton *button, const std::string &s) {
    [button setAttributedTitle:
                [[NSAttributedString alloc]
                    initWithString:nsString(s)
                        attributes:[NSDictionary
                                       dictionaryWithObjectsAndKeys:
                                           [NSFont boldSystemFontOfSize:30],
                                       NSFontAttributeName,
                                       [NSColor colorWithRed:33. / 255
                                                       green:57. / 255
                                                        blue:98. / 255
                                                       alpha:1],
                                       NSForegroundColorAttributeName, nil]]];
}

static auto yellowColor() -> NSColor * {
    return [NSColor colorWithRed:250. / 255
                           green:216. / 255
                            blue:111. / 255
                           alpha:1];
}

static auto labelWithAttributedString(
    const std::string &s, NSColor *color, CGFloat size) -> NSTextField * {
    return [NSTextField
        labelWithAttributedString:
            [[NSAttributedString alloc]
                initWithString:nsString(s)
                    attributes:[NSDictionary dictionaryWithObjectsAndKeys:
                                                 [NSFont systemFontOfSize:size],
                                             NSFontAttributeName, color,
                                             NSForegroundColorAttributeName,
                                             nil]]];
}

static auto verticalStackView(NSArray<NSView *> *views) -> NSStackView * {
    const auto view{[NSStackView stackViewWithViews:views]};
    view.orientation = NSUserInterfaceLayoutOrientationVertical;
    return view;
}

static auto labeledView(NSView *field, const std::string &s) -> NSStackView * {
    const auto label_{labelWithAttributedString(s, yellowColor(), 28)};
    [label_ setContentHuggingPriority:251
                       forOrientation:NSLayoutConstraintOrientationHorizontal];
    const auto stack { [NSStackView stackViewWithViews:@[ label_, field ]] };
    return stack;
}

static auto readContents(const LocalUrl &resourceUrl) -> std::string {
    std::ifstream file{resourceUrl.path};
    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

static auto meta(const LocalUrl &resourceUrl) -> std::string {
    return TestSettingsInterpreterImpl::meta(readContents(resourceUrl));
}

static void push_back(NSPopUpButton *button,
    std::map<std::string, LocalUrl> &conditionUrls, const std::string &stem) {
    const auto conditionName{meta(resourceUrl(stem, "txt"))};
    conditionUrls[conditionName] = resourceUrl(stem, "txt");
    [button addItemWithTitle:nsString(conditionName)];
}

static void addBorder(NSView *view) {
    view.wantsLayer = YES;
    view.layer.borderWidth = 1;
}

FacemaskStudySetupView::FacemaskStudySetupView(NSViewController *controller)
    : subjectIdField{[NSTextField textFieldWithString:@""]},
      minusEightdBButton{[NSButton checkboxWithTitle:@"-8 dB SNR"
                                              target:nil
                                              action:nil]},
      condition{[[NSPopUpButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)
                                           pullsDown:NO]},
      actions{[[FacemaskStudySetupViewActions alloc] init]}, controller{
                                                                 controller} {
    actions->controller = this;
    const auto titleLabel{[NSTextField
        labelWithAttributedString:
            [[NSAttributedString alloc]
                initWithString:@"Facemask Study"
                    attributes:[NSDictionary
                                   dictionaryWithObjectsAndKeys:
                                       [NSFont systemFontOfSize:40],
                                   NSFontAttributeName, [NSColor whiteColor],
                                   NSForegroundColorAttributeName, nil]]]};
    const auto confirmButton {
        nsButton("", actions, @selector(notifyThatConfirmButtonHasBeenClicked))
    };
    const auto playLeftSpeakerCalibrationButton {
        nsButton("play left speaker", actions,
            @selector(notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked))
    };
    const auto playRightSpeakerCalibrationButton {
        nsButton("play right speaker", actions,
            @selector
            (notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked))
    };
    setAttributedTitle(confirmButton, "START");
    confirmButton.wantsLayer = YES;
    confirmButton.bordered = NO;
    confirmButton.layer.cornerRadius = 8.0;
    [confirmButton.layer setBackgroundColor:[NSColor colorWithRed:114. / 255
                                                            green:172. / 255
                                                             blue:77. / 255
                                                            alpha:1]
                                                .CGColor];
    [controller.view.window setBackgroundColor:[NSColor colorWithRed:43. / 255
                                                               green:97. / 255
                                                                blue:198. / 255
                                                               alpha:1]];

    const auto logoImage{[NSImage imageNamed:@"btnrh.png"]};
    const auto logo{[NSImageView imageViewWithImage:logoImage]};
    logo.imageScaling = NSImageScaleProportionallyDown;
    logo.wantsLayer = YES;
    logo.layer.backgroundColor = NSColor.whiteColor.CGColor;
    push_back(condition, conditionUrls, "NoMask_AO");
    push_back(condition, conditionUrls, "NoMask_AV");
    push_back(condition, conditionUrls, "ClearMask_AO");
    push_back(condition, conditionUrls, "ClearMask_AV");
    push_back(condition, conditionUrls, "CommunicatorMask_AO");
    push_back(condition, conditionUrls, "CommunicatorMask_AV");
    push_back(condition, conditionUrls, "FabricMask_AO");
    push_back(condition, conditionUrls, "FabricMask_AV");
    push_back(condition, conditionUrls, "HospitalMask_AO");
    push_back(condition, conditionUrls, "HospitalMask_AV");
    push_back(condition, conditionUrls, "NoMask_VO");
    const auto subjectIdWithLabel{labeledView(subjectIdField, "Subject ID:")};
    subjectIdWithLabel.alignment = NSLayoutAttributeFirstBaseline;
    subjectIdWithLabel.distribution = NSStackViewDistributionFill;
    const auto conditionWithLabel{labeledView(condition, "condition:")};
    const auto layoutStack {
        verticalStackView(@[
            [NSStackView stackViewWithViews:@[ logo, titleLabel ]],
            subjectIdWithLabel, conditionWithLabel, minusEightdBButton,
            playLeftSpeakerCalibrationButton, playRightSpeakerCalibrationButton
        ])
    };
    layoutStack.alignment = NSLayoutAttributeLeft;
    [subjectIdField setFont:[NSFont systemFontOfSize:30]];
    [subjectIdField setTextColor:NSColor.blackColor];
    subjectIdField.wantsLayer = YES;
    subjectIdField.layer.backgroundColor = NSColor.whiteColor.CGColor;
    [titleLabel
        setContentCompressionResistancePriority:751
                                 forOrientation:
                                     NSLayoutConstraintOrientationHorizontal];
    addAutolayoutEnabledSubview(controller.view, layoutStack);
    addAutolayoutEnabledSubview(controller.view, confirmButton);
    [NSLayoutConstraint activateConstraints:@[
        [layoutStack.topAnchor constraintEqualToAnchor:controller.view.topAnchor
                                              constant:8],
        [layoutStack.bottomAnchor
            constraintEqualToAnchor:confirmButton.topAnchor
                           constant:-8],
        [layoutStack.leadingAnchor
            constraintEqualToAnchor:controller.view.leadingAnchor
                           constant:8],
        [layoutStack.trailingAnchor
            constraintEqualToAnchor:controller.view.trailingAnchor
                           constant:-8],
        [confirmButton.centerXAnchor
            constraintEqualToAnchor:controller.view.centerXAnchor],
        [confirmButton.bottomAnchor
            constraintEqualToAnchor:controller.view.bottomAnchor
                           constant:-8],
        [confirmButton.widthAnchor constraintEqualToConstant:1.4 *
                                   confirmButton.attributedTitle.size.width],
        [confirmButton.heightAnchor constraintEqualToConstant:1.5 *
                                    confirmButton.attributedTitle.size.height]
    ]];
}

void FacemaskStudySetupView::show() {
    [controller.view.window makeKeyAndOrderFront:nil];
    [controller.view setHidden:NO];
}

void FacemaskStudySetupView::hide() {
    [controller.view setHidden:YES];
    [controller.view.window orderOut:nil];
}

auto FacemaskStudySetupView::testSettingsFile() -> std::string {
    return conditionUrls.at(condition.titleOfSelectedItem.UTF8String).path;
}

void FacemaskStudySetupView::attach(Observer *e) { listener_ = e; }

void FacemaskStudySetupView::notifyThatConfirmButtonHasBeenClicked() {
    listener_->notifyThatConfirmButtonHasBeenClicked();
}

void FacemaskStudySetupView::
    notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked() {
    listener_->notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked();
}

void FacemaskStudySetupView::
    notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked() {
    listener_->notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked();
}

auto FacemaskStudySetupView::startingSnr() -> std::string {
    return minusEightdBButton.state == NSControlStateValueOn ? "-8" : "0";
}
}
