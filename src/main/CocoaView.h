#ifndef CocoaView_h
#define CocoaView_h

#include <presentation/Presenter.h>
#import <Cocoa/Cocoa.h>

class CocoaView;

@interface ViewActions : NSObject
@property CocoaView * controller;
- (void) newTest;
- (void) openTest;
@end

class CocoaTestSetupView;

@interface SetupViewActions : NSObject
@property CocoaTestSetupView *controller;
- (void) confirmTestSetup;
- (void) browseForTargetList;
- (void) browseForMasker;
- (void) browseForCalibration;
- (void) playCalibration;
@end

class CocoaSubjectView;

@interface SubjectViewActions : NSObject
@property CocoaSubjectView *controller;
- (void) respond:(id)sender;
- (void) playTrial;
@end

class CocoaTesterView : public presentation::View::Tester {
    NSView *view_;
public:
    CocoaTesterView();
    void show() override;
    void hide() override;
    NSView *view();
};

class CocoaTestSetupView : public presentation::View::TestSetup {
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
    NSPopUpButton *conditionMenu;
    SetupViewActions *actions;
    EventListener *listener_{};
public:
    CocoaTestSetupView();
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
    void setTargetListDirectory(std::string) override;
    void setMasker(std::string) override;
    void setCalibrationFilePath(std::string) override;
    std::string session() override;
    std::string calibrationFilePath() override;
    void subscribe(EventListener *) override;
    NSView *view();
    void confirm();
    void browseForTargetList();
    void browseForMasker();
    void browseForCalibration();
    void playCalibration();
private:
    NSTextField *allocLabel(NSString *label, NSRect frame);
};

class CocoaSubjectView : public presentation::View::Subject {
    NSWindow *window;
    NSView *responseButtons;
    NSView *nextTrialButton;
    NSButton *lastButtonPressed;
    SubjectViewActions *actions;
    EventListener *listener_{};
public:
    CocoaSubjectView();
    std::string numberResponse() override;
    bool greenResponse() override;
    bool blueResponse() override;
    bool whiteResponse() override;
    void showResponseButtons() override;
    void hideResponseButtons() override;
    void showNextTrialButton() override;
    void hideNextTrialButton() override;
    void subscribe(EventListener *) override;
    void respond(id sender);
    void playTrial();
private:
    NSColor *lastPressedColor();
    void addButtonRow(NSColor *color, int row);
};

class CocoaView : public presentation::View {
    EventListener *listener{};
    NSApplication *app;
    NSWindow *window;
    NSPopUpButton *deviceMenu;
    ViewActions *actions;
    bool browseCancelled_{};
public:
    CocoaView();
    void subscribe(EventListener *) override;
    void eventLoop() override;
    DialogResponse showConfirmationDialog() override;
    void showErrorMessage(std::string) override;
    std::string browseForDirectory() override;
    bool browseCancelled() override;
    std::string browseForOpeningFile() override;
    std::string audioDevice() override;
    void populateAudioDeviceMenu(std::vector<std::string>) override;
    void newTest();
    void openTest();
    void addSubview(NSView *);
    
private:
    std::string browseModal(NSOpenPanel *panel);
};

#endif
