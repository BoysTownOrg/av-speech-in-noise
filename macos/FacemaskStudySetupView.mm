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
    return [NSButton buttonWithTitle:asNsString(s) target:target action:action];
}

static void addAutolayoutEnabledSubview(NSView *parent, NSView *child) {
    child.translatesAutoresizingMaskIntoConstraints = NO;
    [parent addSubview:child];
}

FacemaskStudySetupView::FacemaskStudySetupView(NSViewController *controller)
    : testSettingsField{[NSTextField textFieldWithString:@""]},
      actions{[[FacemaskStudySetupViewActions alloc] init]}, controller{
                                                                 controller} {
    const auto testSettingsLabel{
        [NSTextField labelWithString:@"Session file:"]};
    const auto titleLabel{[NSTextField labelWithString:@"Facemask Study"]};
    const auto instructionsLabel{[NSTextField
        labelWithString:@"Click browse to choose the session file."]};
    const auto browseForTestSettingsButton {
        button("", actions,
            @selector(notifyThatBrowseForTestSettingsButtonHasBeenClicked))
    };
    const auto confirmButton {
        button(
            "START", actions, @selector(notifyThatConfirmButtonHasBeenClicked))
    };
    const auto playCalibrationButton {
        button("play calibration", actions,
            @selector(notifyThatPlayCalibrationButtonHasBeenClicked))
    };
    [browseForTestSettingsButton
        setAttributedTitle:
            [[NSAttributedString alloc]
                initWithString:@"Browse"
                    attributes:[NSDictionary
                                   dictionaryWithObjectsAndKeys:
                                       [NSFont boldSystemFontOfSize:36],
                                   NSFontAttributeName, nil]]];
    [browseForTestSettingsButton setBezelStyle:NSBezelStyleRoundRect];
    addAutolayoutEnabledSubview(controller.view, browseForTestSettingsButton);
    addAutolayoutEnabledSubview(controller.view, confirmButton);
    addAutolayoutEnabledSubview(controller.view, playCalibrationButton);
    addAutolayoutEnabledSubview(controller.view, testSettingsField);
    addAutolayoutEnabledSubview(controller.view, testSettingsLabel);
    addAutolayoutEnabledSubview(controller.view, titleLabel);
    addAutolayoutEnabledSubview(controller.view, instructionsLabel);
    [NSLayoutConstraint activateConstraints:@[
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
            constraintEqualToAnchor:controller.view.centerXAnchor]
    ]];
}

void FacemaskStudySetupView::show() { [controller.view setHidden:NO]; }

void FacemaskStudySetupView::hide() { [controller.view setHidden:YES]; }

auto FacemaskStudySetupView::testSettingsFile() -> std::string {
    return testSettingsField.stringValue.UTF8String;
}

void FacemaskStudySetupView::setTestSettingsFile(std::string s) {
    [testSettingsField setStringValue:asNsString(s)];
}

void FacemaskStudySetupView::subscribe(EventListener *) {}

void FacemaskStudySetupView::notifyThatConfirmButtonHasBeenClicked() {}

void FacemaskStudySetupView::
    notifyThatBrowseForTestSettingsButtonHasBeenClicked() {}

void FacemaskStudySetupView::notifyThatPlayCalibrationButtonHasBeenClicked() {}
}
