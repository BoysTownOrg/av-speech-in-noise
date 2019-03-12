#ifndef CocoaView_h
#define CocoaView_h

#include <presentation/Presenter.h>
#import <Cocoa/Cocoa.h>

class CocoaView;

@interface ViewActions : NSObject
@property CocoaView *controller;
- (void) newTest;
- (void) openTest;
- (void) confirmTestSetup;
- (void) playTrial;
@end

class CocoaTesterView : public presentation::View::Tester {
    NSPopUpButton *deviceMenu;
    NSView *view_;
public:
    CocoaTesterView();
    NSView *view();
    void show() override;
    void hide() override;
    std::string audioDevice() override;
    void populateAudioDeviceMenu(std::vector<std::string>) override;
};


class CocoaTestSetupView : public presentation::View::TestSetup {
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
};

class CocoaSubjectView : public presentation::View::SubjectView {
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
    NSView *tbdView;
    NSView *confirmTestSetupButtonView_;
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
    SubjectView *subject() override;
    DialogResponse showConfirmationDialog() override;
    void showErrorMessage(std::string) override;
    void showConfirmTestSetupButton() override;
    void hideConfirmTestSetupButton() override;
};

#endif
