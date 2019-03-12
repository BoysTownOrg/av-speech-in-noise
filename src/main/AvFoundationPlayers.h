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
    CoreAudioDevice();
    UInt32 deviceCount_();
    AudioObjectPropertyAddress propertyAddress(AudioObjectPropertySelector);
    int deviceCount();
    std::string description(int device);
    std::string stringProperty(AudioObjectPropertySelector, int device);
    AudioObjectID objectId(int device);
    std::string uid(int device);
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
