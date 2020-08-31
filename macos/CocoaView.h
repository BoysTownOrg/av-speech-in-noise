#ifndef MACOS_MAIN_COCOAVIEW_H_
#define MACOS_MAIN_COCOAVIEW_H_

#include "MacOsTestSetupViewFactory.h"
#include <presentation/Presenter.hpp>
#import <Cocoa/Cocoa.h>
#include <unordered_map>

@class SetupViewActions;
@class CoordinateResponseMeasureViewActions;
@class ConsonantViewActions;
@class ExperimenterViewActions;

namespace av_speech_in_noise {
class CocoaExperimenterView : public View::Experimenter {
  public:
    explicit CocoaExperimenterView(NSViewController *);
    void subscribe(EventListener *) override;
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
    void exitTest();
    void playTrial();
    void submitFreeResponse();
    void submitPassedTrial();
    void submitFailedTrial();
    void submitCorrectKeywords();
    void acceptContinuingTesting();
    void declineContinuingTesting();

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
    EventListener *listener_{};
};

class CocoaTestSetupView : public View::TestSetup {
  public:
    explicit CocoaTestSetupView(NSViewController *);
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
    void subscribe(EventListener *) override;
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
    EventListener *listener_{};
};

class CocoaTestSetupViewFactory : public MacOsTestSetupViewFactory {
  public:
    auto make(NSViewController *c)
        -> std::unique_ptr<View::TestSetup> override {
        return std::make_unique<CocoaTestSetupView>(c);
    }
};

class CocoaConsonantView : public View::Consonant {
  public:
    explicit CocoaConsonantView(NSRect);
    void subscribe(EventListener *) override;
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

  private:
    std::unordered_map<id, std::string> consonants;
    NSWindow *window;
    NSStackView *responseButtons;
    NSView *readyButton;
    NSButton *lastButtonPressed{};
    ConsonantViewActions *actions;
    EventListener *listener_{};
};

class CocoaCoordinateResponseMeasureView
    : public View::CoordinateResponseMeasure {
  public:
    CocoaCoordinateResponseMeasureView(NSRect);
    auto numberResponse() -> std::string override;
    auto greenResponse() -> bool override;
    auto blueResponse() -> bool override;
    auto whiteResponse() -> bool override;
    void showResponseButtons() override;
    void hideResponseButtons() override;
    void showNextTrialButton() override;
    void hideNextTrialButton() override;
    void subscribe(EventListener *) override;
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
    EventListener *listener_{};
};

class CocoaView : public View {
  public:
    explicit CocoaView(NSApplication *, NSViewController *);
    void eventLoop() override;
    void showErrorMessage(std::string) override;
    auto browseForDirectory() -> std::string override;
    auto browseCancelled() -> bool override;
    auto browseForOpeningFile() -> std::string override;
    auto audioDevice() -> std::string override;
    void populateAudioDeviceMenu(std::vector<std::string>) override;
    void showCursor() override;

  private:
    auto browseModal(NSOpenPanel *panel) -> std::string;

    NSApplication *app;
    NSPopUpButton *audioDeviceMenu;
    bool browseCancelled_{};
};
}

#endif
