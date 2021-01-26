#ifndef MACOS_MAIN_COCOAVIEW_H_
#define MACOS_MAIN_COCOAVIEW_H_

#include "AppKitTestSetupUIFactory.h"
#include <presentation/Consonant.hpp>
#include <presentation/CoordinateResponseMeasure.hpp>
#include <presentation/FreeResponse.hpp>
#include <presentation/ChooseKeywords.hpp>
#include <presentation/PassFail.hpp>
#include <presentation/CorrectKeywords.hpp>
#include <presentation/TestSetupImpl.hpp>
#include <presentation/TestImpl.hpp>
#include <presentation/SessionController.hpp>
#import <AppKit/AppKit.h>
#include <unordered_map>

@class TestSetupUIActions;
@class CoordinateResponseMeasureUIActions;
@class ConsonantUIActions;
@class TestUIActions;
@class FreeResponseUIActions;
@class ChooseKeywordsUIActions;
@class CorrectKeywordsUIActions;
@class PassFailUIActions;

namespace av_speech_in_noise {
class ChooseKeywordsUI : public ChooseKeywordsControl,
                         public ChooseKeywordsView {
  public:
    explicit ChooseKeywordsUI(NSViewController *);
    void attach(Observer *) override;
    auto firstKeywordCorrect() -> bool override;
    auto secondKeywordCorrect() -> bool override;
    auto thirdKeywordCorrect() -> bool override;
    auto flagged() -> bool override;
    void markFirstKeywordIncorrect() override;
    void markSecondKeywordIncorrect() override;
    void markThirdKeywordIncorrect() override;
    void markFirstKeywordCorrect() override;
    void markSecondKeywordCorrect() override;
    void markThirdKeywordCorrect() override;
    void hideResponseSubmission() override;
    void showResponseSubmission() override;
    void setFirstKeywordButtonText(const std::string &s) override;
    void setSecondKeywordButtonText(const std::string &s) override;
    void setThirdKeywordButtonText(const std::string &s) override;
    void setTextPrecedingFirstKeywordButton(const std::string &s) override;
    void setTextFollowingFirstKeywordButton(const std::string &s) override;
    void setTextFollowingSecondKeywordButton(const std::string &s) override;
    void setTextFollowingThirdKeywordButton(const std::string &s) override;

  private:
    NSStackView *responseView{};
    NSTextField *textFieldBeforeFirstKeywordButton;
    NSTextField *textFieldAfterFirstKeywordButton;
    NSTextField *textFieldAfterSecondKeywordButton;
    NSTextField *textFieldAfterThirdKeywordButton;
    NSButton *firstKeywordButton;
    NSButton *secondKeywordButton;
    NSButton *thirdKeywordButton;
    NSButton *flaggedButton;
    ChooseKeywordsUIActions *actions;
};

class FreeResponseUI : public FreeResponseView, public FreeResponseControl {
  public:
    explicit FreeResponseUI(NSViewController *);
    void attach(Observer *) override;
    void showFreeResponseSubmission() override;
    void hideFreeResponseSubmission() override;
    auto freeResponse() -> std::string override;
    auto flagged() -> bool override;
    void clearFreeResponse() override;
    void clearFlag() override;

  private:
    NSStackView *responseView{};
    NSTextField *responseField;
    NSButton *flaggedButton;
    FreeResponseUIActions *actions;
};

class CorrectKeywordsUI : public CorrectKeywordsControl,
                          public CorrectKeywordsView {
  public:
    explicit CorrectKeywordsUI(NSViewController *);
    void attach(Observer *) override;
    auto correctKeywords() -> std::string override;
    void showCorrectKeywordsSubmission() override;
    void hideCorrectKeywordsSubmission() override;

  private:
    NSStackView *responseView{};
    NSTextField *responseField;
    CorrectKeywordsUIActions *actions;
};

class PassFailUI : public PassFailControl, public PassFailView {
  public:
    explicit PassFailUI(NSViewController *);
    void attach(Observer *) override;
    void showEvaluationButtons() override;
    void hideEvaluationButtons() override;

  private:
    NSStackView *responseView{};
    PassFailUIActions *actions;
};

class AppKitTestUI : public TestView, public TestControl {
  public:
    explicit AppKitTestUI(NSViewController *);
    void attach(TestControl::Observer *) override;
    void showExitTestButton() override;
    void hideExitTestButton() override;
    void show() override;
    void hide() override;
    void display(std::string) override;
    void secondaryDisplay(std::string) override;
    void showNextTrialButton() override;
    void hideNextTrialButton() override;
    void showContinueTestingDialog() override;
    void hideContinueTestingDialog() override;
    void setContinueTestingDialogMessage(const std::string &) override;

  private:
    NSViewController *viewController;
    NSWindow *continueTestingDialog;
    NSTextField *continueTestingDialogField;
    NSTextField *primaryTextField;
    NSTextField *secondaryTextField;
    NSButton *exitTestButton;
    NSButton *nextTrialButton;
    TestUIActions *actions;
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
    void setTestSettingsFile(std::string_view) override;
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
    TestSetupUIActions *actions;
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
    ConsonantUIActions *actions;
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
    CoordinateResponseMeasureUIActions *actions;
    Observer *listener_{};
};

class AppKitSessionUI : public SessionView, public SessionControl {
  public:
    explicit AppKitSessionUI(NSApplication *, NSViewController *);
    void eventLoop() override;
    void showErrorMessage(std::string_view) override;
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
