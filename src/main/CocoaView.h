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
- (void) browseForStimulusList;
- (void) browseForMasker;
@end

class CocoaTesterView;

@interface TesterViewActions : NSObject
@property CocoaTesterView *controller;
- (void) playTrial;
@end

class CocoaSubjectView;

@interface SubjectViewActions : NSObject
@property CocoaSubjectView *controller;
- (void) respond:(id)sender;
- (void) playTrial;
@end

class CocoaTesterView : public presentation::View::Tester {
    CocoaView *parent_;
    NSPopUpButton *deviceMenu;
    NSView *view_;
    TesterViewActions *actions;
public:
    CocoaTesterView();
    NSView *view();
    void show() override;
    void hide() override;
    void populateAudioDeviceMenu(std::vector<std::string>) override;
    void playTrial();
    void becomeChild(CocoaView *);
};


class CocoaTestSetupView : public presentation::View::TestSetup {
    CocoaView *parent_;
    NSView *view_;
    NSTextField *subjectIdLabel;
    NSTextField *subjectId_;
    NSTextField *testerIdLabel;
    NSTextField *testerId_;
    NSTextField *sessionLabel;
    NSTextField *session_;
    NSTextField *signalLevel_dB_SPL_label;
    NSTextField *signalLevel_dB_SPL_;
    NSTextField *startingSnr_dB_label;
    NSTextField *startingSnr_dB_;
    NSTextField *stimulusListDirectoryLabel;
    NSTextField *stimulusListDirectory_;
    NSTextField *maskerFilePath_label;
    NSTextField *maskerFilePath_;
    NSTextField *calibrationFilePath_label;
    NSTextField *calibrationFilePath_;
    NSPopUpButton *conditionMenu;
    SetupViewActions *actions;
public:
    CocoaTestSetupView();
    NSView *view();
    void show() override;
    void hide() override;
    std::string signalLevel_dB_SPL() override;
    std::string maskerFilePath() override;
    std::string startingSnr_dB() override;
    std::string targetListDirectory() override;
    std::string testerId() override;
    std::string subjectId() override;
    std::string condition() override;
    void populateConditionMenu(std::vector<std::string> items) override;
    void setStimulusList(std::string) override;
    void setMasker(std::string) override;
    std::string session() override;
    std::string calibrationFilePath() override;
    void confirm();
    void browseForStimulusList();
    void browseForMasker();
    void becomeChild(CocoaView *);
private:
    NSTextField *allocLabel(NSString *label, NSRect frame);
};

class CocoaSubjectView : public presentation::View::Subject {
    CocoaView *parent_;
    NSWindow *window;
    NSView *responseButtons;
    NSView *nextTrialButton;
    NSButton *lastButtonPressed;
    SubjectViewActions *actions;
public:
    CocoaSubjectView();
    void respond(id sender);
    void playTrial();
    std::string numberResponse() override;
    bool greenResponse() override;
    bool blueResponse() override;
    bool grayResponse() override;
    void showResponseButtons() override;
    void hideResponseButtons() override;
    void showNextTrialButton() override;
    void hideNextTrialButton() override;
    void becomeChild(CocoaView *);
private:
    NSColor *lastPressedColor();
    void addButtonRow(NSColor *color, int row);
};

class CocoaView : public presentation::View {
    CocoaTestSetupView testSetupView_{};
    CocoaTesterView testerView_{};
    CocoaSubjectView subjectView_{};
    EventListener *listener{};
    NSApplication *app;
    NSWindow *window;
    NSPopUpButton *deviceMenu;
    ViewActions *actions;
    bool browseCancelled_{};
public:
    CocoaView();
    void confirmTestSetup();
    void browseForStimulusList();
    void browseForMasker();
    void playTrial();
    void newTest();
    void openTest();
    void submitResponse();
    void subscribe(EventListener *) override;
    void eventLoop() override;
    TestSetup *testSetup() override;
    Tester *tester() override;
    Subject *subject() override;
    DialogResponse showConfirmationDialog() override;
    void showErrorMessage(std::string) override;
    std::string browseForDirectory() override;
    bool browseCancelled() override;
    std::string browseForOpeningFile() override;
    std::string audioDevice() override;
    
private:
    std::string browseModal(NSOpenPanel *panel);
};

#endif
