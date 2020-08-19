#include "FacemaskStudySetupView.h"

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
FacemaskStudySetupView::FacemaskStudySetupView(NSViewController *)
    : testSettingsField{[NSTextField textFieldWithString:@""]},
      actions{[[FacemaskStudySetupViewActions alloc] init]} {}

void FacemaskStudySetupView::show() {}

void FacemaskStudySetupView::hide() {}

auto FacemaskStudySetupView::testSettingsFile() -> std::string {
    return testSettingsField.stringValue.UTF8String;
}

void FacemaskStudySetupView::setTestSettingsFile(std::string) {}

void FacemaskStudySetupView::subscribe(EventListener *) {}

void FacemaskStudySetupView::notifyThatConfirmButtonHasBeenClicked() {}

void FacemaskStudySetupView::
    notifyThatBrowseForTestSettingsButtonHasBeenClicked() {}

void FacemaskStudySetupView::notifyThatPlayCalibrationButtonHasBeenClicked() {}
}
