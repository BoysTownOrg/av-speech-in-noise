#ifndef AV_SPEECH_IN_NOISE_MACOS_FACEMASKSTUDYSETUPVIEW_H_
#define AV_SPEECH_IN_NOISE_MACOS_FACEMASKSTUDYSETUPVIEW_H_

#include "AppKitTestSetupUIFactory.h"
#include <av-speech-in-noise/Model.hpp>
#include <map>

@class FacemaskStudySetupViewActions;

namespace av_speech_in_noise {
class FacemaskStudySetupView : public TestSetupUI {
  public:
    explicit FacemaskStudySetupView(NSViewController *);
    void show() override;
    void hide() override;
    auto testSettingsFile() -> std::string override;
    void attach(Observer *) override;
    void populateTransducerMenu(std::vector<std::string>) override {}
    auto startingSnr() -> std::string override;
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
    std::map<std::string, LocalUrl> conditionUrls;
    NSTextField *subjectIdField;
    NSPopUpButton *condition;
    FacemaskStudySetupViewActions *actions;
    Observer *listener_{};
    NSViewController *controller;
};

class FacemaskStudySetupViewFactory : public AppKitTestSetupUIFactory {
  public:
    auto make(NSViewController *c) -> std::unique_ptr<TestSetupUI> override {
        return std::make_unique<FacemaskStudySetupView>(c);
    }
};
}

#endif
