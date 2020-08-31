#include "FacemaskStudySetupView.h"
#include "common-objc.h"

@interface FacemaskStudySetupViewActions : NSObject
@end

@implementation FacemaskStudySetupViewActions {
  @public
    av_speech_in_noise::FacemaskStudySetupView *controller;
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

namespace av_speech_in_noise {
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
                                           [NSFont boldSystemFontOfSize:36],
                                       NSFontAttributeName,
                                       [NSColor colorWithRed:33. / 255
                                                       green:57. / 255
                                                        blue:98. / 255
                                                       alpha:1],
                                       NSForegroundColorAttributeName, nil]]];
}

static auto labelWithAttributedString(const std::string &s) -> NSTextField * {
    return [NSTextField
        labelWithAttributedString:
            [[NSAttributedString alloc]
                initWithString:nsString(s)
                    attributes:[NSDictionary dictionaryWithObjectsAndKeys:
                                                 [NSFont systemFontOfSize:36],
                                             NSFontAttributeName,
                                             [NSColor colorWithRed:250. / 255
                                                             green:216. / 255
                                                              blue:111. / 255
                                                             alpha:1],
                                             NSForegroundColorAttributeName,
                                             nil]]];
}

FacemaskStudySetupView::FacemaskStudySetupView(NSViewController *controller)
    : testSettingsField{[NSTextField textFieldWithString:@""]},
      actions{[[FacemaskStudySetupViewActions alloc] init]}, controller{
                                                                 controller} {
    const auto testSettingsLabel{labelWithAttributedString("Session file:")};
    const auto titleLabel{[NSTextField
        labelWithAttributedString:
            [[NSAttributedString alloc]
                initWithString:@"Facemask Study"
                    attributes:[NSDictionary
                                   dictionaryWithObjectsAndKeys:
                                       [NSFont systemFontOfSize:36],
                                   NSFontAttributeName, [NSColor whiteColor],
                                   NSForegroundColorAttributeName, nil]]]};
    const auto instructionsLabel{
        labelWithAttributedString("Click browse to choose the session file.")};
    const auto browseForTestSettingsButton {
        button("", actions,
            @selector(notifyThatBrowseForTestSettingsButtonHasBeenClicked))
    };
    const auto confirmButton {
        button("", actions, @selector(notifyThatConfirmButtonHasBeenClicked))
    };
    const auto playCalibrationButton {
        button("play calibration", actions,
            @selector(notifyThatPlayCalibrationButtonHasBeenClicked))
    };

    setAttributedTitle(browseForTestSettingsButton, "Browse");
    setAttributedTitle(confirmButton, "START");
    confirmButton.bezelStyle = NSBezelStyleTexturedSquare;
    confirmButton.wantsLayer = YES;
    [confirmButton.layer setBackgroundColor:[NSColor colorWithRed:114. / 255
                                                            green:172. / 255
                                                             blue:77. / 255
                                                            alpha:1]
                                                .CGColor];
    browseForTestSettingsButton.bezelStyle = NSBezelStyleTexturedSquare;
    browseForTestSettingsButton.wantsLayer = YES;
    [browseForTestSettingsButton.layer
        setBackgroundColor:[NSColor colorWithRed:247. / 255
                                           green:191. / 255
                                            blue:44. / 255
                                           alpha:1]
                               .CGColor];
    [controller.view.window setBackgroundColor:[NSColor colorWithRed:43. / 255
                                                               green:97. / 255
                                                                blue:198. / 255
                                                               alpha:1]];

    const auto logo{
        [NSImageView imageViewWithImage:[NSImage imageNamed:@"b.bmp"]]};
    addAutolayoutEnabledSubview(controller.view, browseForTestSettingsButton);
    addAutolayoutEnabledSubview(controller.view, confirmButton);
    addAutolayoutEnabledSubview(controller.view, playCalibrationButton);
    addAutolayoutEnabledSubview(controller.view, testSettingsField);
    addAutolayoutEnabledSubview(controller.view, testSettingsLabel);
    addAutolayoutEnabledSubview(controller.view, titleLabel);
    addAutolayoutEnabledSubview(controller.view, instructionsLabel);
    addAutolayoutEnabledSubview(controller.view, logo);
    [testSettingsField
        setPlaceholderString:
            @"/Users/username/Documents/example-test-session-file.txt"];
    [testSettingsField sizeToFit];
    [NSLayoutConstraint activateConstraints:@[
        [logo.leadingAnchor
            constraintEqualToAnchor:controller.view.leadingAnchor
                           constant:8],
        [logo.topAnchor constraintEqualToAnchor:controller.view.topAnchor
                                       constant:8],
        [controller.view.topAnchor constraintEqualToAnchor:titleLabel.topAnchor
                                                  constant:-8],
        [titleLabel.bottomAnchor
            constraintEqualToAnchor:instructionsLabel.topAnchor
                           constant:-8],
        [instructionsLabel.bottomAnchor
            constraintEqualToAnchor:browseForTestSettingsButton.topAnchor
                           constant:-8],
        [browseForTestSettingsButton.bottomAnchor
            constraintEqualToAnchor:testSettingsField.topAnchor
                           constant:-8],
        [testSettingsField.bottomAnchor
            constraintEqualToAnchor:confirmButton.topAnchor
                           constant:-8],
        [testSettingsLabel.trailingAnchor
            constraintEqualToAnchor:testSettingsField.leadingAnchor
                           constant:-8],
        [testSettingsLabel.centerYAnchor
            constraintEqualToAnchor:testSettingsField.centerYAnchor],
        [browseForTestSettingsButton.centerXAnchor
            constraintEqualToAnchor:controller.view.centerXAnchor],
        [confirmButton.centerXAnchor
            constraintEqualToAnchor:controller.view.centerXAnchor],
        [instructionsLabel.centerXAnchor
            constraintEqualToAnchor:controller.view.centerXAnchor],
        [titleLabel.centerXAnchor
            constraintEqualToAnchor:controller.view.centerXAnchor],
        [testSettingsField.centerXAnchor
            constraintEqualToAnchor:controller.view.centerXAnchor],
        [testSettingsField.widthAnchor
            constraintEqualToConstant:NSWidth(testSettingsField.frame)]
    ]];
    [testSettingsField setPlaceholderString:@""];
}

void FacemaskStudySetupView::show() { [controller.view setHidden:NO]; }

void FacemaskStudySetupView::hide() { [controller.view setHidden:YES]; }

auto FacemaskStudySetupView::testSettingsFile() -> std::string {
    return testSettingsField.stringValue.UTF8String;
}

void FacemaskStudySetupView::setTestSettingsFile(std::string s) {
    [testSettingsField setStringValue:nsString(s)];
}

void FacemaskStudySetupView::subscribe(EventListener *) {}

void FacemaskStudySetupView::notifyThatConfirmButtonHasBeenClicked() {}

void FacemaskStudySetupView::
    notifyThatBrowseForTestSettingsButtonHasBeenClicked() {}

void FacemaskStudySetupView::notifyThatPlayCalibrationButtonHasBeenClicked() {}
}
