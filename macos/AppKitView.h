#ifndef MACOS_MAIN_COCOAVIEW_H_
#define MACOS_MAIN_COCOAVIEW_H_

#include "AppKitTestSetupUIFactory.h"
#include <presentation/Consonant.hpp>
#include <presentation/CoordinateResponseMeasure.hpp>
#include <presentation/FreeResponse.hpp>
#include <presentation/PassFail.hpp>
#include <presentation/CorrectKeywords.hpp>
#include <presentation/TestSetupImpl.hpp>
#include <presentation/TestImpl.hpp>
#include <presentation/SessionControllerImpl.hpp>
#import <AppKit/AppKit.h>
#include <unordered_map>

@class SetupViewActions;
@class CoordinateResponseMeasureViewActions;
@class ConsonantViewActions;
@class ExperimenterViewActions;
@class FreeResponseViewActions;

namespace av_speech_in_noise {
class AppKitTestUI : public TestView,
                     public TestControl,
                     public FreeResponseControl,
                     public FreeResponseView,
                     public CorrectKeywordsControl,
                     public CorrectKeywordsView,
                     public PassFailControl,
                     public PassFailView {
  public:
    explicit AppKitTestUI(NSViewController *);
    void attach(TestControl::Observer *) override;
    void attach(FreeResponseControl::Observer *) override;
    void attach(CorrectKeywordsControl::Observer *) override;
    void attach(PassFailControl::Observer *) override;
    void showExitTestButton() override;
    void hideExitTestButton() override;
    void show() override;
    void hide() override;
    void display(std::string) override;
    void secondaryDisplay(std::string) override;
    void showNextTrialButton() override;
    void hideNextTrialButton() override;
    void showEvaluationButtons() override;
    void showFreeResponseSubmission() override;
    auto freeResponse() -> std::string override;
    auto correctKeywords() -> std::string override;
    auto flagged() -> bool override;
    void hideFreeResponseSubmission() override;
    void hideEvaluationButtons() override;
    void showCorrectKeywordsSubmission() override;
    void hideCorrectKeywordsSubmission() override;
    void showContinueTestingDialog() override;
    void hideContinueTestingDialog() override;
    void setContinueTestingDialogMessage(const std::string &) override;
    void clearFreeResponse() override;
    void notifyThatExitTestButtonHasBeenClicked();
    void notifyThatPlayTrialButtonHasBeenClicked();
    void notifyThatSubmitFreeResponseButtonHasBeenClicked();
    void notifyThatCorrectButtonHasBeenClicked();
    void notifyThatIncorrectButtonHasBeenClicked();
    void notifyThatSubmitCorrectKeywordsButtonHasBeenClicked();
    void notifyThatContinueTestingButtonHasBeenClicked();
    void notifyThatDeclineContinueTestingButtonHasBeenClicked();

  private:
    NSViewController *viewController;
    NSStackView *freeResponseView{};
    NSStackView *correctKeywordsView{};
    NSStackView *evaluationButtons{};
    NSWindow *continueTestingDialog;
    NSTextField *continueTestingDialogField;
    NSTextField *primaryTextField;
    NSTextField *secondaryTextField;
    NSTextField *freeResponseField;
    NSTextField *correctKeywordsField;
    NSButton *freeResponseFlaggedButton;
    NSButton *exitTestButton;
    NSButton *nextTrialButton;
    ExperimenterViewActions *actions;
    FreeResponseViewActions *freeResponseActions;
    TestControl::Observer *observer{};
    FreeResponseControl::Observer *freeResponseObserver{};
    CorrectKeywordsControl::Observer *correctKeywordsObserver{};
    PassFailControl::Observer *passFailObserver{};
};

class AppKitTestSetupUI : public TestSetupUI {
  public:
    explicit AppKitTestSetupUI(NSViewController *);
    void show() override;
    void hide() override;
    auto testerId() -> std::string override;
    auto subjectId() -> std::string override;
    auto session() -> std::string override;
    auto testSettingsFile() -> std::string override;
    auto startingSnr() -> std::string override;
    auto transducer() -> std::string override;
    auto rmeSetting() -> std::string override;
    void populateTransducerMenu(std::vector<std::string>) override;
    void setTestSettingsFile(std::string) override;
    void attach(Observer *) override;
    void notifyThatConfirmButtonHasBeenClicked();
    void notifyThatBrowseForTestSettingsButtonHasBeenClicked();
    void notifyThatPlayCalibrationButtonHasBeenClicked();

  private:
    NSViewController *viewController;
    NSTextField *subjectIdField;
    NSTextField *testerIdField;
    NSTextField *sessionField;
    NSTextField *rmeSettingField;
    NSPopUpButton *transducerMenu;
    NSTextField *testSettingsField;
    NSTextField *startingSnrField;
    SetupViewActions *actions;
    Observer *observer{};
};

class AppKitTestSetupUIFactoryImpl : public AppKitTestSetupUIFactory {
  public:
    auto make(NSViewController *c) -> std::unique_ptr<TestSetupUI> override {
        return std::make_unique<AppKitTestSetupUI>(c);
    }
};

class AppKitConsonantUI : public ConsonantTaskView,
                          public ConsonantTaskControl {
  public:
    explicit AppKitConsonantUI(NSRect);
    void attach(Observer *) override;
    void show() override;
    void hide() override;
    void showResponseButtons() override;
    void hideResponseButtons() override;
    void showReadyButton() override;
    void hideReadyButton() override;
    auto consonant() -> std::string override;
    void hideCursor() override;
    void notifyThatResponseButtonHasBeenClicked(id sender);
    void notifyThatReadyButtonHasBeenClicked();
    void showCursor() override;

  private:
    std::unordered_map<void *, std::string> consonants;
    NSWindow *window;
    NSStackView *responseButtons;
    NSView *readyButton;
    NSButton *lastButtonPressed{};
    ConsonantViewActions *actions;
    Observer *listener_{};
};

class AppKitCoordinateResponseMeasureUI
    : public CoordinateResponseMeasureControl,
      public CoordinateResponseMeasureView {
  public:
    AppKitCoordinateResponseMeasureUI(NSRect);
    auto numberResponse() -> std::string override;
    auto greenResponse() -> bool override;
    auto blueResponse() -> bool override;
    auto whiteResponse() -> bool override;
    void showResponseButtons() override;
    void hideResponseButtons() override;
    void showNextTrialButton() override;
    void hideNextTrialButton() override;
    void attach(Observer *) override;
    void show() override;
    void hide() override;
    void notifyThatResponseButtonHasBeenClicked(id sender);
    void notifyThatReadyButtonHasBeenClicked();

  private:
    void addNextTrialButton();
    auto lastPressedColor() -> NSColor *;
    void addNumberButton(NSColor *color, int number, int row, std::size_t col);
    void addButtonRow(NSColor *color, int row);

    NSWindow *window;
    NSView *responseButtons;
    NSView *nextTrialButton;
    NSButton *lastButtonPressed{};
    CoordinateResponseMeasureViewActions *actions;
    Observer *listener_{};
};

class AppKitView : public SessionView {
  public:
    explicit AppKitView(NSApplication *, NSViewController *);
    void eventLoop() override;
    void showErrorMessage(std::string) override;
    auto browseForDirectory() -> std::string override;
    auto browseCancelled() -> bool override;
    auto browseForOpeningFile() -> std::string override;
    auto audioDevice() -> std::string override;
    void populateAudioDeviceMenu(std::vector<std::string>) override;

  private:
    auto browseModal(NSOpenPanel *panel) -> std::string;

    NSApplication *app;
    NSPopUpButton *audioDeviceMenu;
    bool browseCancelled_{};
};
}

#endif
