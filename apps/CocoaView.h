#ifndef APPS_COCOAVIEW_H_
#define APPS_COCOAVIEW_H_

#include <presentation/Presenter.hpp>
#import <Cocoa/Cocoa.h>

@class SetupViewActions;
@class SubjectViewActions;
@class ExperimenterViewActions;
@class TestingViewActions;

namespace av_speech_in_noise {
class CocoaExperimenterView : public View::Experimenter {
    NSView *view_;
    NSTextView *displayedText_;
    NSButton *exitTestButton_;
    EventListener *listener_{};
    ExperimenterViewActions *actions;
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
};

class CocoaTestingView : public View::Testing {
    NSView *view_;
    NSView *nextTrialButton;
    NSView *evaluationButtons;
    NSView *responseSubmission;
    NSTextField *response_;
    NSButton *flagged_;
    EventListener *listener_{};
    TestingViewActions *actions;
public:
    CocoaTestingView(NSRect);
    NSView *view();
    void playTrial();
    void submitResponse();
    void submitPassedTrial();
    void submitFailedTrial();
    void exitTest();
    void subscribe(EventListener *) override;
    void showNextTrialButton() override;
    void hideNextTrialButton() override;
    void show() override;
    void hide() override;
    void showEvaluationButtons() override;
    void showResponseSubmission() override;
    std::string response() override;
    bool flagged() override;
    void hideResponseSubmission() override;
    void hideEvaluationButtons() override;
};

class CocoaTestSetupView : public View::TestSetup {
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
    const char * stringValue(NSTextField *field);
};

class CocoaSubjectView : public View::Subject {
    NSWindow *window;
    NSView *responseButtons;
    NSView *nextTrialButton;
    NSButton *lastButtonPressed;
    SubjectViewActions *actions;
    EventListener *listener_{};
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
};

class CocoaView : public View {
    NSApplication *app;
    NSWindow *window;
    NSTextField *audioDevice_label;
    NSPopUpButton *deviceMenu;
    bool browseCancelled_{};
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
};
}

#endif
