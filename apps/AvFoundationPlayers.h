#ifndef APPS_AVFOUNDATIONPLAYERS_H_
#define APPS_AVFOUNDATIONPLAYERS_H_

#include <recognition-test/RecognitionTestModel.hpp>
#include <stimulus-players/MaskerPlayerImpl.hpp>
#include <stimulus-players/TargetPlayerImpl.hpp>
#include <stimulus-players/AudioReaderImpl.hpp>
#import <Cocoa/Cocoa.h>
#import <AVFoundation/AVFoundation.h>
#include <vector>

class CoreAudioDevices {
  public:
    CoreAudioDevices();
    auto deviceCount() -> int;
    auto description(int device) -> std::string;
    auto uid(int device) -> std::string;
    auto outputDevice(int device) -> bool;
    auto objectId(int device) -> AudioObjectID;

  private:
    void loadDevices();
    auto stringProperty(AudioObjectPropertySelector, int device) -> std::string;

    std::vector<AudioObjectID> devices{};
};

class CoreAudioBuffer : public stimulus_players::AudioBuffer {
  public:
    explicit CoreAudioBuffer(AVAssetReaderTrackOutput *);
    ~CoreAudioBuffer() override;
    void set(CMSampleBufferRef);
    auto channels() -> int override;
    auto channel(int) -> std::vector<int> override;
    auto empty() -> bool override;

  private:
    AudioBufferList audioBufferList{};
    CMItemCount frames{};
    CMSampleBufferRef sampleBuffer;
    // possibly critical: initialize blockBuffer to null
    CMBlockBufferRef blockBuffer{};
};

class CoreAudioBufferedReader : public stimulus_players::BufferedAudioReader {
  public:
    void loadFile(std::string) override;
    auto failed() -> bool override;
    auto readNextBuffer()
        -> std::shared_ptr<stimulus_players::AudioBuffer> override;
    auto minimumPossibleSample() -> int override;
    auto sampleRateHz() -> double;

  private:
    AVAssetReaderTrackOutput *trackOutput{};
};

class AvFoundationVideoPlayer;

@interface VideoPlayerActions : NSObject
@property AvFoundationVideoPlayer *controller;
- (void)playbackComplete;
@end

class AvFoundationVideoPlayer : public stimulus_players::VideoPlayer {
  public:
    explicit AvFoundationVideoPlayer(NSScreen *);
    void playbackComplete();
    auto audio() -> std::vector<gsl::span<float>> & { return audio_; }
    void fillAudioBuffer();
    void play() override;
    void loadFile(std::string filePath) override;
    void setDevice(int index) override;
    void hide() override;
    void show() override;
    void subscribe(EventListener *) override;
    auto deviceCount() -> int override;
    auto deviceDescription(int index) -> std::string override;
    auto playing() -> bool override;
    void subscribeToPlaybackCompletion() override;
    auto durationSeconds() -> double override;

  private:
    void addPlayerLayer();
    void showWindow();
    void prepareWindow();
    void schedulePlaybackCompletion();
    void centerVideo();
    void resizeVideo();
    void prepareVideo();
    static void prepareTap(MTAudioProcessingTapRef tap,
        CMItemCount maxFrames,
        const AudioStreamBasicDescription *processingFormat);
    void prepareTap_(MTAudioProcessingTapRef tap,
        CMItemCount maxFrames,
        const AudioStreamBasicDescription *processingFormat);

    std::vector<gsl::span<float>> audio_;
    MTAudioProcessingTapRef tap{};
    CoreAudioDevices device{};
    VideoPlayerActions *actions;
    NSWindow *videoWindow;
    AVPlayer *player;
    AVPlayerLayer *playerLayer;
    NSScreen *screen;
    EventListener *listener_{};
};

class AvFoundationAudioPlayer : public stimulus_players::AudioPlayer {
  public:
    AvFoundationAudioPlayer();
    ~AvFoundationAudioPlayer() override;
    void subscribe(EventListener *) override;
    void loadFile(std::string filePath) override;
    auto deviceCount() -> int override;
    auto deviceDescription(int index) -> std::string override;
    void setDevice(int index) override;
    auto playing() -> bool override;
    void play() override;
    auto sampleRateHz() -> double override;
    void stop() override;
    void timerCallback();
    auto outputDevice(int index) -> bool override;
    auto durationSeconds() -> double override;

  private:
    static auto AU_RenderCallback(void *inRefCon,
        AudioUnitRenderActionFlags *ioActionFlags,
        const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
        UInt32 inNumberFrames, AudioBufferList *ioData) -> OSStatus;
    auto audioBufferReady(AudioUnitRenderActionFlags *ioActionFlags,
        const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
        UInt32 inNumberFrames, AudioBufferList *ioData) -> OSStatus;

    std::vector<gsl::span<float>> audio_;
    CoreAudioDevices device{};
    std::string filePath_{};
    EventListener *listener_{};
    AudioUnit audioUnit{};
    double sampleRate_{};
};

#endif
