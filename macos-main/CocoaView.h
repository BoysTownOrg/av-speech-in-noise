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

  private:
    NSView *view_;
    NSView *nextTrialButton;
    NSView *evaluationButtons;
    NSView *responseSubmission;
    NSView *correctKeywordsSubmission;
    NSTextField *response_;
    NSTextField *correctKeywordsEntry_;
    NSTextField *displayedText_;
    NSTextField *secondaryDisplayedText_;
    NSButton *flagged_;
    NSButton *exitTestButton_;
    ExperimenterViewActions *actions;
    EventListener *listener_{};
};

class CocoaTestSetupView : public View::TestSetup {
  public:
    CocoaTestSetupView(NSRect);
    void show() override;
    void hide() override;
    std::string maskerLevel_dB_SPL() override;
    std::string calibrationLevel_dB_SPL() override;
    std::string maskerFilePath() override;
    std::string startingSnr_dB() override;
    std::string targetListDirectory() override;
    std::string testerId() override;
    std::string subjectId() override;
    std::string condition() override;
    void populateConditionMenu(std::vector<std::string> items) override;
    void populateMethodMenu(std::vector<std::string> items) override;
    void setTargetListDirectory(std::string) override;
    void setMasker(std::string) override;
    void setCalibrationFilePath(std::string) override;
    std::string session() override;
    std::string calibrationFilePath() override;
    void subscribe(EventListener *) override;
    std::string method() override;
    std::string trackSettingsFile() override;
    void setTrackSettingsFile(std::string) override;
    NSView *view();
    void confirm();
    void browseForTargetList();
    void browseForMasker();
    void browseForCalibration();
    void browseForTrackSettings();
    void playCalibration();
    void setMaskerLevel_dB_SPL(std::string);
    void setStartingSnr_dB(std::string);
    void setCalibrationLevel_dB_SPL(std::string);
    void setCalibration(std::string);

  private:
    void addSubview(NSView *subview);
    const char *stringValue(NSTextField *field);

    NSView *view_;
    NSTextField *subjectIdLabel;
    NSTextField *subjectId_;
    NSTextField *testerIdLabel;
    NSTextField *testerId_;
    NSTextField *sessionLabel;
    NSTextField *session_;
    NSTextField *maskerLevel_dB_SPL_label;
    NSTextField *maskerLevel_dB_SPL_;
    NSTextField *calibrationLevel_dB_SPL_label;
    NSTextField *calibrationLevel_dB_SPL_;
    NSTextField *startingSnr_dB_label;
    NSTextField *startingSnr_dB_;
    NSTextField *targetListDirectoryLabel;
    NSTextField *targetListDirectory_;
    NSTextField *maskerFilePath_label;
    NSTextField *maskerFilePath_;
    NSTextField *calibrationFilePath_label;
    NSTextField *calibrationFilePath_;
    NSTextField *trackSettingsFile_label;
    NSTextField *trackSettingsFile_;
    NSTextField *condition_label;
    NSPopUpButton *conditionMenu;
    NSTextField *method_label;
    NSPopUpButton *methodMenu;
    SetupViewActions *actions;
    EventListener *listener_{};
};

class CocoaSubjectView : public View::CoordinateResponseMeasure {
  public:
    CocoaSubjectView(NSRect);
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
    NSButton *lastButtonPressed;
    SubjectViewActions *actions;
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
    void addSubview(NSView *);
    void setDelegate(id<NSWindowDelegate>);
    void center();

  private:
    std::string browseModal(NSOpenPanel *panel);

    NSApplication *app;
    NSWindow *window;
    NSTextField *audioDevice_label;
    NSPopUpButton *deviceMenu;
    bool browseCancelled_{};
};
}

#endif
