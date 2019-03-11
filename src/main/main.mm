#include <presentation/Presenter.h>
#include <recognition-test/Model.hpp>
#include <stimulus-list/RandomizedStimulusList.hpp>
#include <stimulus-list/FileFilterDecorator.hpp>
#import <AVFoundation/AVFoundation.h>
#import <Cocoa/Cocoa.h>
#include <gsl/gsl>

class CoreAudioDevice {
    std::vector<AudioObjectID> devices{};
public:
    CoreAudioDevice() {
        AudioObjectPropertyAddress address = {
            kAudioHardwarePropertyDevices,
            kAudioObjectPropertyScopeGlobal,
            kAudioObjectPropertyElementMaster
        };
        UInt32 dataSize{};
        if (kAudioHardwareNoError !=
            AudioObjectGetPropertyDataSize(
                kAudioObjectSystemObject,
                &address,
                0,
                nullptr,
                &dataSize
            )
        )
            throw std::runtime_error{"Bad news bears"};
        const auto _deviceCount = dataSize / sizeof(AudioDeviceID);
        devices.resize(_deviceCount);
        if (kAudioHardwareNoError !=
            AudioObjectGetPropertyData(
                kAudioObjectSystemObject,
                &address,
                0,
                nullptr,
                &dataSize,
                &devices[0]
            )
        )
            throw std::runtime_error{"Bad news bears"};
    }

    int deviceCount() {
        return gsl::narrow<int>(devices.size());
    }

    std::string description(int device) {
        AudioObjectPropertyAddress address = {
            kAudioObjectPropertyName,
            kAudioObjectPropertyScopeGlobal,
            kAudioObjectPropertyElementMaster
        };
        CFStringRef deviceName{};
        UInt32 dataSize = sizeof(CFStringRef);
        if (kAudioHardwareNoError !=
            AudioObjectGetPropertyData(
                devices.at(gsl::narrow<decltype(devices)::size_type>(device)),
                &address,
                0,
                nullptr,
                &dataSize,
                &deviceName
            )
        )
            throw std::runtime_error{"Bad news bears"};
        char buffer[128];
        CFStringGetCString(deviceName, buffer, sizeof(buffer), kCFStringEncodingUTF8);
        return buffer;
    }
};

static AVURLAsset *makeAvAsset(std::string filePath) {
    const auto url = [NSURL URLWithString:
        [NSString stringWithFormat:@"file://%@/",
            [
                [NSString stringWithCString:
                    filePath.c_str()
                    encoding:[NSString defaultCStringEncoding]
                ]
                stringByAddingPercentEncodingWithAllowedCharacters:
                    [NSCharacterSet URLQueryAllowedCharacterSet]
            ]
        ]
    ];
    return [AVURLAsset URLAssetWithURL:url options:nil];
}

class AvFoundationStimulusPlayer;

@interface StimulusPlayerActions : NSObject
@property AvFoundationStimulusPlayer *controller;
- (void) playbackComplete;
@end

class AvFoundationStimulusPlayer : public recognition_test::StimulusPlayer {
    EventListener *listener{};
    StimulusPlayerActions *actions{[StimulusPlayerActions alloc]};
    NSWindow *videoWindow{};
    AVPlayer *player{[AVPlayer playerWithPlayerItem:nil]};
    AVPlayerLayer *playerLayer{[AVPlayerLayer playerLayerWithPlayer:player]};
public:
    AvFoundationStimulusPlayer() {
        actions.controller = this;
    }
    
    void setWindow(NSWindow *window) {
        videoWindow = window;
        [videoWindow.contentView setWantsLayer:YES];
        [videoWindow.contentView.layer addSublayer:playerLayer];
    }
    
    void subscribe(EventListener *listener_) override {
        listener = listener_;
    }
    
    void playbackComplete() {
        listener->playbackComplete();
    }
    
    void play() override {
        [player play];
    }
    
    void loadFile(std::string filePath) override {
        const auto asset = makeAvAsset(filePath);
        [player replaceCurrentItemWithPlayerItem:
            [AVPlayerItem playerItemWithAsset:asset]];
        [videoWindow setContentSize:NSSizeFromCGSize(
            [asset tracksWithMediaType:AVMediaTypeVideo].firstObject.naturalSize)];
        [playerLayer setFrame:videoWindow.contentView.bounds];
        [NSNotificationCenter.defaultCenter addObserver:
            actions
            selector:@selector(playbackComplete)
            name:AVPlayerItemDidPlayToEndTimeNotification
            object:player.currentItem
        ];
    }
    
    void setDevice(int index) override {
        ;
    }
};

@implementation StimulusPlayerActions
@synthesize controller;
- (void)playbackComplete {
    controller->playbackComplete();
}
@end

class CocoaView;
class CocoaTestSetupView;
class CocoaTesterView;

@interface ViewActions : NSObject
@property CocoaView *controller;
- (void) newTest;
- (void) openTest;
@end

@interface SetupViewActions : NSObject
@property CocoaTestSetupView *controller;
- (void) confirmTestSetup;
@end

@interface TesterViewActions : NSObject
@property CocoaTesterView *controller;
- (void) playTrial;
@end

class CocoaTesterView : public presentation::View::Tester {
    EventListener *listener{};
    NSPopUpButton *deviceMenu;
    NSView *view{
        [[NSView alloc] initWithFrame:NSMakeRect(50, 50, 500, 600)]
    };
    TesterViewActions *actions{[TesterViewActions alloc]};
public:
    CocoaTesterView(NSWindow *window) :
    deviceMenu{[[NSPopUpButton alloc] initWithFrame:NSMakeRect(50, 50, 140, 30) pullsDown:NO]}
    {
        [view setHidden:YES];
        const auto playTrialButton = [NSButton buttonWithTitle:
            @"Play Next Trial"
            target:actions
            action:@selector(playTrial)
        ];
        playTrialButton.target = actions;
        [view addSubview:playTrialButton];
        [window.contentView addSubview:view];
    }
    
    void subscribe(EventListener *listener_) override {
        listener = listener_;
    }
    
    void show() override {
        [view setHidden:NO];
    }
    
    void hide() override {
        [view setHidden:YES];
    }

    void playTrial() {
        listener->playTrial();
    }
    
    std::string audioDevice() override {
        return [deviceMenu.titleOfSelectedItem UTF8String];
    }
};

class CocoaTestSetupView : public presentation::View::TestSetup {
    EventListener *listener{};
    NSView *view{
        [[NSView alloc] initWithFrame:NSMakeRect(100, 100, 500, 600)]
    };
    SetupViewActions *actions{[SetupViewActions alloc]};
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
        [view addSubview:signalLevel_dB_SPL_label];
        [view addSubview:signalLevel_dB_SPL_];
        [view addSubview:maskerLevel_dB_SPL_label];
        [view addSubview:maskerLevel_dB_SPL_];
        [view addSubview:stimulusListDirectoryLabel];
        [view addSubview:stimulusListDirectory_];
        [view addSubview:maskerFilePath_label];
        [view addSubview:maskerFilePath_];
        const auto confirmButton = [NSButton buttonWithTitle:
            @"Confirm"
            target:actions
            action:@selector(confirmTestSetup)
        ];
        confirmButton.target = actions;
        [view addSubview:confirmButton];
        [window.contentView addSubview:view];
        stimulusListDirectory_.stringValue =
            @"/Users/basset/Documents/maxdetection/Stimuli/Video/List_Detection";
        maskerFilePath_.stringValue =
            @"/Users/basset/Documents/maxdetection/Stimuli/Masker/L1L2_EngEng.wav";
    }
    
    void subscribe(EventListener *listener_) override {
        listener = listener_;
    }

    void confirmTestSetup() {
        listener->confirmTestSetup();
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
        return "";
    }
    
    std::string subjectId() override {
        return "";
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
    CocoaTestSetupView testSetupView_{window};
    CocoaTesterView testerView_{window};
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
        actions.controller = this;
        [window makeKeyAndOrderFront:nil];
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
    
    void showErrorMessage(std::string) override {
        ;
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
@end

@implementation SetupViewActions
@synthesize controller;

- (void)confirmTestSetup {
    controller->confirmTestSetup();
}
@end

@implementation TesterViewActions
@synthesize controller;
- (void)playTrial {
    controller->playTrial();
}
@end

class CocoaSubjectView {
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
};

class CoreAudioMaskerPlayer : public recognition_test::MaskerPlayer {
    CoreAudioDevice device{};
    AVPlayer *player{[AVPlayer playerWithPlayerItem:nil]};
    EventListener *listener{};
public:
    void subscribe(EventListener *listener_) override {
        listener = listener_;
    }
    
    void fadeIn() override {
        [player play];
        listener->fadeInComplete();
    }
    
    void fadeOut() override {
        [player pause];
    }
    
    void loadFile(std::string filePath) override {
        const auto asset = makeAvAsset(filePath);
        [player replaceCurrentItemWithPlayerItem:
            [AVPlayerItem playerItemWithAsset:asset]];
    }
    
    int deviceCount() override {
        return device.deviceCount();
    }
    
    std::string deviceDescription(int index) override {
        return device.description(index);
    }
    
    void setDevice(int index) override {
        ;
    }
};

class MacOsDirectoryReader : public stimulus_list::DirectoryReader {
    std::vector<std::string> filesIn(std::string directory) override {
        std::vector<std::string> files{};
        const auto path = [NSString stringWithCString:
            directory.c_str()
            encoding:[NSString defaultCStringEncoding]
        ];
        const auto contents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:
            path
            error: nil
        ];
        for (id thing in contents)
            files.push_back([thing UTF8String]);
        return files;
    }
};

#include <random>

class MersenneTwisterRandomizer : public stimulus_list::Randomizer {
    std::mt19937 engine{std::random_device{}()};
public:
    void shuffle(shuffle_iterator begin, shuffle_iterator end) override {
        std::shuffle(begin, end, engine);
    }
};

int main() {
    CoreAudioMaskerPlayer maskerPlayer;
    MacOsDirectoryReader reader;
    stimulus_list::FileFilterDecorator filter{&reader, ".mov"};
    MersenneTwisterRandomizer randomizer;
    stimulus_list::RandomizedStimulusList list{&filter, &randomizer};
    CocoaSubjectView subjectView{};
    recognition_test::Model model{&maskerPlayer, &list, subjectView.stimulusPlayer()};
    CocoaView view;
    presentation::Presenter presenter{&model, &view};
    presenter.run();
}
