#include "main.h"
#include "MacOsTestSetupViewFactory.h"
#include "common-objc.h"
#include <presentation/SessionControllerImpl.hpp>
#import <Cocoa/Cocoa.h>
#include <string>

@class FacemaskStudySetupViewActions;

namespace av_speech_in_noise {
class FacemaskStudySetupView : public TestSetupUI {
  public:
    explicit FacemaskStudySetupView(NSViewController *);
    void show() override;
    void hide() override;
    auto testSettingsFile() -> std::string override;
    void setTestSettingsFile(std::string) override;
    void attach(Observer *) override;
    void populateTransducerMenu(std::vector<std::string>) override {}
    auto startingSnr() -> std::string override { return "0"; }
    auto testerId() -> std::string override { return {}; }
    auto subjectId() -> std::string override {
        return subjectIdField.stringValue.UTF8String;
    }
    auto session() -> std::string override { return {}; }
    auto rmeSetting() -> std::string override { return {}; }
    auto transducer() -> std::string override { return {}; }
    void notifyThatConfirmButtonHasBeenClicked();
    void notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked();
    void notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked();

  private:
    NSTextField *subjectIdField;
    FacemaskStudySetupViewActions *actions;
    Observer *listener_{};
    NSViewController *controller;
};

class FacemaskStudySetupViewFactory : public MacOsTestSetupViewFactory {
  public:
    auto make(NSViewController *c) -> std::unique_ptr<TestSetupUI> override {
        return std::make_unique<FacemaskStudySetupView>(c);
    }
};
}

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
class EyeTrackerStub : public EyeTracker {
    void allocateRecordingTimeSeconds(double) override {}
    void start() override {}
    void stop() override {}
    auto gazeSamples() -> BinocularGazeSamples override { return {}; }
    auto currentSystemTime() -> EyeTrackerSystemTime override { return {}; }
};

static auto button(const std::string &s, id target, SEL action) -> NSButton * {
    return [NSButton buttonWithTitle:nsString(s) target:target action:action];
}

static void addAutolayoutEnabledSubview(NSView *parent, NSView *child) {
    child.translatesAutoresizingMaskIntoConstraints = NO;
    [parent addSubview:child];
}

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

static auto labelWithAttributedString(const std::string &s) -> NSTextField * {
    return labelWithAttributedString(s, yellowColor(), 36);
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

FacemaskStudySetupView::FacemaskStudySetupView(NSViewController *controller)
    : subjectIdField{[NSTextField textFieldWithString:@""]},
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
    const auto instructionsLabel{
        labelWithAttributedString("Click browse to choose the session file.")};
    const auto confirmButton {
        button("", actions, @selector(notifyThatConfirmButtonHasBeenClicked))
    };
    const auto playLeftSpeakerCalibrationButton {
        button("play left speaker", actions,
            @selector(notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked))
    };
    const auto playRightSpeakerCalibrationButton {
        button("play right speaker", actions,
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
    const auto conditionA{
        [NSButton radioButtonWithTitle:nsString("A = No mask AUDIO ONLY")
                                target:actions
                                action:nil]};
    const auto conditionB{
        [NSButton radioButtonWithTitle:nsString("B = No mask AV")
                                target:actions
                                action:nil]};
    const auto conditionC{
        [NSButton radioButtonWithTitle:nsString("C = Clear Mask AO")
                                target:actions
                                action:nil]};
    const auto conditionD{
        [NSButton radioButtonWithTitle:nsString("D = Clear mask AV")
                                target:actions
                                action:nil]};
    const auto conditionE{
        [NSButton radioButtonWithTitle:nsString("E = Communicator AO")
                                target:actions
                                action:nil]};
    const auto conditionF{
        [NSButton radioButtonWithTitle:nsString("F = Communicator AV")
                                target:actions
                                action:nil]};
    const auto conditionG{
        [NSButton radioButtonWithTitle:nsString("G = Fabric mask AO")
                                target:actions
                                action:nil]};
    const auto conditionH{
        [NSButton radioButtonWithTitle:nsString("H = Fabric mask AV")
                                target:actions
                                action:nil]};
    const auto conditionI{
        [NSButton radioButtonWithTitle:nsString("I = Hospital mask AO")
                                target:actions
                                action:nil]};
    const auto conditionJ{
        [NSButton radioButtonWithTitle:nsString("J = Hospital mask AV")
                                target:actions
                                action:nil]};
    const auto layoutStack {
        verticalStackView(@[
            [NSStackView stackViewWithViews:@[ logo, titleLabel ]],
            instructionsLabel, labeledView(subjectIdField, "Subject ID:"),
            confirmButton
        ])
    };
    [subjectIdField setFont:[NSFont systemFontOfSize:30]];
    [subjectIdField setTextColor:NSColor.blackColor];
    subjectIdField.wantsLayer = YES;
    subjectIdField.layer.backgroundColor = NSColor.whiteColor.CGColor;
    [instructionsLabel
        setContentCompressionResistancePriority:751
                                 forOrientation:
                                     NSLayoutConstraintOrientationHorizontal];
    const auto playCalibrationButtonsStack {
        verticalStackView(@[
            playLeftSpeakerCalibrationButton, playRightSpeakerCalibrationButton
        ])
    };
    addAutolayoutEnabledSubview(controller.view, layoutStack);
    addAutolayoutEnabledSubview(controller.view, playCalibrationButtonsStack);
    [NSLayoutConstraint activateConstraints:@[
        [layoutStack.topAnchor constraintEqualToAnchor:controller.view.topAnchor
                                              constant:8],
        [layoutStack.bottomAnchor
            constraintEqualToAnchor:playCalibrationButtonsStack.topAnchor
                           constant:-8],
        [layoutStack.leadingAnchor
            constraintEqualToAnchor:controller.view.leadingAnchor
                           constant:8],
        [layoutStack.trailingAnchor
            constraintEqualToAnchor:controller.view.trailingAnchor
                           constant:-8],
        [playCalibrationButtonsStack.trailingAnchor
            constraintEqualToAnchor:controller.view.trailingAnchor
                           constant:-8],
        [playCalibrationButtonsStack.bottomAnchor
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

auto FacemaskStudySetupView::testSettingsFile() -> std::string { return {}; }

void FacemaskStudySetupView::setTestSettingsFile(std::string) {}

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
}

int main() {
    av_speech_in_noise::EyeTrackerStub eyeTracker;
    av_speech_in_noise::FacemaskStudySetupViewFactory testSetupViewFactory;
    av_speech_in_noise::main(eyeTracker, &testSetupViewFactory);
}
