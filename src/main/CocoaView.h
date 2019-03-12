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
    NSView *view{
        [[NSView alloc] initWithFrame:NSMakeRect(50, 50, 500, 600)]
    };
public:
    CocoaTesterView(NSWindow *window) {
        [view setHidden:YES];
        [view addSubview:deviceMenu];
        [window.contentView addSubview:view];
    }
    
    void show() override {
        [view setHidden:NO];
    }
    
    void hide() override {
        [view setHidden:YES];
    }
    
    std::string audioDevice() override {
        return [deviceMenu.titleOfSelectedItem UTF8String];
    }
    
    void populateAudioDeviceMenu(std::vector<std::string> items) override {
        for (const auto &item : items) {
            auto title = [NSString stringWithCString:
                item.c_str()
                encoding:[NSString defaultCStringEncoding]
            ];
            [deviceMenu addItemWithTitle: title];
        }
    }
};

class CocoaTestSetupView : public presentation::View::TestSetup {
    NSView *view{
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
    CocoaTestSetupView(NSWindow *window) {
        [view setHidden:YES];
        [view addSubview:subjectIdLabel];
        [view addSubview:subjectId_];
        [view addSubview:testerIdLabel];
        [view addSubview:testerId_];
        [view addSubview:signalLevel_dB_SPL_label];
        [view addSubview:signalLevel_dB_SPL_];
        [view addSubview:maskerLevel_dB_SPL_label];
        [view addSubview:maskerLevel_dB_SPL_];
        [view addSubview:stimulusListDirectoryLabel];
        [view addSubview:stimulusListDirectory_];
        [view addSubview:maskerFilePath_label];
        [view addSubview:maskerFilePath_];
        [window.contentView addSubview:view];
        stimulusListDirectory_.stringValue =
            @"/Users/basset/Documents/maxdetection/Stimuli/Video/List_Detection";
        maskerFilePath_.stringValue =
            @"/Users/basset/Documents/maxdetection/Stimuli/Masker/L1L2_EngEng.wav";
    }
    
    void show() override {
        [view setHidden:NO];
    }
    
    void hide() override {
        [view setHidden:YES];
    }
    
    std::string maskerLevel_dB_SPL() override {
        return [maskerLevel_dB_SPL_.stringValue UTF8String];
    }
    
    std::string signalLevel_dB_SPL() override {
        return [signalLevel_dB_SPL_.stringValue UTF8String];
    }
    
    std::string maskerFilePath() override {
        return [maskerFilePath_.stringValue UTF8String];
    }
    
    std::string stimulusListDirectory() override {
        return [stimulusListDirectory_.stringValue UTF8String];
    }
    
    std::string testerId() override {
        return [testerId_.stringValue UTF8String];
    }
    
    std::string subjectId() override {
        return [subjectId_.stringValue UTF8String];
    }
    
    std::string condition() override {
        return "";
    }
    
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
    AvFoundationStimulusPlayer player{};
    // Defer may be critical here...
    NSWindow *videoWindow{
        [[NSWindow alloc]
            initWithContentRect: NSMakeRect(400, 400, 0, 0)
            styleMask:NSWindowStyleMaskBorderless
            backing:NSBackingStoreBuffered
            defer:YES
        ]
    };
public:
    CocoaSubjectView() {
        player.setWindow(videoWindow);
        [videoWindow makeKeyAndOrderFront:nil];
    }
    
    recognition_test::StimulusPlayer *stimulusPlayer() {
        return &player;
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
        [[NSView alloc] initWithFrame:NSMakeRect(100, 100, 500, 600)]
    };
    CocoaTestSetupView testSetupView_{window};
    CocoaTesterView testerView_{window};
    CocoaSubjectView subjectView_{};
    EventListener *listener{};
    ViewActions *actions{[ViewActions alloc]};
public:
    CocoaView() {
        app.mainMenu = [[NSMenu alloc] init];
        auto appMenu = [[NSMenuItem alloc] init];
        [app.mainMenu addItem:appMenu];
        auto fileMenu = [[NSMenuItem alloc] init];
        [app.mainMenu addItem:fileMenu];
        auto appSubMenu = [[NSMenu alloc] init];
        [appSubMenu addItemWithTitle:
            @"Quit"
            action:@selector(terminate:)
            keyEquivalent:@"q"
        ];
        [appMenu setSubmenu:appSubMenu];
        auto fileSubMenu = [[NSMenu alloc] initWithTitle:@"File"];
        auto newTestItem = [[NSMenuItem alloc] initWithTitle:
            @"New Test..."
            action:@selector(newTest)
            keyEquivalent:@"n"
        ];
        newTestItem.target = actions;
        [fileSubMenu addItem:newTestItem];
        auto openTestItem = [[NSMenuItem alloc] initWithTitle:
            @"Open Test..."
            action:@selector(openTest)
            keyEquivalent:@"o"
        ];
        openTestItem.target = actions;
        [fileSubMenu addItem:openTestItem];
        [fileMenu setSubmenu:fileSubMenu];
        const auto playTrialButton = [NSButton buttonWithTitle:
            @"Play Next Trial"
            target:actions
            action:@selector(playTrial)
        ];
        playTrialButton.target = actions;
        const auto confirmButton = [NSButton buttonWithTitle:
            @"Confirm"
            target:actions
            action:@selector(confirmTestSetup)
        ];
        confirmButton.target = actions;
        [tbdView addSubview:confirmButton];
        [tbdView addSubview:playTrialButton];
        [window.contentView addSubview:tbdView];
        actions.controller = this;
        [window makeKeyAndOrderFront:nil];
    }

    void confirmTestSetup() {
        listener->confirmTestSetup();
    }

    void playTrial() {
        listener->playTrial();
    }
    
    void newTest() {
        listener->newTest();
    }
    
    void openTest() {
        listener->openTest();
    }
    
    void subscribe(EventListener *listener_) override {
        listener = listener_;
    }
    
    void eventLoop() override {
        [app run];
    }
    
    TestSetup *testSetup() override {
        return &testSetupView_;
    }
    
    Tester *tester() override {
        return &testerView_;
    }
    
    DialogResponse showConfirmationDialog() override {
        const auto alert = [[NSAlert alloc] init];
        [alert setMessageText:@"Are you sure?"];
        [alert setInformativeText:@"huehuehue"];
        [alert addButtonWithTitle:@"Cancel"];
        [alert addButtonWithTitle:@"No"];
        [alert addButtonWithTitle:@"Yes"];
        switch([alert runModal]) {
            case NSAlertSecondButtonReturn:
                return DialogResponse::decline;
            case NSAlertThirdButtonReturn:
                return DialogResponse::accept;
            default:
                return DialogResponse::cancel;
        }
    }
    
    void showErrorMessage(std::string s) override {
        auto alert = [[NSAlert alloc] init];
        [alert setMessageText:@"Error."];
        auto errorMessage_ = [
            NSString stringWithCString:s.c_str()
            encoding:[NSString defaultCStringEncoding]
        ];
        [alert setInformativeText:errorMessage_];
        [alert addButtonWithTitle:@"Ok"];
        [alert runModal];
    }
    
    SubjectView *subject() override {
        return &subjectView_;
    }
};

@implementation ViewActions
@synthesize controller;
- (void)newTest {
    controller->newTest();
}

- (void)openTest {
    controller->openTest();
}

- (void)confirmTestSetup {
    controller->confirmTestSetup();
}

- (void)playTrial {
    controller->playTrial();
}
@end

#endif /* CocoaView_h */
