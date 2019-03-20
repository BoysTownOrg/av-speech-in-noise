#ifndef AvFoundationPlayers_h
#define AvFoundationPlayers_h

#include <recognition-test/RecognitionTestModel.hpp>
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
    void hideVideo() override;
    void showVideo() override;
};

class AvFoundationAudioPlayer : public masker_player::AudioPlayer {
    std::vector<gsl::span<float>> audio;
    CoreAudioDevice device{};
    MTAudioProcessingTapRef tap{};
    EventListener *listener{};
    AVPlayer *player;
    double sampleRate_{};
public:
    AvFoundationAudioPlayer();
    void subscribe(EventListener *) override;
    void loadFile(std::string filePath) override;
    int deviceCount() override;
    std::string deviceDescription(int index) override;
    void setDevice(int index) override;
    bool playing() override;
    void play() override;
    double sampleRateHz() override;
    void stop() override;
    
private:
    static void init(MTAudioProcessingTapRef, void *, void **);
    static void finalize(MTAudioProcessingTapRef);
    static void prepare(
        MTAudioProcessingTapRef,
        CMItemCount,
        const AudioStreamBasicDescription *
    );
    static void unprepare(MTAudioProcessingTapRef);
    static void process(
        MTAudioProcessingTapRef,
        CMItemCount,
        MTAudioProcessingTapFlags,
        AudioBufferList *,
        CMItemCount *,
        MTAudioProcessingTapFlags *
    );
};

#endif
