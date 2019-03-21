#ifndef AvFoundationPlayers_h
#define AvFoundationPlayers_h

#include <recognition-test/RecognitionTestModel.hpp>
#include <masker-player/RandomizedMaskerPlayer.hpp>
#include <stimulus-player/StimulusPlayerImpl.hpp>
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

class AvFoundationVideoPlayer;

@interface StimulusPlayerActions : NSObject
@property AvFoundationVideoPlayer *controller;
- (void) playbackComplete;
@end

class AvFoundationVideoPlayer : public stimulus_player::VideoPlayer {
    std::vector<gsl::span<float>> audio_;
    MTAudioProcessingTapRef tap{};
    CoreAudioDevice device{};
    StimulusPlayerActions *actions;
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
};

class AvFoundationAudioPlayer : public masker_player::AudioPlayer {
    std::vector<gsl::span<float>> audio_;
    CoreAudioDevice device{};
    MTAudioProcessingTapRef tap{};
    EventListener *listener_{};
    AVPlayer *player;
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
};

#endif
