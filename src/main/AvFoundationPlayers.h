#ifndef AvFoundationPlayers_h
#define AvFoundationPlayers_h

#include <recognition-test/RecognitionTestModel.hpp>
#include <masker-player/MaskerPlayerImpl.hpp>
#include <target-player/TargetPlayerImpl.hpp>
#import <Cocoa/Cocoa.h>
#import <AVFoundation/AVFoundation.h>
#include <vector>

class CoreAudioDevices {
    std::vector<AudioObjectID> devices{};
public:
    CoreAudioDevices();
    void loadDevices();
    UInt32 deviceCount_();
    AudioObjectPropertyAddress globalAddress(AudioObjectPropertySelector);
    int deviceCount();
    std::string description(int device);
    std::string stringProperty(AudioObjectPropertySelector, int device);
    AudioObjectID objectId(int device);
    std::string uid(int device);
    bool outputDevice(int device);
};

class AvFoundationVideoPlayer;

@interface VideoPlayerActions : NSObject
@property AvFoundationVideoPlayer *controller;
- (void) playbackComplete;
@end

class AvFoundationVideoPlayer : public target_player::VideoPlayer {
    std::vector<gsl::span<float>> audio_;
    MTAudioProcessingTapRef tap{};
    CoreAudioDevices device{};
    VideoPlayerActions *actions;
    NSWindow *videoWindow;
    AVPlayer *player;
    AVPlayerLayer *playerLayer;
    EventListener *listener_{};
public:
    AvFoundationVideoPlayer();
    void playbackComplete();
    void setSampleRate(double x) {}
    std::vector<gsl::span<float>> &audio() { return audio_; }
    void fillAudioBuffer() { listener_->fillAudioBuffer(audio_); }
    void play() override;
    void loadFile(std::string filePath) override;
    void setDevice(int index) override;
    void hide() override;
    void show() override;
    void subscribe(EventListener *) override;
    int deviceCount() override;
    std::string deviceDescription(int index) override;
    std::vector<std::vector<float>> readAudio(std::string filePath) override;
    bool playing() override;
    void subscribeToPlaybackCompletion() override;
    
private:
    void schedulePlaybackCompletion();
    void resizeVideo();
    void prepareVideo();
};

class AvFoundationAudioPlayer;

@interface CallbackScheduler : NSObject
@property AvFoundationAudioPlayer *controller;
- (void) scheduleCallbackAfterSeconds: (double) x;
- (void) timerCallback;
@end

class AvFoundationAudioPlayer : public masker_player::AudioPlayer {
    std::vector<gsl::span<float>> audio_;
    CoreAudioDevices device{};
    MTAudioProcessingTapRef tap{};
    EventListener *listener_{};
    AVPlayer *player;
    CallbackScheduler *scheduler;
    double sampleRate_{};
public:
    AvFoundationAudioPlayer();
    void setSampleRate(double x) { sampleRate_ = x;}
    std::vector<gsl::span<float>> &audio() { return audio_; }
    void fillAudioBuffer() { listener_->fillAudioBuffer(audio_); }
    void subscribe(EventListener *) override;
    void loadFile(std::string filePath) override;
    int deviceCount() override;
    std::string deviceDescription(int index) override;
    void setDevice(int index) override;
    bool playing() override;
    void play() override;
    double sampleRateHz() override;
    void stop() override;
    void scheduleCallbackAfterSeconds(double) override;
    void timerCallback();
    bool outputDevice(int index) override;
    std::vector<std::vector<float>> readAudio(std::string filePath) override;
};

#endif
