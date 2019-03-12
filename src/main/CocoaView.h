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
    NSPopUpButton *deviceMenu{[
        [NSPopUpButton alloc] initWithFrame:NSMakeRect(50, 50, 140, 30)
        pullsDown:NO
    ]};
    NSView *view_{
        [[NSView alloc] initWithFrame:NSMakeRect(50, 50, 500, 600)]
    };
public:
    CocoaTesterView();
    NSView *view();
    void show() override;
    void hide() override;
    std::string audioDevice() override;
    void populateAudioDeviceMenu(std::vector<std::string> items) override;
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
};

class CocoaSubjectView : public presentation::View::SubjectView {
    NSWindow *window;
public:
    CocoaSubjectView();
    int numberResponse() override;
    bool greenResponse() override;
};

class CocoaView : public presentation::View {
    NSApplication *app{[NSApplication sharedApplication]};
    NSWindow *window{
        [[NSWindow alloc] initWithContentRect:
            NSMakeRect(300, 500, 900, 800)
            styleMask:
                NSWindowStyleMaskClosable |
                NSWindowStyleMaskResizable |
                NSWindowStyleMaskTitled
            backing:NSBackingStoreBuffered
            defer:NO
        ]
    };
    NSView *tbdView{
        [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 500, 100)]
    };
    CocoaTestSetupView testSetupView_{};
    CocoaTesterView testerView_{};
    CocoaSubjectView subjectView_{};
    EventListener *listener{};
    ViewActions *actions{[ViewActions alloc]};
public:
    CocoaView();
    void confirmTestSetup();
    void playTrial();
    void newTest();
    void openTest();
    void subscribe(EventListener *listener_) override;
    void eventLoop() override;
    TestSetup *testSetup() override;
    Tester *tester() override;
    SubjectView *subject() override;
    DialogResponse showConfirmationDialog() override;
    void showErrorMessage(std::string s) override;
};

#endif
