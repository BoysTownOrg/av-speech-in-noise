//
//  AvFoundationPlayers.h
//  av-coordinated-response-measure
//
//  Created by Bashford, Seth on 3/12/19.
//

#ifndef AvFoundationPlayers_h
#define AvFoundationPlayers_h

#import <Cocoa/Cocoa.h>
#import <AVFoundation/AVFoundation.h>
#include <gsl/gsl>

class CoreAudioDevice {
    std::vector<AudioObjectID> devices{};
public:
    CoreAudioDevice() {
        auto count = deviceCount_();
        devices.resize(count);
        UInt32 dataSize = count * sizeof(AudioDeviceID);
        auto address = propertyAddress(kAudioHardwarePropertyDevices);
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
            throw std::runtime_error{"Cannot determine audio device IDs."};
    }
    
    UInt32 deviceCount_() {
        auto address = propertyAddress(kAudioHardwarePropertyDevices);
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
            throw std::runtime_error{"Cannot determine number of audio devices."};
        return dataSize / sizeof(AudioDeviceID);
    }
    
    AudioObjectPropertyAddress propertyAddress(AudioObjectPropertySelector s) {
        return {
            s,
            kAudioObjectPropertyScopeGlobal,
            kAudioObjectPropertyElementMaster
        };
    }

    int deviceCount() {
        return gsl::narrow<int>(devices.size());
    }

    std::string description(int device) {
        return stringProperty(kAudioObjectPropertyName, device);
    }
    
    std::string stringProperty(AudioObjectPropertySelector s, int device) {
        auto address = propertyAddress(s);
        CFStringRef deviceName{};
        UInt32 dataSize = sizeof(CFStringRef);
        if (kAudioHardwareNoError !=
            AudioObjectGetPropertyData(
                objectId(device),
                &address,
                0,
                nullptr,
                &dataSize,
                &deviceName
            )
        )
            throw std::runtime_error{"Cannot do something..."};
        char buffer[128];
        CFStringGetCString(deviceName, buffer, sizeof(buffer), kCFStringEncodingUTF8);
        return buffer;
    }
    
    AudioObjectID objectId(int device) {
        return devices.at(gsl::narrow<decltype(devices)::size_type>(device));
    }

    std::string uid(int device) {
        return stringProperty(kAudioDevicePropertyDeviceUID, device);
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
    CoreAudioDevice device{};
    EventListener *listener{};
    StimulusPlayerActions *actions{[StimulusPlayerActions alloc]};
    NSWindow *videoWindow{
        [[NSWindow alloc]
            initWithContentRect: NSMakeRect(400, 400, 0, 0)
            styleMask:NSWindowStyleMaskBorderless
            backing:NSBackingStoreBuffered
            defer:YES
        ]
    };
    AVPlayer *player{[AVPlayer playerWithPlayerItem:nil]};
    AVPlayerLayer *playerLayer{[AVPlayerLayer playerLayerWithPlayer:player]};
public:
    AvFoundationStimulusPlayer() {
        [videoWindow.contentView setWantsLayer:YES];
        [videoWindow.contentView.layer addSublayer:playerLayer];
        [videoWindow makeKeyAndOrderFront:nil];
        actions.controller = this;
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
        auto uid_ = device.uid(index);
        player.audioOutputDeviceUniqueID = [
            NSString stringWithCString:uid_.c_str()
            encoding:[NSString defaultCStringEncoding]
        ];
    }
};

@implementation StimulusPlayerActions
@synthesize controller;
- (void)playbackComplete {
    controller->playbackComplete();
}
@end

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
        auto uid_ = device.uid(index);
        player.audioOutputDeviceUniqueID = [
            NSString stringWithCString:uid_.c_str()
            encoding:[NSString defaultCStringEncoding]
        ];
    }
};

#endif /* AvFoundationPlayers_h */
