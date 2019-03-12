//
//  CocoaView.h
//  av-coordinated-response-measure
//
//  Created by Bashford, Seth on 3/12/19.
//

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
    NSView *view_{
        [[NSView alloc] initWithFrame:NSMakeRect(100, 100, 500, 600)]
    };
    NSTextField *subjectIdLabel{allocLabel(
        @"subject id:",
        NSMakeRect(10, 490, 140, 25))
    };
    NSTextField *subjectId_{
        [[NSTextField alloc]
            initWithFrame:NSMakeRect(155, 490, 150, 25)]
    };
    NSTextField *testerIdLabel{allocLabel(
        @"tester id:",
        NSMakeRect(10, 460, 140, 25))
    };
    NSTextField *testerId_{
        [[NSTextField alloc]
            initWithFrame:NSMakeRect(155, 460, 150, 25)]
    };
    NSTextField *signalLevel_dB_SPL_label{allocLabel(
        @"signal level (dB SPL):",
        NSMakeRect(10, 430, 140, 25))
    };
    NSTextField *signalLevel_dB_SPL_{
        [[NSTextField alloc]
            initWithFrame:NSMakeRect(155, 430, 150, 25)]
    };
    NSTextField *maskerLevel_dB_SPL_label{allocLabel(
        @"masker level (dB SPL):",
        NSMakeRect(10, 400, 140, 25))
    };
    NSTextField *maskerLevel_dB_SPL_{
        [[NSTextField alloc]
            initWithFrame:NSMakeRect(155, 400, 150, 25)]
    };
    NSTextField *stimulusListDirectoryLabel{allocLabel(
        @"stimulus directory:",
        NSMakeRect(10, 370, 140, 25))
    };
    NSTextField *stimulusListDirectory_{
        [[NSTextField alloc]
            initWithFrame:NSMakeRect(155, 370, 300, 25)]
    };
    NSTextField *maskerFilePath_label{allocLabel(
        @"masker file path:",
        NSMakeRect(10, 340, 140, 25))
    };
    NSTextField *maskerFilePath_{
        [[NSTextField alloc]
            initWithFrame:NSMakeRect(155, 340, 300, 25)]
    };
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
private:
    NSTextField *allocLabel(NSString *label, NSRect frame) {
        const auto text = [[NSTextField alloc] initWithFrame:frame];
        [text setStringValue:label];
        [text setBezeled:NO];
        [text setDrawsBackground:NO];
        [text setEditable:NO];
        [text setSelectable:NO];
        return text;
    }
};

class CocoaSubjectView : public presentation::View::SubjectView {
    // Defer may be critical here...
    NSWindow *window{
        [[NSWindow alloc]
            initWithContentRect: NSMakeRect(600, 400, 400, 400)
            styleMask:NSWindowStyleMaskBorderless
            backing:NSBackingStoreBuffered
            defer:YES
        ]
    };
public:
    CocoaSubjectView() {
        const auto greenButton = [NSButton buttonWithTitle:@"hello" target:nil action:nil];
        //greenButton.bezelStyle = NSBezelStyleTexturedSquare;
        greenButton.bordered = false;
        greenButton.wantsLayer = true;
        [[greenButton layer] setBackgroundColor:[[NSColor systemGreenColor] CGColor]];
        [window.contentView addSubview:greenButton];
        [window makeKeyAndOrderFront:nil];
    }
    
    int numberResponse() override {
        return 0;
    }
    
    bool greenResponse() override {
        return false;
    }
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

#endif /* CocoaView_h */
