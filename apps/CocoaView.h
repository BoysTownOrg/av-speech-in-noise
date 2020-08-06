#ifndef MACOS_MAIN_COCOAVIEW_H_
#define MACOS_MAIN_COCOAVIEW_H_

#include <presentation/Presenter.hpp>
#import <Cocoa/Cocoa.h>

@class SetupViewActions;
@class CoordinateResponseMeasureViewActions;
@class ConsonantViewActions;
@class ExperimenterViewActions;

namespace av_speech_in_noise {
class CocoaExperimenterView : public View::Experimenter {
  public:
    CocoaExperimenterView(NSRect);
    NSView *view();
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
    std::string freeResponse() override;
    auto correctKeywords() -> std::string override;
    bool flagged() override;
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
    NSTextField *response_;
    NSTextField *correctKeywordsEntry_;
    NSTextField *displayedText_;
    NSTextField *secondaryDisplayedText_;
    NSTextField *continueTestingDialogMessage_;
    NSButton *flagged_;
    NSButton *exitTestButton_;
    NSButton *nextTrialButton_;
    ExperimenterViewActions *actions;
    EventListener *listener_{};
};

class CocoaTestSetupView : public View::TestSetup {
  public:
    CocoaTestSetupView(NSRect);
    void show() override;
    void hide() override;
    std::string testerId() override;
    std::string subjectId() override;
    std::string session() override;
    std::string testSettingsFile() override;
    auto startingSnr() -> std::string override;
    auto transducer() -> std::string override;
    auto rmeSetting() -> std::string override;
    void populateTransducerMenu(std::vector<std::string>) override;
    void setTestSettingsFile(std::string) override;
    void subscribe(EventListener *) override;
    NSView *view();
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

class CocoaConsonantView : public View::Consonant {
  public:
    explicit CocoaConsonantView(NSRect);
    void subscribe(EventListener *) override;
    void show() override;
    void showNextTrialButton() override;
    void hideNextTrialButton() override;
    void respond(id sender);
    void playTrial();

  private:
    NSWindow *window;
    NSView *nextTrialButton;
    NSButton *lastButtonPressed{};
    ConsonantViewActions *actions;
    EventListener *listener_{};
};

class CocoaCoordinateResponseMeasureView
    : public View::CoordinateResponseMeasure {
  public:
    CocoaCoordinateResponseMeasureView(NSRect);
    std::string numberResponse() override;
    bool greenResponse() override;
    bool blueResponse() override;
    bool whiteResponse() override;
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
    NSColor *lastPressedColor();
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
    CocoaView(NSRect);
    void eventLoop() override;
    void showErrorMessage(std::string) override;
    std::string browseForDirectory() override;
    bool browseCancelled() override;
    std::string browseForOpeningFile() override;
    std::string audioDevice() override;
    void populateAudioDeviceMenu(std::vector<std::string>) override;
    void setDelegate(id<NSWindowDelegate>);
    void center();
    auto testSetup() -> View::TestSetup &;
    auto experimenter() -> View::Experimenter &;

  private:
    std::string browseModal(NSOpenPanel *panel);

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
