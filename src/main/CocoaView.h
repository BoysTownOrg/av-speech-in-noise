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
    CocoaTesterView() {
        [view_ setHidden:YES];
        [view_ addSubview:deviceMenu];
    }
    
    NSView *view() {
        return view_;
    }
    
    void show() override;
    
    void hide() override {
        [view_ setHidden:YES];
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
    CocoaTestSetupView() {
        [view_ setHidden:YES];
        [view_ addSubview:subjectIdLabel];
        [view_ addSubview:subjectId_];
        [view_ addSubview:testerIdLabel];
        [view_ addSubview:testerId_];
        [view_ addSubview:signalLevel_dB_SPL_label];
        [view_ addSubview:signalLevel_dB_SPL_];
        [view_ addSubview:maskerLevel_dB_SPL_label];
        [view_ addSubview:maskerLevel_dB_SPL_];
        [view_ addSubview:stimulusListDirectoryLabel];
        [view_ addSubview:stimulusListDirectory_];
        [view_ addSubview:maskerFilePath_label];
        [view_ addSubview:maskerFilePath_];
        stimulusListDirectory_.stringValue =
            @"/Users/basset/Documents/maxdetection/Stimuli/Video/List_Detection";
        maskerFilePath_.stringValue =
            @"/Users/basset/Documents/maxdetection/Stimuli/Masker/L1L2_EngEng.wav";
    }
    
    NSView *view() {
        return view_;
    }
    
    void show() override {
        [view_ setHidden:NO];
    }
    
    void hide() override {
        [view_ setHidden:YES];
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
        const auto confirmButton = [NSButton buttonWithTitle:
            @"Confirm"
            target:actions
            action:@selector(confirmTestSetup)
        ];
        confirmButton.frame = NSMakeRect(0, 0, 130, 40);
        playTrialButton.frame = NSMakeRect(200, 0, 130, 40);
        [tbdView addSubview:confirmButton];
        [tbdView addSubview:playTrialButton];
        [window.contentView addSubview:tbdView];
        [window.contentView addSubview:testerView_.view()];
        [window.contentView addSubview:testSetupView_.view()];
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
    
    SubjectView *subject() override {
        return &subjectView_;
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
};

#endif /* CocoaView_h */
