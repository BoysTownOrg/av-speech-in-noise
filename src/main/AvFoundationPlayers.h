#ifndef AvFoundationPlayers_h
#define AvFoundationPlayers_h

#include <recognition-test/Model.hpp>
#include <masker-player/RandomizedMaskerPlayer.hpp>
#import <Cocoa/Cocoa.h>
#import <AVFoundation/AVFoundation.h>
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
    StimulusPlayerActions *actions;
    NSWindow *videoWindow;
    AVPlayer *player;
    AVPlayerLayer *playerLayer;
public:
    AvFoundationStimulusPlayer();
    void subscribe(EventListener *) override;
    void playbackComplete();
    void play() override;
    void loadFile(std::string filePath) override;
    void setDevice(int index) override;
};

class AvFoundationVideoPlayer : public masker_player::VideoPlayer {
    CoreAudioDevice device{};
    EventListener *listener{};
    AVPlayer *player;
public:
    AvFoundationVideoPlayer();
    void subscribe(EventListener *) override;
    void loadFile(std::string filePath) override;
    int deviceCount() override;
    std::string deviceDescription(int index) override;
    void setDevice(int index) override;
    bool playing() override;
    void play() override;
};

#endif
