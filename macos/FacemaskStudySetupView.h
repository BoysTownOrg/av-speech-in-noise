#ifndef MACOS_MAIN_FACEMASKSTUDYVIEW_H_
#define MACOS_MAIN_FACEMASKSTUDYVIEW_H_

#include <presentation/Presenter.hpp>
#import <Cocoa/Cocoa.h>
#include <string>

@class FacemaskStudySetupViewActions;

namespace av_speech_in_noise {
class FacemaskStudySetupView : public View::TestSetup {
  public:
    explicit FacemaskStudySetupView(NSViewController *);
    void show() override;
    void hide() override;
    auto testSettingsFile() -> std::string override;
    void setTestSettingsFile(std::string) override;
    void subscribe(EventListener *) override;
    void populateTransducerMenu(std::vector<std::string>) override {}
    auto startingSnr() -> std::string override { return {}; }
    auto testerId() -> std::string override { return {}; }
    auto subjectId() -> std::string override { return {}; }
    auto session() -> std::string override { return {}; }
    auto rmeSetting() -> std::string override { return {}; }
    auto transducer() -> std::string override { return {}; }
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
