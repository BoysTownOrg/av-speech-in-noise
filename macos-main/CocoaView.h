#ifndef MACOS_MAIN_COCOAVIEW_H_
#define MACOS_MAIN_COCOAVIEW_H_

#include <presentation/Presenter.hpp>
#import <Cocoa/Cocoa.h>

@class SetupViewActions;
@class SubjectViewActions;
@class ExperimenterViewActions;

namespace av_speech_in_noise {
class CocoaExperimenterView : public View::Experimenter {
  public:
    explicit CocoaExperimenterView(NSRect);
    auto view() -> NSView *;
    void exitTest();
    void subscribe(EventListener *) override;
    void showExitTestButton() override;
    void hideExitTestButton() override;
    void show() override;
    void hide() override;
    void display(std::string) override;
    void secondaryDisplay(std::string) override;
    void playTrial();
    void submitFreeResponse();
    void submitPassedTrial();
    void submitFailedTrial();
    void submitCorrectKeywords();
    void acceptContinuingTesting();
    void declineContinuingTesting();
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

  private:
    NSView *view_;
    NSView *evaluationButtons;
    NSView *responseSubmission;
    NSView *correctKeywordsSubmission;
    NSWindow *continueTestingDialog;
    NSTextField *continueTestingDialogMessage_;
    NSTextField *displayedText_;
    NSTextField *secondaryDisplayedText_;
    NSTextField *response_;
    NSTextField *correctKeywordsEntry_;
    NSButton *flagged_;
    NSButton *exitTestButton_;
    NSButton *nextTrialButton_;
    ExperimenterViewActions *actions;
    EventListener *listener_{};
};

class CocoaTestSetupView : public View::TestSetup {
  public:
    explicit CocoaTestSetupView(NSRect);
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
    auto view() -> NSView *;
    void confirm();
    void browseForTargetList();
    void browseForMasker();
    void browseForTestSettings();
    void playCalibration();
    void setMaskerLevel_dB_SPL(std::string);
    void setStartingSnr_dB(std::string);
    void setCalibrationLevel_dB_SPL(std::string);
    void setCalibration(std::string);

  private:
    NSView *view_;
    NSTextField *subjectIdLabel;
    NSTextField *subjectId_;
    NSTextField *testerIdLabel;
    NSTextField *testerId_;
    NSTextField *sessionLabel;
    NSTextField *session_;
    NSTextField *rmeSettingLabel;
    NSTextField *rmeSetting_;
    NSTextField *transducerLabel;
    NSPopUpButton *transducerMenu;
    NSTextField *testSettingsFile_label;
    NSTextField *testSettingsFile_;
    NSTextField *startingSnr_label;
    NSTextField *startingSnr_;
    SetupViewActions *actions;
    EventListener *listener_{};
};

class CocoaSubjectView : public View::CoordinateResponseMeasure {
  public:
    explicit CocoaSubjectView(NSRect);
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
    void respond(id sender);
    void playTrial();

  private:
    void addNextTrialButton();
    auto lastPressedColor() -> NSColor *;
    void addNumberButton(NSColor *color, int number, int row, std::size_t col);
    void addButtonRow(NSColor *color, int row);

    NSWindow *window;
    NSView *responseButtons;
    NSView *nextTrialButton;
    NSButton *lastButtonPressed;
    SubjectViewActions *actions;
    EventListener *listener_{};
};

class CocoaView : public View {
  public:
    explicit CocoaView(NSRect);
    void eventLoop() override;
    void showErrorMessage(std::string) override;
    auto browseForDirectory() -> std::string override;
    auto browseCancelled() -> bool override;
    auto browseForOpeningFile() -> std::string override;
    auto audioDevice() -> std::string override;
    void populateAudioDeviceMenu(std::vector<std::string>) override;
    void setDelegate(id<NSWindowDelegate>);
    void center();
    auto testSetup() -> View::TestSetup &;
    auto experimenter() -> View::Experimenter &;

  private:
    auto browseModal(NSOpenPanel *panel) -> std::string;

    CocoaTestSetupView testSetup_;
    CocoaExperimenterView experimenter_;
    NSApplication *app;
    NSWindow *window;
    NSView *view;
    NSTextField *audioDevice_label;
    NSPopUpButton *deviceMenu;
    bool browseCancelled_{};
};
}

#endif
