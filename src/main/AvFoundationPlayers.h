#ifndef AvFoundationPlayers_h
#define AvFoundationPlayers_h

#include <recognition-test/Model.hpp>
#import <Cocoa/Cocoa.h>
#import <AVFoundation/AVFoundation.h>
#include <gsl/gsl>
#include <vector>

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
    AvFoundationStimulusPlayer();
    void subscribe(EventListener *) override;
    void playbackComplete();
    void play() override;
    void loadFile(std::string filePath) override;
    void setDevice(int index) override;
};

class AvFoundationMaskerPlayer : public recognition_test::MaskerPlayer {
    CoreAudioDevice device{};
    AVPlayer *player{[AVPlayer playerWithPlayerItem:nil]};
    EventListener *listener{};
public:
    void subscribe(EventListener *listener_) override;
    void fadeIn() override;
    void fadeOut() override;
    void loadFile(std::string filePath) override;
    int deviceCount() override;
    std::string deviceDescription(int index) override;
    void setDevice(int index) override;
};

#endif
