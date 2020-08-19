#ifndef MACOS_MAIN_FACEMASKSTUDYVIEW_H_
#define MACOS_MAIN_FACEMASKSTUDYVIEW_H_

#import <Cocoa/Cocoa.h>
#include <string>

@class FacemaskStudySetupViewActions;

namespace av_speech_in_noise {
class FacemaskStudySetupView {
  public:
    class EventListener {};
    explicit FacemaskStudySetupView(NSViewController *);
    void show();
    void hide();
    auto testSettingsFile() -> std::string;
    void setTestSettingsFile(std::string);
    void subscribe(EventListener *);
    void notifyThatConfirmButtonHasBeenClicked();
    void notifyThatBrowseForTestSettingsButtonHasBeenClicked();
    void notifyThatPlayCalibrationButtonHasBeenClicked();

  private:
    NSTextField *testSettingsField;
    FacemaskStudySetupViewActions *actions;
    EventListener *listener_{};
    NSViewController *controller;
};
}

#endif
