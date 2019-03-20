#ifndef CocoaView_h
#define CocoaView_h

#include <presentation/Presenter.h>
#import <Cocoa/Cocoa.h>

class CocoaView;

@interface ViewActions : NSObject
@property CocoaView *controller;
- (void) newTest;
- (void) openTest;
@end

class CocoaTestSetupView;

@interface SetupViewActions : NSObject
@property CocoaTestSetupView *controller;
- (void) confirmTestSetup;
@end

class CocoaTesterView;

@interface TesterViewActions : NSObject
@property CocoaTesterView *controller;
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
    std::string audioDevice() override;
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
    NSTextField *signalLevel_dB_SPL_label;
    NSTextField *signalLevel_dB_SPL_;
    NSTextField *maskerLevel_dB_SPL_label;
    NSTextField *maskerLevel_dB_SPL_;
    NSTextField *stimulusListDirectoryLabel;
    NSTextField *stimulusListDirectory_;
    NSTextField *maskerFilePath_label;
    NSTextField *maskerFilePath_;
    NSPopUpButton *conditionMenu;
    SetupViewActions *actions;
public:
    CocoaTestSetupView();
    NSView *view();
    void show() override;
    void hide() override;
    std::string maskerLevel_dB_SPL() override;
    std::string signalLevel_dB_SPL() override;
    std::string maskerFilePath() override;
    std::string stimulusListDirectory() override;
    std::string testerId() override;
    std::string subjectId() override;
    std::string condition() override;
    void populateConditionMenu(std::vector<std::string> items) override;
    void confirm();
    void becomeChild(CocoaView *);
};

class CocoaSubjectView : public presentation::View::Subject {
    NSWindow *window;
public:
    CocoaSubjectView();
    int numberResponse() override;
    bool greenResponse() override;
};

class CocoaView : public presentation::View {
    CocoaTestSetupView testSetupView_{};
    CocoaTesterView testerView_{};
    CocoaSubjectView subjectView_{};
    EventListener *listener{};
    NSApplication *app;
    NSWindow *window;
    ViewActions *actions;
public:
    CocoaView();
    void confirmTestSetup();
    void playTrial();
    void newTest();
    void openTest();
    void subscribe(EventListener *) override;
    void eventLoop() override;
    TestSetup *testSetup() override;
    Tester *tester() override;
    Subject *subject() override;
    DialogResponse showConfirmationDialog() override;
    void showErrorMessage(std::string) override;
};

#endif
