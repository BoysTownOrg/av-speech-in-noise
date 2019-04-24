#ifndef AvFoundationPlayers_h
#define AvFoundationPlayers_h

#include <recognition-test/RecognitionTestModel.hpp>
#include <stimulus-players/MaskerPlayerImpl.hpp>
#include <stimulus-players/TargetPlayerImpl.hpp>
#include <stimulus-players/AudioReaderImpl.hpp>
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

class CoreAudioBuffer : public stimulus_players::AudioBuffer {
    AudioBufferList audioBufferList{};
    CMItemCount frames{};
    CMSampleBufferRef sampleBuffer;
    // possibly critical: initialize blockBuffer to null
    CMBlockBufferRef blockBuffer{};
public:
    explicit CoreAudioBuffer(AVAssetReaderTrackOutput *);
    ~CoreAudioBuffer() override;
    void set(CMSampleBufferRef);
    int channels() override;
    std::vector<int> channel(int) override;
    bool empty() override;
};

class CoreAudioBufferedReader : public stimulus_players::BufferedAudioReader {
    AVAssetReaderTrackOutput *trackOutput{};
public:
    void loadFile(std::string) override;
    bool failed() override;
    std::shared_ptr<stimulus_players::AudioBuffer> readNextBuffer() override;
    int minimumPossibleSample() override;
};

class AvFoundationVideoPlayer;

@interface VideoPlayerActions : NSObject
@property AvFoundationVideoPlayer *controller;
- (void) playbackComplete;
@end

class AvFoundationVideoPlayer : public stimulus_players::VideoPlayer {
    std::vector<gsl::span<float>> audio_;
    MTAudioProcessingTapRef tap{};
    CoreAudioDevices device{};
    VideoPlayerActions *actions;
    NSWindow *videoWindow;
    AVPlayer *player;
    AVPlayerLayer *playerLayer;
    EventListener *listener_{};
public:
    AvFoundationVideoPlayer(NSRect);
    void playbackComplete();
    void setSampleRate(double x) {}
    std::vector<gsl::span<float>> &audio() { return audio_; }
    void fillAudioBuffer();
    void play() override;
    void loadFile(std::string filePath) override;
    void setDevice(int index) override;
    void hide() override;
    void show() override;
    void subscribe(EventListener *) override;
    int deviceCount() override;
    std::string deviceDescription(int index) override;
    bool playing() override;
    void subscribeToPlaybackCompletion() override;
    double durationSeconds() override;
    
private:
    void addPlayerLayer();
    void showWindow();
    void prepareWindow();
    void schedulePlaybackCompletion();
    void centerVideo();
    void resizeVideo();
    void prepareVideo();
};

class AvFoundationAudioPlayer : public stimulus_players::AudioPlayer {
    std::vector<gsl::span<float>> audio_;
    CoreAudioDevices device{};
    MTAudioProcessingTapRef tap{};
    EventListener *listener_{};
    AVPlayer *player;
    double sampleRate_{};
public:
    AvFoundationAudioPlayer();
    void setSampleRate(double x) { sampleRate_ = x; }
    std::vector<gsl::span<float>> &audio() { return audio_; }
    void fillAudioBuffer();
    void subscribe(EventListener *) override;
    void loadFile(std::string filePath) override;
    int deviceCount() override;
    std::string deviceDescription(int index) override;
    void setDevice(int index) override;
    bool playing() override;
    void play() override;
    double sampleRateHz() override;
    void stop() override;
    void timerCallback();
    bool outputDevice(int index) override;
    double durationSeconds() override;
    void seekSeconds(double) override;
};

#endif
